USE_FFMPEG=0

CPPFLAGS = -I../../lib

# Uncomment one set of flags below
# Deployment flags
#CFLAGS += -O2 -Wall
# Debug & Profile flags
CFLAGS += -g -Wall

CFLAGS += `sdl-config --cflags`

OBJS = vtestcall.o

EXEEXT =

# glean the platform
# We will define this as either Linux, Win32, or MacOSX
ifneq (,$(findstring Linux,$(shell uname)))
OSTYPE=LINUX
CFLAGS:= $(CFLAGS) -DLINUX
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
ifneq (,$(findstring FreeBSD,$(shell uname)))
OSTYPEREAL=FREEBSD
else
$(warning OSTYPE cannot be detected, assuming Linux)
endif
OSTYPE=LINUX
endif
endif
endif
endif
endif

LDLIBS = -L../../lib -liaxclient

ifeq ($(OSTYPE),LINUX)
 CPPFLAGS += -DPOSIXSLEEP
 LDLIBS += `sdl-config --libs` -logg -lspeex -ltheora
endif

ifeq ($(OSTYPE),MACOSX)
 CPPFLAGS += -DPOSIXSLEEP
 LDLIBS += -framework QuickTime \
           -framework Carbon \
	   -framework AudioUnit \
	   -framework CoreAudio \
	   -framework AudioToolbox \
           `sdl-config --libs` \
	   -logg -lspeex -ltheora
SYSLIBS=
endif

ifeq ($(USE_FFMPEG),1)
 CPPFLAGS += -DUSE_FFMPEG
endif

ifeq ($(OSTYPEREAL),FREEBSD)
MAKE=gmake
else
MAKE=make
endif

ifeq ($(OSTYPE),WIN32)
EXEEXT = .exe
LDLIBS += -lwinmm -lwsock32
endif

ifeq ($(OSTYPE),LINUX)
ifeq ($(OSTYPEREAL),FREEBSD)
LDLIBS += -lc_r -lm
else
LDLIBS += -lpthread -lm -lasound
endif
endif

ifeq ($(OSTYPE),SOLARIS)
CPPFLAGS += -DPOSIXSLEEP
LDLIBS += -lsocket -lnsl -lpthread -lrt -lresolv -lm
MAKE = gmake
endif

PROG = vtestcall$(EXEEXT)

CC = $(CXX)

all: makelib $(PROG)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(PROG)

.PHONY: distclean
distclean: clean
	$(MAKE) -C ../../lib clean

.PHONY: makelib
makelib:
	$(MAKE) -C ../../lib

$(PROG): $(OBJS)

