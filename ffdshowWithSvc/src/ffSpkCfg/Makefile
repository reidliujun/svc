CFLAGS+=-I. -w

SRCS=ffSpkCfg.cpp

SLIB=../../bin/ffSpkCfg.dll
DEF=ffSpkCfg.def

GCCLIBS=-ldsound
CLLIBS=dsound.lib

NOINTRIN=1

include ../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o $(SLIB) *~ *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
