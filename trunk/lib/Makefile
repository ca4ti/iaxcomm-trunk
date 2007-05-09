#
# iaxclient: a cross-platform IAX softphone library
#
# Copyrights:
# Copyright (C) 2003 HorizonLive.com, (c) 2004, Horizon Wimba, Inc.
#
# Contributors <see README for complete list>
# Steve Kann <stevek@stevek.com>
# Michael Van Donselaar <mvand@vandonselaar.org>
# Shawn Lawrence <shawn.lawrence@terracecomm.com>
#
#
# This program is free software, distributed under the terms of
# the GNU Lesser (Library) General Public License
#

PRODUCTION_BUILD ?= 0

CODEC_ILBC=0

SPEEX_PREPROCESS=1
SPAN_EC=0
SPEEX_EC=1
MEC2_EC=0

#Enable only ONE of the following
#Enable this if using ffmpeg
USE_FFMPEG=0
# Enable this if using Vanguard Softare Solutions H.264 codec
USE_H264_VSS=0

# For Linux, you can choose between portaudio with OSS, Alsa and Jack backend,
# and native Alsa
# Choice is inclusive
USE_PA_OSS=1
USE_PA_ALSA=1
USE_PA_JACK=0
AUDIO_ALSA=0

CPPFLAGS += -I. \
	-Igsm/inc \
	-Iportaudio/include \
	-Iportaudio/src/common \
	-Iportaudio/pablio \
	-Iportmixer/px_common

CFLAGS += -Wall -Wmissing-noreturn

ifeq ($(PRODUCTION_BUILD),1)
CFLAGS += -O2 -Wpointer-arith
else
CFLAGS += -g -O0
endif

CPPFLAGS += -DLIBVER='"SVN $(shell svnversion .)"'

ifeq ($(SPEEX_PREPROCESS),1)
CPPFLAGS += -DSPEEX_PREPROCESS=1
endif

# debug flags
# CPPFLAGS += -DDEBUG_SUPPORT -DDEBUG_DEFAULT -DEXTREME_DEBUG -D_DEBUG
# CPPFLAGS += -DDEBUG_SUPPORT -DDEBUG_DEFAULT -D_DEBUG
# enable this for API level debug info for PortAudio
# CPPFLAGS += -DPA_LOG_API_CALLS

OBJS=\
	portaudio/src/common/pa_allocation.o \
	portaudio/src/common/pa_converters.o \
	portaudio/src/common/pa_cpuload.o \
	portaudio/src/common/pa_dither.o \
	portaudio/src/common/pa_front.o \
	portaudio/src/common/pa_process.o \
	portaudio/src/common/pa_skeleton.o \
	portaudio/src/common/pa_stream.o \
	portaudio/src/common/pa_trace.o \
	spandsp/plc.o \
	audio_encode.o \
	audio_portaudio.o \
	codec_gsm.o \
	codec_ulaw.o \
	codec_alaw.o \
	codec_speex.o \
	iaxclient_lib.o \
	video.o \
	videoLib/video_grab.o \
	codec_theora.o

OBJS_GSM=\
	gsm/src/add.o \
	gsm/src/code.o \
	gsm/src/debug.o \
	gsm/src/decode.o \
	gsm/src/gsm_create.o \
	gsm/src/gsm_decode.o \
	gsm/src/gsm_destroy.o \
	gsm/src/gsm_encode.o \
	gsm/src/gsm_explode.o \
	gsm/src/gsm_implode.o \
	gsm/src/gsm_option.o \
	gsm/src/gsm_print.o \
	gsm/src/long_term.o \
	gsm/src/lpc.o \
	gsm/src/preprocess.o \
	gsm/src/rpe.o \
	gsm/src/short_term.o \
	gsm/src/table.o

OBJS_ILBC=\
	iLBC/anaFilter.o\
	iLBC/iCBSearch.o\
	iLBC/packing.o\
	iLBC/constants.o\
	iLBC/gainquant.o\
	iLBC/iLBC_decode.o\
	iLBC/StateConstructW.o \
	iLBC/createCB.o\
	iLBC/getCBvec.o\
	iLBC/iLBC_encode.o\
	iLBC/StateSearchW.o\
	iLBC/doCPLC.o\
	iLBC/helpfun.o\
	iLBC/syntFilter.o\
	iLBC/enhancer.o\
	iLBC/hpInput.o\
	iLBC/LPCdecode.o\
	iLBC/filter.o\
	iLBC/hpOutput.o\
	iLBC/LPCencode.o\
	iLBC/FrameClassify.o\
	iLBC/iCBConstruct.o\
	iLBC/lsf.o

OBJS:= $(OBJS) $(OBJS_GSM)

ifeq ($(CODEC_ILBC),1)
OBJS:= $(OBJS) $(OBJS_ILBC) codec_ilbc.o
CPPFLAGS += -DCODEC_ILBC
endif

