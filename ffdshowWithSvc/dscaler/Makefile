CFLAGS+= -I. -I../src -I/dx/Include -L/dx/MingLib -ldx9

SRCS=FLT_ffdshow.cpp

SLIB=../bin/FLT_ffdshow.dll
DEF=FLT_ffdshow.def

GCCLIBS=-lole32 -lstdc++
CLLIBS=ole32.lib

include ../src/makefile_c.inc

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
