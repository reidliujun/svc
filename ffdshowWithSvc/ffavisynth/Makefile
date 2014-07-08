CFLAGS+= -I. -I../src -I../src/cygwin

SRCS=ffavisynth.cpp

SLIB=../bin/ffavisynth.dll
DEF=ffavisynth.def

GCCLIBS=-lstdc++ -lole32
CLLIBS=ole32.lib

GCCDLLENTRY=--entry=_DllEntryPoint@12
CLDLLENTRY=/entry:"DllEntryPoint@12"

include ../src/makefile_c.inc

CFLAGS+=-UUNICODE -U_UNICODE

GCCDEF=ffavisynth.def.gcc

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
