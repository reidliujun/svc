CFLAGS+= -I. -I../src -I../include

SRCS=combase.cpp ffvdub.cpp Tffvdub.cpp TpropertyPageSite.cpp base64.cpp

SRCS_C=bzip2/blocksort.c bzip2/bzlib.c  bzip2/compress.c bzip2/crctable.c \
       bzip2/decompress.c bzip2/huffman.c bzip2/randtable.c

SLIB=../../bin/ffvdub.vdf
DEF=ffvdub.def

GCCLIBS=-lstdc++ -lole32 -lgdi32 -lcomdlg32 -lwinmm -lcomctl32 -lole32 -luuid
CLLIBS=ole32.lib gdi32.lib comdlg32.lib winmm.lib comctl32.lib uuid.lib user32.lib

RESSRC=ffvdub.rc

include ../../src/makefile_c.inc

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) bzip2/*.o *.d bzip2/*.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
