/*
 * iaxclient: a cross-platform IAX softphone library
 *
 * Copyrights:
 * Copyright (C) 2003-2006, Horizon Wimba, Inc.
 * Copyright (C) 2007, Wimba, Inc.
 *
 * Contributors:
 * Mihai Balea <mihaiAThatesDOTms>
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License.
 */

#define USE_H264_RTP_FOO

#include "iaxclient_lib.h"
#include "codec_h264_vss.h"
#include "vssh_encoder.h"
#include "vssh_decoder.h"
#ifdef USE_H264_RTP_FOO
#include "vss_rtp.h"
#endif

#define MAX_SLICE_SIZE	8000

#ifndef USE_H264_RTP_FOO
// Annex B file format start code
#define START_CODE_LEN 4
static const byte START_CODE[START_CODE_LEN] = {0, 0, 0, 1};
#endif

struct encoder_t
{
	void * handle;
	vssh_encoder_settings_t settings;
#ifdef USE_H264_RTP_FOO
	rtp_context_t * rtp_context;
	vss_time start_time;
	int output_buffer_size;
	unsigned char * output_buffer;
#endif
};

struct decoder_t
{
	void * handle;
	vssh_decoder_settings_t settings;
#ifdef USE_H264_RTP_FOO
	rtp_context_t * rtp_context;
#endif
};

static char *get_error_text(int rc)
{
	switch (rc)
	{
	case VSSH_ERR_GENERAL:  // -1
		return "general fault";
	case VSSH_ERR_MEMORY:   // -2
		return "not enough memory";
	case VSSH_ERR_ARG:  // -3
		return "wrong function argument value";
	case VSSH_ERR_EXPIRED:  // -4
		return "evaluation has expired";
	case VSSH_ERR_SKIP_FRAME:   // -5
		return "frame was skipped";
	case VSSH_ERR_FRAME_DIMENSIONS: // -6
		return "invalid input frame dimensions ";
	case VSSH_ERR_SETTINGS: // -7
		return "invalid encoder settings";
	case VSSH_ERR_MORE_DATA:    // -8
		return "need more input data";
	case VSSH_ERR_FILE: // -9
		return "file operation failed";
	case VSSH_ERR_STREAM_PRED_TYPE: // -10
		return "invalid prediction type";
	case VSSH_ERR_STREAM_DIRECT:    // -11
		return "invalid direct parameters";
	case VSSH_ERR_STREAM_EOS:   // -12
		return "unexpected end of stream";
	case VSSH_ERR_STREAM_CAVLC: // -13
		return "invalid CAVLC data";
	case VSSH_ERR_STREAM_REF_FRAME_NO:  // -14
		return "invalid ref. frame number";
	case VSSH_ERR_STREAM_CBP:   // -15
		return "invalid CBP";
	case VSSH_ERR_STREAM_SUBDIV_TYPE:   // -16
		return "invalid subdivision type";
	case VSSH_ERR_STREAM_SPS:   // -17
		return "invalid SPS";
	case VSSH_ERR_STREAM_MB_TYPE:   // -18
		return "invalid macroblock type";
	case VSSH_ERR_STREAM_SUBDIV_8X8_TYPE:   // -19
		return "invalid subdivision 8x8 type";
	case VSSH_ERR_STREAM_SLICE_BEFORE_SPS_OR_PPS:   // -20
		return "invalid SPS or PPS reference";
	case VSSH_ERR_STREAM_WRONG_PPS_ID:  // -21
		return "wrong (non-existing) PPS id";
	case VSSH_ERR_PPS_FMO_6_PARAM:  // -22
		return "wrong FMO paramater value in PPS";
	case VSSH_ERR_WRONG_REORDER_CODE:   // -23
		return "wrong reorder mode";
	case VSSH_ERR_PROFILE_NOT_SUPPORTED:    // -24
		return "profile is not supported";
	case VSSH_ERR_NON_REF_IDR_SLICE:    // -25
		return "IDR slice marked as non-ref";
	case VSSH_ERR_STREAM_FU:    // -27
		return "invalid FU (see RFC 3984)";
	case VSSH_ERR_STREAM_EXTRA_MBS: // -28
		return "extra (unexpected) macroblocks detected";
	case VSSH_ERR_STREAM_QP_DELTA:  // -29
		return "wrong QP delta encountered";
	case VSSH_ERR_NOT_SUPPORTED_SLICE_GROUPS:   // -101
		return "slice groups not supported";
	case VSSH_ERR_NOT_SUPPORTED_ADAPTIVE_FRAME_FIELD:   // -102
		return "adaptive frame/field coding not supported";
	case VSSH_ERR_NOT_SUPPORTED_FRAMES_AND_FIELDS_MUX:  // -103
		return "frames and fields mux not supported";
	case VSSH_ERR_NOT_SUPPORTED_SLICE_TYPE: // -104
		return "unsupported slice type (SI or SP)";
	case VSSH_ERR_NOT_SUPPORTED_PIC_TIMING_PIC_STRUCT:  // -106
		return "not supported pic_timing structure";
	}
	return "unknown error";
}

