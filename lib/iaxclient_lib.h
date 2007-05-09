/*
 * iaxclient: a cross-platform IAX softphone library
 *
 * Copyrights:
 * Copyright (C) 2003 HorizonLive.com, (c) 2004, Horizon Wimba, Inc.
 *
 * Contributors:
 * Steve Kann <stevek@stevek.com>
 * Michael Van Donselaar <mvand@vandonselaar.org> 
 * Shawn Lawrence <shawn.lawrence@terracecomm.com>
 *
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License
 */
#ifndef _iaxclient_lib_h
#define _iaxclient_lib_h

#ifdef __cplusplus
extern "C" {
#endif


/* This is the internal include file for IAXCLIENT -- externally
 * accessible APIs should be declared in iaxclient.h */

#ifndef _MSC_VER
#include <stdint.h>
#endif

#include <stdio.h>
#include <string.h>

#if defined(WIN32)  ||  defined(_WIN32_WCE)
void gettimeofday(struct timeval *tv, void /*struct timezone*/ *tz);
#include <winsock.h>
#if !defined(_WIN32_WCE)
#include <process.h>
#endif
#include <stddef.h>
#include <time.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#endif

#if (SPEEX_PREPROCESS == 1)
#include <speex/speex_preprocess.h>
#endif
#include "sox/sox.h"

#ifdef USE_FFMPEG
// To access to check_ff function
#include "codec_ffmpeg.h"
#endif

#include <stdlib.h>
#include <math.h>
#include "spandsp/plc.h"



/* os-dependent macros, etc */
#if defined(WIN32) || defined(_WIN32_WCE)
#define THREAD HANDLE
#define THREADID unsigned
#define THREADCREATE(func, args, thread, id) \
(thread = (HANDLE)_beginthreadex(NULL, 0, func, (PVOID)args, 0, &id))
#define THREADCREATE_ERROR NULL
#define THREADFUNCDECL(func) unsigned __stdcall func(PVOID args)
#define THREADFUNCRET(r) int r = 0
#define THREADJOIN(t)
/* causes deadlock with wx GUI on MSW */
/* #define THREADJOIN(t) WaitForSingleObject(t, INFINITE) */
#define MUTEX CRITICAL_SECTION
#define MUTEXINIT(m) InitializeCriticalSection(m)
#define MUTEXLOCK(m) EnterCriticalSection(m)
#define MUTEXTRYLOCK(m) (!TryEnterCriticalSection(m))
#define MUTEXUNLOCK(m) LeaveCriticalSection(m)
#define MUTEXDESTROY(m) DeleteCriticalSection(m)

#else
#define THREAD pthread_t
#define THREADID unsigned /* unused for Posix Threads */
#define THREADCREATE(func, args, thread, id) \
pthread_create(&thread, NULL, func, args)
#define THREADCREATE_ERROR -1
#define THREADFUNCDECL(func) void * func(void *args)
#define THREADFUNCRET(r) void * r = 0
#define THREADJOIN(t) pthread_join(t, 0)
#define MUTEX pthread_mutex_t
#define MUTEXINIT(m) pthread_mutex_init(m, NULL) //TODO: check error
#define MUTEXLOCK(m) pthread_mutex_lock(m)
#define MUTEXTRYLOCK(m) pthread_mutex_trylock(m)
#define MUTEXUNLOCK(m) pthread_mutex_unlock(m)
#define MUTEXDESTROY(m) pthread_mutex_destroy(m)
#endif

#ifdef MACOSX
#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <sched.h>
#include <sys/sysctl.h>
#endif

#define MAXARGS 10
#define MAXARG 256
#define MAX_SESSIONS 4
#define OUT_INTERVAL 20

/* millisecond interval to time out calls */
#define IAXC_CALL_TIMEOUT 30000


void os_init(void);
void iaxc_usermsg(int type, const char *fmt, ...);
void iaxc_do_levels_callback(float input, float output);
void iaxc_do_audio_callback(int callNo, unsigned int ts, int remote,
		int encoded, int format, int size, unsigned char *data);

#include "iaxclient.h"

struct iaxc_audio_driver {
	/* data */
	char *name; 	/* driver name */
	struct iaxc_audio_device *devices; /* list of devices */
	int nDevices;	/* count of devices */
	void *priv;	/* pointer to private data */

	/* methods */
	int (*initialize)(struct iaxc_audio_driver *d, int sample_rate);
	int (*destroy)(struct iaxc_audio_driver *d);  /* free resources */
	int (*select_devices)(struct iaxc_audio_driver *d, int input, int output, int ring);
	int (*selected_devices)(struct iaxc_audio_driver *d, int *input, int *output, int *ring);

	/* 
	 * select_ring ? 
	 * set_latency
	 */

	int (*start)(struct iaxc_audio_driver *d);
	int (*stop)(struct iaxc_audio_driver *d);
	int (*output)(struct iaxc_audio_driver *d, void *samples, int nSamples);
	int (*input)(struct iaxc_audio_driver *d, void *samples, int *nSamples);

	/* levels */
	double (*input_level_get)(struct iaxc_audio_driver *d);
	double (*output_level_get)(struct iaxc_audio_driver *d);
	int (*input_level_set)(struct iaxc_audio_driver *d, double level);
	int (*output_level_set)(struct iaxc_audio_driver *d, double level);

	/* sounds */
	int (*play_sound)(struct iaxc_sound *s, int ring);
	int (*stop_sound)(int id);

	/* mic boost */
	int (*mic_boost_get)(struct iaxc_audio_driver *d ) ;
	int (*mic_boost_set)(struct iaxc_audio_driver *d, int enable);
}; 

struct iaxc_video_driver {
	/* data */
	char *name; 	/* driver name */
	//struct iaxc_audio_device *devices; /* list of devices */
	//int nDevices;	/* count of devices */
	void *priv;	/* pointer to private data */

	/* methods */
	int (*initialize)(struct iaxc_video_driver *d, int w, int h, int framerate);
	int (*destroy)(struct iaxc_video_driver *d);  /* free resources */
	int (*select_device)(struct iaxc_video_driver *d, int input);
	int (*selected_device)(struct iaxc_video_driver *d, int *input);

	/* 
	 * select_ring ? 
	 * set_latency
	 */

	int (*start)(struct iaxc_video_driver *d);
	int (*stop)(struct iaxc_video_driver *d);
	int (*input)(struct iaxc_video_driver *d, char **data);
	
	int (*is_camera_working)(struct iaxc_video_driver *d);
	
	// HACK ALERT!!!
	// If the requested size of the video is not 320x240, then we
	// resize the image in software rather than relying on the camera
	// to provide an adequate frame
	int needs_resize;
	int req_width;
	int req_height;
	
	int camera_working; /* true if the camera is working */
}; 

struct iaxc_audio_codec {
	char name[256];
	int format;
	int minimum_frame_size;
	void *encstate;
	void *decstate;
	int (*encode) ( struct iaxc_audio_codec *codec, int *inlen, short *in, int *outlen, unsigned char *out );
	int (*decode) ( struct iaxc_audio_codec *codec, int *inlen, unsigned char *in, int *outlen, short *out );
	void (*destroy) ( struct iaxc_audio_codec *codec);
};

#define MAX_TRUNK_LEN	(1<<16)
#define MAX_NO_SLICES	32

struct slice_set_t
{
	int	num_slices;
	int	key_frame;
	int	size[MAX_NO_SLICES];
	char	data[MAX_NO_SLICES][MAX_TRUNK_LEN];
};

struct iaxc_video_codec {
	char name[256];
	int format;
	int width;
	int height;
	int framerate;
	int bitrate;
	int fragsize;
	int params_changed;
	void *encstate;
	void *decstate;
	struct iaxc_video_stats video_stats;
	int (*encode) ( struct iaxc_video_codec *codec, int inlen, char *in, struct slice_set_t *out );
	int (*decode) ( struct iaxc_video_codec *codec, int inlen, char *in, int *outlen, char *out );
	void (*destroy) ( struct iaxc_video_codec *codec);
};



struct iaxc_call {
	/* to be replaced with codec-structures, with codec-private data  */
	struct iaxc_audio_codec *encoder;
	struct iaxc_audio_codec *decoder;
	struct iaxc_video_codec *vencoder;
	struct iaxc_video_codec *vdecoder;
	int vformat;

	/* the "state" of this call */
	int state;
	char remote[IAXC_EVENT_BUFSIZ];
	char remote_name[IAXC_EVENT_BUFSIZ];
	char local[IAXC_EVENT_BUFSIZ];
	char local_context[IAXC_EVENT_BUFSIZ];

	/* Outbound CallerID */
	char callerid_name[IAXC_EVENT_BUFSIZ];
	char callerid_number[IAXC_EVENT_BUFSIZ];

	/* reset whenever we receive packets from remote */
	struct 	 timeval 	last_activity;
	struct 	 timeval 	last_ping;

	/* our negotiated format */
	int format;

	/* we've sent a silent frame since the last audio frame */
	int tx_silent;

	struct iax_session *session;
};

#include "audio_encode.h"
#include "audio_portaudio.h"

#include "videoLib/video_grab.h"

int iaxc_video_initialize();
int iaxc_video_destroy();
int iaxc_receive_video(struct iaxc_call * call, int sel_call,
		void * encoded_video, int encoded_video_len,
		unsigned int ts, int format);
int iaxc_send_video(struct iaxc_call *, int);

extern double iaxc_silence_threshold;
extern int iaxc_audio_output_mode;

int post_event_callback(iaxc_event e);

/* post an event to the application */
void iaxc_post_event(iaxc_event e);

/* parameters for callback */
extern void * post_event_handle;
extern int post_event_id;

/* Priority boost support */
extern int iaxc_prioboostbegin(void);
extern int iaxc_prioboostend(void);

/* get the raw in/out levels, as int */
extern int iaxc_get_inout_volumes(int *input, int *output);

void iaxc_reset_vcodec_stats(struct iaxc_video_codec *vcodec);
void iaxc_reset_stats(struct iaxc_call *call);
int iaxc_get_video_stats(struct iaxc_call *call, struct iaxc_video_stats *stats, int reset);

long iaxc_usecdiff(struct timeval *t0, struct timeval *t1);
long iaxc_msecdiff(struct timeval *t0, struct timeval *t1);

#ifdef __cplusplus
}
#endif
#endif
