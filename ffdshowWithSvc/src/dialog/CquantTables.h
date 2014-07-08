#ifndef _CQUANTTABLESPAGE_H_
#define _CQUANTTABLESPAGE_H_

#include "TconfPageEnc.h"

class TquantTablesPageBase :public Twindow
{
private:
 static const char_t *matrices[];
 int codecId;
 void table2dlg(void);
 char_t matrixflnm[MAX_PATH];
 char_t matrixdir[MAX_PATH];bool firstdir;
 void onLoad(void),onSave(void);
 static const char_t* parse_jmlist(char_t *buf,const char_t *name,uint8_t *cqm,const uint8_t *jvt,int length);
 static void writeMatrix(FILE *f,const char *section,const uint8_t *matrix,unsigned int len);
 struct TwidgetMatrix : TwindowWidget
  {
   bool intra;
   int id;
   TwidgetMatrix(HWND h,Twindow *w):TwindowWidget(h,w) {}
   void set(bool Iintra,int Iid)
    {
     intra=Iintra;
     id=Iid;
    }
  };
protected:
 static const int idIntra8[],idInter8[],idIntra4[],idInter4[];
 TquantTablesPageBase(IffdshowBase *Ideci);
 virtual int getCodecId(void)=0;
 virtual HRESULT getCustomQuantMatrixes(uint8_t* *intra8,uint8_t* *inter8,uint8_t* *intra4Y,uint8_t* *inter4Y,uint8_t* *intra4C,uint8_t* *inter4C)=0;
 virtual void setChange(void)=0;
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 void create(HWND parent);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

class TquantTablesPage :public TconfPageEnc
{
private:
 struct TquantTables :public TquantTablesPageBase
  {
  protected:
   virtual int getCodecId(void);
   virtual HRESULT getCustomQuantMatrixes(uint8_t* *intra8,uint8_t* *inter8,uint8_t* *intra4Y,uint8_t* *inter4Y,uint8_t* *intra4C,uint8_t* *inter4C);
   virtual void setChange(void);
  public:
   IffdshowEnc *deciE;TffdshowPageBase *parent;
   TquantTables(IffdshowBase *Ideci):TquantTablesPageBase(Ideci) {}
  } *base;
public:
 TquantTablesPage(TffdshowPageEnc *Iparent);
 virtual ~TquantTablesPage();
 virtual void init(void);
 virtual bool enabled(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

class TcurrentQuantDlg :public TdlgWindow
{
private:
 HWND hParent;
protected:
 struct TquantTables :public TquantTablesPageBase
  {
  protected:
   uint8_t *inter,*intra;
   virtual int getCodecId(void);
   virtual HRESULT getCustomQuantMatrixes(uint8_t* *intra8,uint8_t* *inter8,uint8_t* *intra4Y,uint8_t* *inter4Y,uint8_t* *intra4C,uint8_t* *inter4C);
   virtual void setChange(void) {}
  public:
   TquantTables(IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra);
   virtual void cfg2dlg(void);
  } *base;
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TcurrentQuantDlg(HWND hParent,IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra,TquantTables *Ibase=NULL);
 virtual ~TcurrentQuantDlg();
 void show(void);
 virtual void init(void);
 virtual void translate(void);
};

class TdctQuantDlg :public TcurrentQuantDlg
{
private:
 static const int idffs[16];
 struct TdctQuantTables :public TquantTables
  {
  protected:
   virtual void setChange(void);
   virtual int getCodecId(void) {return 0;}
  public:
   TdctQuantTables(IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra);
   virtual void cfg2dlg(void);
  } *base;
 uint8_t inter[64];
public:
 TdctQuantDlg(HWND hParent,IffdshowBase *Ideci);
};

#endif

