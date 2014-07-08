CFLAGS+= -I. -Iinclude -I../..

ifneq ($(64BIT),yes)
	CFLAGS+=-DARCH_X86
endif

#CFLAGS+=-fverbose-asm -save-temps

SRCS_C= libmpeg2/alloc.c libmpeg2/decode.c libmpeg2/header.c \
		libmpeg2/idct.c libmpeg2/idct_mmx.c libmpeg2/motion_comp.c \
		libmpeg2/motion_comp_mmx.c libmpeg2/slice.c libmpeg2/cpu_accel.c libmpeg2/cpu_state.c\
		version.c

SLIB=../../../bin/libmpeg2_ff.dll
DEF=libmpeg2_ff.def

include ../../makefile_c.inc

.all: $(SLIB)

clean:
	rm -f *.o libmpeg2/*.o *~ $(SLIB) *.d libmpeg2/*.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
