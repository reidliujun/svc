/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on VideoLAN headphone virtual spatialization channel mixer module by Boris Dorès
 * HRTF by ylai
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
#include "TaudioFilterHeadphone.h"
#include "TmixerSettings.h"
#include "firfilter.h"
#include "TfirSettings.h"

TaudioFilterHeadphone::TaudioFilterHeadphone(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 oldfmt.freq=0;olddim=-1;
 p_sys=NULL;
 inited=false;
}
void TaudioFilterHeadphone::done(void)
{
 if (p_sys)
  {
   if (p_sys->p_overflow_buffer) free(p_sys->p_overflow_buffer);
   if (p_sys->p_atomic_operations) free(p_sys->p_atomic_operations);
   free(p_sys);p_sys=NULL;
  }
}

/*****************************************************************************
 * Init: initialize internal data structures
 * and computes the needed atomic operations
 *****************************************************************************/
/* x and z represent the coordinates of the virtual speaker
 *  relatively to the center of the listener's head, measured in meters :
 *
 *  left              right
 *Z
 *-
 *a          head
 *x
 *i
 *s
 *  rear left    rear right
 *
 *          x-axis
 *  */
void TaudioFilterHeadphone::aout_filter_sys_t::ComputeChannelOperations (
        unsigned int i_rate , unsigned int i_next_atomic_operation
        , int i_source_channel_offset , double d_x , double d_z
        , double d_channel_amplitude_factor )
{
    double d_c = 340; /*sound celerity (unit: m/s)*/

    /* Left ear */
    p_atomic_operations[i_next_atomic_operation]
        .i_source_channel_offset = i_source_channel_offset;
    p_atomic_operations[i_next_atomic_operation]
        .i_dest_channel_offset = 0;/* left */
    p_atomic_operations[i_next_atomic_operation]
        .i_delay = (int)( sqrt( (-0.1-d_x)*(-0.1-d_x) + (0-d_z)*(0-d_z) )
                          / d_c * i_rate );
    if ( d_x < 0 )
    {
        p_atomic_operations[i_next_atomic_operation]
            .d_amplitude_factor = d_channel_amplitude_factor * 1.1 / 2;
    }
    else if ( d_x > 0 )
    {
        p_atomic_operations[i_next_atomic_operation]
            .d_amplitude_factor = d_channel_amplitude_factor * 0.9 / 2;
    }
    else
    {
        p_atomic_operations[i_next_atomic_operation]
            .d_amplitude_factor = d_channel_amplitude_factor / 2;
    }

    /* Right ear */
    p_atomic_operations[i_next_atomic_operation + 1]
        .i_source_channel_offset = i_source_channel_offset;
    p_atomic_operations[i_next_atomic_operation + 1]
        .i_dest_channel_offset = 1;/* right */
    p_atomic_operations[i_next_atomic_operation + 1]
        .i_delay = (int)( sqrt( (0.1-d_x)*(0.1-d_x) + (0-d_z)*(0-d_z) )
                          / d_c * i_rate );
    if ( d_x < 0 )
    {
        p_atomic_operations[i_next_atomic_operation + 1]
            .d_amplitude_factor = d_channel_amplitude_factor * 0.9 / 2;
    }
    else if ( d_x > 0 )
    {
        p_atomic_operations[i_next_atomic_operation + 1]
            .d_amplitude_factor = d_channel_amplitude_factor * 1.1 / 2;
    }
    else
    {
        p_atomic_operations[i_next_atomic_operation + 1]
            .d_amplitude_factor = d_channel_amplitude_factor / 2;
    }
}

int TaudioFilterHeadphone::aout_filter_sys_t::Init(const TsampleFormat &fmt,const TmixerSettings *cfg)
{
    double d_x = cfg->headphone_dim;//config_GetInt ( p_filter , "headphone-dim" );
    double d_z = d_x;
    double d_z_rear = -d_x/3;
    unsigned int i_next_atomic_operation;
    int i_source_channel_offset;

    /* Number of elementary operations */
    i_nb_atomic_operations = fmt.nchannels * 2;
    p_atomic_operations = (atomic_operation_t*)malloc ( sizeof(atomic_operation_t)
            * i_nb_atomic_operations );
    /* For each virtual speaker, computes elementary wave propagation time
     * to each ear */
    i_next_atomic_operation = 0;
    i_source_channel_offset = 0;
    for (unsigned int ch=0;ch<fmt.nchannels;ch++)
     if ( fmt.speakers[ch] == SPEAKER_FRONT_LEFT)
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , -d_x , d_z , 2.0 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] ==  SPEAKER_FRONT_RIGHT )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , d_x , d_z , 2.0 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] == SPEAKER_BACK_LEFT )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , -d_x , d_z_rear , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] == SPEAKER_BACK_RIGHT )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , d_x , d_z_rear , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] ==  SPEAKER_BACK_CENTER )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , 0 , -d_z , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] ==  SPEAKER_FRONT_CENTER )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , 0 , d_z , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] == SPEAKER_LOW_FREQUENCY )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , 0 , d_z_rear , 5.0 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] ==  SPEAKER_SIDE_LEFT )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , -d_x , 0 , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }
     else if ( fmt.speakers[ch] == SPEAKER_SIDE_RIGHT )
      {
          ComputeChannelOperations ( fmt.freq
                  , i_next_atomic_operation , i_source_channel_offset
                  , d_x , 0 , 1.5 / fmt.nchannels );
          i_next_atomic_operation += 2;
          i_source_channel_offset++;
      }

    /* Initialize the overflow buffer
     * we need it because the process induce a delay in the samples */
    i_overflow_buffer_size = 0;
    for (unsigned int i= 0 ; i < i_nb_atomic_operations ; i++ )
    {
        if ( i_overflow_buffer_size
                < p_atomic_operations[i].i_delay * ((fmt.nchannels >= 2) ? fmt.nchannels : 2)
                * sizeof (float) )
        {
            i_overflow_buffer_size
                = p_atomic_operations[i].i_delay * ((fmt.nchannels >= 2) ? fmt.nchannels : 2)
                * sizeof (float);
        }
    }
    p_overflow_buffer = (byte_t*)malloc ( i_overflow_buffer_size );
    memset ( p_overflow_buffer , 0 , i_overflow_buffer_size );

    /* end */
    return 0;
}

