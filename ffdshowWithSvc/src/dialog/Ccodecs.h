#ifndef _CCODECSPAGE_H_
#define _CCODECSPAGE_H_

#include "TconfPageBase.h"

class TcodecsPage :public TconfPageBase
{
private:
 const char_t *thirdCol;
 int setHints;
 void options2dlg(int i),dlg2options(int id);
 struct TwidgetCodecs : TwindowWidget
  {
  protected:
   TcodecsPage *codecsPage;
   TwidgetCodecs(HWND h,TcodecsPage *Iself):TwindowWidget(h,Iself),codecsPage(Iself) {}
  };
 struct TwidgetCodecsLv : TwidgetCodecs
  {
  protected:
   virtual LRESULT onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onVscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onHscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onPaint(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onRbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetCodecsLv(HWND h,TcodecsPage *Iself):TwidgetCodecs(h,Iself) {allowOwnProc();}
  };
 struct TwidgetCodecsCbx :TwidgetCodecs
  {
  protected:
   virtual LRESULT onKillFocus(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onGetDlgCode(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetCodecsCbx(HWND h,TcodecsPage *Iself):TwidgetCodecs(h,Iself) {allowOwnProc();}
  };
protected:
 enum LAVC_STATUS
  {
   LAVC_STABLE,
   LAVC_UNSTABLE
  };
 struct Tformat
  {
   struct Tdecoder
    {
     const char_t *name;
     int id;
     static Tdecoder init(const char_t *Iname,int Iid)
      {
       Tdecoder d;d.name=Iname;d.id=Iid;
       return d;
      }
    };
   struct Tdecoders : array_vector<Tdecoder,IDFF_MOVIE_MAX*2>
    {
     int find(int id) const
      {
       for (size_t i=0;i<size();i++)
        if (at(i).id==id)
         return (int)i;
       return -1;
      }
    };
   struct Toption
    {
     int forCodec; // 0 - for all
     const char_t *name;
     int id,val;
    };
   typedef array_vector<Toption,20> Toptions;
   Tformat(const char_t *Idescr,int Iidff,const int *movie_sources,const char_t *Ihint=_l(""),const Toption *Ioptions=NULL,LAVC_STATUS Ilavc_status=LAVC_STABLE);
   Tformat(const char_t *Idescr,int Iidff,int movie_source,const char_t *Ihint=_l(""),const Toption *Ioptions=NULL,LAVC_STATUS Ilavc_status=LAVC_STABLE);
   Tformat(const char_t *Idescr,int Iidff,const char_t *Ihint=_l(""),LAVC_STATUS Ilavc_status=LAVC_STABLE);
   Tformat(const char_t *Idescr,int Iidff,const Tdecoder *Idecoders,const char_t *Ihint=_l(""),const Toption *Ioptions=NULL,LAVC_STATUS Ilavc_status=LAVC_STABLE);
   const char_t *descr,*hint;
   LAVC_STATUS lavc_status;
   int idff;
   Toptions options;
   const char_t *getDecoderName(int val) const;
   Tdecoders decoders;
   bool decoderCheck(const Tconfig *config);
  };
 typedef std::vector<Tformat> Tformats;
 Tformats formats;

 HWND hlv,hcbx;

 int curitem;
 void beginCodecChange(int iItem);
 void moveCBX(bool isScroll);

 TcodecsPage(TffdshowPageBase *Iparent);
 void initLV(const char_t *IthirdCol=NULL);
 virtual void fillCodecs(void)=0;
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual void enableHints(bool is);
public:
 virtual void translate(void);
};

class TffdshowPageDec;
class TcodecsPageVideo :public TcodecsPage
{
protected:
 virtual void fillCodecs(void);
public:
 TcodecsPageVideo(TffdshowPageDec *Iparent);
 virtual void init(void);
};

class TffdshowPageDec;
class TcodecsPageAudio :public TcodecsPage
{
protected:
 virtual void fillCodecs(void);
public:
 TcodecsPageAudio(TffdshowPageDec *Iparent);
 virtual void init(void);
};

#endif

