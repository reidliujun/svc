#include "stdafx.h"
#include "mixer.h"

const double TmixerMatrix::LEVEL_PLUS6DB=2.0;
const double TmixerMatrix::LEVEL_PLUS3DB=1.4142135623730951;
const double TmixerMatrix::LEVEL_3DB    =0.7071067811865476;
const double TmixerMatrix::LEVEL_45DB   =0.5946035575013605;
const double TmixerMatrix::LEVEL_6DB    =0.5;

TmixerMatrix::TmixerMatrix(void)
{
 for (int i=0;i<NCHANNELS;i++)
  for (int j=0;j<NCHANNELS;j++)
    matrix[i][j]=0;
}

TsampleFormat TmixerMatrix::calc_matrix(const TsampleFormat &infmt,const TmixerSettings *cfg)
{
 TsampleFormat outfmt=infmt;
 cfg->setFormatOut(outfmt,infmt);

 int in_ch      = infmt.makeChannelMask();
 int out_ch     = outfmt.makeChannelMask();
 int in_nfront  = infmt.nfront();
 int in_nrear   = infmt.nrear();
 int in_nside   = infmt.nside();
 int out_nfront = outfmt.nfront();
 int out_nrear  = outfmt.nrear();
 int out_nside  = outfmt.nside();
 double clev=cfg->clev/100.0;             // central mix level
 double slev=cfg->slev/100.0;             // surround mix level
 double lfelev=cfg->lfelev/100.0;         // lfe mix level

 if (cfg->customMatrix)
  for (int i=0;i<9;i++)
   for (int j=0;j<9;j++)
    matrix[i][j]=((int (*)[9])&cfg->matrix00)[i][j]/100000.0;
 else
  {
   memset(&matrix, 0, sizeof(mixer_matrix_t));
   if (infmt.dolby & outfmt.dolby) // Dolby modes are backwards-compatible
    {
     matrix[CH_L][CH_L] = 1;
     matrix[CH_R][CH_R] = 1;
    }
   else if (outfmt.dolby) // Downmix to Dolby Surround/ProLogic/ProLogicII
    {
     if (in_nfront >= 2)
      {
       matrix[CH_L][CH_L] = 1;
       matrix[CH_R][CH_R] = 1;
      }
     if (in_nfront != 2)
      {
       matrix[CH_C][CH_L] = 0.7071 * clev;
       matrix[CH_C][CH_R] = 0.7071 * clev;
      }
     if (in_nrear == 1)
      {
       matrix[CH_BC][CH_L] = -0.7071 * slev;
       matrix[CH_BC][CH_R] = +0.7071 * slev;
      }
     else if (in_nrear == 2)
      {
       switch (outfmt.dolby)
        {
         case TsampleFormat::DOLBY_PROLOGICII:
          matrix[CH_BL][CH_L] = -0.8660*slev;
          matrix[CH_BR][CH_L] = -0.5000*slev;
          matrix[CH_BL][CH_R] = +0.5000*slev;
          matrix[CH_BR][CH_R] = +0.8660*slev;
          break;
         case TsampleFormat::DOLBY_SURROUND:
         case TsampleFormat::DOLBY_PROLOGIC:
         default:
          matrix[CH_BL][CH_L] = -slev;
          matrix[CH_BR][CH_L] = -slev;
          matrix[CH_BL][CH_R] = +slev;
          matrix[CH_BR][CH_R] = +slev;
          break;
        }
      }
    }
   else // A/52 standart mixes
    {
     // direct route equal channels
     if (in_ch & out_ch & SPEAKER_FRONT_LEFT)   matrix[CH_L ][CH_L ]=1.0;
     if (in_ch & out_ch & SPEAKER_FRONT_RIGHT)  matrix[CH_R ][CH_R ]=1.0;
     if (in_ch & out_ch & SPEAKER_FRONT_CENTER) matrix[CH_C ][CH_C ]=clev;
     if (in_ch & out_ch & SPEAKER_BACK_LEFT)    matrix[CH_BL][CH_BL]=slev;
     if (in_ch & out_ch & SPEAKER_BACK_RIGHT)   matrix[CH_BR][CH_BR]=slev;
     if (in_ch & out_ch & SPEAKER_SIDE_LEFT)    matrix[CH_AL][CH_AL]=slev;
     if (in_ch & out_ch & SPEAKER_SIDE_RIGHT)   matrix[CH_AR][CH_AR]=slev;
     if (in_ch & out_ch & SPEAKER_BACK_CENTER)  matrix[CH_BC][CH_BC]=slev;

     // calc matrix for fbw channels
     if (out_nfront == 1)
      {
       if (in_nfront != 1)
        {
         matrix[CH_L][CH_M]=LEVEL_3DB;
         matrix[CH_R][CH_M]=LEVEL_3DB;
        }
       if (in_nfront == 3)
        {
         matrix[CH_C][CH_M] = clev * LEVEL_PLUS3DB;
        }
       if (in_nrear == 1)
        {
         matrix[CH_BC][CH_M] = slev * LEVEL_3DB;
        }
       else
        {
         matrix[CH_BL][CH_M] = slev * LEVEL_3DB;
         matrix[CH_BR][CH_M] = slev * LEVEL_3DB;
         matrix[CH_AL][CH_M] = slev * LEVEL_3DB;
         matrix[CH_AR][CH_M] = slev * LEVEL_3DB;
        }
      }
     else // not mono modes
      {
       if (out_nfront == 2)
        {
         if (in_nfront == 1)
          {
           matrix[CH_M][CH_L] = LEVEL_3DB;
           matrix[CH_M][CH_R] = LEVEL_3DB;
          }
         else if (in_nfront == 3)
          {
           matrix[CH_C][CH_L] = clev;
           matrix[CH_C][CH_R] = clev;
          }
        }
       if (in_nrear == 1)
        {
         if (out_nrear == 0)
          {
           matrix[CH_BC][CH_L] = slev * LEVEL_3DB;
           matrix[CH_BC][CH_R] = slev * LEVEL_3DB;
          }
         else if (out_nrear == 2)
          {
           matrix[CH_BC][CH_BL] = slev * LEVEL_3DB;
           matrix[CH_BC][CH_BR] = slev * LEVEL_3DB;
          }
        }
       else if (in_nrear == 2)
        {
         if (out_nrear == 0)
          {
           matrix[CH_BL][CH_L] = slev;
           matrix[CH_BR][CH_R] = slev;
          }
         else if (out_nrear == 1)
          {
           matrix[CH_BL][CH_BC] = slev * LEVEL_3DB;
           matrix[CH_BR][CH_BC] = slev * LEVEL_3DB;
          }
        }
       if (in_nside == 2)
        {
         if (out_nside == 0 && in_nrear > 0)
          {
           if (out_nrear == 2)
            {
             matrix[CH_AL][CH_L]  = slev * LEVEL_3DB;
             matrix[CH_AL][CH_BL] = slev * LEVEL_3DB;
             matrix[CH_AR][CH_R]  = slev * LEVEL_3DB;
             matrix[CH_AR][CH_BR] = slev * LEVEL_3DB;
            }
           else if (out_nrear == 1)
            {
             matrix[CH_AL][CH_L] = slev * LEVEL_3DB;
             matrix[CH_AL][CH_BC] = slev * LEVEL_3DB;
             matrix[CH_AR][CH_R] = slev * LEVEL_3DB;
             matrix[CH_AR][CH_BC] = slev * LEVEL_3DB;
            }
           else
            {
             matrix[CH_AL][CH_L] = slev;
             matrix[CH_AR][CH_R] = slev;
            }
          }
         else if (out_nside == 0 && in_nrear == 0)
          {
           if (out_nrear ==2)
            {
             matrix[CH_AL][CH_BL] = slev;
             matrix[CH_AR][CH_BR] = slev;
            }
           else if (out_nrear == 1)
            {
             matrix[CH_AL][CH_BC] = slev;
             matrix[CH_AR][CH_BC] = slev;
            }
           else
            {
             matrix[CH_AL][CH_L] = slev;
             matrix[CH_AR][CH_R] = slev;
            }
          }
        }
      }
    }

   // Expand stereo & Voice control
   bool expand_stereo_allowed = cfg->expandStereo && !in_nrear;
   bool voice_control_allowed = cfg->voiceControl && (in_nfront == 2);

   if ((voice_control_allowed || expand_stereo_allowed) && !outfmt.dolby)
    {
     if (voice_control_allowed && out_nfront != 2)
      {
       // C' = clev * (L + R) * LEVEL_3DB
       matrix[CH_L][CH_M] = clev * LEVEL_3DB;
       matrix[CH_R][CH_M] = clev * LEVEL_3DB;
      }

     if (expand_stereo_allowed && in_nfront == 2 && out_nrear)
      {
       if (out_nrear == 1)
        {
         // S' = slev * (L - R)
         matrix[CH_L][CH_BC] = + slev;
         matrix[CH_R][CH_BC] = - slev;
        }
       if (out_nrear == 2)
        {
         // SL' = slev * 1/2 (L - R)
         // SR' = slev * 1/2 (R - L)
         matrix[CH_L][CH_BL] = + 0.5 * slev;
         matrix[CH_R][CH_BL] = - 0.5 * slev;
         matrix[CH_L][CH_BR] = - 0.5 * slev;
         matrix[CH_R][CH_BR] = + 0.5 * slev;
        }
      }

     if (in_nfront != 1)
      {
       if (expand_stereo_allowed && voice_control_allowed)
        {
         // L' = L * 1/2 (slev + clev) - R * 1/2 (slev - clev)
         // R' = R * 1/2 (slev + clev) - L * 1/2 (slev - clev)
         matrix[CH_L][CH_L] = + 0.5 * (slev + clev);
         matrix[CH_R][CH_L] = - 0.5 * (slev - clev);
         matrix[CH_L][CH_R] = - 0.5 * (slev - clev);
         matrix[CH_R][CH_R] = + 0.5 * (slev + clev);
        }
       else if (expand_stereo_allowed)
        {
         matrix[CH_L][CH_L] = + 0.5 * (slev + 1);
         matrix[CH_R][CH_L] = - 0.5 * (slev - 1);
         matrix[CH_L][CH_R] = - 0.5 * (slev - 1);
         matrix[CH_R][CH_R] = + 0.5 * (slev + 1);
        }
       else // if (voice_control_allowed)
        {
         matrix[CH_L][CH_L] = + 0.5 * (1 + clev);
         matrix[CH_R][CH_L] = - 0.5 * (1 - clev);
         matrix[CH_L][CH_R] = - 0.5 * (1 - clev);
         matrix[CH_R][CH_R] = + 0.5 * (1 + clev);
        }
      }
    }

   // LFE mixing
   if (in_ch & out_ch & SPEAKER_LOW_FREQUENCY)
    matrix[CH_LFE][CH_LFE] = lfelev;

   // mix LFE into front channels if exists in input
   // and absent in output
   if (in_ch & ~out_ch & SPEAKER_LOW_FREQUENCY)
    {
     if (out_nfront > 1)
      {
       matrix[CH_LFE][CH_L]  = lfelev;
       matrix[CH_LFE][CH_R]  = lfelev;
      }
     else
      matrix[CH_LFE][CH_C]  = lfelev;
    }
  }

 if (cfg->normalizeMatrix)
  {
   double levels[NCHANNELS] = { 0, 0, 0, 0, 0, 0 };
   double max_level;
   double norm;
   int i, j;

   for (i = 0; i < NCHANNELS; i++)
    for (j = 0; j < NCHANNELS; j++)
     levels[i] += matrix[j][i];

   max_level = levels[0];
   for (i = 1; i < NCHANNELS; i++)
    if (levels[i] > max_level)
     max_level = levels[i];

   if (max_level > 0)
    norm = 1.0/max_level;
   else
    norm = 1.0;

   for (i = 0; i < NCHANNELS; i++)
    for (j = 0; j < NCHANNELS; j++)
     matrix[j][i] *= norm;
  }

 for (int i = 0; i < NCHANNELS; i++)
  for (int j = 0; j < NCHANNELS; j++)
   matrix[j][i] = limit(matrix[j][i],-4.0,4.0);
 return outfmt;
}
