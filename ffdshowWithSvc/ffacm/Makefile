CFLAGS+= -I. -I../src -I../src/cygwin

SRCS=driverproc.cpp

SRCS_C=../src/ffdebug.c

SLIB=../bin/ff_acm.acm
DEF=ff_acm.def

GCCLIBS=-lole32 -lwinmm -lcomctl32 -ladvapi32 -lstdc++
CLLIBS=ole32.lib winmm.lib comctl32.lib advapi32.lib

RESSRC=ff_acm.rc

include ../src/makefile_c.inc

CFLAGS+=-UUNICODE -U_UNICODE

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
