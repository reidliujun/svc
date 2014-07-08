
### 1. About ffdshow video encoder ###

VFW (Video for Windows) and DirectShow codec. Uses the libavcodec library from the FFmpeg project for encoding. A few other encoding libraries, such as xvidcore and libtheora, are also supported.

### 2. Features: ###

- Various compression methods:
H.264
MPEG-4 compatible (Xvid, DivX) using libavcodec or Xvid
Divx 3 compatible, MSMPEG4v2, MSMPEG4v1
WMV1/7, WMV2/8
H263, H263+
HuffYUV in YV12 colorspace
MJPEG
MPEG 1/2
Theora (not for regular use, format specification hasn't stabilized yet)

- All common encoding modes: constant bitrate, fixed quantizer, fixed quality, two pass encoding (depends on compressor features).
- For libavcodec and Xvid detailed selection of motion estimation method.
- Minimum and maximum I frames interval.
- Minimum and maximum quantizer selection, quantizer type and trellis, quantization selection for libavcodec.
- Adaptive quantization (aka. masking) for libavcodec and Xvid, single coefficient elimination.
- Selectable compression of starting and ending credits.
- Two curve compression algorithms for second pass of two pass encoding thanks to Xvid developers.
- Second pass simulation: although not very precise, still helpful for tweaking advanced curve compression parameters.
- Ability to use libavcodec internal two pass code.
- Image preprocessing with ffdshow image filters.
- Graph during encoding: if your encoding program doesn't provide one.
- B frames support: from one to eight consecutive B frames.
- Support for MPEG-4 quarterpel and GMC.
- Selectable interlaced encoding.
- Libavcodec's MPEG4/MPEG2/MPEG1, xvidcore 1.2 and x264 encoders are multithreaded.
- Libavcodec encoders pop up error messages.
- Decompression.

### 3. ffdshow-tryouts ###

ffdshow was originally developed by Milan Cutka. Since Milan Cutka stopped updating in 2006, we launched a new project called 'ffdshow-tryouts'.

### 4. Web links ###

ffdshow-tryouts:
http://ffdshow-tryout.sourceforge.net/
http://sourceforge.net/project/showfiles.php?group_id=173941

ffdshow (old):
http://ffdshow.sourceforge.net/tikiwiki/
http://sourceforge.net/projects/ffdshow/

FFmpeg:
http://ffmpeg.mplayerhq.hu/

Xvid:
http://www.xvid.org/

MPlayer:
http://www.mplayerhq.hu/

MJPEGtools:
http://mjpeg.sourceforge.net/

Doom9:
http://forum.doom9.org/showthread.php?t=120465

or Wikipedia, the free encyclopedia:
http://en.wikipedia.org/wiki/Ffdshow

### 5. Copying ###

ffdshow is distributed under GPL. See copying.txt