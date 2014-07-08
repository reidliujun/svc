SRCS=ffdshow_muxers_all.cpp

include ../makefile.inc
 
.all: $(OBJS)

clean:	
	$(RM) $(TEMPFILES) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
