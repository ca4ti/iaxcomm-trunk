#ifndef __FILE_H__
#define __FILE_H__

#include <ogg/ogg.h>

#define SPEEX_FRAME_DURATION  20
#define SPEEX_SAMPLING_RATE   8000

void load_ogg_file(const char *filename);

ogg_packet * get_next_audio_op();
ogg_packet * get_next_video_op();

int audio_is_eos();
int video_is_eos();

#endif
