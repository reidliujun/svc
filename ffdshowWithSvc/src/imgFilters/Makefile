#CFLAGS+=-Wall
#CFLAGS+=-save-temps -fverbose-asm -masm=intel

SRCS=ffdshow_imgfilters_all.cpp

ifneq ($(CC),icl)
SRCS_YASM=skal/skl_dct_sse.asm skal/skl_fdct_mmx.asm simple_idct_mmx.asm

YASMFLAGS=-I../ffmpeg/libavcodec/i386/ -DWIN32 -DUSE32 -Dalloc -Dexec
endif

# enable SSE because compilation fails without it
SSE=yes

include ../makefile.inc

.all: $(OBJS)

clean:	
	$(RM) $(TEMPFILES) asharp/asharp.o avisynth/*.o

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
