SRCS=ffdshow_settings_all.cpp

SRCS_C=lzo/minilzo.c

# enable SSE because compilation fails without it
SSE=yes

include ../makefile.inc

.all: $(OBJS)

clean:	
	$(RM) $(TEMPFILES) filters/*.o lzo/minilzo.o filters/*.d lzo/minilzo.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
