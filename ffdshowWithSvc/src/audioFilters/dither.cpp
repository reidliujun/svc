/* This program is licensed under the GNU Library General Public License, version 2,
 *
 * (c) 2002 John Edwards
 * mostly lifted from work by Frank Klemm
 * random functions for dithering.
 *
 */

#include "stdafx.h"
#include "dither.h"

const float  Tdither::F44_0 [16 + 32] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};


const float  Tdither::F44_1 [16 + 32] = {  /* SNR(w) = 4.843163 dBf, SNR = -3.192134 dB */
	 0.85018292704024355931f,  0.29089597350995344721f, -0.05021866022121039450f, -0.23545456294599161833f,
	-0.58362726442227032096f, -0.67038978965193036429f, -0.38566861572833459221f, -0.15218663390367969967f,
	-0.02577543084864530676f,  0.14119295297688728127f,  0.22398848581628781612f,  0.15401727203382084116f,
	 0.05216161232906000929f, -0.00282237820999675451f, -0.03042794608323867363f, -0.03109780942998826024f,

	 0.85018292704024355931f,  0.29089597350995344721f, -0.05021866022121039450f, -0.23545456294599161833f,
	-0.58362726442227032096f, -0.67038978965193036429f, -0.38566861572833459221f, -0.15218663390367969967f,
	-0.02577543084864530676f,  0.14119295297688728127f,  0.22398848581628781612f,  0.15401727203382084116f,
	 0.05216161232906000929f, -0.00282237820999675451f, -0.03042794608323867363f, -0.03109780942998826024f,

	 0.85018292704024355931f,  0.29089597350995344721f, -0.05021866022121039450f, -0.23545456294599161833f,
	-0.58362726442227032096f, -0.67038978965193036429f, -0.38566861572833459221f, -0.15218663390367969967f,
	-0.02577543084864530676f,  0.14119295297688728127f,  0.22398848581628781612f,  0.15401727203382084116f,
	 0.05216161232906000929f, -0.00282237820999675451f, -0.03042794608323867363f, -0.03109780942998826024f,
};


const float  Tdither::F44_2 [16 + 32] = {  /* SNR(w) = 10.060213 dBf, SNR = -12.766730 dB */
	 1.78827593892108555290f,  0.95508210637394326553f, -0.18447626783899924429f, -0.44198126506275016437f,
	-0.88404052492547413497f, -1.42218907262407452967f, -1.02037566838362314995f, -0.34861755756425577264f,
	-0.11490230170431934434f,  0.12498899339968611803f,  0.38065885268563131927f,  0.31883491321310506562f,
	 0.10486838686563442765f, -0.03105361685110374845f, -0.06450524884075370758f, -0.02939198261121969816f,

	 1.78827593892108555290f,  0.95508210637394326553f, -0.18447626783899924429f, -0.44198126506275016437f,
	-0.88404052492547413497f, -1.42218907262407452967f, -1.02037566838362314995f, -0.34861755756425577264f,
	-0.11490230170431934434f,  0.12498899339968611803f,  0.38065885268563131927f,  0.31883491321310506562f,
	 0.10486838686563442765f, -0.03105361685110374845f, -0.06450524884075370758f, -0.02939198261121969816f,

	 1.78827593892108555290f,  0.95508210637394326553f, -0.18447626783899924429f, -0.44198126506275016437f,
	-0.88404052492547413497f, -1.42218907262407452967f, -1.02037566838362314995f, -0.34861755756425577264f,
	-0.11490230170431934434f,  0.12498899339968611803f,  0.38065885268563131927f,  0.31883491321310506562f,
	 0.10486838686563442765f, -0.03105361685110374845f, -0.06450524884075370758f, -0.02939198261121969816f,
};


