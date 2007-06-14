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


struct iaxc_video_codec *codec_video_ffmpeg_new(int format, int w, int h, int framerate, int bitrate, int fragsize);

int codec_video_ffmpeg_check_codec(int format);
