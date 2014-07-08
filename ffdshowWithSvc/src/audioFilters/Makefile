SRCS=ffdshow_audiofilters_all.cpp

ifneq ($(CC),icl)
SRCS_YASM=TaudioFilterConvert.asm dotproduct.asm
endif

# enable SSE because compilation fails without it
SSE=yes

include ../makefile.inc

.all: $(OBJS)

clean:	
	$(RM) $(TEMPFILES)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
