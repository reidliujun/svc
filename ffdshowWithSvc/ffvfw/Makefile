CFLAGS+= -I. -I../src

SRCS=driverproc.cpp

SRCS_C=../src/ffdebug.c

SLIB=../bin/ff_vfw.dll
DEF=ff_vfw.def

GCCLIBS=-lole32 -lwinmm -lcomctl32 -lstdc++
CLLIBS=ole32.lib winmm.lib comctl32.lib

include ../src/makefile_c.inc

CFLAGS+=-UUNICODE -U_UNICODE

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
