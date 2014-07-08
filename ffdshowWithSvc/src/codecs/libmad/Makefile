CFLAGS+= -I. -I../.. -DFPM_INTEL -D_LIB -DHAVE_CONFIG_H -DASO_ZEROCHECK
       
SRCS_C=bit.c decoder.c fixed.c frame.c huffman.c layer12.c layer3.c stream.c \
       synth.c timer.c version.c

SLIB=../../../bin/ff_libmad.dll
DEF=ff_libmad.def

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
