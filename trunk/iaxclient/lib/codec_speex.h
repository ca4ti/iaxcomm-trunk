/*
 * iaxclient: a portable telephony toolkit
 *
 * Copyright (C) 2003-2004, Horizon Wimba, Inc.
 *
 * Steve Kann <stevek@stevek.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License
 */

struct iaxc_speex_settings {
	int decode_enhance;
	float quality;
	int bitrate;
	int vbr;
	int abr; /* abr bitrate */
	int complexity;
};

struct iaxc_audio_codec *iaxc_audio_codec_speex_new(struct iaxc_speex_settings *settings);
