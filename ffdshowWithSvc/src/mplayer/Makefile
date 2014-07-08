CFLAGS+=-I. -I.. -I../imgFilters -I../ffmpeg/libavutil -Ilibvo -Imp3lib -Ipostproc -std=gnu99

SRCS_C=\
		mplayer.c \
		../ffmpeg/libavutil/mem.c \
		postproc/rgb2rgb.c \
		postproc/yuv2rgb.c \
		postproc/swscale.c \
		postproc/postprocess.c \
		postproc/w32thread.c \
		libaf/reorder_ch.c \
		libvo/aclib.c \
		yadif/vf_yadif.c \
		yadif/yadif_thread.c \
		mp3lib/sr1.c
		
ifneq ($(64BIT),yes)
SRCS_C+=\
		mp3lib/decode_MMX.c \
		mp3lib/dct64_MMX.c \
		mp3lib/tabinit_MMX.c \
		mp3lib/dct64_3dnow.c \
		mp3lib/dct64_sse.c \
		mp3lib/dct64_k7.c \
		mp3lib/dct36_3dnow.c \
		mp3lib/dct36_k7.c
endif

ifneq ($(64BIT),yes)
    CFLAGS+=-DARCH_X86_32
else
    CFLAGS+=-DHAVE_FAST_64BIT
endif

ifeq ($(FFDEBUG),yes)
    SRCS_C+= ../ffdebug.c 
    CFLAGS+= -DFFDEBUG=1
else
    CFLAGS+= -DFFDEBUG=0
endif

SLIB=../../bin/libmplayer.dll
DEF=libmplayer.def

NOINTRIN=1

include ../makefile_c.inc

.all: $(SLIB)

clean:
	rm -f *.o *.d postproc/*.o postproc/*.d yadif/*.o yadif/*.d libaf/*.o libaf/*.d libvo/*.o libvo/*.d mp3lib/*.o mp3lib/*.d ../ffmpeg/libavutil/mem.o $(SLIB)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
