/*
* vtestcall: make a single video test call with IAXCLIENT
*
* IAX Support for talking to Asterisk and other Gnophone clients
*
* Copyright (C) 1999, Linux Support Services, Inc.
*
* Mark Spencer <markster@linux-support.net>
* Stefano Falsetto <falsetto@gnu.org>
* Mihai Balea <mihai AT hates DOT ms>
*
* This program is free software, distributed under the terms of
* the GNU Lesser (Library) General Public License
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "iaxclient.h"
#include "slice.h"
#include "file.h"

#ifdef WIN32
// Only under windows...
#undef main
#endif

#define MAX_CALLS 1

#define TEST_OK            0
#define TEST_NO_CONNECTION -1
#define TEST_NO_MEDIA      -2
#define TEST_UNKNOWN_ERROR -99

//int format = IAXC_FORMAT_THEORA | IAXC_FORMAT_SPEEX;
int format = IAXC_FORMAT_H263 | IAXC_FORMAT_H263_PLUS | IAXC_FORMAT_H264 | IAXC_FORMAT_MPEG4 | IAXC_FORMAT_THEORA;
int formatp = IAXC_FORMAT_H264; //IAXC_FORMAT_THEORA;
int framerate = 15;
int bitrate = 200000;
int width = 320;
int height = 240;
int fragsize = 1400;

int call_established = 0;
int running = 0;

// Forward declaration
void process_text_message(char *message);

char caption[80] = "";

int send_video = 1;
int send_audio = 1;
int print_netstats = 0;
int timeout = 0;
int video_frames_count = 0;
int audio_frames_count = 0;

struct timeval start_time;

// Audio-cosmetic...
struct iaxc_sound sound_ringOUT, sound_ringIN;

/* routine used to shutdown and close nicely.*/
void hangup_and_exit(int code)
{
	fprintf(stderr,"Dump call\n");
	iaxc_dump_call();
	fprintf(stderr,"Sleep for 500 msec\n");
	iaxc_millisleep(500);
	fprintf(stderr,"Stop processing thread\n");
	iaxc_stop_processing_thread();
	fprintf(stderr,"Calling iaxc_shutdown...");
	iaxc_shutdown();
	fprintf(stderr,"Exiting with code %d\n", code);
	exit(code);
}

void signal_handler(int signum)
{
	if ( signum == SIGTERM || signum == SIGINT ) 
	{
		running = 0;
	}
}

void fatal_error(char *err) 
{
	fprintf(stderr, "FATAL ERROR: %s\n", err);
	exit(TEST_UNKNOWN_ERROR);
}

int levels_callback(float input, float output) {
	//fprintf(stderr,"Input level: %f\nOutput level: %f\n",input,output);
	return 1;
}

int netstat_callback(struct iaxc_ev_netstats n) {
	static int i;
	
	if ( !print_netstats )
		return 0;
	
	if(i++%25 == 0)
		fprintf(stderr, "RTT\t"
		"Rjit\tRlos%%\tRlosC\tRpkts\tRdel\tRdrop\tRooo\t"
		"Ljit\tLlos%%\tLlosC\tLpkts\tLdel\tLdrop\tLooo\n");

	fprintf(stderr, "%d\t"
		"%d\t%d\t%d\t%d\t%d\t%d\t%d\t"
		"%d\t%d\t%d\t%d\t%d\t%d\t%d\n",

		n.rtt,

		n.remote.jitter,
		n.remote.losspct,
		n.remote.losscnt,
		n.remote.packets,
		n.remote.delay,
		n.remote.dropped,
		n.remote.ooo,

		n.local.jitter,
		n.local.losspct,
		n.local.losscnt,
		n.local.packets,
		n.local.delay,
		n.local.dropped,
		n.local.ooo
		);

	return 0;
}

void process_text_message(char *message)
{
	unsigned int prefs;
	
	if ( strncmp(message, "CONTROL:", strlen("CONTROL:")) == 0 )
	{
		message += strlen("CONTROL:");
		if ( strcmp(message, "STOPVIDEO") == 0 )
		{
			// Stop sending video
			prefs = iaxc_get_video_prefs();
			prefs = prefs | IAXC_VIDEO_PREF_SEND_DISABLE ;
			iaxc_set_video_prefs(prefs);
		} else if ( strcmp(message, "STARTVIDEO") == 0 )
		{
			// Start sending video
			prefs = iaxc_get_video_prefs();
			prefs = prefs & ~IAXC_VIDEO_PREF_SEND_DISABLE ;
			iaxc_set_video_prefs(prefs);
		}
	} else
		fprintf(stderr, "Text message received: %s\n", message);
}

void usage()
{ 
	printf(
		"\n"
		"Usage is: tescall <options>\n\n"
		"available options:\n"
		"-F <codec,framerate,bitrate,width,height,fragsize> set video parameters\n"
		"-o <filename> media file to run\n"
		"-v stop sending video\n"
		"-a stop sending audio\n"
		"-l run file in a loop\n"
		"-n dump periodic netstats to stderr\n"
		"-t <timeout> terminate after timeout seconds and report status via return code\n"
		"\n"
		);
	exit(1);
}

