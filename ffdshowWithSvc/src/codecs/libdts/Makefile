CFLAGS+= -I. -I../..
       
SRCS_C=bitstream.c downmix.c parse.c

SLIB=../../../bin/ff_libdts.dll
DEF=ff_libdts.def

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
