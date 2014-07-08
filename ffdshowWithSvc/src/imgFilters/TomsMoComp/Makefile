CFLAGS+= -I. -I../..
       
SRCS=TomsMoComp.cpp

SLIB=../../../bin/TomsMoComp_ff.dll
DEF=TomsMoComp_ff.def

GCCLIBS=-lstdc++

# enable SSE because compilation fails without it
SSE=yes

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
