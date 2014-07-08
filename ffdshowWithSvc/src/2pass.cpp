/**************************************************************************
 *
 *	XVID 2PASS CODE
 *	codec
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *************************************************************************/

#include "stdafx.h"
#include "2pass.h"
#include "TencStats.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "IffdshowEnc.h"
#include "TffPict.h"

//================================= TxvidStats =================================
TxvidStats::TxvidStats(const char_t *Iflnm,bool write):flnm(Iflnm)
{
 ok=false;
 fw=NULL;
 if (!flnm || !flnm[0]) return;
 if (write)
  {
   fw=fopen(flnm,_l("wb"));
   if (!fw) return;
   int32_t version=-20;
   size_t len=fwrite(&version,1,4,fw);
   ok=(len==4);
  }
 else
  {
   FILE *f=fopen(flnm,_l("rb"));if (!f) return;
   int ver;fread(&ver,1,4,f);if (ver!=-20) {fclose(f);return;}
   fseek(f,0,SEEK_END);
   long flen=ftell(f);
   fseek(f,4,SEEK_SET);
   while (!feof(f))
    {
     NNSTATS nns;
     size_t len=fread(&nns,1,sizeof(nns),f);
     if (len!=sizeof(NNSTATS)) break;
     stats.push_back(nns);
    }
   fclose(f);
   resetRead();
  }
 ok=true;
}
TxvidStats::~TxvidStats()
{
 if (fw) fclose(fw);
}
size_t TxvidStats::size(void) const
{
 return stats.size();
}
void TxvidStats::resetRead(void)
{
 pos=0;
}
bool TxvidStats::readNext(NNSTATS *nns)
{
 if (pos<size())
  {
   *nns=stats[pos++];
   return true;
  }
 else
  return false;
}
const NNSTATS* TxvidStats::operator [](size_t i) const
{
 return (i<size())?&stats[i]:NULL;
}
TxvidStats::operator const NNSTATS* (void) const
{
 return stats.empty()?NULL:&*stats.begin();
}

size_t TxvidStats::write(const NNSTATS *nns)
{
 stats.push_back(*nns);
 return flnm?fw?fwrite(nns,1,sizeof(NNSTATS),fw):0:sizeof(NNSTATS);
}

