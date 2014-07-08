CFLAGS+= -I. -I../.. -DRARDLL -DNOCRYPT -DSILENT -DNOVOLUME -DSFX_MODULE -DSHELL_EXT

SRCS=archive.cpp arcread.cpp cmddata.cpp crc.cpp dll.cpp encname.cpp errhnd.cpp \
     extinfo.cpp extract.cpp filcreat.cpp file.cpp filefn.cpp filestr.cpp \
     find.cpp getbits.cpp int64.cpp isnt.cpp match.cpp options.cpp pathfn.cpp \
     rar.cpp rarvm.cpp rawread.cpp rdwrfn.cpp recvol.cpp rs.cpp savepos.cpp \
     scantree.cpp strfn.cpp strlist.cpp system.cpp timefn.cpp unicode.cpp unpack.cpp

EXCEPTIONS=1

SLIB=../bin/ff_unrar.dll
DEF=dll.def

GCCLIBS=-lstdc++
CLLIBS=user32.lib

include ../src/makefile_c.inc

CFLAGS+=-UUNICODE -U_UNICODE

.all: $(SLIB)

clean:
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
