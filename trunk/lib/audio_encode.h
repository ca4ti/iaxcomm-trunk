/*
 * iaxclient: a cross-platform IAX softphone library
 *
 * Copyrights:
 * Copyright (C) 2003-2006, Horizon Wimba, Inc.
 * Copyright (C) 2007, Wimba, Inc.
 *
 * Contributors:
 * Steve Kann <stevek@stevek.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License.
 */

#ifndef _AUDIO_ENCODE_H
#define _AUDIO_ENCODE_H

extern int iaxci_sample_rate;

/* Minimum dB possible in the iaxclient world. This level
 * is intended to represent silence.
 */
#define AUDIO_ENCODE_SILENCE_DB -99.0f

/* AAGC threshold */
#define AAGC_VERY_HOT 16
#define AAGC_HOT      8
#define AAGC_COLD     4

/* AAGC increments */
#define AAGC_RISE_SLOW 0.10f
#define AAGC_DROP_SLOW 0.15f
#define AAGC_DROP_FAST 0.20f

struct iaxc_call;
struct iax_event;

int audio_initialize();
int audio_destroy();

int audio_send_encoded_audio(struct iaxc_call * most_recent_answer, int callNo,
		void * data, int iEncodeType, int samples);

int audio_decode_audio(struct iaxc_call * p, void * out, void * data, int len,
		int iEncodeType, int * samples);

#endif