//================================= Txvid_2pass =================================
int Txvid_2pass::codec_2pass_init(void)
{
	int	frames = 0, bframes = 0, pframes = 0, credits_frames = 0, i_frames = 0, recminbsize = 0, recminpsize = 0, recminisize = 0;
	int64_t bframe_total_ext = 0, pframe_total_ext = 0, pframe_total = 0, bframe_total = 0, i_total = 0, i_total_ext = 0, i_boost_total = 0, start = 0, end = 0, start_curved = 0, end_curved = 0;
	int64_t desired = (int64_t)config.desiredSize * 1024;

	double total1 = 0.0;
	double total2 = 0.0;
	double dbytes, dbytes2;

	/* ensure free() is called safely */
	twopass.nns1_array = NULL;
	twopass.nns2_array = NULL;

	switch (config.mode)
	{
	case DLG_MODE_2PASS_1 :
		if (!statsWrite) statsWrite = ownStatsWrite=new TxvidStats(config.stats1flnm,true);
		if (!statsWrite->ok)
		{
			DEBUGERR(_l("2pass init error - couldn't write to stats1"));
			return ICERR_ERROR;
		}
		break;

	case DLG_MODE_2PASS_2_INT :
	case DLG_MODE_2PASS_2_EXT :
		if (!statsRead) statsRead = ownStatsRead=new TxvidStats(config.stats1flnm,false);
		if (!statsRead->ok)
		{
			DEBUGERR(_l("2pass init error - couldn't open stats1"));
			return ICERR_ERROR;
		}

		twopass.nns_array_length = 0;
		twopass.nns_array_pos = 0;

		// read the stats file(s) into array(s) and reorder them so they
		// correctly represent the frames that the encoder will receive.
		if (config.mode == DLG_MODE_2PASS_2_EXT)
		{
			if (!statsRead2) statsRead2=ownStatsRead2 = new TxvidStats(config.stats2flnm,false);

			if (!statsRead2->ok)
			{
				DEBUGERR(_l("2pass init error - couldn't open stats2"));
				return ICERR_ERROR;
			}
                        twopass.nns1_array =*statsRead;
                        twopass.nns2_array =*statsRead2;
                        frames=(int)std::min(statsRead->size(),statsRead2->size());
		}
		else	// DLG_MODE_2PASS_2_INT
		{
		        twopass.nns1_array=*statsRead;
			frames=(int)statsRead->size();
		}
		if (config.xvid2pass_use_write && !statsWrite)
		        statsWrite=ownStatsWrite=new TxvidStats(config.stats1flnm,true);
		twopass.nns_array_length = frames;
		frames = 0;

		// continue with the initialization..
		if (config.mode == DLG_MODE_2PASS_2_EXT)
		{
			while (1)
			{
				if (twopass.nns_array_pos >= twopass.nns_array_length)
				{
					twopass.nns_array_pos = 0;
					break;
				}

				memcpy(&twopass.nns1, &twopass.nns1_array[twopass.nns_array_pos], sizeof(NNSTATS));
				memcpy(&twopass.nns2, &twopass.nns2_array[twopass.nns_array_pos], sizeof(NNSTATS));
				twopass.nns_array_pos++;

				// skip unnecessary frames.
				if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME ||
					twopass.nns1.dd_v & NNSTATS_PADFRAME ||
					twopass.nns1.dd_v & NNSTATS_DELAYFRAME)
					continue;

				if (!codec_is_in_credits(frames))
				{
					if (twopass.nns1.quant & NNSTATS_KEYFRAME)
					{
						i_total += twopass.nns2.bytes;
						i_boost_total += twopass.nns2.bytes * config.keyframe_boost / 100;
						twopass.keyframe_locations[i_frames] = frames;
						++i_frames;
					}
					else
					{
						if (twopass.nns1.dd_v & NNSTATS_BFRAME)
						{
							bframe_total += twopass.nns1.bytes;
							bframe_total_ext += twopass.nns2.bytes;
							bframes++;
						}
						else
						{
							pframe_total += twopass.nns1.bytes;
							pframe_total_ext += twopass.nns2.bytes;
							pframes++;
						}
					}
				}
				else
					++credits_frames;

				if (twopass.nns1.quant & NNSTATS_KEYFRAME)
				{
					// this test needs to be corrected..
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminisize = twopass.nns1.bytes;
				}
				else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
				{
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminbsize = twopass.nns1.bytes;
				}
				else
				{
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminpsize = twopass.nns1.bytes;
				}

				++frames;
			}
			twopass.keyframe_locations[i_frames] = frames;

			twopass.movie_curve = ((double)(bframe_total_ext + pframe_total_ext + i_boost_total) /
				(bframe_total_ext + pframe_total_ext));

			if (bframes)
				twopass.average_bframe = (double)bframe_total_ext / bframes / twopass.movie_curve;

			if (pframes)
				twopass.average_pframe = (double)pframe_total_ext / pframes / twopass.movie_curve;
			else
				if (bframes)
					twopass.average_pframe = twopass.average_bframe;  // b-frame packed bitstream fix
				else
				{
					DEBUGERR(_l("ERROR:  No p-frames or b-frames were present in the 1st pass.  Rate control cannot function properly!"));
					return ICERR_ERROR;
				}

			// perform prepass to compensate for over/undersizing
			frames = 0;

			if (config.use_alt_curve)
			{
				twopass.alt_curve_low = twopass.average_pframe - twopass.average_pframe * (double)config.alt_curve_low_dist / 100.0;
				twopass.alt_curve_low_diff = twopass.average_pframe - twopass.alt_curve_low;
				twopass.alt_curve_high = twopass.average_pframe + twopass.average_pframe * (double)config.alt_curve_high_dist / 100.0;
				twopass.alt_curve_high_diff = twopass.alt_curve_high - twopass.average_pframe;
				if (config.alt_curve_use_auto)
				{
					if (bframe_total + pframe_total > bframe_total_ext + pframe_total_ext)
					{
						config.alt_curve_min_rel_qual = (int)(100.0 - (100.0 - 100.0 /
							((double)(bframe_total + pframe_total) / (double)(bframe_total_ext + pframe_total_ext))) *
							(double)config.alt_curve_auto_str / 100.0);

						if (config.alt_curve_min_rel_qual < 20)
							config.alt_curve_min_rel_qual = 20;
					}
					else
						config.alt_curve_min_rel_qual = 100;
				}
				twopass.alt_curve_mid_qual = (1.0 + (double)config.alt_curve_min_rel_qual / 100.0) / 2.0;
				twopass.alt_curve_qual_dev = 1.0 - twopass.alt_curve_mid_qual;
				if (config.alt_curve_low_dist > 100)
				{
					switch(config.alt_curve_type)
					{
					case 2: // Sine Curve (high aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							sin(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff)));
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							sin(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff));
						break;
					case 1: // Linear (medium aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							twopass.average_pframe / twopass.alt_curve_low_diff);
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							twopass.average_pframe / twopass.alt_curve_low_diff;
						break;
					case 0: // Cosine Curve (low aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							(1.0 - cos(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff))));
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							(1.0 - cos(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff)));
					}
				}
			}

			while (1)
			{
				if (twopass.nns_array_pos >= twopass.nns_array_length)
				{
					twopass.nns_array_pos = 0;
					break;
				}

				memcpy(&twopass.nns1, &twopass.nns1_array[twopass.nns_array_pos], sizeof(NNSTATS));
				memcpy(&twopass.nns2, &twopass.nns2_array[twopass.nns_array_pos], sizeof(NNSTATS));
				twopass.nns_array_pos++;

				if (frames == 0)
				{
					twopass.minbsize = (twopass.nns1.kblk + 88) / 8;
					twopass.minpsize = (twopass.nns1.kblk + 88) / 8;
					twopass.minisize = ((twopass.nns1.kblk * 22) + 240) / 8;
					if (recminbsize > twopass.minbsize)
						twopass.minbsize = recminbsize;
					if (recminpsize > twopass.minpsize)
						twopass.minpsize = recminpsize;
					if (recminisize > twopass.minisize)
						twopass.minisize = recminisize;
				}

				// skip unnecessary frames.
				if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME ||
					twopass.nns1.dd_v & NNSTATS_PADFRAME ||
					twopass.nns1.dd_v & NNSTATS_DELAYFRAME)
					continue;

				if (!codec_is_in_credits( frames) &&
					!(twopass.nns1.quant & NNSTATS_KEYFRAME))
				{
					dbytes = twopass.nns2.bytes / twopass.movie_curve;
					total1 += dbytes;

					if (twopass.nns1.dd_v & NNSTATS_BFRAME)
						dbytes *= twopass.average_pframe / twopass.average_bframe;

					if (config.use_alt_curve)
					{
						if (dbytes > twopass.average_pframe)
						{
							if (dbytes >= twopass.alt_curve_high)
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev);
							else
							{
								switch(config.alt_curve_type)
								{
								case 2:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff)));
									break;
								case 1:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(dbytes - twopass.average_pframe) / twopass.alt_curve_high_diff);
									break;
								case 0:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff))));
								}
							}
						}
						else
						{
							if (dbytes <= twopass.alt_curve_low)
								dbytes2 = dbytes;
							else
							{
								switch(config.alt_curve_type)
								{
								case 2:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff)));
									break;
								case 1:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(dbytes - twopass.average_pframe) / twopass.alt_curve_low_diff);
									break;
								case 0:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual + twopass.alt_curve_qual_dev *
									(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff))));
								}
							}
						}
					}
					else
					{
						if (dbytes > twopass.average_pframe)
						{
							dbytes2 = ((double)dbytes + (twopass.average_pframe - dbytes) *
								config.curve_compression_high / 100.0);
						}
						else
						{
							dbytes2 = ((double)dbytes + (twopass.average_pframe - dbytes) *
								config.curve_compression_low / 100.0);
						}
					}

					if (twopass.nns1.dd_v & NNSTATS_BFRAME)
					{
						dbytes2 *= twopass.average_bframe / twopass.average_pframe;
						if (dbytes2 < twopass.minbsize)
							dbytes2 = twopass.minbsize;
					}
					else
					{
						if (dbytes2 < twopass.minpsize)
							dbytes2 = twopass.minpsize;
					}

					total2 += dbytes2;
				}

				++frames;
			}

			twopass.curve_comp_scale = total1 / total2;

			if (!config.use_alt_curve)
			{
				int asymmetric_average_frame;
				char_t s[100];

				asymmetric_average_frame = (int)(twopass.average_pframe * twopass.curve_comp_scale);
				wsprintf(s, _l("middle frame size for asymmetric curve compression: %i"), asymmetric_average_frame);
				DEBUG2P(s);
			}
		}
		else	// DLG_MODE_2PASS_2_INT
		{
			while (1)
			{
				if (twopass.nns_array_pos >= twopass.nns_array_length)
				{
					twopass.nns_array_pos = 0;
					break;
				}

				memcpy(&twopass.nns1, &twopass.nns1_array[twopass.nns_array_pos], sizeof(NNSTATS));
				twopass.nns_array_pos++;

				// skip unnecessary frames.
				if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME ||
					twopass.nns1.dd_v & NNSTATS_PADFRAME ||
					twopass.nns1.dd_v & NNSTATS_DELAYFRAME)
					continue;

				if (codec_is_in_credits( frames) == CREDITS_START)
				{
					start += twopass.nns1.bytes;
					++credits_frames;
				}
				else if (codec_is_in_credits( frames) == CREDITS_END)
				{
					end += twopass.nns1.bytes;
					++credits_frames;
				}
				else if (twopass.nns1.quant & NNSTATS_KEYFRAME)
				{
					i_total += twopass.nns1.bytes + twopass.nns1.bytes * config.keyframe_boost / 100;
					twopass.keyframe_locations[i_frames] = frames;
					++i_frames;
				}
				else
				{
					if (twopass.nns1.dd_v & NNSTATS_BFRAME)
					{
						bframe_total += twopass.nns1.bytes;
						bframes++;
					}
					else
					{
						pframe_total += twopass.nns1.bytes;
						pframes++;
					}
				}

				if (twopass.nns1.quant & NNSTATS_KEYFRAME)
				{
					// this test needs to be corrected..
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminisize = twopass.nns1.bytes;
				}
				else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
				{
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminbsize = twopass.nns1.bytes;
				}
				else
				{
					if (!(twopass.nns1.kblk + twopass.nns1.mblk))
						recminpsize = twopass.nns1.bytes;
				}

				++frames;
			}
			twopass.keyframe_locations[i_frames] = frames;

			// compensate for avi frame overhead
			desired -= frames * 24;

			switch (config.credits_mode)
			{
			case CREDITS_MODE_RATE :

				// credits curve = (total / desired_size) * (100 / credits_rate)
				twopass.credits_start_curve = twopass.credits_end_curve =
					((double)(bframe_total + pframe_total + i_total + start + end) / desired) *
					((double)100 / config.credits_percent);

				start_curved = (int64_t)(start / twopass.credits_start_curve);
				end_curved = (int64_t)(end / twopass.credits_end_curve);

				// movie curve = (total - credits) / (desired_size - curved credits)
				twopass.movie_curve = (double)
					(bframe_total + pframe_total + i_total) /
					(desired - start_curved - end_curved);

				break;

			case CREDITS_MODE_QUANT :

				// movie curve = (total - credits) / (desired_size - credits)
				twopass.movie_curve = (double)
					(bframe_total + pframe_total + i_total) / (desired - start - end);

				// aid the average asymmetric frame calculation below
				start_curved = start;
				end_curved = end;

				break;

			case CREDITS_MODE_SIZE :

				// start curve = (start / start desired size)
				twopass.credits_start_curve = (double)
					(start / 1024) / config.credits_size_start;

				// end curve = (end / end desired size)
				twopass.credits_end_curve = (double)
					(end / 1024) / config.credits_size_end;

				start_curved = (int64_t)(start / twopass.credits_start_curve);
				end_curved = (int64_t)(end / twopass.credits_end_curve);

				// movie curve = (total - credits) / (desired_size - curved credits)
				twopass.movie_curve = (double)
					(bframe_total + pframe_total + i_total) /
					(desired - start_curved - end_curved);

				break;
			}

			if (bframes)
				twopass.average_bframe = (double)bframe_total / bframes / twopass.movie_curve;

			if (pframes)
				twopass.average_pframe = (double)pframe_total / pframes / twopass.movie_curve;
			else
				if (bframes)
					twopass.average_pframe = twopass.average_bframe;  // b-frame packed bitstream fix
				else
				{
					DEBUGERR(_l("ERROR:  No p-frames or b-frames were present in the 1st pass.  Rate control cannot function properly!"));
					return ICERR_ERROR;
				}

			// perform prepass to compensate for over/undersizing
			frames = 0;

			if (config.use_alt_curve)
			{
				twopass.alt_curve_low = twopass.average_pframe - twopass.average_pframe * (double)config.alt_curve_low_dist / 100.0;
				twopass.alt_curve_low_diff = twopass.average_pframe - twopass.alt_curve_low;
				twopass.alt_curve_high = twopass.average_pframe + twopass.average_pframe * (double)config.alt_curve_high_dist / 100.0;
				twopass.alt_curve_high_diff = twopass.alt_curve_high - twopass.average_pframe;
				if (config.alt_curve_use_auto)
				{
					if (twopass.movie_curve > 1.0)
					{
						config.alt_curve_min_rel_qual = (int)(100.0 - (100.0 - 100.0 / twopass.movie_curve) * (double)config.alt_curve_auto_str / 100.0);
						if (config.alt_curve_min_rel_qual < 20)
							config.alt_curve_min_rel_qual = 20;
					}
					else
						config.alt_curve_min_rel_qual = 100;
				}
				twopass.alt_curve_mid_qual = (1.0 + (double)config.alt_curve_min_rel_qual / 100.0) / 2.0;
				twopass.alt_curve_qual_dev = 1.0 - twopass.alt_curve_mid_qual;
				if (config.alt_curve_low_dist > 100)
				{
					switch(config.alt_curve_type)
					{
					case 2: // Sine Curve (high aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							sin(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff)));
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							sin(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff));
						break;
					case 1: // Linear (medium aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							twopass.average_pframe / twopass.alt_curve_low_diff);
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							twopass.average_pframe / twopass.alt_curve_low_diff;
						break;
					case 0: // Cosine Curve (low aggressiveness)
						twopass.alt_curve_qual_dev *= 2.0 / (1.0 +
							(1.0 - cos(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff))));
						twopass.alt_curve_mid_qual = 1.0 - twopass.alt_curve_qual_dev *
							(1.0 - cos(DEG2RAD * (twopass.average_pframe * 90.0 / twopass.alt_curve_low_diff)));
					}
				}
			}

			while (1)
			{
				if (twopass.nns_array_pos >= twopass.nns_array_length)
				{
					twopass.nns_array_pos = 0;
					break;
				}

				memcpy(&twopass.nns1, &twopass.nns1_array[twopass.nns_array_pos], sizeof(NNSTATS));
				twopass.nns_array_pos++;

				if (frames == 0)
				{
					twopass.minbsize = (twopass.nns1.kblk + 88) / 8;
					twopass.minpsize = (twopass.nns1.kblk + 88) / 8;
					twopass.minisize = ((twopass.nns1.kblk * 22) + 240) / 8;
					if (recminbsize > twopass.minbsize)
						twopass.minbsize = recminbsize;
					if (recminpsize > twopass.minpsize)
						twopass.minpsize = recminpsize;
					if (recminisize > twopass.minisize)
						twopass.minisize = recminisize;
				}

				// skip unnecessary frames.
				if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME ||
					twopass.nns1.dd_v & NNSTATS_PADFRAME ||
					twopass.nns1.dd_v & NNSTATS_DELAYFRAME)
					continue;

				if (!codec_is_in_credits( frames) &&
					!(twopass.nns1.quant & NNSTATS_KEYFRAME))
				{
					dbytes = twopass.nns1.bytes / twopass.movie_curve;
					total1 += dbytes;

					if (twopass.nns1.dd_v & NNSTATS_BFRAME)
						dbytes *= twopass.average_pframe / twopass.average_bframe;

					if (config.use_alt_curve)
					{
						if (dbytes > twopass.average_pframe)
						{
							if (dbytes >= twopass.alt_curve_high)
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev);
							else
							{
								switch(config.alt_curve_type)
								{
								case 2:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff)));
									break;
								case 1:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(dbytes - twopass.average_pframe) / twopass.alt_curve_high_diff);
									break;
								case 0:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff))));
								}
							}
						}
						else
						{
							if (dbytes <= twopass.alt_curve_low)
								dbytes2 = dbytes;
							else
							{
								switch(config.alt_curve_type)
								{
								case 2:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff)));
									break;
								case 1:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
									(dbytes - twopass.average_pframe) / twopass.alt_curve_low_diff);
									break;
								case 0:
								dbytes2 = dbytes * (twopass.alt_curve_mid_qual + twopass.alt_curve_qual_dev *
									(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff))));
								}
							}
						}
					}
					else
					{
						if (dbytes > twopass.average_pframe)
						{
							dbytes2 = ((double)dbytes + (twopass.average_pframe - dbytes) *
								config.curve_compression_high / 100.0);
						}
						else
						{
							dbytes2 = ((double)dbytes + (twopass.average_pframe - dbytes) *
								config.curve_compression_low / 100.0);
						}
					}

					if (twopass.nns1.dd_v & NNSTATS_BFRAME)
					{
						dbytes2 *= twopass.average_bframe / twopass.average_pframe;
						if (dbytes2 < twopass.minbsize)
							dbytes2 = twopass.minbsize;
					}
					else
					{
						if (dbytes2 < twopass.minpsize)
							dbytes2 = twopass.minpsize;
					}

					total2 += dbytes2;
				}

				++frames;
			}

			twopass.curve_comp_scale = total1 / total2;

			if (!config.use_alt_curve)
			{
				int asymmetric_average_frame;
				char_t s[100];

				asymmetric_average_frame = (int)(twopass.average_pframe * twopass.curve_comp_scale);
				wsprintf(s, _l("middle frame size for asymmetric curve compression: %i"), asymmetric_average_frame);
				DEBUG2P(s);
			}
		}

		if (config.use_alt_curve)
		{
			if (config.alt_curve_use_auto_bonus_bias)
				config.alt_curve_bonus_bias = config.alt_curve_min_rel_qual;

			twopass.curve_bias_bonus = (total1 - total2) * (double)config.alt_curve_bonus_bias / 100.0 / (double)(frames - credits_frames - i_frames);
			twopass.curve_comp_scale = ((total1 - total2) * (1.0 - (double)config.alt_curve_bonus_bias / 100.0) + total2) / total2;

			// special info for alt curve:  bias bonus and quantizer thresholds,
			{
				double curve_temp, dbytes;
				char_t s[100];
				int i, newquant, percent;
				int oldquant = 1;

				wsprintf(s, _l("avg scaled framesize:%i"), (int)(twopass.average_pframe));
				DEBUG2P(s);

				wsprintf(s, _l("bias bonus:%i bytes"), (int)(twopass.curve_bias_bonus));
				DEBUG2P(s);

				for (i=1; i <= (int)(twopass.alt_curve_high*2)+1; i++)
				{
					dbytes = i;
					if (dbytes > twopass.average_pframe)
					{
						if (dbytes >= twopass.alt_curve_high)
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev);
						else
						{
							switch(config.alt_curve_type)
							{
							case 2:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
								sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff)));
								break;
							case 1:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
								(dbytes - twopass.average_pframe) / twopass.alt_curve_high_diff);
								break;
							case 0:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
								(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff))));
							}
						}
					}
					else
					{
						if (dbytes <= twopass.alt_curve_low)
							curve_temp = dbytes;
						else
						{
							switch(config.alt_curve_type)
							{
							case 2:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
								sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff)));
								break;
							case 1:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
								(dbytes - twopass.average_pframe) / twopass.alt_curve_low_diff);
								break;
							case 0:
							curve_temp = dbytes * (twopass.alt_curve_mid_qual + twopass.alt_curve_qual_dev *
								(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff))));
							}
						}
					}

					if (twopass.movie_curve > 1.0)
						dbytes *= twopass.movie_curve;

					newquant = (int)(dbytes * 2.0 / (curve_temp * twopass.curve_comp_scale + twopass.curve_bias_bonus));
					if (newquant > 1)
					{
						if (newquant != oldquant)
						{
							oldquant = newquant;
							percent = (int)((i - twopass.average_pframe) * 100.0 / twopass.average_pframe);
							wsprintf(s, _l("quant:%i threshold at %i : %i percent"), newquant, i, percent);
							quant_threshs[newquant].thresh=i;quant_threshs[newquant].percent=percent;
							DEBUG2P(s);
						}
					}
				}
			}
		}

		twopass.overflow = 0;
		twopass.KFoverflow = 0;
		twopass.KFoverflow_partial = 0;
		twopass.KF_idx = 1;

		break;
	}

	return ICERR_OK;
}