#ifdef USE_H264_RTP_FOO
static int rtp_send_callback(void * ctx, char * packet, int packet_size)
{
	/* We are encapsulating these rtp packets in iax packets. To this
	 * end, we put the packet into the slice set.
	 */
	struct slice_set_t * slice_set = (struct slice_set_t *)ctx;

	memcpy(slice_set->data[slice_set->num_slices], packet, packet_size);
	slice_set->size[slice_set->num_slices] = packet_size;
	slice_set->num_slices += 1;

	return 0;
}
#else
static int append_slice(struct slice_set_t * slice_set, byte * src, int srclen)
{
	if ( srclen + START_CODE_LEN > MAX_TRUNK_LEN )
	{
		fprintf(stderr, "h264_vss encoder: failed to append slice\n");
		return -1;
	}

	memcpy(slice_set->data[slice_set->num_slices],
			START_CODE, START_CODE_LEN);

	memcpy(slice_set->data[slice_set->num_slices] + START_CODE_LEN,
			src, srclen);

	slice_set->size[slice_set->num_slices] = srclen + START_CODE_LEN;
	slice_set->num_slices += 1;

	return 0;
}
#endif

static yuv_frame_t * compose_yuv_frame(struct iaxc_video_codec * c, int inlen,
		char * in)
{
	struct encoder_t * encoder;
        yuv_frame_t * input_frame;
	int y_size;
	int i;
	byte * src;
	byte * dst;

	encoder = (struct encoder_t *)c->encstate;

	/* If no input frame can be obtained it means that the encoder buffers
	 * are full, so we need to sleep for a while here to yield control to
	 * other threads.
	 */
	while ( !(input_frame = vssh_enc_get_free_frame(encoder->handle)) )
		iaxc_millisleep(5);

#ifdef USE_H264_RTP_FOO
	/* Set the input frame timestamp. This timestamp eventually makes its
	 * way into the rtp headers for the NALU(s) generated for this frame.
	 * vss_gettime() gives us the time in microseconds, we have to scale
	 * it to units of 10MHz for vss_rtp_rtpts() so that we end up with a
	 * timestamp with 90kHz units which is what rtp uses.
	 */
	input_frame->info.timestamp = vss_rtp_rtpts(encoder->rtp_context,
			10 * (vss_gettime() - encoder->start_time));
#endif

	/* Calculate Y channel size */
	y_size = c->width * c->height;

	/* Fill input frame */

	/* Y channel */
	src = (byte *)in;
	dst = input_frame->y;
	for ( i = 0; i < c->height; i++ )
	{
		memcpy(dst, src, c->width);
		src += c->width;
		dst += input_frame->width;
	}

	/* U channel */
	src = (byte *)(in + y_size);
	dst = input_frame->u;
	for ( i = 0; i < c->height / 2; i++ )
	{
		memcpy(dst, src, c->width / 2);
		src += c->width / 2;
		dst += input_frame->width / 2;
	}

	/* V channel */
	src = (byte *)(in + y_size + y_size / 4);
	dst = input_frame->v;
	for ( i = 0; i < c->height / 2; i++ )
	{
		memcpy(dst, src, c->width / 2);
		src += c->width / 2;
		dst += input_frame->width/2;
	}

	return input_frame;
}