int test_mode_state_callback(struct iaxc_ev_call_state s)
{
	printf("Call #%d state %d\n",s.callNo, s.state);

	if ( s.state & IAXC_CALL_STATE_COMPLETE )
	{
		fprintf(stderr, "Call answered\n");
		call_established = 1;
	}
	if (s.state == IAXC_CALL_STATE_FREE) 
	{
		fprintf(stderr,"Call terminated\n");
		running = 0;
	}

	return 0;
}

int test_mode_callback(iaxc_event e)
{
	switch ( e.type ) 
	{
		case IAXC_EVENT_LEVELS:
			return levels_callback(e.ev.levels.input, e.ev.levels.output);
		case IAXC_EVENT_NETSTAT:
			return netstat_callback(e.ev.netstats);
		case IAXC_EVENT_TEXT:
			process_text_message(e.ev.text.message);
			break;
		case IAXC_EVENT_STATE:
			return test_mode_state_callback(e.ev.call);
		case IAXC_EVENT_VIDEO:
			video_frames_count++;
			break;
		case IAXC_EVENT_AUDIO:
			audio_frames_count++;
			break;
		default:
			break;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int                       i; 
	char                      mydest[80], *dest = NULL;
	char                      *ogg_file = NULL;
	int                       loop = 0;
	int                       video_frame_index;
	static struct slice_set_t slice_set;
	unsigned short            source_id;
	struct timeval            now;
	
	/* install signal handler to catch CRTL-Cs */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	/* Parse command line */
	for(i=1;i<argc;i++)
	{
		if(argv[i][0] == '-') 
		{
			switch(argv[i][1])
			{
			case 'F': /* set video params */
				{
					formatp = 1<<atoi(argv[++i]);
					framerate = atoi(argv[++i]);
					bitrate = atoi(argv[++i]);
					width = atoi(argv[++i]);
					height = atoi(argv[++i]);
					fragsize = atoi(argv[++i]);
				}
				break;
			case 'o':
				if ( i+1 >= argc )
					usage();
				ogg_file = argv[++i];
				break;
			case 'v':
				send_video = 0;
				break;
			case 'a':
				send_audio = 0;
				break;
			case 'l':
				loop = 1;
				break;
			case 'n':
				print_netstats = 1;
				break;
			case 't':
				if ( i+1 >= argc )
					usage();
				timeout = 1000 * atoi(argv[++i]);
				break;
			default:
				usage();
			}
		} else 
			dest=argv[i];
	}
	
	if ( dest == NULL )
	{
		// We need a destination to call
		fprintf(stderr, "No destination, quitting\n");
		return -1;
	}
	
	if ( ogg_file == NULL )
		fprintf(stderr, "No media file, running dry\n");
	
	if ( ogg_file )
	{
		// Load ogg file
		load_ogg_file(ogg_file);
	}
	
	// Get start time for timeouts
	gettimeofday(&start_time, NULL);
	
	// Initialize iaxclient
	iaxc_video_format_set(formatp, format, framerate, bitrate, width, height, fragsize);
	iaxc_set_test_mode(1);
	if (iaxc_initialize(MAX_CALLS)) 
		fatal_error("cannot initialize iaxclient!");
		
	iaxc_set_formats(IAXC_FORMAT_SPEEX, IAXC_FORMAT_SPEEX);
	iaxc_video_bypass_jitter(0);
	iaxc_set_audio_prefs(IAXC_AUDIO_PREF_RECV_REMOTE_ENCODED);
	iaxc_set_video_prefs(IAXC_VIDEO_PREF_RECV_REMOTE_ENCODED);
	iaxc_set_event_callback(test_mode_callback); 
	
	// Crank the engine
	iaxc_start_processing_thread();
	
	// Dial out
	int callNo = iaxc_call(dest);
	if (callNo <= 0)
		iaxc_select_call(callNo);
	else
		fprintf(stderr, "Failed to make call to '%s'", dest);

	// Wait for the call to be established;
	while ( !call_established )
	{
		gettimeofday(&now, NULL);
		if ( timeout > 0 && iaxci_msecdiff(&now, &start_time) > timeout )
			hangup_and_exit(TEST_NO_CONNECTION);
		iaxc_millisleep(5);
	}
	
	running = 1;
	while ( running )
	{
		// We only need this if we actually want to send something
		if ( ogg_file && ( send_audio || send_video ) )
		{
			ogg_packet *op;
			
			op = get_next_audio_op();
			if ( !loop && audio_is_eos() )
				break;
			if ( send_audio && op != NULL && op->bytes > 0 )
				iaxc_push_audio(op->packet, op->bytes, SPEEX_SAMPLING_RATE / 1000 * SPEEX_FRAME_DURATION);
			
			op = get_next_video_op();
			if ( !loop && video_is_eos() )
				break;
			if ( send_video && op != NULL && op->bytes > 0 )
				iaxc_push_video(op->packet, op->bytes, 1);
		}
		
		// Tight spinloops are bad, mmmkay?
		iaxc_millisleep(5);
		
		// Exit after a positive timeout
		gettimeofday(&now, NULL);
		if ( timeout > 0 && iaxci_msecdiff(&now, &start_time) > timeout )
			running = 0;
	}
	
	fprintf(stderr, "Received %d audio frames and %d video frames\n", audio_frames_count, video_frames_count);
	if ( audio_frames_count == 0 && video_frames_count == 0 )
		hangup_and_exit(TEST_NO_MEDIA);
	else
		hangup_and_exit(TEST_OK);
	return 0;
}