bool TaudioFilterHeadphone::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 if (super::getOutputFmt(fmt,cfg))
  {
   fmt.setChannels(2);
   return true;
  }
 else
  return false;
}

HRESULT TaudioFilterHeadphone::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TmixerSettings *cfg=(const TmixerSettings*)cfg0;

 if (!p_sys || oldfmt!=fmt || olddim!=cfg->headphone_dim)
  {
   oldfmt=fmt;olddim=cfg->headphone_dim;
   done();
   /* Allocate the memory needed to store the module's structure */
   p_sys=(aout_filter_sys_t*)malloc(sizeof(aout_filter_sys_t));
   p_sys->i_overflow_buffer_size=0;
   p_sys->p_overflow_buffer=NULL;
   p_sys->i_nb_atomic_operations=0;
   p_sys->p_atomic_operations=NULL;
   inited=p_sys->Init(fmt,cfg)>=0;
  }

 if (inited)
  {
   float *p_in=(float*)init(cfg,fmt,samples0,numsamples);
   unsigned int i_input_nb=fmt.nchannels;
   fmt.setChannels(2);
   float *p_out=(float*)alloc_buffer(fmt,numsamples,buf);
   unsigned int i_output_nb=fmt.nchannels;

   /* Slide the overflow buffer */
   byte_t *p_overflow = p_sys->p_overflow_buffer;
   size_t i_overflow_size = p_sys->i_overflow_buffer_size;
   size_t i_out_size=numsamples*fmt.blockAlign();

   memset ( p_out , 0 , i_out_size );
   if ( i_out_size > i_overflow_size )
    memcpy ( p_out , p_overflow , i_overflow_size );
   else
    memcpy ( p_out , p_overflow , i_out_size );

   byte_t *p_slide = p_sys->p_overflow_buffer;
   while ( p_slide < p_overflow + i_overflow_size )
    {
     if ( p_slide + i_out_size < p_overflow + i_overflow_size )
      {
       memset ( p_slide , 0 , i_out_size );
       if ( p_slide + 2 * i_out_size < p_overflow + i_overflow_size )
        memcpy ( p_slide , p_slide + i_out_size , i_out_size );
       else
        memcpy ( p_slide , p_slide + i_out_size , p_overflow + i_overflow_size - ( p_slide + i_out_size ) );
      }
     else
      {
       memset ( p_slide , 0 , p_overflow + i_overflow_size - p_slide );
      }
     p_slide += i_out_size;
    }

   /* apply the atomic operations */
   for ( unsigned int i = 0 ; i < p_sys->i_nb_atomic_operations ; i++ )
    {
     /* shorter variable names */
     int i_source_channel_offset = p_sys->p_atomic_operations[i].i_source_channel_offset;
     int i_dest_channel_offset = p_sys->p_atomic_operations[i].i_dest_channel_offset;
     unsigned int i_delay = p_sys->p_atomic_operations[i].i_delay;
     double d_amplitude_factor = p_sys->p_atomic_operations[i].d_amplitude_factor;

     if ( numsamples > i_delay )
      {
       unsigned int j;
       /* current buffer coefficients */
       for ( j = 0 ; j < numsamples - i_delay ; j++ )
        {
         p_out[ (i_delay+j)*i_output_nb + i_dest_channel_offset ]
             += float(p_in[ j * i_input_nb + i_source_channel_offset ]
                * d_amplitude_factor);
        }

       /* overflow buffer coefficients */
       for ( j = 0 ; j < i_delay ; j++ )
        {
         ((float*)p_overflow)[ j*i_output_nb + i_dest_channel_offset ]
             += float(p_in[ (numsamples - i_delay + j)
                * i_input_nb + i_source_channel_offset ]
                * d_amplitude_factor);
        }
      }
     else
      {
       /* overflow buffer coefficients only */
       for ( unsigned int j = 0 ; j < numsamples ; j++ )
        {
         ((float*)p_overflow)[ (i_delay - numsamples + j)
             * i_output_nb + i_dest_channel_offset ]
             += float(p_in[ j * i_input_nb + i_source_channel_offset ]
                * d_amplitude_factor);
        }
      }
    }
   samples0=p_out;
  }
 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}

void TaudioFilterHeadphone::onSeek(void)
{
 if (p_sys)
  memset(p_sys->p_overflow_buffer , 0 , p_sys->i_overflow_buffer_size );
}

