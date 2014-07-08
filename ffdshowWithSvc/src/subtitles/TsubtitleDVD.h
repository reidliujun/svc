#ifndef _TSUBTITLEDVD_H_
#define _TSUBTITLEDVD_H_

#include "Tsubtitle.h"
#include "autoptr.h"
#include "Crect.h"
#include "TffRect.h"
#include "TspuImage.h"

struct Tstream;
struct TsubtitleDVDparent
{
private:
 TspuPlane planes[3];
 int tridx;
protected:
 int delay;
 bool forced_subs;
 bool custom_colors;AM_DVD_YUV cuspal[4];
 enum PARSE_RES
  {
   PARSE_OK,
   PARSE_ERROR,
   PARSE_IGNORE
  };
 PARSE_RES idx_parse_delay(const char *line),idx_parse_size(const char *line),idx_parse_origin(const char *line),idx_parse_forced_subs(const char *line),idx_parse_palette(const char *line),idx_parse_custom(const char *line),idx_parse_tridx(const char *line),idx_parse_cuspal(const char *line);
 virtual PARSE_RES idx_parse_one_line(const char *line);
public:
 TsubtitleDVDparent(void);
 TsubtitleDVDparent::PARSE_RES idx_parse(Tstream &fs);
 Trect rectOrig;
 autoptr<AM_PROPERTY_SPHLI> psphli;
 AM_DVD_YUV sppal[16];bool fsppal;
 bool spon;
 TspuPlane* allocPlanes(const CRect &r);

 static const char* parseTimestamp(const char* &line,int &ms);
};

struct TsubtitleDVD :public Tsubtitle
{
private:
 bool forced;
 mutable TspuImage *image;
protected:
 TsubtitleDVDparent *parent;
 TbyteBuffer data;
 virtual bool parse(void);
 AM_PROPERTY_SPHLI sphli;
 DWORD offset[2];
 static BYTE getNibble(const BYTE *p,DWORD *offset,int &nField,int &fAligned);
 static BYTE getHalfNibble(const BYTE *p,DWORD *offset,int &nField,int &n);
 void drawPixel(const CPoint &pt,const AM_DVD_YUV &c,CRect &rectReal,TspuPlane plane[3]) const;
 void drawPixels(CPoint pt,int len,const AM_DVD_YUV &c,const CRect &rc,CRect &rectReal,TspuPlane plane[3]) const;
 mutable TrenderedSubtitleLines lines;
 void createImage(const TspuPlane src[3],const CRect &rcclip,CRect rectReal,const TrenderedSubtitleLines::TprintPrefs &prefs) const;
 void linesprint(const TrenderedSubtitleLines::TprintPrefs &prefs) const;
public:
 static REFERENCE_TIME pts2rt(int pts)
  {
   return 10000LL*pts/90;
  }

 TsubtitleDVD(REFERENCE_TIME start,const unsigned char *Idata,unsigned int Idatalen,TsubtitleDVDparent *Iparent);
 virtual ~TsubtitleDVD();
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const;
 virtual Tsubtitle* copy(void) {return new TsubtitleDVD(*this);}
 virtual Tsubtitle* create(void) {return new TsubtitleDVD(0,NULL,0,parent);}
 virtual void append(const unsigned char *data,unsigned int datalen);

 autoptr<AM_PROPERTY_SPHLI> psphli;
 mutable bool changed;
};

struct TsubtitleSVCD :public TsubtitleDVD
{
protected:
 virtual bool parse(void);
 AM_DVD_YUV sppal[4];
public:
 TsubtitleSVCD(REFERENCE_TIME start,const unsigned char *Idata,unsigned int Idatalen,TsubtitleDVDparent *Iparent):TsubtitleDVD(start,Idata,Idatalen,Iparent)
  {
   memset(sppal,0,sizeof(sppal));
  }
 virtual Tsubtitle* copy(void) {return new TsubtitleSVCD(*this);}
 virtual Tsubtitle* create(void) {return new TsubtitleSVCD(0,NULL,0,parent);}
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs) const;
};

struct TsubtitleCVD :public TsubtitleDVD
{
private:
 AM_DVD_YUV sppal[2][4];
protected:
 virtual bool parse(void);
public:
 TsubtitleCVD(REFERENCE_TIME start,const unsigned char *Idata,unsigned int Idatalen,TsubtitleDVDparent *Iparent):TsubtitleDVD(start,Idata,Idatalen,Iparent)
  {
   memset(sppal,0,sizeof(sppal));
  }
 virtual Tsubtitle* copy(void) {return new TsubtitleCVD(*this);}
 virtual Tsubtitle* create(void) {return new TsubtitleCVD(0,NULL,0,parent);}
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs) const;
};

#endif
