all:.all

#64BIT=yes
#DEBUG=yes

#if FFDSHOW_PREFIX is set, assume cross-compilation
ifeq ($(FFDSHOW_PREFIX),)
    YASM=yasm.exe
else
    YASM=yasm
endif

ifeq ($(64BIT),yes)
    FFDSHOW_PREFIX=x86_64-pc-mingw32-
endif

OUTPUTFLAG=-MMD -o\

CFLAGS+=-mno-cygwin -mdll -fno-rtti -mthreads -pipe -D_WINGDI_ -DUCLIBCPP -D_GLIBCPP_HAVE_MBSTATE_T -D_WIN32_IE=0x0500
CFLAGS+=-DHAVE_MMX

ifeq ($(64BIT),yes)
    CFLAGS+=-DWIN64 -D_WIN64
else
    CFLAGS+=-DWIN32 -D_WIN32
endif

ifeq ($(64BIT),yes)
	CFLAGS+=-DARCH_IS_GENERIC -DARCH_IS_64BIT
else
	CFLAGS+=-DARCH_IS_IA32 -DARCH_IS_32BIT
endif

ifneq ($(64BIT),yes)
CFLAGS+=-mmmx
endif

ifeq ($(SSE),yes)
    CFLAGS+=-msse -mfpmath=sse
endif
ifeq ($(SSE2),yes)
    CFLAGS+=-msse2
endif

ifeq ($(DEBUG), yes)
    CFLAGS+=-DDEBUG -D_DEBUG -g
    OPTFLAGS=-O2 -fomit-frame-pointer
    YASMFLAGS+=-g cv8
else
    CFLAGS+=-DNDEBUG -UDEBUG -DFFDEBUG=0
    OPTFLAGS=-O3 -fomit-frame-pointer
endif
#CFLAGS+=-w

ifeq ($(64BIT),yes)
    OPTFLAGS+=-m64
else
    OPTFLAGS+=-march=i686
endif

ifeq ($(UNICODE),yes)
  CFLAGS+=-DUNICODE -D_UNICODE
endif

CFLAGS+=-I. -I.. -Iuclibc++ -Ibaseclasses -I../baseclasses \
        -IimgFilters -I../imgFilters -Implayer -I../mplayer -Isettings -I../settings \
        -Isettings/filters -I../settings/filters -Icodecs -I../codecs \
        -Isubtitles -I../subtitles -Iconvert -I../convert -Idialog -I../dialog \
        -IaudioFilters -I../audioFilters -Icygwin -I../cygwin -Iffmpeg -I../ffmpeg \
        -Iacm -I../acm -Ifilters -I../filters -Imuxers -I../muxers -I/dx/Include -L/dx/MingLib -ldx9

OBJS=$(SRCS:.cpp=.o) $(SRCS_C:.c=.o) $(SRCS_YASM:.asm=.o) $(RESSRC:.rc:.res.o)

.c.o:
	$(FFDSHOW_PREFIX)$(CC) -c $(CFLAGS) $(OPTFLAGS) $(OUTPUTFLAG)$@ $<

.cpp.o:
	$(FFDSHOW_PREFIX)$(CC) -c $(CFLAGS) $(OPTFLAGS) $(OUTPUTFLAG)$@ $<

%.o: %.asm
ifeq ($(64BIT),yes)
	$(YASM) -f win64 -DWIN64 -DARCH_X86_64 -DPREFIX $(YASMFLAGS) -I$(<D)/ -o $@ $<
else
	$(YASM) -f win32 -DWIN32 -DARCH_X86_32 -DPREFIX $(YASMFLAGS) -I$(<D)/ -o $@ $<
endif

%.res.o: %.rc
	$(FFDSHOW_PREFIX)windres $< -o $@ -D_WIN32_IE=0x0500 -DIDCLOSE=8

$(LIBNAME): $(OBJS)
	$(FFDSHOW_PREFIX)ar rcs $@ $^

.distclean: clean

PHONY distclean: .distclean

TEMPFILES=*.i *.ii *.d *.s *.o
