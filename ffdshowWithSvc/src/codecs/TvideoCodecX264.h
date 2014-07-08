#ifndef _TVIDEOCODECX264_H_
#define _TVIDEOCODECX264_H_

#include "TvideoCodec.h"

class Tdll;
struct x264_t;
struct x264_param_t;
struct x264_picture_t;
struct x264_nal_t;
struct ff_x264_stats_t;
struct x264_frame_t;
class TvideoCodecX264 :public TvideoCodecEnc
{
private:
 Tdll *dll;
 x264_t* (*x264_encoder_open   )( x264_param_t * );
 int     (*x264_encoder_headers)( x264_t *, x264_nal_t **, int * );
 int     (*x264_encoder_encode )( x264_t *, x264_nal_t **, int *, void*,const void *pic_in_fill_data, x264_picture_t *);
 void    (*x264_encoder_close  )( x264_t * );
 void    (*x264_param_default  )( x264_param_t * );
 void    (*x264_picture_alloc  )( x264_picture_t *pic, int i_csp, int i_width, int i_height );
 void    (*x264_picture_clean  )( x264_picture_t *pic );
 int     (*x264_nal_encode     )( void *, int *, int b_annexeb, x264_nal_t *nal );

 x264_t *h;
 static void fill_x264_pict(x264_frame_t *out,const TffPict *in);
 bool lossless;
 mutable char_t codecName[50];
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
public:
 TvideoCodecX264(IffdshowBase *Ideci,IencVideoSink *Isink);
 virtual ~TvideoCodecX264();
 static const char_t *dllname;
 virtual const char_t* getName(void) const;
 virtual int getType(void) const {return IDFF_MOVIE_X264;}
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);
 virtual HRESULT flushEnc(const TffPict &pict,TencFrameParams &params) {return compress(pict,params);}
 virtual void end(void);
};

#endif