static int encode(struct iaxc_video_codec *c, int inlen, char *in, struct slice_set_t *slice_set)
{
	struct encoder_t * encoder;
        yuv_frame_t * input_frame;
	int sps_done = 0;

	// Sanity checks
	if ( !c || !c->encstate || in == NULL || slice_set == NULL )
		return -1;

	encoder = (struct encoder_t *)c->encstate;

	input_frame = compose_yuv_frame(c, inlen, in);

	// Pass the frame to the encoder
	vssh_enc_set_frame(encoder->handle, input_frame, NULL);

#ifdef USE_H264_RTP_FOO
	/* The rtp_send_callback() function gets this as its ctx argument */
	encoder->rtp_context->network_context = slice_set;
#endif

	slice_set->num_slices = 0;
	slice_set->key_frame = 0;

	while ( 1 )
	{
		vssh_slice_data_t slice;
#ifdef USE_H264_RTP_FOO
		vss_uint encoded_ts;
		int actual_size;
#endif
		int ret;

#ifdef USE_H264_RTP_FOO
		slice.slice_data = encoder->output_buffer;
		slice.slice_size = encoder->output_buffer_size;

		ret = vssh_enc_encode_packet(encoder->handle, &slice,
				0, /* single NAL packetization mode */
				c->fragsize,
				&actual_size);
#else
		ret = vssh_enc_get_slice(encoder->handle, &slice);
#endif

		if ( ret != VSSH_OK )
			break;

#ifdef USE_H264_RTP_FOO
		encoded_ts = (vss_uint)slice.frame_info.timestamp;
#endif

		// If the slice is the first slice in an IDR (key) frame, add the
		// SPS and PPS headers to allow for decoder (re)initialziation
		// This way, a client can connect in the middle of a stream, as soon as
		// an IDR frame arrives
		if ( !sps_done && slice.frame_info.idr_flag )
		{
			vssh_spps_data_t spps;

			if ( vssh_enc_get_sps(encoder->handle, &spps) != VSSH_OK )
			{
				fprintf(stderr,
					"h264_vss encoder: failed getting sps\n");
				return -1;
			}

#ifdef USE_H264_RTP_FOO
			vss_rtp_send_packet(encoder->rtp_context,
					spps.spps_data, spps.spps_size,
					0, encoded_ts, 0, 0);
#else
			if ( append_slice(slice_set, spps.spps_data, spps.spps_size) < 0 )
				return -1;
#endif

			if ( vssh_enc_get_pps(encoder->handle, &spps) != VSSH_OK )
			{
				fprintf(stderr,
					"h264_vss encoder: failed getting pps\n");
				return -1;
			}

#ifdef USE_H264_RTP_FOO
			vss_rtp_send_packet(encoder->rtp_context,
					spps.spps_data, spps.spps_size,
					0, encoded_ts, 0, 0);
#else
			if ( append_slice(slice_set, spps.spps_data, spps.spps_size) < 0 )
				return -1;
#endif

			sps_done = 1;
			slice_set->key_frame = 1;
		}

#ifdef USE_H264_RTP_FOO
		vss_rtp_send_packet(encoder->rtp_context,
				slice.slice_data, actual_size,
				0, encoded_ts, 0, 0);
#else
		if ( append_slice(slice_set, slice.slice_data, slice.slice_size) < 0 )
			return -1;
#endif
	}

	return 0;
}

