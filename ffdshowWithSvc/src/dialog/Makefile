SRCS=ffdshow_dialog_all.cpp TSpecifyPropertyPagesVE.cpp

include ../makefile.inc

.all: $(OBJS)

clean:
	$(RM) $(TEMPFILES)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