// NOTE: codec_2pass_get_quant() should be called for all the frames that are in the stats file(s)
int Txvid_2pass::codec_2pass_get_quant(TencFrameParams *frame)
{

//	DWORD read;
	int bytes1, bytes2;
	int overflow;
	int credits_pos;
	int capped_to_max_framesize = 0;
	int KFdistance, KF_min_size;

	if (framenum == 0)
	{
		int i;

		for (i=0 ; i<32 ; ++i)
		{
			bquant_error[i] = 0.0;
			pquant_error[i] = 0.0;
		}

		curve_comp_error = 0.0;
		last_bquant = 0;
		last_pquant = 0;
	}

	if (twopass.nns_array_pos >= twopass.nns_array_length)
	{
		// fix for VirtualDub 1.4.13 bframe handling
		if (config.max_b_frames > 0 &&
			framenum < twopass.nns_array_length + config.max_b_frames)
		{
			return ICERR_OK;
		}
		else
		{
			DEBUGERR(_l("ERROR: VIDEO EXCEEDS 1ST PASS!!!"));
			return ICERR_ERROR;
		}
	}

	memcpy(&twopass.nns1, &twopass.nns1_array[twopass.nns_array_pos], sizeof(NNSTATS));
	if (config.mode == DLG_MODE_2PASS_2_EXT)
		memcpy(&twopass.nns2, &twopass.nns2_array[twopass.nns_array_pos], sizeof(NNSTATS));
	twopass.nns_array_pos++;

	bytes1 = twopass.nns1.bytes;

	// skip unnecessary frames.
	if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME)
	{
		twopass.bytes1 = bytes1;
		twopass.bytes2 = bytes1;
		twopass.desired_bytes2 = bytes1;
		frame->frametype = FRAME_TYPE::SKIP;
		return ICERR_OK;
	}
	else if (twopass.nns1.dd_v & NNSTATS_PADFRAME)
	{
		twopass.bytes1 = bytes1;
		twopass.bytes2 = bytes1;
		twopass.desired_bytes2 = bytes1;
		frame->frametype = FRAME_TYPE::PAD;
		return ICERR_OK;
	}
	else if (twopass.nns1.dd_v & NNSTATS_DELAYFRAME)
	{
		twopass.bytes1 = bytes1;
		twopass.bytes2 = bytes1;
		twopass.desired_bytes2 = bytes1;
		frame->frametype = FRAME_TYPE::DELAY;
		return ICERR_OK;
	}

	overflow = twopass.overflow / 8;

	// override codec i-frame choice (reenable in credits)
	if (twopass.nns1.quant & NNSTATS_KEYFRAME)
		frame->frametype=FRAME_TYPE::I;
	else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
		frame->frametype=FRAME_TYPE::B;
	else
		frame->frametype=FRAME_TYPE::P;

	if (frame->frametype==FRAME_TYPE::I)
	{
		overflow = 0;
	}

	credits_pos = codec_is_in_credits( framenum);

	if (credits_pos)
	{
		if (config.mode == DLG_MODE_2PASS_2_INT)
		{
			switch (config.credits_mode)
			{
			case CREDITS_MODE_RATE :
			case CREDITS_MODE_SIZE :
				if (credits_pos == CREDITS_START)
				{
					bytes2 = (int)(bytes1 / twopass.credits_start_curve);
				}
				else // CREDITS_END
				{
					bytes2 = (int)(bytes1 / twopass.credits_end_curve);
				}

				frame->frametype = -1;
				break;

			case CREDITS_MODE_QUANT :
				if (config.credits_quant_i != config.credits_quant_p)
				{
					frame->quant = frame->frametype ?
						config.credits_quant_i :
						config.credits_quant_p;
				}
				else
				{
					frame->quant = config.credits_quant_p;
					frame->frametype = -1;
				}

				twopass.bytes1 = bytes1;
				twopass.bytes2 = bytes1;
				twopass.desired_bytes2 = bytes1;
				return ICERR_OK;
			}
		}
		else	// DLG_MODE_2PASS_2_EXT
		{
			if (config.credits_mode == CREDITS_MODE_QUANT)
			{
				if (config.credits_quant_i != config.credits_quant_p)
				{
					frame->quant = frame->frametype == FRAME_TYPE::I ?
						config.credits_quant_i :
						config.credits_quant_p;
				}
				else
				{
					frame->quant = config.credits_quant_p;
					frame->frametype = -1;
				}

				twopass.bytes1 = bytes1;
				twopass.bytes2 = bytes1;
				twopass.desired_bytes2 = bytes1;
				return ICERR_OK;
			}
			else
				bytes2 = twopass.nns2.bytes;
		}
	}
	else	// Foxer: apply curve compression outside credits
	{
		double dbytes, curve_temp;

		bytes2 = (config.mode == DLG_MODE_2PASS_2_INT) ? bytes1 : twopass.nns2.bytes;

		if (frame->frametype==FRAME_TYPE::I)
		{
			dbytes = ((int)(bytes2 + bytes2 * config.keyframe_boost / 100)) /
				twopass.movie_curve;
		}
		else
		{
			dbytes = bytes2 / twopass.movie_curve;
		}

		if (twopass.nns1.dd_v & NNSTATS_BFRAME)
			dbytes *= twopass.average_pframe / twopass.average_bframe;

		// spread the compression error across payback_delay frames
		if (config.bitrate_payback_method == 0)
		{
			bytes2 = (int)(curve_comp_error / config.bitrate_payback_delay);
		}
		else
		{
			bytes2 = (int)(curve_comp_error * dbytes /
				twopass.average_pframe / config.bitrate_payback_delay);

			if (labs(bytes2) > fabs(curve_comp_error))
			{
				bytes2 = (int)curve_comp_error;
			}
		}

		curve_comp_error -= bytes2;

		if (config.use_alt_curve)
		{
			if (!(frame->frametype==FRAME_TYPE::I))
			{
				if (dbytes > twopass.average_pframe)
				{
					if (dbytes >= twopass.alt_curve_high)
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev);
					else
					{
						switch(config.alt_curve_type)
						{
						case 2:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
							sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff)));
							break;
						case 1:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
							(dbytes - twopass.average_pframe) / twopass.alt_curve_high_diff);
							break;
						case 0:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
							(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_high_diff))));
						}
					}
				}
				else
				{
					if (dbytes <= twopass.alt_curve_low)
						curve_temp = dbytes;
					else
					{
						switch(config.alt_curve_type)
						{
						case 2:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
							sin(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff)));
							break;
						case 1:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual - twopass.alt_curve_qual_dev *
							(dbytes - twopass.average_pframe) / twopass.alt_curve_low_diff);
							break;
						case 0:
						curve_temp = dbytes * (twopass.alt_curve_mid_qual + twopass.alt_curve_qual_dev *
							(1.0 - cos(DEG2RAD * ((dbytes - twopass.average_pframe) * 90.0 / twopass.alt_curve_low_diff))));
						}
					}
				}
				if (twopass.nns1.dd_v & NNSTATS_BFRAME)
					curve_temp *= twopass.average_bframe / twopass.average_pframe;

				curve_temp = curve_temp * twopass.curve_comp_scale + twopass.curve_bias_bonus;

				bytes2 += ((int)curve_temp);
				curve_comp_error += curve_temp - ((int)curve_temp);
			}
			else
			{
				if (twopass.nns1.dd_v & NNSTATS_BFRAME)
					dbytes *= twopass.average_bframe / twopass.average_pframe;

				bytes2 += ((int)dbytes);
				curve_comp_error += dbytes - ((int)dbytes);
			}
		}
		else if ((config.curve_compression_high + config.curve_compression_low) &&
			!(frame->frametype==FRAME_TYPE::I))
		{
			if (dbytes > twopass.average_pframe)
			{
				curve_temp = twopass.curve_comp_scale *
					((double)dbytes + (twopass.average_pframe - dbytes) *
					config.curve_compression_high / 100.0);
			}
			else
			{
				curve_temp = twopass.curve_comp_scale *
					((double)dbytes + (twopass.average_pframe - dbytes) *
					config.curve_compression_low / 100.0);
			}

			if (twopass.nns1.dd_v & NNSTATS_BFRAME)
				curve_temp *= twopass.average_bframe / twopass.average_pframe;

			bytes2 += ((int)curve_temp);
			curve_comp_error += curve_temp - ((int)curve_temp);
		}
		else
		{
			if (twopass.nns1.dd_v & NNSTATS_BFRAME)
				dbytes *= twopass.average_bframe / twopass.average_pframe;

			bytes2 += ((int)dbytes);
			curve_comp_error += dbytes - ((int)dbytes);
		}

		// cap bytes2 to first pass size, lowers number of quant=1 frames
		if (bytes2 > bytes1)
		{
			curve_comp_error += bytes2 - bytes1;
			bytes2 = bytes1;
		}
		else
		{
			if (frame->frametype==FRAME_TYPE::I)
			{
				if (bytes2 < twopass.minisize)
				{
					curve_comp_error -= twopass.minisize - bytes2;
					bytes2 = twopass.minisize;
				}
			}
			else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
			{
				if (bytes2 < twopass.minbsize)
					bytes2 = twopass.minbsize;
			}
			else
			{
				if (bytes2 < twopass.minpsize)
					bytes2 = twopass.minpsize;
			}
		}
	}

	twopass.desired_bytes2 = bytes2;

	// if this keyframe is too close to the next,
	// reduce it's byte allotment
	if ((frame->frametype==FRAME_TYPE::I) && !credits_pos)
	{
		KFdistance = twopass.keyframe_locations[twopass.KF_idx] -
			twopass.keyframe_locations[twopass.KF_idx - 1];

		if (KFdistance < config.kftreshold)
		{
			KFdistance = KFdistance - config.min_key_interval;

			if (KFdistance >= 0)
			{
				KF_min_size = bytes2 * (100 - config.kfreduction) / 100;
				if (KF_min_size < 1)
					KF_min_size = 1;

				bytes2 = KF_min_size + (bytes2 - KF_min_size) * KFdistance /
					(config.kftreshold - config.min_key_interval);

				if (bytes2 < 1)
					bytes2 = 1;
			}
		}
	}

	// Foxer: scale overflow in relation to average size, so smaller frames don't get
	// too much/little bitrate
	overflow = (int)((double)overflow * bytes2 / twopass.average_pframe);

	// Foxer: reign in overflow with huge frames
	if (labs(overflow) > labs(twopass.overflow))
	{
		overflow = twopass.overflow;
	}

	// Foxer: make sure overflow doesn't run away
	if (overflow > bytes2 * config.twopass_max_overflow_improvement / 100)
	{
		bytes2 += (overflow <= bytes2) ? bytes2 * config.twopass_max_overflow_improvement / 100 :
			overflow * config.twopass_max_overflow_improvement / 100;
	}
	else if (overflow < bytes2 * config.twopass_max_overflow_degradation / -100)
	{
		bytes2 += bytes2 * config.twopass_max_overflow_degradation / -100;
	}
	else
	{
		bytes2 += overflow;
	}

	if (bytes2 > twopass.max_framesize)
	{
		capped_to_max_framesize = 1;
		bytes2 = twopass.max_framesize;
	}

	// make sure to not scale below the minimum framesize
	if (twopass.nns1.quant & NNSTATS_KEYFRAME)
	{
		if (bytes2 < twopass.minisize)
			bytes2 = twopass.minisize;
	}
	else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
	{
		if (bytes2 < twopass.minbsize)
			bytes2 = twopass.minbsize;
	}
	else
	{
		if (bytes2 < twopass.minpsize)
			bytes2 = twopass.minpsize;
	}

	twopass.bytes1 = bytes1;
	twopass.bytes2 = bytes2;

	// very 'simple' quant<->filesize relationship
	frame->quant = ((twopass.nns1.quant & ~NNSTATS_KEYFRAME) * bytes1) / bytes2;

	if (frame->quant < 1)
	{
		frame->quant = 1;
	}
	else if (frame->quant > 31)
	{
		frame->quant = 31;
	}
	else if (!(frame->frametype==FRAME_TYPE::I))
	{
		// Foxer: aid desired quantizer precision by accumulating decision error
		if (twopass.nns1.dd_v & NNSTATS_BFRAME)
		{
			bquant_error[frame->quant] += ((double)((twopass.nns1.quant & ~NNSTATS_KEYFRAME) *
				bytes1) / bytes2) - frame->quant;

			if (bquant_error[frame->quant] >= 1.0)
			{
				bquant_error[frame->quant] -= 1.0;
				++frame->quant;
			}
		}
		else
		{
			pquant_error[frame->quant] += ((double)((twopass.nns1.quant & ~NNSTATS_KEYFRAME) *
				bytes1) / bytes2) - frame->quant;

			if (pquant_error[frame->quant] >= 1.0)
			{
				pquant_error[frame->quant] -= 1.0;
				++frame->quant;
			}
		}
	}

	// we're done with credits
	if (codec_is_in_credits( framenum))
	{
		return ICERR_OK;
	}

	if ((frame->frametype==FRAME_TYPE::I))
	{
		if (frame->quant < config.q_i_min)
		{
			frame->quant = config.q_i_min;
			DEBUG2P(_l("I-frame quantizer raised"));
		}
		if (frame->quant > config.q_i_max)
		{
			frame->quant = config.q_i_max;
			DEBUG2P(_l("I-frame quantizer lowered"));
		}
	}
	else
	{
		if (frame->quant > config.q_p_max)
		{
			frame->quant = config.q_p_max;
		}
		if (frame->quant < config.q_p_min)
		{
			frame->quant = config.q_p_min;
		}

		// subsequent frame quants can only be +- 2
		if ((last_pquant || last_bquant) && capped_to_max_framesize == 0)
		{
			if (twopass.nns1.dd_v & NNSTATS_BFRAME)
			{
				// this bframe quantizer variation
				// restriction needs to be redone.
				if (frame->quant > last_bquant + 2)
				{
					frame->quant = last_bquant + 2;
					DEBUG2P(_l("B-frame quantizer prevented from rising too steeply"));
				}
				if (frame->quant < last_bquant - 2)
				{
					frame->quant = last_bquant - 2;
					DEBUG2P(_l("B-frame quantizer prevented from falling too steeply"));
				}
			}
			else
			{
				if (frame->quant > last_pquant + 2)
				{
					frame->quant = last_pquant + 2;
					DEBUG2P(_l("P-frame quantizer prevented from rising too steeply"));
				}
				if (frame->quant < last_pquant - 2)
				{
					frame->quant = last_pquant - 2;
					DEBUG2P(_l("P-frame quantizer prevented from falling too steeply"));
				}
			}
		}
	}

	if (capped_to_max_framesize == 0)
	{
		if (twopass.nns1.quant & NNSTATS_KEYFRAME)
		{
			last_bquant = frame->quant;
			last_pquant = frame->quant;
		}
		else if (twopass.nns1.dd_v & NNSTATS_BFRAME)
			last_bquant = frame->quant;
		else
			last_pquant = frame->quant;
	}

	return ICERR_OK;
}