OBJS_WIN32=\
	winfuncs.o \
	portmixer/px_win_wmme/px_win_wmme.o \
	portaudio/src/hostapi/wmme/pa_win_wmme.o \
	portaudio/src/os/win/pa_win_hostapis.o \
	portaudio/src/os/win/pa_win_util.o \
	portaudio/pablio/ringbuffer.o

OBJS_LIBFG = \
	videoLib/linux/linuxgrab.o \
	videoLib/linux/libfg/capture.o \
	videoLib/linux/libfg/frame.o

OBJS_LINUX=\
	unixfuncs.o \
	portmixer/px_unix_oss/px_unix_oss.o \
	portaudio/src/os/unix/pa_unix_util.o \
	portaudio/src/os/unix/pa_unix_hostapis.o \
	portaudio/pablio/ringbuffer.o \
	$(OBJS_LIBFG)

OBJS_SOLARIS=\
	unixfuncs.o \
	portmixer/px_solaris/px_solaris.o \
	portaudio/pa_unix_oss/pa_unix_solaris.o \
	portaudio/pa_unix_oss/pa_unix.o

OBJS_MACOSX=\
	unixfuncs.o \
	portmixer/px_mac_core/px_mac_core.o \
	portaudio/src/hostapi/coreaudio/pa_mac_core.o \
	portaudio/src/hostapi/coreaudio/pa_mac_core_utilities.o \
	portaudio/src/hostapi/coreaudio/pa_mac_core_blocking.o \
	portaudio/src/os/unix/pa_unix_util.o \
	portaudio/src/os/mac_osx/pa_mac_hostapis.o \
	portaudio/src/hostapi/coreaudio/ringbuffer.o \
	videoLib/macosx/SSUtility.o \
	videoLib/macosx/vdigGrab.o \
	videoLib/macosx/macgrab.o

OBJS_IAX2=\
	libiax2/src/md5.o \
	libiax2/src/iax.o \
	libiax2/src/iax2-parser.o \
	libiax2/src/jitterbuf.o


LIB=libiaxclient.a

RANLIB= echo

all: $(LIB)

# glean the platform
# We will define this as either Linux, Win32, SOLARIS, or MacOSX
ifneq (,$(findstring Linux,$(shell uname)))
OSTYPE=LINUX
else
ifneq (,$(findstring Darwin,$(shell uname)))
OSTYPE=MACOSX
else
# CYGWIN reports CYGWIN_NT-5.0 under Win2K
ifneq (,$(findstring WIN,$(shell uname)))
OSTYPE=WIN32
else
ifneq (,$(findstring MINGW,$(shell uname)))
OSTYPE=WIN32
else
ifneq (,$(findstring SunOS,$(shell uname)))
OSTYPE=SOLARIS
else
$(warning OSTYPE cannot be detected, assuming Linux)
OSTYPE=LINUX
endif
endif
endif
endif
endif

# Setup platform-specific stuff based on gleaning
ifeq ($(OSTYPE),WIN32)
    OBJS += $(OBJS_WIN32)
    CPPFLAGS += -DPA_NO_DS -DPA_NO_ASIO \
                -IPortVideoSDL/win32/dslib/include \
                -Ic:/DX90SDK/include-gcc \
                -Ic:/DX90SDK/Samples/C++/DirectShow/BaseClasses
    CFLAGS += -mno-cygwin
    DYNLDFLAGS = -mno-cygwin -lwinmm -lwsock32 -lcomdlg32 -luser32 -lgdi32 \
                 -lole32 -lcomctl32 -lctl3d32 -lstdc++ -lshell32 -loleaut32 \
                 -ladvapi32 -luuid \
                 -Wl,--output-def,iaxclient.def,--add-stdcall-alias,--enable-stdcall-fixup,--out-implib,$(LIB)
    DYNCFLAGS = -DBUILDING_DLL
    DYNLIB = iaxclient.dll
endif

ifeq ($(OSTYPE),LINUX)
    CPPFLAGS += -Iportaudio/src/os/unix -IvideoLib/linux/libfg
    DYNCFLAGS = -fPIC
    DYNLIB = libiaxclient.so
    ifeq ($(USE_PA_OSS),1)
        OBJS_LINUX += portaudio/src/hostapi/oss/pa_unix_oss.o
        CPPFLAGS += -DPA_USE_OSS
    endif
    ifeq ($(USE_PA_ALSA),1)
        OBJS_LINUX += portaudio/src/hostapi/alsa/pa_linux_alsa.o
        CPPFLAGS += -DPA_USE_ALSA
    endif
    ifeq ($(USE_PA_JACK),1)
        OBJS_LINUX += portaudio/src/hostapi/jack/pa_jack.o
        CPPFLAGS += -DPA_USE_JACK
    endif
    OBJS += $(OBJS_LINUX)