static int decode(struct iaxc_video_codec *c, int inlen, char *in, int *outlen, char *out)
{
	struct decoder_t * decoder;
	yuv_frame_t frame;
	int ret;
#ifdef USE_H264_RTP_FOO
	rtp_hdr_t * hdr;
	vss_byte * payload;
	int payload_size;
#endif

	// Sanity checks
	if ( !c || !c->decstate || in == NULL || inlen <= 0 || out == NULL || outlen == NULL )
		return -1;

	decoder = (struct decoder_t *)c->decstate;

	if ( decoder->handle == NULL )
		return -1;

#ifdef USE_H264_RTP_FOO
	ret = vss_rtp_parse_packet(decoder->rtp_context, (unsigned char *)in,
			inlen, &hdr, &payload);

	if ( ret < 0 )
	{
		fprintf(stderr, "h264_vss decoder: failed to parse rtp packet\n");
		return 1;
	}

	payload_size = inlen - (payload - (vss_byte *)in);

	if ( !payload )
		vssh_dec_flush(decoder->handle, 1);
	else
		vssh_dec_feed_data_ex(decoder->handle, payload, payload_size,
				1, /* is nal unit */
				hdr->ts);
#else
	// Feed the encoded data to the decoder
	if ( vssh_dec_feed_data(decoder->handle, (byte *)in, inlen, 0) != VSSH_OK )
	{
		fprintf(stderr, "h264_vss decoder: failed to feed decoder\n");
		return 1;
	}
#endif

	// Check for a completely decoded frame
	ret = vssh_dec_decode_frame(decoder->handle, &frame);

	if ( ret > 0 )
	{
		/* Valid decoded frame returned */
		int y_size;
		int frame_size;

		y_size = frame.width * frame.height;
		frame_size = y_size * 3 / 2;

		if ( frame_size > *outlen )
		{
			fprintf(stderr,
				"h264_vss decoder: decoded frame bigger than output buffer\n");
			*outlen = 0;
			return -1;
		}

		/* Copy frame to output buffer */
		memcpy(out, frame.y, y_size);
		out += y_size;
		memcpy(out, frame.u, y_size/4);
		out += y_size / 4;
		memcpy(out, frame.v, y_size/4);

		*outlen = frame_size;

		return 0;
	}
	else if ( ret < 0 )
	{
		if ( ret == VSSH_ERR_STREAM_SLICE_BEFORE_SPS_OR_PPS )
		{
			fprintf(stderr,
				"h264_vss decoder: stream slice received when waiting for SPS\n");
		}
		else
		{
			fprintf(stderr,
				"h264_vss decoder: failed to decode frame\n");
			return -1;
		}
	}

	/* Decoded frame is not yet available */
	return 1;
}

static void destroy(struct iaxc_video_codec *c)
{
	struct encoder_t	*encoder;
	struct decoder_t	*decoder;

	if ( c == NULL ) return;

	if ( c->encstate != NULL )
	{
		encoder = (struct encoder_t *)c->encstate;
		vssh_enc_close(encoder->handle);
#ifdef USE_H264_RTP_FOO
		vss_rtp_close(encoder->rtp_context);
		free(encoder->output_buffer);
#endif
		free(encoder);
	}
	if ( c->decstate != NULL )
	{
		decoder = (struct decoder_t *)c->decstate;
		vssh_dec_close(decoder->handle);
#ifdef USE_H264_RTP_FOO
		vss_rtp_close(decoder->rtp_context);
#endif
		free(decoder);
	}
	free(c);
}

static void prepareEncoderSettings(vssh_encoder_settings_t *settings, int width, int height, int framerate, int bitrate, int fragsize)
{
	// zero all structure fields
	memset(settings, 0, sizeof(*settings));
	vssh_enc_default_settings(settings);

	// disable MultiThreading
	settings->mt_settings.disable = 1;

	// disable SNR calculation
	settings->calc_snr = 0;

	// enable auto configuration
	settings->auto_config = 1;

	// describe input (source)
	settings->user_settings.input.material = 0; // material: 0=progressive, 1=interlaced;
	settings->user_settings.input.noisy = 0;    // noise level [0..64];
	settings->frame_width  = width;
	settings->frame_height = height;

	// describe desired output
	settings->user_settings.output.bitrate = bitrate;        // desired bitrate, kbps;
	settings->user_settings.output.detect = 1;               // scene change detection: 0/1;
	settings->user_settings.output.interval = framerate * 1; // key frame interval: [0..300]
	settings->user_settings.output.target = 1;               // target application: 0=playback, 1=streaming;

	// codec operational parameters
	settings->user_settings.codec.multipass = 0;    // multipass: 0=single pass, 1=first pass, 2=second pass;
	settings->user_settings.codec.quality = 0;    // codec quality: 0=realtime, 1=fast, 2=good, 3=best;

	// provide frame rate
	settings->gop_settings.num_units  = 10000;
	settings->gop_settings.time_scale = framerate * settings->gop_settings.num_units;

	// slice settings
	// 0=off; 1=#mb in slice; 2=#bytes in slice; 3 =#slices in picture
	settings->slice_settings.slice_mode = 2;
	settings->slice_settings.slice_param = fragsize;
}