int Txvid_2pass::codec_2pass_update(const TencFrameParams *frame)
{
	int credits_pos, tempdiv;
	char_t* frame_type;

	if (framenum == 0)
	{
		total_size = 0;
	}

        if (statsWrite)
        {
        	NNSTATS nns1;
		nns1.bytes = frame->length;	// total bytes
		nns1.dd_v = 0;
		nns1.dd_u = nns1.dd_y = 0;
		nns1.dk_v = nns1.dk_u = nns1.dk_y = 0;
		nns1.md_u = nns1.md_y = 0;
		nns1.mk_u = nns1.mk_y = 0;

		nns1.quant = frame->quant;				// 2 ugly fix for lumi masking in 1st pass returning new quant
		nns1.lum_noise[0] = nns1.lum_noise[1] = 1;
		frame_type=_l("inter");
		if (frame->frametype ==FRAME_TYPE::I) {
			nns1.quant |= NNSTATS_KEYFRAME;
			frame_type=_l("intra");
		}
		else if (frame->frametype==FRAME_TYPE::B) {
			nns1.dd_v |= NNSTATS_BFRAME;
			frame_type=_l("bframe");
		}
		else if (frame->frametype==FRAME_TYPE::SKIP) {
			nns1.dd_v |= NNSTATS_SKIPFRAME;
			frame_type=_l("skiped");
		}
		else if (frame->frametype==FRAME_TYPE::PAD) {
			nns1.dd_v |= NNSTATS_PADFRAME;
			frame_type=_l("padded");
		}
		else if (frame->frametype==FRAME_TYPE::DELAY) {
			nns1.dd_v |= NNSTATS_DELAYFRAME;
			frame_type=_l("delayed");
		}
                if (frame->kblks+frame->mblks+frame->ublks==0)
                 {
		  nns1.kblk = 1;
		  nns1.mblk = 1;
		  nns1.ublk = 1;
                 }
                else
                 {
		  nns1.kblk = frame->kblks;
		  nns1.mblk = frame->mblks;
		  nns1.ublk = frame->ublks;
                 }

		total_size += frame->length;

		DEBUG1ST(frame->length, (int)total_size/1024, frame_type, frame->quant, _l(""), 1, 1);

		if (statsWrite->write(&nns1)!=sizeof(NNSTATS))
		{
			DEBUGERR(_l("stats1: WriteFile error"));
			return ICERR_ERROR;
		}
        }

        if (config.mode==DLG_MODE_2PASS_2_INT || config.mode==DLG_MODE_2PASS_2_EXT)
        {
		credits_pos = codec_is_in_credits(framenum);
		if (!(twopass.nns1.dd_v & NNSTATS_SKIPFRAME) &&
			!(twopass.nns1.dd_v & NNSTATS_PADFRAME) &&
			!(twopass.nns1.dd_v & NNSTATS_DELAYFRAME))
		{
			if (!credits_pos)
			{
				if ((twopass.nns1.quant & NNSTATS_KEYFRAME))
				{
					// calculate how much to distribute per frame in
					// order to make up for this keyframe's overflow

					twopass.overflow += twopass.KFoverflow;
					twopass.KFoverflow = twopass.desired_bytes2 - frame->length;

					tempdiv = (twopass.keyframe_locations[twopass.KF_idx] -
						twopass.keyframe_locations[twopass.KF_idx - 1]);

					if (tempdiv > 1)
					{
						// non-consecutive keyframes
						twopass.KFoverflow_partial = twopass.KFoverflow / (tempdiv - 1);
					}
					else
					{
						// consecutive keyframes
						twopass.overflow += twopass.KFoverflow;
						twopass.KFoverflow = 0;
						twopass.KFoverflow_partial = 0;
					}
					twopass.KF_idx++;
				}
				else
				{
					// distribute part of the keyframe overflow

					twopass.overflow += twopass.desired_bytes2 - frame->length +
						twopass.KFoverflow_partial;
					twopass.KFoverflow -= twopass.KFoverflow_partial;
				}
			}
			else
			{
				twopass.overflow += twopass.desired_bytes2 - frame->length;

				// ugly fix for credits..
				twopass.overflow += twopass.KFoverflow;
				twopass.KFoverflow = 0;
				twopass.KFoverflow_partial = 0;
				// end of ugly fix.
			}
		}

		frame_type=_l("inter");
		if (frame->frametype==FRAME_TYPE::I) {
			frame_type=_l("intra");
		}
		else if (twopass.nns1.dd_v & NNSTATS_BFRAME) {
			frame_type=_l("bframe");
		}
		else if (twopass.nns1.dd_v & NNSTATS_SKIPFRAME) {
			frame_type=_l("skipped");
			//frame->quant = 2;
			twopass.bytes1 = 1;
			twopass.desired_bytes2 = 1;
			//frame->length = 1;
		}
		else if (twopass.nns1.dd_v & NNSTATS_PADFRAME) {
			frame_type=_l("padded");
			//frame->quant = 2;
			twopass.bytes1 = 7;
			twopass.desired_bytes2 = 7;
			//frame->length = 7;
		}
		else if (twopass.nns1.dd_v & NNSTATS_DELAYFRAME) {
			frame_type=_l("delayed");
			//frame->quant = 2;
			twopass.bytes1 = 1;
			twopass.desired_bytes2 = 1;
			//frame->length = 1;
		}

		DEBUG2ND(frame->quant, _l(""), frame_type, twopass.bytes1, twopass.desired_bytes2, frame->length, twopass.overflow, credits_pos);
	}

	return ICERR_OK;
}

