ifeq ($(64BIT),yes)
    TARGET_OS=x86_64-pc-mingw32
else
    TARGET_OS=i686-pc-mingw32
endif

LIBSONAME=../bin/ffdshow.ax

SRCS=ffdshow_all.cpp TffDecoder_reg.cpp Tconfig.cpp

SRCS_C=ffdebug.c

SRCS_YASM=cpuid.asm memcpy.asm subtitles/fontRenderer.asm convert/colorspace_rgb_mmx.asm

include makefile.inc

ALLDLLS=LIBMPEG2 LIBMAD LIBFAAD2 THEORA X264 XVIDCORE LIBDTS SAMPLERATE TREMOR WMV9 \
        REALAAC TOMSMOCOMP KDINT UNRAR VFW ACM FFAVS LIBA52 FFVDUB DSCALER MAKEAVIS \
        FFMPEG LIBMP VERINC REBASE

ALLS=../bin/libmpeg2_ff.dll \
     ../bin/ff_libmad.dll \
     ../bin/ff_libfaad2.dll \
     ../bin/ff_theora.dll \
     ../bin/ff_x264.dll \
     ../bin/ff_xvidcore.dll \
     ../bin/ff_liba52.dll \
     ../bin/ff_libdts.dll \
     ../bin/ff_samplerate.dll \
     ../bin/ff_tremor.dll \
     ../bin/ff_wmv9.dll \
     ../bin/ff_realaac.dll \
     ../bin/TomsMoComp_ff.dll \
     ../bin/ff_kernelDeint.dll \
     ../bin/ff_unrar.dll \
     ../bin/ff_vfw.dll \
     ../bin/ff_acm.acm \
     ../bin/libmplayer.dll \
     ../bin/libavcodec.dll \
     ../bin/ffvdub.vdf \
     ../bin/makeAVIS.exe \
     ../bin/ffavisynth.dll \
     ../bin/FLT_ffdshow.dll \
     ../bin/verinc.exe \
     ../bin/rebase.exe

.all: lib $(ALLDLLS) $(LIBSONAME)

LIBS=acm/Tacm.o \
     audioFilters/ffdshow_audiofilters_all.o \
     codecs/ffdshow_codecs_all.o \
     codecs/TaudioCodecTremor.o \
     codecs/TvideoCodecTheora.o \
     codecs/TaudioCodecLibavcodec.o \
     codecs/TaudioCodec.o \
     codecs/TvideoCodecLibavcodec.o \
     convert/ffdshow_convert_all.o \
     dialog/ffdshow_dialog_all.o dialog/TSpecifyPropertyPagesVE.o \
     filters/ffdshow_filters_all.o \
     imgFilters/ffdshow_imgfilters_all.o \
     muxers/ffdshow_muxers_all.o xiph/ogg/bitwise.o xiph/ogg/framing.o \
     settings/ffdshow_settings_all.o settings/lzo/minilzo.o \
     subtitles/ffdshow_subtitles_all.o \
     baseclasses/baseclasses_all.o \
     ffmpeg/libavcodec/golomb.o \
     audioFilters/TaudioFilterConvert.o audioFilters/dotproduct.o \
     imgFilters/simple_idct_mmx.o imgFilters/Skal/skl_dct_sse.o \
     imgFilters/Skal/skl_fdct_mmx.o

lib:    
	$(MAKE) -C baseclasses
	$(MAKE) -C acm
	$(MAKE) -C audioFilters
	$(MAKE) -C convert
	$(MAKE) -C dialog
	$(MAKE) -C imgFilters
	$(MAKE) -C codecs
	$(MAKE) -C settings
	$(MAKE) -C subtitles
	$(MAKE) -C muxers
	$(MAKE) -C filters
	
../bin/libavcodec.dll:
	$(MAKE) -C ffmpeg
FFMPEG:
	$(MAKE) -C ffmpeg

../bin/libmplayer.dll:
	$(MAKE) -C mplayer
LIBMP:
	$(MAKE) -C mplayer

../bin/libmpeg2_ff.dll:
	$(MAKE) -C codecs/libmpeg2
LIBMPEG2:
	$(MAKE) -C codecs/libmpeg2

