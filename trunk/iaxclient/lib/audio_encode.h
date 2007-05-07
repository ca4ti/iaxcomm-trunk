/*
 * iaxclient: a cross-platform IAX softphone library
 *
 * Copyrights:
 * Copyright (C) 2003 HorizonLive.com, (c) 2004, Horizon Wimba, Inc.
 *
 * Contributors:
 * Steve Kann <stevek@stevek.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License
 */
#ifndef _AUDIO_ENCODE_H
#define _AUDIO_ENCODE_H

#include <stdio.h>
#include "iaxclient_lib.h"

struct iax_event;

extern int iaxc_filters;

int send_encoded_audio(struct iaxc_call *most_recent_answer, int callNo, void *data, int iEncodeType, int samples);
int decode_audio(struct iaxc_call *p, void *out, void *data, int len, int iEncodeType, int *samples);
void iaxc_set_speex_filters(void);

#endif