//=====================================
const float TaudioFilterHeadphone2::cf_filt[128] = {
   -0.00008638082319075036f, 0.0003198059946385229f,
   -0.0005010631339162132f, 0.0011424741331126876f,
   -0.001584220794688753f, 0.001742715363246275f,
   -0.0011080796626780694f, 0.0001651829990860167f,
   0.005235028337314985f, 0.0035223828473357776f,
   0.010057681388431303f, -0.033469432129545514f,
   0.013391253316233523f, 0.004858462839827063f,
   0.08172161220103627f, 0.26158596134500023f,
   -0.12420314583323326f, -0.14298458356097565f,
   0.14421897280453896f, -0.1635792507629016f,
   -0.02187136722480014f, 0.2426863044711817f,
   0.07229814207917194f, 0.0942742651913879f,
   0.29856830878076834f, 0.2944146162057754f,
   -0.12122157003421209f, -0.19640092165631157f,
   0.11623836502034968f, -0.05794027397995521f,
   -0.34313138341973776f, -0.19487516249168105f,
   0.010118993953802401f, -0.09460218797710966f,
   -0.16761521117359582f, 0.004154461610153861f,
   0.052768641758969316f, -0.00041823982226147407f,
   0.021634960445143514f, 0.07562793486871108f,
   0.08895407129506479f, 0.039857755093416214f,
   0.044257936180422945f, 0.061557584906101664f,
   0.015547268541895703f, -0.023908191934932484f,
   0.009498030443468223f, 0.03816269003221337f,
   -0.009820500607303615f, -0.042003975527908084f,
   -0.03335447117311547f, -0.029294510859746596f,
   -0.05212623136198511f, -0.073427547153205f,
   -0.061190797824958836f, -0.04620925059966413f,
   -0.04204619420702159f, -0.02331915902615157f,
   0.00211481411477094f, 0.00852563995740107f,
   0.008766809731743399f, 0.008666632180812078f,
   0.018279202191625352f, 0.02924332751289675f,
   0.022293148257836494f, 0.012362146008584188f,
   0.008572582458807008f, 0.006491370763597344f,
   -0.0019366944997535774f, -0.006318669309634434f,
   -0.006457921690218669f, -0.015050265524669974f,
   -0.02110660282616213f, -0.017027809096377904f,
   -0.01651052305334348f, -0.022770064150046673f,
   -0.01999875754219472f, -0.012294792027337775f,
   -0.011506057031057188f, -0.011448970577312903f,
   -0.004823572302580925f, 0.0022451134042777883f,
   0.004145473526859826f, 0.005629030064546135f,
   0.008588029213398785f, 0.010092048834844231f,
   0.007182013245552008f, 0.0014600979508720656f,
   -0.0038314646272511756f, -0.003443901997881347f,
   -0.0029483418254804047f, -0.007609357112679647f,
   -0.006518368948030822f, -0.004495803701497202f,
   -0.007109113004849672f, -0.008346237278084265f,
   -0.005560847336252453f, -0.002993453167553188f,
   -0.005122897816824269f, -0.004389782626604215f,
   -0.0010912633695218108f, -0.0019712029474458835f,
   -0.005870162265802235f, -0.005626159534954128f,
   -0.00027254977910844407f, 0.0013794425431633785f,
   -0.0005919083190430062f, -0.0007861203545416682f,
   -0.0007049560240893946f, -0.0032720188494468868f,
   -0.004460645567968504f, -0.0032018528193571696f,
   -0.0030579229375062105f, -0.003593998902656612f,
   -0.0038032977997776445f, -0.004613776010454773f,
   -0.0059796549143736845f, -0.00420126194319768f,
   -0.0012374419948287222f, 0.0008572699213050608f,
   0.0021490971020081094f, 0.00047295283198381995f,
   -0.0009670277915884887f, -0.001354440866080231f,
   -0.002962902746547851f, -0.00533935813338141f,
   -0.005469203016468759f, -0.004355784273189485f
};
/* Front,   same side (30 degree) */
const float TaudioFilterHeadphone2::af_filt[128] = {
   -0.004140580614755493f, 0.005790934614385445f,
   0.003318916682081112f, 0.014257145544366063f,
   0.007328442487127339f, -0.06550381777876194f,
   0.03502225818161845f, -0.013090579770708259f,
   0.2517776798694195f, 0.420770489950659f,
   -0.3418854608834852f, -0.24371032493696737f,
   0.04901356150030018f, -0.1778083521632833f,
   0.26448004245714163f, 0.23245199964546834f,
   0.033053145803936305f, 0.46811222821062415f,
   0.5359265986255611f, -0.011912195468533787f,
   -0.39763432601411647f, -0.034482864386898314f,
   0.029445398240649626f, -0.3850940407863262f,
   -0.3272328478175581f, -0.14701421403616477f,
   -0.08522137400169517f, -0.14936851633336035f,
   -0.09432605283433723f, 0.0991200405937827f,
   0.011075012089917331f, -0.0051036489980274685f,
   0.0933903289749412f, 0.1344189369609565f,
   0.10507466913017807f, 0.04240159324684365f,
   0.06559270110638656f, 0.026984119875617524f,
   -0.03359846103238096f, -0.018000197099174275f,
   0.042031818548178244f, 0.03849039666888434f,
   -0.02450829674011345f, -0.03407882403088576f,
   -0.029230189282961977f, -0.046964865291761734f,
   -0.09458258700116245f, -0.1527349330901158f,
   -0.15411577687826097f, -0.08761679790956928f,
   -0.033623549089171874f, -0.007204768531481949f,
   0.008273425020444852f, 0.021368717994908505f,
   0.04366608267875025f, 0.05660907333076205f,
   0.06775726495503939f, 0.05621881735271431f,
   0.03576231950669927f, 0.02500825834889175f,
   0.015423811076054601f, 0.007903258334503761f,
   -0.0053873014137761945f, -0.006987955469434698f,
   -0.012027972007598602f, -0.025228281243816594f,
   -0.026225091797257318f, -0.023809293997344882f,
   -0.03250172017712295f, -0.03195696301067249f,
   -0.01784813952189948f, -0.01663267233760342f,
   -0.016184530450468065f, -0.011659883749357463f,
   -0.0035378511240219163f, -0.0005485800790443406f,
   0.0018432660108168625f, 0.011634844139907534f,
   0.018333603402051105f, 0.020447379185133056f,
   0.00850783664147828f, 0.0004694148911037838f,
   -0.0017047130409786676f, -0.0022409152834483997f,
   -0.000860472174892845f, -0.004111075059198666f,
   -0.003527843382056666f, -0.009640160874903018f,
   -0.01750044574231376f, -0.015613389403672443f,
   -0.010790028120953001f, -0.0095313499955768f,
   -0.007469721416726809f, -0.0019186578145117315f,
   -0.00014977322572890802f, -0.0029803838028179728f,
   -0.006520567233727221f, 0.000035015132033882596f,
   0.009245098100543752f, 0.009896930052308656f,
   0.008316744929565786f, 0.004575207140193997f,
   -0.0000647420103997081f, -0.004502916832871627f,
   -0.004225962213251224f, -0.002886014126381486f,
   -0.006416834142585976f, -0.007156609995423569f,
   -0.008840274447579472f, -0.01441763751386817f,
   -0.015435817484659574f, -0.00924487254924743f,
   -0.0021571721940235205f, 0.0028540722992305453f,
   0.00273577475088536f, -0.000788412365513805f,
   -0.0032650029728365907f, -0.003880217646231338f,
   -0.0035302087299613778f, -0.0038531436176586246f,
   -0.0011921632190514074f, -0.0020722967099011938f,
   -0.004985351145629344f, -0.0042375588844648735f,
   -0.003030360463006021f, -0.0014161075428041471f,
   -0.0005083025643192044f, 0.00035096963769606926f
};
/* Front,   opposite (-30 degree) */
const float TaudioFilterHeadphone2::of_filt[128] = {
   -0.000013472538374193126f, -0.00008048061877079751f,
   0.000043927265781258155f, -0.000017931700794858892f,
   -0.000034774602476112886f, -0.00009576223008735474f,
   0.0001557797638630691f, -0.00018742885883751094f,
   0.00026512448626705716f, -0.0001451040203319678f,
   -0.00008263233117758043f, 0.0006486245853639179f,
   -0.0010631408451846698f, 0.0026571994100746143f,
   0.0014179177997092787f, 0.0062326502956616256f,
   -0.008194149324545333f, -0.006568029415878379f,
   0.009538759710818582f, 0.012309193558632693f,
   0.12336638055838955f, 0.046164307101829005f,
   -0.10228706407884815f, 0.04047687260345798f,
   -0.00296595313977046f, -0.07057949208414134f,
   0.08172114840714612f, 0.08736490764127891f,
   0.05105250431333021f, 0.11627179512747428f,
   0.20717888490340705f, 0.09375052213570291f,
   -0.09784374168330194f, -0.010493571845901443f,
   0.053131894303891716f, -0.10157443971694806f,
   -0.16264032634244974f, -0.05402369511361273f,
   -0.0274403608654217f, -0.09860277022495063f,
   -0.06841875821090282f, -0.004434574400066223f,
   -0.0005222661652743502f, -0.006231881259827263f,
   0.014410397820340159f, 0.04885649512730243f,
   0.04361962569042684f, 0.03399214029009391f,
   0.04961073933475931f, 0.04067325604132289f,
   0.007850647519227257f, 0.004564440466905299f,
   0.02257107958021618f, 0.008183791928884486f,
   -0.014913479343180557f, -0.018685938460856224f,
   -0.01745737397226911f, -0.02327177054233603f,
   -0.03723048632685227f, -0.044739390162299685f,
   -0.042651220125613766f, -0.03730017561004743f,
   -0.029039465434276192f, -0.01885087458914294f,
   -0.01207127752277769f, -0.006779800724164512f,
   -0.001930416967444157f, 0.000029454577995528385f,
   0.0013822760965755472f, 0.0014799128583230202f,
   0.0002068200609199832f, 0.0022254295286201083f,
   0.005143858159434566f, 0.0018580542060917013f,
   -0.0019426046325146259f, -0.0014464042108543495f,
   -0.0034430083560735582f, -0.009692758426099499f,
   -0.011840035292593485f, -0.010716508855893968f,
   -0.012939889036853034f, -0.0121846427935653f,
   -0.006198503315630782f, -0.0023186723099380305f,
   -0.002679872498314837f, -0.003086020446226295f,
   -0.0015709623347698936f, -0.0008147490468332398f,
   -0.0012384575726770983f, -0.0005212877089109362f,
   0.0017707578744906142f, 0.001324932723905786f,
   -0.0017023653780617696f, -0.0045108927752919425f,
   -0.005422155613096912f, -0.0039489323837623835f,
   -0.005295995750506547f, -0.00629706566356189f,
   -0.004685732198036754f, -0.0048076735568143f,
   -0.005978864279217503f, -0.005928999306332966f,
   -0.004187703549017582f, -0.003213999896976475f,
   -0.0028068699816073414f, -0.0010889703907593833f,
   0.0003276714243495386f, -0.0013015007040186994f,
   -0.003208050402434782f, -0.0025115319088208545f,
   -0.0013787553006401076f, -0.0018279087370218635f,
   -0.0025904836507747754f, -0.002071221947222004f,
   -0.0026424212922485594f, -0.0039837031817577f,
   -0.0041635566057380965f, -0.004355223489150822f,
   -0.004350395332709937f, -0.0036693292471930263f,
   -0.003386384394185026f, -0.003972568655001128f,
   -0.004332336840023821f, -0.002648767912111827f,
   -0.001384410080218114f, -0.0011353792711849466f,
   -0.0013726264946164232f, -0.0020075119315034313f
};
/* Rear,   same side (135 degree) */
const float TaudioFilterHeadphone2::ar_filt[128] = {
   0.004573315040810066f, 0.0013592578059426913f,
   0.01553271930902704f, -0.0002356117224941317f,
   -0.05746098219774702f, 0.03430688963370445f,
   0.00808371687447385f, 0.21893535841158596f,
   0.2984357591724814f, -0.3302799746504719f,
   -0.3194029149806245f, 0.21633225051331056f,
   0.24371260938097083f, -0.08843705549751085f,
   0.03939684701343366f, 0.45386926431114494f,
   0.07599118140753386f, -0.18114706160474578f,
   0.285640624686038f, 0.4049515236666218f,
   -0.05347890222071792f, -0.31464359045319074f,
   -0.1033502246468194f, -0.04553593949283157f,
   -0.1880747731157464f, -0.13629090230626037f,
   -0.10435789106123239f, -0.19818232801888755f,
   -0.16701805476330397f, -0.022793111199284f,
   0.058049696762683685f, 0.007048321372693906f,
   -0.002966419183225961f, 0.10140569697797763f,
   0.11648999956673124f, 0.05218347182779882f,
   0.028427001212735392f, 0.04151900310166159f,
   -0.0006960604221423734f, -0.05898623212226975f,
   -0.03801934531173312f, -0.029306970535287986f,
   -0.04549125782835908f, -0.0599222718506552f,
   -0.058299618975430116f, -0.03765579129720727f,
   -0.03559302657499581f, -0.020647901025903054f,
   -0.005720957338744348f, -0.0041915732688915545f,
   -0.0011470880098346143f, 0.008737404798553f,
   0.023444168098121512f, 0.024204226042172663f,
   0.01894897166475026f, 0.020807655257479588f,
   0.021570431128040954f, 0.006800556178576289f,
   -0.009000089216921362f, -0.010969824547067934f,
   -0.0033653428332822374f, -0.012676936164668659f,
   -0.026739938673413587f, -0.023427869194287573f,
   -0.023302007105117244f, -0.023647155590533712f,
   -0.021289317515613106f, -0.009120487305069884f,
   0.0009251551667728967f, 0.00004285344125653763f,
   -0.00009042365479456271f, 0.00022573242339446494f,
   0.00720168491586098f, 0.007111875505402431f,
   0.003186514817683482f, 0.00810087718334745f,
   0.012619557025922575f, 0.007854726400013397f,
   -0.0024013592881066267f, -0.001452457473161119f,
   -0.0025535188366093945f, -0.012428911627809337f,
   -0.013729251536694145f, -0.0070099675146427344f,
   -0.007165284278706593f, -0.01639289295622301f,
   -0.015831795079778305f, -0.007305768485523729f,
   -0.003608863157004021f, -0.0032640528878698084f,
   0.0030901263998481944f, 0.00749497566124848f,
   0.002515185532327241f, 0.00004840875738621367f,
   0.0017596043486043966f, 0.0046229941553338144f,
   0.0034259167322926096f, 0.003707347634186093f,
   0.0035584806528586328f, -0.0019078936035275198f,
   -0.006057343815214898f, -0.0069262470468817f,
   -0.004345020728618624f, -0.004177623355574794f,
   -0.005373506556122508f, -0.006624933928893836f,
   -0.008679541408588839f, -0.010718719681595322f,
   -0.011392246979594496f, -0.007893917064389902f,
   -0.0027572935365832536f, -0.00006064707149834412f,
   -0.0012512537319656323f, -0.0024501501002409786f,
   -0.0022106788572895998f, -0.00234124933370301f,
   -0.0008953445167066823f, 0.0005393670625637734f,
   -0.00033175600142209297f, -0.004023994309351289f,
   -0.008655472335784443f, -0.009899957354849682f,
   -0.008664952919996412f, -0.00553483124503576f,
   -0.003735336089277662f, -0.002754824348643885f,
   -0.0026884314856593368f, -0.004084181815125924f
};
/* Rear,   opposite (-135 degree) */
const float TaudioFilterHeadphone2::or_filt[128] = {
   0.0001220944028243897f, -0.000021785381808441314f,
   5.823057988603169e-6f, -0.00001217768176447613f,
   -0.00006123604397345513f, 5.574117262531134e-6f,
   -0.00004935331914366778f, 1.1771577934768211e-6f,
   -0.000059236211621095756f, 9.503536190497286e-6f,
   -0.0001494445696103564f, 0.00012248858284145305f,
   -0.0000963975321456313f, 6.017905197665205e-6f,
   0.00003353395360402643f, -0.0001931511015359506f,
   0.0005113536523931485f, -0.0005676652619386114f,
   0.0012057159755477467f, 0.0009370492250339692f,
   0.004596472288877981f, -0.0018831773384237068f,
   -0.008208535225621212f, 0.0038178646400751056f,
   0.008726517739105965f, 0.06664363898418262f,
   0.06788684221502142f, -0.04492315162807267f,
   -0.04019906311255255f, 0.026203059677375153f,
   0.013678129114847544f, -0.014334962223993527f,
   0.010141709596167392f, 0.11559131576945537f,
   0.1586402064538425f, 0.059975334707967023f,
   0.004671725963777715f, 0.031498678282775874f,
   0.014338626006524587f, -0.014749719448472231f,
   -0.02055508237941379f, -0.05407690143992048f,
   -0.07767559836886143f, -0.05029091786216801f,
   -0.030808335706574427f, -0.03401958135442541f,
   -0.030520368430288967f, -0.014168302104259355f,
   0.011907621259989802f, 0.014286081013069f,
   0.006377467879613449f, 0.018546823568277478f,
   0.028297012771618273f, 0.025222339408338186f,
   0.021931611353415138f, 0.019708894333646355f,
   0.01729258494072014f, 0.017468204169564034f,
   0.009729094845051928f, -0.002976992018531901f,
   -0.00956986166277019f, -0.016125733548332074f,
   -0.02934094241442545f, -0.04133767871051455f,
   -0.043536981145416466f, -0.0385966307108608f,
   -0.02784453599342459f, -0.018995135307247116f,
   -0.012849534096536747f, -0.004437491064613308f,
   0.00028385411598204655f, 0.003776874988516643f,
   0.008069432041547833f, 0.008764754183751848f,
   0.008020908861878062f, 0.006830351461360802f,
   0.002218330884267235f, -0.0020478725582339444f,
   -0.003997428121462543f, -0.007066287373515421f,
   -0.00940847412544698f, -0.010938998446237963f,
   -0.011775483016151306f, -0.011391103919484287f,
   -0.010586061195163017f, -0.009842793078929053f,
   -0.007753202010139829f, -0.00569213732353025f,
   -0.006506783349722073f, -0.005346134281903736f,
   -0.003913089814898934f, -0.0036091443854759727f,
   -0.0020328564301266287f, 0.00017932870773467683f,
   0.0032779786679056357f, 0.003969695813293966f,
   0.0020339334412434987f, -0.00011345940675415259f,
   -0.0018344103399567666f, -0.003556764701666365f,
   -0.004263523639408391f, -0.002940568582022133f,
   -0.0034341188272627556f, -0.006023399920020824f,
   -0.0077456903203677865f, -0.007912219312377842f,
   -0.00625202770436523f, -0.00530785086116117f,
   -0.005569722659634311f, -0.004664448462594344f,
   -0.0037747773914077747f, -0.004175649656985592f,
   -0.004659601521384289f, -0.005008602967819641f,
   -0.004730625079902729f, -0.0034039554356604146f,
   -0.0017110333873406587f, -0.0006091938771510242f,
   -0.0016051679050678297f, -0.003312864664007262f,
   -0.004505512715977288f, -0.004152222189861692f,
   -0.003218596419678823f, -0.0027277806209877343f,
   -0.001715005444317267f, -0.0012589960071233749f,
   -0.001852908777923165f, -0.002540339553144362f
};
/* Center rear (180 degree) */
const float TaudioFilterHeadphone2::cr_filt[128] = {
   -0.00005989110716536726f, -0.00022790291829128702f,
   0.0002659166098971966f, -0.0003774772716776257f,
   0.0004540309551867803f, -0.000420238187386368f,
   0.00025518536450885686f, 0.00028285526288953955f,
   -0.001016391007574093f, 0.0028634984299063795f,
   0.0021574799687976045f, 0.01035121276682072f,
   -0.010481720917298163f, -0.013197198495899292f,
   0.0031928225328717195f, 0.02903137843618603f,
   0.1632429772511569f, 0.1047487989875262f,
   -0.10464685060623742f, -0.09260196288035998f,
   -0.007514241993554443f, 0.013596249226741712f,
   -0.019876166508450258f, 0.1255626123599804f,
   0.3648170359521724f, 0.19458249753223478f,
   -0.04434070930031298f, 0.046582528121935265f,
   0.09484427347230277f, -0.03137795311969644f,
   -0.10297437925363695f, -0.09351091015917065f,
   -0.1129521092162064f, -0.14925322995658827f,
   -0.1231466295584665f, -0.06356719756705227f,
   -0.05442277895126282f, -0.07568433015661316f,
   -0.023314932828602003f, 0.04260950721318558f,
   0.02249026315598923f, 0.02048195669571197f,
   0.05651342117268278f, 0.05885038917623213f,
   0.03797102097397795f, 0.011767394419953451f,
   0.00560502503429481f, 0.005051125343961189f,
   -0.012925933188033823f, -0.023918884651306566f,
   -0.013251659441678816f, -0.010694772488866284f,
   -0.03080486448617846f, -0.03661278237783158f,
   -0.0379227303416262f, -0.042189005718490775f,
   -0.026595666344596286f, -0.009759025956801257f,
   -0.002064986663513004f, -0.002420117028098389f,
   -0.006629991977552491f, 0.004619970897631026f,
   0.019450642967537877f, 0.0173521119057514f,
   0.017641425439988062f, 0.02270029598048491f,
   0.018976431925275348f, 0.009299852902290885f,
   -0.001695039371619912f, -0.00675162574265618f,
   -0.009380968871003034f, -0.011208396125485165f,
   -0.01308640049201482f, -0.0165636375633249f,
   -0.022004099870933345f, -0.025173458684139286f,
   -0.016918759559175375f, -0.00865150653575917f,
   -0.006999929082792643f, -0.005454830010518988f,
   -0.0021129521131095317f, 0.00018717090054046307f,
   -0.0002864344788569993f, 0.0017615225381095569f,
   0.006985907557802283f, 0.010577308310476465f,
   0.006466104789306027f, -0.0014988738575948326f,
   -0.0039669755229277195f, -0.0065156971200080235f,
   -0.009343206924192169f, -0.0076430644693577495f,
   -0.004395214976600924f, -0.003052735340422483f,
   -0.007019103043066595f, -0.00974109267696527f,
   -0.007968015032797376f, -0.007801513845528344f,
   -0.007535748903681969f, -0.003543341967287925f,
   0.0015083125553729722f, 0.0023345972556147025f,
   -0.0010043623069557037f, -0.0025295765105203746f,
   -0.0023701840891643634f, -0.0005908186035024362f,
   0.0029826252289082847f, 0.004829048542117764f,
   0.004488360022902081f, 0.00002643748103005408f,
   -0.0042100779212597295f, -0.006170600558114495f,
   -0.007267149164680168f, -0.006825522903494639f,
   -0.006899834372739123f, -0.0073493916110062675f,
   -0.009554351265163382f, -0.011790297433830197f,
   -0.010645796603734424f, -0.0064661575394022106f,
   -0.002026743466524137f, -0.0004337034584909932f,
   -0.0011172647031654614f, -0.0017947816283674731f,
   -0.00255615052036616f, -0.0017721562881944813f,
   -0.0002379619297227554f, 0.0007130120121089036f
};

