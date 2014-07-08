CFLAGS+= -I. -I.. -I../.. -I../../xiph -Idec -Ienc -DUSE_ASM

SRCS_C=\
  cpu.c \
  comment.c \
	huffman.c \
	idct.c \
	misc_common.c \
	toplevel.c \
	dec/decode.c \
	dec/quant.c \
	enc/blockmap.c \
	enc/dct.c \
  enc/dct_decode.c \
	enc/dct_encode.c \
	enc/dsp.c \
	enc/encode.c \
	enc/encoder_toplevel.c \
	enc/frarray.c \
	enc/frinit.c \
	enc/mcomp.c \
	enc/pb.c \
	enc/pp.c \
	enc/reconstruct.c \
	enc/scan.c

SRCS_C+=\
	version.c
	
SRCS_C+=\
	../../xiph/ogg/bitwise.c \
	../../xiph/ogg/framing.c

ifeq ($(64BIT),yes)
SRCS_C+=\
	enc/x86_64/recon_mmx.c \
	enc/x86_64/dsp_mmx.c \
	enc/x86_64/dsp_mmxext.c \
	enc/x86_64/fdct_mmx.c \
	enc/x86_64/idct_mmx.c \
	enc/x86_64/dct_decode_mmx.c
else
SRCS_C+=\
	enc/x86_32/recon_mmx.c \
	enc/x86_32/dsp_mmx.c \
	enc/x86_32/dsp_mmxext.c \
	enc/x86_32/fdct_mmx.c \
	enc/x86_32/idct_mmx.c \
	enc/x86_32/dct_decode_mmx.c
endif


SLIB=../../../bin/ff_theora.dll

DEF=ff_theora.def

include ../../makefile_c.inc

.all: $(SLIB)

clean:
	rm -f $(SLIB) *.o *.d dec/*.o dec/*.d enc/*.o enc/*.d enc/x86_32/*.o enc/x86_32/*.d enc/x86_64/*.o *.d enc/x86_64/*.d ../../xiph/ogg/*.o ../../xiph/ogg/*.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
