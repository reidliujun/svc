CFLAGS+= -I. -I../.. -DSTDC_HEADERS

SRCS_C=bits.c cfft.c common.c decoder.c drc.c error.c filtbank.c hcr.c huffman.c \
      ic_predict.c is.c lt_predict.c mdct.c mp4.c ms.c output.c pns.c ps_dec.c \
      ps_syntax.c pulse.c rvlc.c sbr_dct.c sbr_dec.c sbr_e_nf.c sbr_fbt.c \
      sbr_hfadj.c sbr_hfgen.c sbr_huff.c sbr_qmf.c sbr_syntax.c sbr_tf_grid.c \
      specrec.c ssr.c ssr_fb.c ssr_ipqf.c syntax.c tns.c drm_dec.c version.c

SLIB=../../../bin/ff_libfaad2.dll
DEF=ff_libfaad2.def

include ../../makefile_c.inc

.all: $(SLIB)

clean: 
	rm -f *.o *~ $(SLIB) *.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
