SRCS=TaudioCodecTremor.cpp TvideoCodecTheora.cpp TaudioCodecLibavcodec.cpp TaudioCodec.cpp TvideoCodecLibavcodec.cpp ffdshow_codecs_all.cpp


# enable SSE because compilation fails without it
SSE=yes

include ../makefile.inc

.all:    $(OBJS)

clean:	
	$(RM) $(TEMPFILES)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