../bin/ff_libmad.dll:
	$(MAKE) -C codecs/libmad
LIBMAD:
	$(MAKE) -C codecs/libmad

../bin/ff_libfaad2.dll:
	$(MAKE) -C codecs/faad
LIBFAAD2:
	$(MAKE) -C codecs/faad

../bin/ff_theora.dll:
	$(MAKE) -C codecs/theora
THEORA:
	$(MAKE) -C codecs/theora

../bin/ff_x264.dll:
	$(MAKE) -C codecs/x264
X264:
	$(MAKE) -C codecs/x264

../bin/ff_xvidcore.dll:
	$(MAKE) -C codecs/xvidcore
XVIDCORE:
	$(MAKE) -C codecs/xvidcore

../bin/ff_liba52.dll:
	$(MAKE) -C codecs/liba52
LIBA52:
	$(MAKE) -C codecs/liba52

../bin/ff_libdts.dll:
	$(MAKE) -C codecs/libdts
LIBDTS:
	$(MAKE) -C codecs/libdts

../bin/ff_samplerate.dll:
	$(MAKE) -C audioFilters/resample/libsamplerate
SAMPLERATE:
	$(MAKE) -C audioFilters/resample/libsamplerate

../bin/ff_tremor.dll:
	$(MAKE) -C codecs/tremor
TREMOR:
	$(MAKE) -C codecs/tremor

../bin/ff_wmv9.dll:
	$(MAKE) -C codecs/wmv9
WMV9:
	$(MAKE) -C codecs/wmv9

../bin/ff_realaac.dll:
	$(MAKE) -C codecs/realaac
REALAAC:
	$(MAKE) -C codecs/realaac

../bin/TomsMoComp_ff.dll:
	$(MAKE) -C imgFilters/TomsMoComp
TOMSMOCOMP:
	$(MAKE) -C imgFilters/TomsMoComp

../bin/ff_kernelDeint.dll:
	$(MAKE) -C imgFilters/KernelDeint
KDINT:
	$(MAKE) -C imgFilters/KernelDeint

../bin/ff_unrar.dll:
	$(MAKE) -C ../unrar
UNRAR:
	$(MAKE) -C ../unrar

../bin/ff_vfw.dll:
	$(MAKE) -C ../ffvfw
VFW:
	$(MAKE) -C ../ffvfw

../bin/ff_acm.acm:
	$(MAKE) -C ../ffacm
ACM:
	$(MAKE) -C ../ffacm

../bin/ffavisynth.dll:
	$(MAKE) -C ../ffavisynth
FFAVS:
	$(MAKE) -C ../ffavisynth

../bin/ffvdub.vdf:
	$(MAKE) -C ../ffvdub/src
FFVDUB:
	$(MAKE) -C ../ffvdub/src

../bin/FLT_ffdshow.dll:
	$(MAKE) -C ../dscaler
DSCALER:
	$(MAKE) -C ../dscaler

../bin/makeAVIS.exe:
	$(MAKE) -C ../makeAVIS
MAKEAVIS:
	$(MAKE) -C ../makeAVIS

../bin/verinc.exe:
	$(MAKE) -C ../verinc
	../bin/verinc.exe version.ver
VERINC:
	$(MAKE) -C ../verinc
	../bin/verinc.exe version.ver

../bin/rebase.exe:
	$(MAKE) -C ../rebase
REBASE:
	$(MAKE) -C ../rebase

DO_REBASE=../bin/rebase.exe ../bin/rebase.txt

RESSRC=ffdshow.rc 

ffdshow.res.o:../bin/verinc.exe 


$(LIBSONAME):../bin/verinc.exe $(OBJS) $(LIBS) $(RESSRC:.rc=.res.o) ffdshow.def.gcc
	$(FFDSHOW_PREFIX)dllwrap -mno-cygwin --target=$(TARGET_OS) \
                -o $(LIBSONAME) -def ffdshow.def.gcc \
                -Wl,--strip-all,--enable-stdcall-fixup \
    $(OBJS) $(LIBS) ffdshow.res.o \
                -lmsvcrt -lstdc++ -lsupc++ \
                -lgdi32 -lcomdlg32 -lwinmm -lcomctl32 -luuid -lole32 -loleaut32 \
                -ldinput -ldxguid -lshlwapi
	$(DO_REBASE)

