#ifndef _2PASS_H_
#define _2PASS_H_

#include "TcodecSettings.h"
#include "interfaces.h"

struct NNSTATS
{
 // frame length (bytes)
 DWORD bytes;

 // ignored & zero'd by gk xvid specific: dk_v = frame header bytes
 int dk_v, dk_u, dk_y;
 int dd_v, dd_u, dd_y;
 int mk_u, mk_y;
 int md_u, md_y;

 // q used for this frame set bit 31 for keyframe
 int quant;

 // key, motion, not-coded macroblocks
 int kblk, mblk, ublk;

 // lum_noise is actually a double (as 8 bytes) for some reason msvc6.0 stores doubles as 12 bytes!? lum_noise is not used so it doesnt matter
 float lum_noise[2];
};

struct TxvidStats
{
private:
 unsigned int pos;
 const char_t *flnm;
 FILE *fw;
 typedef std::vector<NNSTATS> NNSTATSs;
 NNSTATSs stats;
public:
 bool ok;
 TxvidStats(const char_t *flnm,bool write);
 ~TxvidStats();
 size_t size(void) const;
 void resetRead(void);
 bool readNext(NNSTATS *nns);
 const NNSTATS* operator [](size_t i) const;
 operator const NNSTATS* (void) const;
 size_t write(const NNSTATS *nns);
};

struct TencFrameParams;
struct Txvid_2pass
{
private:
 void DEBUG1ST(...) {}
 void DEBUG2ND(...) {}
 int64_t total_size;
 double bquant_error[32];
 double pquant_error[32];
 double curve_comp_error;
 int last_bquant, last_pquant;
 TxvidStats *ownStatsWrite,*ownStatsRead,*ownStatsRead2;
 enum
  {
   DLG_MODE_CBR=ENC_MODE::CBR,
   DLG_MODE_VBR_QUAL=ENC_MODE::VBR_QUAL,
   DLG_MODE_VBR_QUANT=ENC_MODE::VBR_QUANT,
   DLG_MODE_2PASS_1=ENC_MODE::PASS2_1,
   DLG_MODE_2PASS_2_EXT=ENC_MODE::PASS2_2_EXT,
   DLG_MODE_2PASS_2_INT=ENC_MODE::PASS2_2_INT
  };
 enum
  {
   CREDITS_MODE_RATE=CREDITS_MODE::PERCENT,
   CREDITS_MODE_QUANT=CREDITS_MODE::QUANT,
   CREDITS_MODE_SIZE=CREDITS_MODE::SIZE
  };
 enum
  {
   CREDITS_START=CREDITS_POS::START,
   CREDITS_END=CREDITS_POS::END
  };
protected:
 TcoSettings config;
 TxvidStats *statsWrite,*statsRead,*statsRead2;
 virtual void DEBUGERR(const char_t*) {}
 virtual void DEBUG2P(const char_t*s) {}
 virtual int codec_is_in_credits(int framenum) {return 0;}

 struct
  {
   int bytes1;
   int bytes2;
   int desired_bytes2;

   int keyframe_locations[20480];
   int max_framesize;
   int minbsize, minpsize, minisize;
   double movie_curve;
   double credits_start_curve;
   double credits_end_curve;

   double average_pframe;
   double average_bframe;
   double curve_comp_scale;
   double curve_bias_bonus;
   double alt_curve_low;
   double alt_curve_high;
   double alt_curve_low_diff;
   double alt_curve_high_diff;
   double alt_curve_mid_qual;
   double alt_curve_qual_dev;
   int overflow, KF_idx, KFoverflow, KFoverflow_partial;

   NNSTATS nns1;
   NNSTATS nns2;

   const NNSTATS* nns1_array;
   const NNSTATS* nns2_array;
   int nns_array_length;
   int nns_array_pos;
  } twopass;

 int framenum;

 int codec_2pass_init(void);
 int codec_2pass_get_quant(TencFrameParams *frame);
 int codec_2pass_update(const TencFrameParams *frame);
 void codec_2pass_finish(void);
public:
 enum
  {
   NNSTATS_QUANTMASK=127,
   NNSTATS_KEYFRAME=1<<31,
   NNSTATS_BFRAME=1<<30,
   NNSTATS_SKIPFRAME=1<<29,
   NNSTATS_PADFRAME=1<<28,
   NNSTATS_DELAYFRAME=1<<27
  };
 Txvid_2pass(void);
 virtual ~Txvid_2pass();
 virtual bool getQuantSecond(TencFrameParams &params) {return false;}
 virtual void writeInfo(const TencFrameParams &frame) {};
 virtual bool update(const TencFrameParams &frame)=0;

 struct
  {
   int thresh;
   int percent;
  } quant_threshs[32*2];
};

class T2passFirst :public Txvid_2pass
{
public:
 T2passFirst(const TcoSettings &Iconfig);
 virtual ~T2passFirst();
 virtual bool update(const TencFrameParams &frame);
};

struct IffdshowEnc;
class T2passSecond :public Txvid_2pass
{
private:
 comptrQ<IffdshowBase> deci;comptr<IffdshowEnc> deciE;
 bool quiet;
 void start(void);
protected:
 virtual void DEBUG2P(const char_t*s);
public:
 T2passSecond(IffdshowEnc *IdeciE,bool Iquiet=false);
 T2passSecond(IffdshowEnc *IdeciE,TxvidStats *statsRead,TxvidStats *statsRead2,TxvidStats *statsWrite,bool Iquiet=false);
 virtual ~T2passSecond();
 virtual bool getQuantSecond(TencFrameParams &params);
 int getQuantFirst(void);
 unsigned int getBytesFirst(void);
 virtual void writeInfo(const TencFrameParams &frame);
 virtual bool update(const TencFrameParams &frame);
 virtual int codec_is_in_credits(int framenum);
};

#endif