void Txvid_2pass::codec_2pass_finish(void)
{
	twopass.nns_array_length = 0;
	twopass.nns_array_pos = 0;
}
Txvid_2pass::Txvid_2pass(void)
{
 statsRead=statsRead2=statsWrite=NULL;
 ownStatsRead=ownStatsRead2=ownStatsWrite=NULL;
}
Txvid_2pass::~Txvid_2pass()
{
 if (ownStatsRead ) delete ownStatsRead ;
 if (ownStatsRead2) delete ownStatsRead2;
 if (ownStatsWrite) delete ownStatsWrite;
}

//================================= T2passFirst ==================================
T2passFirst::T2passFirst(const TcoSettings &Iconfig)
{
 config=Iconfig;
 codec_2pass_init();
}
T2passFirst::~T2passFirst()
{
 codec_2pass_finish();
}
bool T2passFirst::update(const TencFrameParams &frame)
{
 return codec_2pass_update(&frame)==ICERR_OK;
}

//================================= T2passSecond =================================
T2passSecond::T2passSecond(IffdshowEnc *IdeciE,bool Iquiet):quiet(Iquiet),deciE(IdeciE),deci(IdeciE)
{
 start();
}
T2passSecond::T2passSecond(IffdshowEnc *IdeciE,TxvidStats *IstatsRead,TxvidStats *IstatsRead2,TxvidStats *IstatsWrite,bool Iquiet):quiet(Iquiet),deciE(IdeciE),deci(IdeciE)
{
 statsRead =IstatsRead;
 statsRead2=IstatsRead2;
 statsWrite=IstatsWrite;
 start();
}
T2passSecond::~T2passSecond()
{
 codec_2pass_finish();
}
void T2passSecond::start(void)
{
 const TcoSettings *Iconfig;deciE->getCoSettingsPtr(&Iconfig);
 config=*Iconfig;

 twopass.max_framesize=int((double)config.twopass_max_bitrate/8.0/((double)deci->getParam2(IDFF_enc_fpsRate)/(double)deci->getParam2(IDFF_enc_fpsScale)));
 framenum=0;

 memset(quant_threshs,0,sizeof(quant_threshs));

 codec_2pass_init();
}
void T2passSecond::DEBUG2P(const char_t*s)
{
 if (!quiet) deci->dbgWrite(_l("%s"),s);
}
bool T2passSecond::getQuantSecond(TencFrameParams &params)
{
 framenum=params.framenum;
 int ret=codec_2pass_get_quant(&params);
 return ret==ICERR_OK;
}
int T2passSecond::getQuantFirst(void)
{
 return twopass.nns1.quant&NNSTATS_QUANTMASK;
}
unsigned int T2passSecond::getBytesFirst(void)
{
 return twopass.bytes1;
}
void  T2passSecond::writeInfo(const TencFrameParams &params)
{
 deci->dbgWrite(_l("2nd-pass: quant:%d %s %s stats1:%d scaled:%d actual:%d overflow:%d %s\n"),params.quant,encQuantTypes[params.quanttype],FRAME_TYPE::name(params.frametype),twopass.bytes1,twopass.bytes2,params.length,twopass.overflow,config.isInCredits(params.framenum)?_l("credits"):_l("movie"));
}
bool T2passSecond::update(const TencFrameParams &frame)
{
 codec_2pass_update(&frame);
 return true;
}
int T2passSecond::codec_is_in_credits(int framenum)
{
 return config.isInCredits(framenum);
}