clean:	
	$(RM) $(TEMPFILES) $(LIBSONAME) mplayer/Tlibmplayer.o ffmpeg/Tlibavcodec.o xiph/ogg/bitwise.o xiph/ogg/framing.o xiph/ogg/bitwise.d xiph/ogg/framing.d
	$(MAKE) -C baseclasses clean
	$(MAKE) -C acm clean
	$(MAKE) -C audioFilters clean
	$(MAKE) -C convert clean
	$(MAKE) -C dialog clean
	$(MAKE) -C imgFilters clean
	$(MAKE) -C codecs clean
	$(MAKE) -C settings clean
	$(MAKE) -C subtitles clean
	$(MAKE) -C muxers clean
	$(MAKE) -C filters clean
	$(MAKE) -C ffmpeg clean
	$(MAKE) -C mplayer clean
	$(MAKE) -C codecs/libmpeg2 clean
	$(MAKE) -C codecs/libmad clean
	$(MAKE) -C codecs/faad clean
	$(MAKE) -C codecs/theora clean
	$(MAKE) -C codecs/x264 clean
	$(MAKE) -C codecs/xvidcore clean
	$(MAKE) -C codecs/liba52 clean
	$(MAKE) -C codecs/libdts clean
	$(MAKE) -C audioFilters/resample/libsamplerate clean
	$(MAKE) -C codecs/tremor clean
	$(MAKE) -C codecs/wmv9 clean
	$(MAKE) -C codecs/realaac clean
	$(MAKE) -C imgFilters/TomsMoComp clean
	$(MAKE) -C imgFilters/KernelDeint clean
	$(MAKE) -C ../unrar clean
	$(MAKE) -C ../ffvfw clean
	$(MAKE) -C ../ffacm clean
	$(MAKE) -C ../ffavisynth clean
	$(MAKE) -C ../ffvdub/src clean
	$(MAKE) -C ../dscaler clean
	$(MAKE) -C ../makeAVIS clean
	$(MAKE) -C ../verinc clean

distclean:
	$(RM) $(TEMPFILES) $(LIBSONAME) mplayer/Tlibmplayer.o ffmpeg/Tlibavcodec.o xiph/ogg/bitwise.o xiph/ogg/framing.o xiph/ogg/bitwise.d xiph/ogg/framing.d
	$(MAKE) -C baseclasses distclean
	$(MAKE) -C acm distclean
	$(MAKE) -C audioFilters distclean
	$(MAKE) -C convert distclean
	$(MAKE) -C dialog distclean
	$(MAKE) -C imgFilters distclean
	$(MAKE) -C codecs distclean
	$(MAKE) -C settings distclean
	$(MAKE) -C subtitles distclean
	$(MAKE) -C muxers distclean
	$(MAKE) -C filters distclean
	$(MAKE) -C ffmpeg distclean
	$(MAKE) -C mplayer distclean
	$(MAKE) -C codecs/libmpeg2 distclean
	$(MAKE) -C codecs/libmad distclean
	$(MAKE) -C codecs/faad distclean
	$(MAKE) -C codecs/theora distclean
	$(MAKE) -C codecs/x264 distclean
	$(MAKE) -C codecs/xvidcore distclean
	$(MAKE) -C codecs/liba52 distclean
	$(MAKE) -C codecs/libdts distclean
	$(MAKE) -C audioFilters/resample/libsamplerate distclean
	$(MAKE) -C codecs/tremor distclean
	$(MAKE) -C codecs/wmv9 distclean
	$(MAKE) -C codecs/realaac distclean
	$(MAKE) -C imgFilters/TomsMoComp distclean
	$(MAKE) -C imgFilters/KernelDeint distclean
	$(MAKE) -C ../unrar distclean
	$(MAKE) -C ../ffvfw distclean
	$(MAKE) -C ../ffacm distclean
	$(MAKE) -C ../ffavisynth distclean
	$(MAKE) -C ../ffvdub/src distclean
	$(MAKE) -C ../dscaler distclean
	$(MAKE) -C ../makeAVIS distclean
	$(MAKE) -C ../verinc distclean

.PHONY: lib $(ALLDLLS)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