/* Amplitude scaling factors */
const float TaudioFilterHeadphone2::M17_0DB=0.1414213562f;
const float TaudioFilterHeadphone2::M9_03DB=0.3535533906f;
const float TaudioFilterHeadphone2::M6_99DB=0.4472135955f;
const float TaudioFilterHeadphone2::M4_77DB=0.5773502692f;
const float TaudioFilterHeadphone2::M3_01DB=0.7071067812f;
const float TaudioFilterHeadphone2::M1_76DB=0.8164965809f;

const float TaudioFilterHeadphone2::IRTHRESH=0.001f;	/* Impulse response pruning thresh. */
const float TaudioFilterHeadphone2::BASSGAIN=(float)M_SQRT2;	/* Bass compensation gain */
const float TaudioFilterHeadphone2::BASSCROSS=0.35f;	/* Bass cross talk */

const float TaudioFilterHeadphone2::STEXPAND2=0.07f;	/* Stereo expansion / 2 */

TaudioFilterHeadphone2::TaudioFilterHeadphone2(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 s=NULL;
 oldfmt.freq=0;
}

void TaudioFilterHeadphone2::done(void)
{
 if (s) delete s;s=NULL;
}

bool TaudioFilterHeadphone2::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 if (super::getOutputFmt(fmt,cfg))
  {
   fmt.setChannels(2);
   return true;
  }
 else
  return false;
}

