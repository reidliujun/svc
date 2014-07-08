CFLAGS+= -I. -I../src -I../src/cygwin

SRCS=makeAVIS.cpp
RESSRC=makeAVIS.rc
EXCEPTIONS=1

SEXE=../bin/makeAVIS.exe

GCCLIBS=-lstdc++ -lvfw32 -lcomdlg32
CLLIBS=vfw32.lib comdlg32.lib user32.lib advapi32.lib

include ../src/makefile_c.inc

.all: $(SEXE)

clean:
	rm -f *.o *~ $(SEXE) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
