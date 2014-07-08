/*
 * libmad - MPEG audio decoder library (C) 2000-2001 Robert Leslie
 * based on code from libmad - MPEG audio decoder library (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "mp3header.h"
#include "libavcodec/bitstream.h"

const unsigned long mp3header::bitrate_table[5][15] =
{
  /* MPEG-1 */
  { 0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,  /* Layer I   */
       256000, 288000, 320000, 352000, 384000, 416000, 448000 },
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer II  */
       128000, 160000, 192000, 224000, 256000, 320000, 384000 },
  { 0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,  /* Layer III */
       112000, 128000, 160000, 192000, 224000, 256000, 320000 },

  /* MPEG-2 LSF */
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer I   */
       128000, 144000, 160000, 176000, 192000, 224000, 256000 },
  { 0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,  /* Layers    */
        64000,  80000,  96000, 112000, 128000, 144000, 160000 } /* II & III  */
};
const unsigned int mp3header::samplerate_table[3] =
{
 44100, 48000, 32000
};
unsigned int mp3header::findNearestFreq(unsigned int f)
{
 unsigned int mindiff=_UI32_MAX,dstf=0;
 for (int i=0;i<countof(samplerate_table);i++)
  for (int d=0;d<=2;d++)
   {
    unsigned int diff=ff_abs(int(samplerate_table[i]>>d)-int(f));
    if (diff<mindiff)
     {
      mindiff=diff;
      dstf=samplerate_table[i]>>d;
      if (mindiff==0) goto end;
     }
   }
end:
 return dstf?dstf:f;
}

mp3header::mp3header(const unsigned char *buf,unsigned int buflen)
{
  ok=false;
  GetBitContext stream;init_get_bits(&stream,buf,buflen*8);

  unsigned int index;

  flags        = 0;
  private_bits = 0;

  /* header() */

  /* syncword */
  if (!(buf[0] == 0xff && (buf[1] & 0xe0) == 0xe0))
   return;
  skip_bits(&stream, 11);

  /* MPEG 2.5 indicator (really part of syncword) */
  if (get_bits(&stream, 1) == 0)
    flags |= MAD_FLAG_MPEG_2_5_EXT;

  /* ID */
  if (get_bits(&stream, 1) == 0)
    flags |= MAD_FLAG_LSF_EXT;
  else if (flags & MAD_FLAG_MPEG_2_5_EXT) {
    //&stream->error = MAD_ERROR_LOSTSYNC;
    return;
  }

  /* layer */
  layer = mad_layer(4 - get_bits(&stream, 2));

  if (layer == 4) {
    //&stream->error = MAD_ERROR_BADLAYER;
    return;
  }

  /* protection_bit */
  if (get_bits(&stream, 1) == 0) {
    flags    |= MAD_FLAG_PROTECTION;
    //crc_check = mad_bit_crc(&stream->ptr, 16, 0xffff);
  }

  /* bitrate_index */
  index = get_bits(&stream, 4);

  if (index == 15) {
    //&stream->error = MAD_ERROR_BADBITRATE;
    return;
  }

  if (flags & MAD_FLAG_LSF_EXT)
    bitrate = bitrate_table[3 + (layer >> 1)][index];
  else
    bitrate = bitrate_table[layer - 1][index];

  /* sampling_frequency */
  index = get_bits(&stream, 2);

  if (index == 3) {
    //&stream->error = MAD_ERROR_BADSAMPLERATE;
    return;
  }

  samplerate = samplerate_table[index];

  if (flags & MAD_FLAG_LSF_EXT) {
    samplerate /= 2;

    if (flags & MAD_FLAG_MPEG_2_5_EXT)
      samplerate /= 2;
  }

  /* padding_bit */
  if (get_bits(&stream, 1))
    flags |= MAD_FLAG_PADDING;

  /* private_bit */
  if (get_bits(&stream, 1))
    private_bits |= MAD_PRIVATE_HEADER;

  /* mode */
  mode = mad_mode(3 - get_bits(&stream, 2));

  /* mode_extension */
  mode_extension = get_bits(&stream, 2);

  /* copyright */
  if (get_bits(&stream, 1))
    flags |= MAD_FLAG_COPYRIGHT;

  /* original/copy */
  if (get_bits(&stream, 1))
    flags |= MAD_FLAG_ORIGINAL;

  /* emphasis */
  emphasis = mad_emphasis(get_bits(&stream, 2));
/*
  if (emphasis == 2) {
    //&stream->error = MAD_ERROR_BADEMPHASIS;
    return;
  }
*/
  /* error_check() */

  /* crc_check */
  if (flags & MAD_FLAG_PROTECTION)
    crc_target = (unsigned short) get_bits(&stream, 16);

  ok=true;
  return;
}

unsigned int mp3header::calc_frame_len(void)
{
  int pad_slot = (flags & MAD_FLAG_PADDING) ? 1 : 0;
  int N=0;
  if (layer == MAD_LAYER_I)
    N = ((12 * bitrate / samplerate) + pad_slot) * 4;
  else {
    unsigned int slots_per_frame= (layer == MAD_LAYER_III &&
		                  (flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

    N = (slots_per_frame * bitrate / samplerate) + pad_slot;
  }
 return N;
}