endif

ifeq ($(OSTYPE),SOLARIS)
    OBJS += $(OBJS_SOLARIS)
    # only SPARC solaris machines need the alignment stuff
    ifeq (sparc,$(findstring sparc,$(shell uname -a)))
        CPPFLAGS += -DALIGN32
    endif
    DYNCFLAGS = -fPIC
    DYNLIB = libiaxclient.so
    CC = gcc
endif

ifeq ($(OSTYPE),MACOSX)
    OBJS += $(OBJS_MACOSX)

    RANLIB = ranlib
    CPPFLAGS += -DPA_USE_COREAUDIO=1 -Iportaudio/src/hostapi/coreaudio \
                -I../../theora/include -I/sw/include

    ifeq ($(PRODUCTION_BUILD),1)
        CFLAGS += -arch i386 -arch ppc
    else
        # gcc will only generate dependencies with these options when not
        # using multiple -arch options (as used above).
        CFLAGS += -MP -MD
    endif

    export MACOSX_DEPLOYMENT_TARGET=10.3
    export NEXT_ROOT=/Developer/SDKs/MacOSX10.4u.sdk

    DYNLIB = libiaxclient.dylib

    # Uncomment these lines in to build against version 10.2.7.
    #export MACOSX_DEPLOYMENT_TARGET=10.2
    #export NEXT_ROOT=/Developer/SDKs/MacOSX10.2.7.sdk
endif

ifneq ($(OSTYPE),MACOSX)
    CFLAGS += -MP -MD
endif

ifeq ($(USE_FFMPEG),1)
    OBJS += codec_ffmpeg.o
    CPPFLAGS += -DUSE_FFMPEG
endif

ifeq ($(USE_H264_VSS),1)
    OBJS += codec_h264_vss.o
    CPPFLAGS += -Ivssh_sdk/include -DUSE_H264_VSS
endif

ifeq ($(AUDIO_ALSA),1)
    OBJS += audio_alsa.o
    CPPFLAGS += -DAUDIO_ALSA
endif

OBJS += $(OBJS_IAX2)
#CPPFLAGS += -Ilibiax2/src -DIAXC_IAX2 -DLIBIAX -DDEBUG_SUPPORT
CPPFLAGS += -Ilibiax2/src -DLIBIAX

ifeq ($(SPAN_EC),1)
    OBJS += ec/echo.o
    CPPFLAGS += -DSPAN_EC=1
endif

ifeq ($(SPEEX_EC),1)
    CPPFLAGS += -DSPEEX_EC=1
endif

ifeq ($(MEC2_EC),1)
    CPPFLAGS += -DUSE_MEC2=1
endif

CPPFLAGS += -D$(OSTYPE)

CXXFLAGS = $(CFLAGS)

CFLAGS += -std=gnu99

# Treat some included libraries specially:
# compile gsm w/o warnings, because it was written to less strict warning systems.
gsm/%.o: gsm/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -w $< -o $@

# These objects are kinda wimpy and so we use the less awesome gnu89
# instead of the more awesome default c99.
$(OBJS_LIBFG) \
portaudio/src/hostapi/jack/pa_jack.o \
portaudio/src/hostapi/alsa/pa_linux_alsa.o \
    : CFLAGS += --std=gnu89

$(LIB): $(OBJS)
	$(RM) $(LIB)
	ar cru $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

#	libtool -static -o $(LIB) $(OBJS) portaudio/lib/libportaudio.a
#	ar cru $(LIB) $(OBJS)


MSLIB=iaxclient.lib
MGLIBDIR=c:/mingw/lib
GCCLIBDIR=$(MGLIBDIR)/gcc-lib/mingw32/3.2/

GCCLIB=$(GCCLIBDIR)/libgcc.a

$(MSLIB): $(OBJS)
#	rm -f $(MSLIB)
	cp $(GCCLIB) $(MSLIB)
	ar rs $(MSLIB) $(OBJS)


#shared: CFLAGS := $(CFLAGS) $(DYNCFLAGS)
#shared: clean libiaxclient.a
#shared: libiaxclient.a
#ifeq ($(OSTYPE),MACOSX)
#	$(CC) -single_module -dylib -dynamiclib -o $(DYNLIB) $(OBJS) $(DYNLDFLAGS) -framework CoreAudio -framework AudioToolbox -framework System -framework Carbon -framework AudioUnit portaudio/lib/libportaudio.a
#else
#	$(CC) -shared -o $(DYNLIB) $(OBJS) $(DYNLDFLAGS)
#endif

plat:
	echo Platform is $(OSTYPE)
	echo OBJS are $(OBJS)
	echo OBJS_LINUX are $(OBJS_LINUX)

clean:
	rm -f $(OBJS) $(LIB) $(addsuffix .d,$(basename $(OBJS)))

-include *.d
