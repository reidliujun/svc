CFLAGS+= -I. -I../.. -DUSE_DEFAULT_STDLIB -DHELIX_FEATURE_AUDIO_CODEC_AAC_SBR

ifeq ($(64BIT),yes)
    CFLAGS+=-D__amd64__
else
    CFLAGS+=-D__i386__
endif

SRCS_C=aacdec.c aactabs.c bitstream.c buffers.c dct4.c decelmnt.c dequant.c \
       fft.c filefmt.c huffman.c hufftabs.c imdct.c noiseless.c pns.c sbr.c \
       sbrfft.c sbrfreq.c sbrhfadj.c sbrhfgen.c sbrhuff.c sbrimdct.c sbrmath.c \
       sbrqmf.c sbrside.c sbrtabs.c stproc.c tns.c trigtabs.c

SLIB=../../../bin/ff_realaac.dll
DEF=ff_realaac.def

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
