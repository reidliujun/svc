/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "Tcodec.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"

//=============================== Tcodec ===============================
Tcodec::Tcodec(IffdshowBase *Ideci):deci(Ideci)
{
 deci->getConfig(&config);
}
Tcodec::~Tcodec()
{
}

const char_t* Tcodec::getMovieSourceName(int source)
{
 switch (source)
  {
   case IDFF_MOVIE_LAVC    :return _l("libavcodec");
   case IDFF_MOVIE_LSVC    :return _l("OpenSVCDecoder");
   case IDFF_MOVIE_THEO    :return _l("libtheora");
   case IDFF_MOVIE_RAW     :return _l("uncompressed");
   case IDFF_MOVIE_LIBMPEG2:return _l("libmpeg2");
   case IDFF_MOVIE_MPLAYER :return _l("mp3lib");
   case IDFF_MOVIE_LIBMAD  :return _l("libmad");
   case IDFF_MOVIE_LIBFAAD :return _l("libfaad2");
   case IDFF_MOVIE_XVID4   :return _l("Xvid");
   case IDFF_MOVIE_AVIS    :return _l("AviSynth");
   case IDFF_MOVIE_WMV9    :return _l("wmv9");
   case IDFF_MOVIE_SKAL    :return _l("sklmp4");
   case IDFF_MOVIE_X264    :return _l("x264");
   case IDFF_MOVIE_LIBA52  :return _l("liba52");
   case IDFF_MOVIE_LIBDTS  :return _l("libdts");
   case IDFF_MOVIE_TREMOR  :return _l("Tremor");
   case IDFF_MOVIE_REALAAC :return _l("realaac");
   case IDFF_MOVIE_AUDX    :return _l("Aud-X");
   default:
   case 0:return _l("disabled");
  }
}

//============================= TcodecDec ==============================
TcodecDec::TcodecDec(IffdshowBase *Ideci,IdecSink *Isink):
 Tcodec(Ideci),
 deciD(Ideci),
 sink(Isink)
{
}
TcodecDec::~TcodecDec()
{
}

HRESULT TcodecDec::flush(void)
{
 HRESULT hr=flushDec();
 return SUCCEEDED(hr) && sink?sink->flushDecodedSamples():hr;
}