/* Detect when the impulse response starts (significantly) */
int TaudioFilterHeadphone2::pulse_detect(const float *sx)
{
    /* nmax must be the reference impulse response length (128) minus
       s->hrflen */
    const int nmax = 128 - HRTFFILTLEN;
    const float thresh = IRTHRESH;
    int i;

    for(i = 0; i < nmax; i++)
	if(fabs(sx[i]) > thresh)
	    return i;
    return 0;
}

TaudioFilterHeadphone2::af_hrtf_s::af_hrtf_s(const TsampleFormat &fmt):
 dlbuflen(DELAYBUFLEN),
 hrflen(HRTFFILTLEN),
 basslen(BASSFILTLEN),
 lf(DELAYBUFLEN),
 rf(DELAYBUFLEN),
 lr(DELAYBUFLEN),
 rr(DELAYBUFLEN),
 cf(DELAYBUFLEN),
 cr(DELAYBUFLEN),
 ba_l(DELAYBUFLEN),
 ba_r(DELAYBUFLEN),
 la(DELAYBUFLEN),
 ra(DELAYBUFLEN)
{
 cyc_pos = dlbuflen - 1;

 for(int i = 0; i < dlbuflen; i++)
  lf[i] = rf[i] = lr[i] = rr[i] = cf[i] =
  cr[i] = /* la[i] = ra[i] = */ 0;

 cf_ir = cf_filt + (cf_o = pulse_detect(cf_filt));
 af_ir = af_filt + (af_o = pulse_detect(af_filt));
 of_ir = of_filt + (of_o = pulse_detect(of_filt));
 ar_ir = ar_filt + (ar_o = pulse_detect(ar_filt));
 or_ir = or_filt + (or_o = pulse_detect(or_filt));
 cr_ir = cr_filt + (cr_o = pulse_detect(cr_filt));

 //ba_ir = (float*)malloc(basslen * sizeof(float));
 float fc = 2.0f * BASSFILTFREQ / (float)fmt.freq;
 ba_ir=TfirFilter::design_fir(&basslen, &fc, TfirSettings::LOWPASS, TfirSettings::WINDOW_KAISER, float(4*M_PI));
 for(unsigned int i = 0; i < basslen; i++)
  ba_ir[i] *= BASSGAIN;
}
TaudioFilterHeadphone2::af_hrtf_s::~af_hrtf_s()
{
 if(ba_ir) aligned_free(ba_ir);
}
void TaudioFilterHeadphone2::af_hrtf_s::update_ch(const float *in, const int k)
{
 /* 5/5+1 channel sources */
 lf[k] = in[0];
 cf[k] = in[4];
 rf[k] = in[1];
 lr[k] = in[2];
 rr[k] = in[3];

 //cr[k] = in[6];
 //la[k] = in[7];
 //ra[k] = in[8];

 /* We need to update the bass compensation delay line, too. */
 ba_l[k] = in[0] + in[4] + in[2] + in[7];
 ba_r[k] = in[4] + in[1] + in[3] + in[8];
}