const float  Tdither::F44_3 [16 + 32] = {  /* SNR(w) = 15.382598 dBf, SNR = -29.402334 dB */
	 2.89072132015058161445f,  2.68932810943698754106f,  0.21083359339410251227f, -0.98385073324997617515f,
	-1.11047823227097316719f, -2.18954076314139673147f, -2.36498032881953056225f, -0.95484132880101140785f,
	-0.23924057925542965158f, -0.13865235703915925642f,  0.43587843191057992846f,  0.65903257226026665927f,
	 0.24361815372443152787f, -0.00235974960154720097f,  0.01844166574603346289f,  0.01722945988740875099f,

	 2.89072132015058161445f,  2.68932810943698754106f,  0.21083359339410251227f, -0.98385073324997617515f,
	-1.11047823227097316719f, -2.18954076314139673147f, -2.36498032881953056225f, -0.95484132880101140785f,
	-0.23924057925542965158f, -0.13865235703915925642f,  0.43587843191057992846f,  0.65903257226026665927f,
	 0.24361815372443152787f, -0.00235974960154720097f,  0.01844166574603346289f,  0.01722945988740875099f,

	 2.89072132015058161445f,  2.68932810943698754106f,  0.21083359339410251227f, -0.98385073324997617515f,
	-1.11047823227097316719f, -2.18954076314139673147f, -2.36498032881953056225f, -0.95484132880101140785f,
	-0.23924057925542965158f, -0.13865235703915925642f,  0.43587843191057992846f,  0.65903257226026665927f,
	 0.24361815372443152787f, -0.00235974960154720097f,  0.01844166574603346289f,  0.01722945988740875099f
};

/*
 *  This is a simple random number generator with good quality for audio purposes.
 *  It consists of two polycounters with opposite rotation direction and different
 *  periods. The periods are coprime, so the total period is the product of both.
 *
 *     -------------------------------------------------------------------------------------------------
 * +-> |31:30:29:28:27:26:25:24:23:22:21:20:19:18:17:16:15:14:13:12:11:10: 9: 8: 7: 6: 5: 4: 3: 2: 1: 0|
 * |   -------------------------------------------------------------------------------------------------
 * |                                                                          |  |  |  |     |        |
 * |                                                                          +--+--+--+-XOR-+--------+
 * |                                                                                      |
 * +--------------------------------------------------------------------------------------+
 *
 *     -------------------------------------------------------------------------------------------------
 *     |31:30:29:28:27:26:25:24:23:22:21:20:19:18:17:16:15:14:13:12:11:10: 9: 8: 7: 6: 5: 4: 3: 2: 1: 0| <-+
 *     -------------------------------------------------------------------------------------------------   |
 *       |  |           |  |                                                                               |
 *       +--+----XOR----+--+                                                                               |
 *                |                                                                                        |
 *                +----------------------------------------------------------------------------------------+
 *
 *
 *  The first has an period of 3*5*17*257*65537, the second of 7*47*73*178481,
 *  which gives a period of 18.410.713.077.675.721.215. The result is the
 *  XORed values of both generators.
 */

unsigned int Tdither::Trandom::__r1 = 1;
unsigned int Tdither::Trandom::__r2 = 1;

const  unsigned char    Tdither::Trandom::Parity [256] = {  // parity
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0
};

unsigned int Tdither::Trandom::random_int ( void )
{
	unsigned int  t1, t2, t3, t4;

	t3   = t1 = __r1;   t4   = t2 = __r2;       // Parity calculation is done via table lookup, this is also available
	t1  &= 0xF5;        t2 >>= 25;              // on CPUs without parity, can be implemented in C and avoid unpredictable
	t1   = Parity [t1]; t2  &= 0x63;            // jumps and slow rotate through the carry flag operations.
	t1 <<= 31;          t2   = Parity [t2];

	return (__r1 = (t3 >> 1) | t1 ) ^ (__r2 = (t4 + t4) | t2 );
}

double Tdither::Trandom::Random_Equi ( double mult )                     // gives a equal distributed random number
{                                               // between -2^31*mult and +2^31*mult
	return mult * (int) random_int ();
}

double Tdither::Trandom::Random_Triangular ( double mult )               // gives a triangular distributed random number
{                                               // between -2^32*mult and +2^32*mult
	return mult * ( (double) (int) random_int () + (double) (int) random_int () );
}

