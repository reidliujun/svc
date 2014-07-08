CFLAGS+= -I. -I../..

ifneq ($(64BIT),yes)
	CFLAGS+=-DARCH_IS_IA32 -DARCH_IS_32BIT
else
	CFLAGS+=-DARCH_IS_GENERIC -DARCH_IS_64BIT
endif

SRCS_C=decoder.c \
		encoder.c \
		xvid.c \
		bitstream/bitstream.c \
		bitstream/cbp.c \
		bitstream/mbcoding.c \
		dct/fdct.c \
		dct/idct.c \
		dct/simple_idct.c \
		image/colorspace.c \
		image/image.c \
		image/interpolate8x8.c \
		image/font.c \
		image/postprocessing.c \
		image/qpel.c \
		image/reduced.c \
		motion/estimation_bvop.c \
		motion/estimation_common.c \
		motion/estimation_gmc.c \
		motion/estimation_pvop.c \
		motion/estimation_rd_based.c \
		motion/estimation_rd_based_bvop.c \
		motion/gmc.c \
		motion/motion_comp.c \
		motion/vop_type_decision.c \
		motion/sad.c \
		prediction/mbprediction.c \
		plugins/plugin_single.c \
		plugins/plugin_2pass1.c \
		plugins/plugin_2pass2.c \
		plugins/plugin_lumimasking.c \
		plugins/plugin_dump.c \
		plugins/plugin_psnr.c \
		plugins/plugin_ssim.c \
		quant/quant_h263.c \
		quant/quant_matrix.c \
		quant/quant_mpeg.c \
		utils/emms.c \
		utils/mbtransquant.c \
		utils/mem_align.c \
		utils/mem_transfer.c \
		utils/timer.c

ifneq ($(64BIT),yes)
SRCS_YASM=bitstream/x86_asm/cbp_3dne.asm \
		bitstream/x86_asm/cbp_mmx.asm \
		bitstream/x86_asm/cbp_sse2.asm \
		dct/x86_asm/fdct_mmx_ffmpeg.asm \
		dct/x86_asm/fdct_mmx_skal.asm \
		dct/x86_asm/fdct_sse2_skal.asm \
		dct/x86_asm/idct_3dne.asm \
		dct/x86_asm/idct_mmx.asm \
		dct/x86_asm/idct_sse2_dmitry.asm \
		dct/x86_asm/simple_idct_mmx.asm \
		image/x86_asm/colorspace_rgb_mmx.asm \
		image/x86_asm/colorspace_yuv_mmx.asm \
		image/x86_asm/colorspace_yuyv_mmx.asm \
		image/x86_asm/interpolate8x8_3dn.asm \
		image/x86_asm/interpolate8x8_3dne.asm \
		image/x86_asm/interpolate8x8_mmx.asm \
		image/x86_asm/interpolate8x8_xmm.asm \
		image/x86_asm/postprocessing_mmx.asm \
		image/x86_asm/postprocessing_sse2.asm \
		image/x86_asm/reduced_mmx.asm \
		image/x86_asm/qpel_mmx.asm \
		image/x86_asm/gmc_mmx.asm \
		image/x86_asm/deintl_sse.asm \
		motion/x86_asm/sad_xmm.asm \
		motion/x86_asm/sad_sse2.asm \
		motion/x86_asm/sad_mmx.asm \
		motion/x86_asm/sad_3dne.asm \
		motion/x86_asm/sad_3dn.asm \
		quant/x86_asm/quantize_h263_mmx.asm \
		quant/x86_asm/quantize_h263_3dne.asm \
		quant/x86_asm/quantize_mpeg_xmm.asm \
		quant/x86_asm/quantize_mpeg_mmx.asm \
		utils/x86_asm/mem_transfer_mmx.asm \
		utils/x86_asm/mem_transfer_3dne.asm \
		utils/x86_asm/interlacing_mmx.asm \
		utils/x86_asm/cpuid.asm \
		plugins/x86_asm/plugin_ssim-a.asm
endif

SLIB=../../../bin/xvidcore.dll
DEF=xvid.def

include ../../makefile_c.inc

.all: $(SLIB)

clean:
	rm -f *.o bitstream/*.o bitstream/x86_asm/*.o dct/*.o dct/x86_asm/*.o dct/x86_64_asm*.o \
		image/*.o image/x86_asm/*.o image/x86_64_asm/*.o motion/*.o motion/x86_asm/*.o motion/x86_64_asm/*.o \
		plugins/*.o plugins/x86_asm/*.o prediction/*.o quant/*.o quant/x86_asm/*.o quant/x86_64_asm/*.o \
		utils/*.o utils/x86_asm/*.o utils/x86_64_asm/*.o \
		*~ $(SLIB) *.d bitstream/*.d dct/*.d image/*.d motion/*.d plugins/*.d prediction/*.d quant/*.d utils/*.d

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
