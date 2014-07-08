CFLAGS+= -I. -DSTDC_HEADERS
       
SRCS_C=samplerate.c src_linear.c src_sinc.c src_zoh.c

SLIB=../../../../bin/ff_samplerate.dll
DEF=ff_samplerate.def

include ../../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