struct iaxc_video_codec *iaxc_video_codec_h264_new(int format, int w, int h, int framerate, int bitrate, int fragsize)
{
	struct iaxc_video_codec	*c = NULL;
	struct encoder_t	*encoder = NULL;
	struct decoder_t	*decoder = NULL;
	int			ret;

	// Sanity check
	// TODO: better sanity checks here
	if ( w <= 0 || h <= 0 || framerate <= 0 || bitrate <= 0 || fragsize <= 0 )
	{
		fprintf(stderr, "Could not initialize H.264 codec: sanity checks failed\n");
		return NULL;
	}
	// Width and height must be multiples of 16
	if ( w % 16 || h % 16 )
	{
		fprintf(stderr, "Could not initialize H.264 codec: width and height must be multiples of 16\n");
		return NULL;
	}
	if ( fragsize > MAX_SLICE_SIZE )
		fragsize = MAX_SLICE_SIZE;

	c = calloc(sizeof(struct iaxc_video_codec), 1);
	encoder = calloc(sizeof(struct encoder_t), 1);
	decoder = calloc(sizeof(struct decoder_t), 1);
	if ( !c || !encoder || !decoder )
	{
		free(encoder);
		free(decoder);
		free(c);
		fprintf(stderr, "Could not allocate memory for the H.264 codec\n");
		return NULL;
	}

	// Set up the encoder
	bitrate = bitrate / 1000;	// Codec requires bitrate to be in kbps
	prepareEncoderSettings(&(encoder->settings), w, h, framerate, bitrate, fragsize);
	ret = vssh_enc_open(&(encoder->handle), &(encoder->settings));
	if ( ret != VSSH_OK )
	{
		fprintf(stderr,
			"Could not initialize H.264 encoder: description = %s\n",
			get_error_text(ret));
		return NULL;
	}

#ifdef USE_H264_RTP_FOO
	encoder->rtp_context = vss_rtp_open(
			PAYLOAD_TYPE_H264, fragsize,
			w, h,
			rtp_send_callback, 0);

	if ( !encoder->rtp_context )
	{
		fprintf(stderr, "h264_vss encoder: failed to open rtp context\n");
		return 0;
	}

	encoder->start_time = vss_gettime();

	encoder->output_buffer_size = w * h * 3 / 2;
	encoder->output_buffer = (unsigned char *)
		malloc(encoder->output_buffer_size);

	if ( !encoder->output_buffer )
	{
		fprintf(stderr, "h264_vss encoder: failed to allocate output buffer\n");
		return 0;
	}
#endif

	// Set up the decoder
	ret = vssh_dec_open_ex(&(decoder->handle), &(decoder->settings));
	if ( ret != VSSH_OK || decoder->handle == NULL )
	{
		fprintf(stderr, "Could not initialize H.264 decoder: description = %s",
				get_error_text(ret));
		return NULL;
	}

#ifdef USE_H264_RTP_FOO
	decoder->rtp_context = vss_rtp_open2(PAYLOAD_TYPE_H264,
			MAX_SLICE_SIZE, /* Maximum rtp packet payload size */
			w, h,
			16,         /* Maximum frames in rtp receiving queue */
			256 * 1024, /* Maximum frame size in receiving queue */
			1);         /* Delete not ready frames */

	if ( !decoder->rtp_context )
	{
		fprintf(stderr, "h264_vss decoder: failed to open rtp context\n");
		return 0;
	}
#endif

	c->width = w;
	c->height = h;
	c->framerate = framerate;
	c->bitrate = bitrate;
	c->fragsize = fragsize;

	// Set up the iaxclient codec structure
	c->encstate = encoder;
	c->decstate = decoder;
	c->encode = encode;
	c->decode = decode;
	c->destroy = destroy;
	c->format = IAXC_FORMAT_H264;
	sprintf(c->name, "Vanguard Software Solutions H.264 codec");

	return c;
}