Tdither::Tdither( int bits, int Ishapingtype ):shapingtype(Ishapingtype)
{
	static const unsigned char default_dither [] = { 92, 92, 88, 84, 81, 78, 74, 67,  0,  0 };
	static const float*                     F [] = { F44_0, F44_1, F44_2, F44_3 };
	int                     index;

	if (shapingtype < 0) shapingtype = 0;
	if (shapingtype > 3) shapingtype = 3;
	index = bits - 11 - shapingtype;
	if (index < 0) index = 0;
	if (index > 9) index = 9;

	memset ( ErrorHistory , 0, sizeof (ErrorHistory ) );
	memset ( DitherHistory, 0, sizeof (DitherHistory) );

	FilterCoeff = F [shapingtype];
	Mask   = ((uint64_t)-1) << (32 - bits);
	Add    = 0.5     * ((1L << (32 - bits)) - 1);
	Dither = 0.01f*default_dither[index] / (((int64_t)1) << bits);
}

double Tdither::scalar16 ( const float* x, const float* y )
{
	return x[ 0]*y[ 0] + x[ 1]*y[ 1] + x[ 2]*y[ 2] + x[ 3]*y[ 3]
	     + x[ 4]*y[ 4] + x[ 5]*y[ 5] + x[ 6]*y[ 6] + x[ 7]*y[ 7]
	     + x[ 8]*y[ 8] + x[ 9]*y[ 9] + x[10]*y[10] + x[11]*y[11]
	     + x[12]*y[12] + x[13]*y[13] + x[14]*y[14] + x[15]*y[15];
}

int64_t Tdither::ROUND64(double x)
{
// double doubletmp=x + Add + 0x001FFFFD80000000LL;
// return *(int64_t*)(&doubletmp) - 0x433FFFFD80000000LL;
 return int64_t(x+Add);
}

/* Dither output */
int64_t Tdither::dither_output(int dithering, long i, double Sum, int k)
{
	double Sum2;
	int64_t val;
	if(dithering)
	{
		if(!shapingtype)
		{
			double  tmp = Trandom::Random_Equi ( Dither );
			Sum2 = tmp - LastRandomNumber [k];
			LastRandomNumber [k] = (int)tmp;
			Sum2 = Sum += Sum2;
			val = ROUND64 (Sum2)  &  Mask;
		}
		else
		{
			Sum2  = Trandom::Random_Triangular ( Dither ) - scalar16 ( DitherHistory[k], FilterCoeff + i );
			Sum  += DitherHistory [k] [(-1-i)&15] = (float)Sum2;
			Sum2  = Sum + scalar16 ( ErrorHistory [k], FilterCoeff + i );
			val = ROUND64 (Sum2)  &  Mask;
			ErrorHistory [k] [(-1-i)&15] = (float)(Sum - val);
		}
		return (val);
	}
	else
		return (ROUND64 (Sum));
}

template<class Tout,int conv> Tout* Tdither::ditherSamples(const float *bufIn,Tout * const bufOut,unsigned int nchannels,size_t numsamples,int dithering)
{
 for (unsigned int k=0;k<nchannels;k++)
  {
   const float *bufInF=bufIn+k;
   Tout *bufOutF=bufOut+k;
   long i=0;
   for (size_t j=0;j<numsamples;j++,i++,bufInF+=nchannels,bufOutF+=nchannels)
    {
     if (i>31) i=0;
     int64_t val=dither_output(dithering,i,*bufInF*INT32_MAX,k)>>conv;
     *bufOutF=TsampleFormatInfo<Tout>::limit(val);
    }
  }
 return bufOut;
}
int16_t* Tdither::process(const float *bufIn,int16_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering)
{
 return ditherSamples<int16_t,16>(bufIn,bufOut,nchannels,numsamples,dithering);
}
int24_t* Tdither::process(const float *bufIn,int24_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering)
{
 return ditherSamples<int24_t,8>(bufIn,bufOut,nchannels,numsamples,dithering);
}
int32_t* Tdither::process(const float *bufIn,int32_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering)
{
 return ditherSamples<int32_t,0>(bufIn,bufOut,nchannels,numsamples,dithering);
}
