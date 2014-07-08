CFLAGS+= -I. -I../..

SRCS_C=bit_allocate.c bitstream.c downmix.c imdct.c parse.c

SLIB=../../../bin/ff_liba52.dll
DEF=ff_liba52.def

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