inline float af_filter_fir(register unsigned int n, const float* w, const float* x)
{
 return TfirFilter::firfilter(w,0,n,n,x);
}

float TaudioFilterHeadphone2::conv(const int nx, const int nk, const float *sx, const float *sk,const int offset)
{
 /* k = reminder of offset / nx */
 int k = offset >= 0 ? offset % nx : nx + (offset % nx);
 if(nk + k <= nx)
  return af_filter_fir(nk, sx + k, sk);
 else
  return af_filter_fir(nk + k - nx, sx, sk + nx - k) + af_filter_fir(nx - k, sx + k, sk);
}

HRESULT TaudioFilterHeadphone2::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TmixerSettings *cfg=(const TmixerSettings*)cfg0;

 if (!s || oldfmt!=fmt)
  {
   oldfmt=fmt;
   done();
   /* MPlayer's 5 channel layout (notation for the variable):
    *
    * 0: L (LF), 1: R (RF), 2: Ls (LR), 3: Rs (RR), 4: C (CF), matrix
    * encoded: Cs (CR)
    *
    * or: L = left, C = center, R = right, F = front, R = rear*/
   indexes[0]=fmt.findSpeaker(SPEAKER_FRONT_LEFT);
   indexes[1]=fmt.findSpeaker(SPEAKER_FRONT_RIGHT);
   indexes[2]=fmt.findSpeaker(SPEAKER_BACK_LEFT);
   indexes[3]=fmt.findSpeaker(SPEAKER_BACK_RIGHT);
   indexes[4]=fmt.findSpeaker(SPEAKER_FRONT_CENTER);
   indexes[5]=fmt.findSpeaker(SPEAKER_LOW_FREQUENCY);
   //indexes[6]=fmt.findSpeaker(SPEAKER_BACK_CENTER);
   //indexes[7]=fmt.findSpeaker(SPEAKER_SIDE_LEFT);
   //indexes[8]=fmt.findSpeaker(SPEAKER_SIDE_RIGHT);
   s=new af_hrtf_s(fmt);
  }

 float *inbuf=(float*)init(cfg,fmt,samples,numsamples); // Input audio data
 float *end=inbuf+numsamples*fmt.nchannels; // Loop end
 fmt.setChannels(2);
 float *out=(float*)(samples=alloc_buffer(fmt,numsamples,buf));

 const int dblen = s->dlbuflen, hlen = s->hrflen, blen = s->basslen;
 while(inbuf < end)
  {
   const int k = s->cyc_pos;
   for (int i=0 ; i<6 /* 9 */ ; i++)
    in[i]=indexes[i]!=-1 ? inbuf[indexes[i]] : 0.0f;

   s->update_ch(in, k);

   /* Simulate a 7.5 ms -20 dB echo of the center channel in the
      front channels (like reflection from a room wall) - a kind of
      psycho-acoustically "cheating" to focus the center front
      channel, which is normally hard to be perceived as front */
   static const float CFECHOAMPL=M17_0DB;	/* Center front echo amplitude */
   s->lf[k] += CFECHOAMPL * s->cf[(k + CFECHODELAY) % s->dlbuflen];
   s->rf[k] += CFECHOAMPL * s->cf[(k + CFECHODELAY) % s->dlbuflen];

   float common,left,right;

   common = conv(dblen, hlen, &s->cf[0], &s->cf_ir[0], k + s->cf_o);

   left    =
    ( conv(dblen, hlen, &s->lf[0], s->af_ir, k + s->af_o) +
      conv(dblen, hlen, &s->rf[0], s->of_ir, k + s->of_o) +
      conv(dblen, hlen, &s->lr[0], s->ar_ir, k + s->ar_o) +
      conv(dblen, hlen, &s->rr[0], s->or_ir, k + s->or_o) +
      common);
   right   =
    ( conv(dblen, hlen, &s->rf[0], s->af_ir, k + s->af_o) +
      conv(dblen, hlen, &s->lf[0], s->of_ir, k + s->of_o) +
      conv(dblen, hlen, &s->rr[0], s->ar_ir, k + s->ar_o) +
      conv(dblen, hlen, &s->lr[0], s->or_ir, k + s->or_o) +
      common);

	/* Bass compensation for the lower frequency cut of the HRTF.  A
	   cross talk of the left and right channel is introduced to
	   match the directional characteristics of higher frequencies.
	   The bass will not have any real 3D perception, but that is
	   OK (note at 180 Hz, the wavelength is about 2 m, and any
	   spatial perception is impossible). */
	float left_b  = conv(dblen, blen, &s->ba_l[0], s->ba_ir, k);
	float right_b = conv(dblen, blen, &s->ba_r[0], s->ba_ir, k);
	left  += (1 - BASSCROSS) * left_b  + BASSCROSS * right_b;
	right += (1 - BASSCROSS) * right_b + BASSCROSS * left_b;
	/* Also mix the LFE channel (if available) */
	if(indexes[5]!=-1) {
     left  += in[5] * M3_01DB;
     right += in[5] * M3_01DB;
	}

	/* Amplitude renormalization. */
	static const float AMPLNORM=M6_99DB;	/* Overall amplitude renormalization */
    left  *= AMPLNORM;
	right *= AMPLNORM;

    /* "Cheating": linear stereo expansion to amplify the 3D
       perception.  Note: Too much will destroy the acoustic space
       and may even result in headaches. */
    float diff = STEXPAND2 * (left - right);
    out[0] = left  + diff;
    out[1] = right - diff;
	/* Next sample... */
	inbuf += oldfmt.nchannels;
	out += fmt.nchannels;
	s->cyc_pos--;
	if(s->cyc_pos < 0)
     s->cyc_pos += dblen;
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}
