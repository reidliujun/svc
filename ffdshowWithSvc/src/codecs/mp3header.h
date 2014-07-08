#ifndef _MP3HEADER_H_
#define _MP3HEADER_H_

// based on code from libmad - MPEG audio decoder library (C) 2000-2001 Robert Leslie

class mp3header
{
private:
 static const unsigned long bitrate_table[5][15];
 static const unsigned int samplerate_table[3];
public:
 struct mad_timer_t
  {
   signed long seconds;         /* whole seconds */
   unsigned long fraction;      /* 1/MAD_TIMER_RESOLUTION seconds */
  };
 enum mad_layer
  {
   MAD_LAYER_I   = 1,                   /* Layer I */
   MAD_LAYER_II  = 2,                   /* Layer II */
   MAD_LAYER_III = 3                    /* Layer III */
  };
 enum mad_mode
  {
   MAD_MODE_SINGLE_CHANNEL = 0,         /* single channel */
   MAD_MODE_DUAL_CHANNEL   = 1,         /* dual channel */
   MAD_MODE_JOINT_STEREO   = 2,         /* joint (MS/intensity) stereo */
   MAD_MODE_STEREO         = 3          /* normal LR stereo */
  };
 enum mad_emphasis
  {
   MAD_EMPHASIS_NONE       = 0,         /* no emphasis */
   MAD_EMPHASIS_50_15_US   = 1,         /* 50/15 microseconds emphasis */
   MAD_EMPHASIS_CCITT_J_17 = 3          /* CCITT J.17 emphasis */
  };
 enum
  {
   MAD_FLAG_NPRIVATE_III = 0x0007,       /* number of Layer III private bits */
   MAD_FLAG_INCOMPLETE   = 0x0008,       /* header but not data is decoded */

   MAD_FLAG_PROTECTION   = 0x0010,       /* frame has CRC protection */
   MAD_FLAG_COPYRIGHT    = 0x0020,       /* frame is copyright */
   MAD_FLAG_ORIGINAL     = 0x0040,       /* frame is original (else copy) */
   MAD_FLAG_PADDING      = 0x0080,       /* frame has additional slot */

   MAD_FLAG_I_STEREO     = 0x0100,       /* uses intensity joint stereo */
   MAD_FLAG_MS_STEREO    = 0x0200,       /* uses middle/side joint stereo */
   MAD_FLAG_FREEFORMAT   = 0x0400,       /* uses free format bitrate */

   MAD_FLAG_LSF_EXT      = 0x1000,       /* lower sampling freq. extension */
   MAD_FLAG_MC_EXT       = 0x2000,       /* multichannel audio extension */
   MAD_FLAG_MPEG_2_5_EXT = 0x4000        /* MPEG 2.5 (unofficial) extension */
  };
 enum
  {
   MAD_PRIVATE_HEADER   = 0x0100,       /* header private bit */
   MAD_PRIVATE_III      = 0x001f        /* Layer III private bits (up to 5) */
  };

 mad_layer layer;               /* audio layer (1, 2, or 3) */
 mad_mode mode;                 /* channel mode (see above) */
 int mode_extension;            /* additional mode info */
 mad_emphasis emphasis;         /* de-emphasis to use (see above) */

 unsigned long bitrate;         /* stream bitrate (bps) */
 unsigned int samplerate;       /* sampling frequency (Hz) */
 static unsigned int findNearestFreq(unsigned int f);

 unsigned short crc_check;      /* frame CRC accumulator */
 unsigned short crc_target;     /* final target CRC checksum */

 int flags;                     /* flags (see below) */
 int private_bits;              /* private bits (see below) */

 mad_timer_t duration;          /* audio playing time of frame */

 mp3header(const unsigned char *buf,unsigned int buflen);
 bool ok;
 unsigned int calc_frame_len(void);
};

#endif
