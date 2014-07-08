CFLAGS+=-I.

SRCS=rebase.cpp

SEXE=../bin/rebase.exe

GCCLIBS=-limagehlp
CLLIBS=imagehlp.lib

include ../src/makefile_c.inc

.all: $(SEXE)

clean:
	rm -f *.o *~ $(SEXE) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
