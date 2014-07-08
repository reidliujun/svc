#ifndef _CDIRECTSHOWCONTROL_H_
#define _CDIRECTSHOWCONTROL_H_

#include "TconfPageDec.h"

class TinfoBase;
class TdirectshowControlPageDec :public TconfPageDec
{
private:
 //struct Titem
 // {
 //  Titem(void):val(NULL),wasChange(1) {}
 //  int id,index;
 //  const char_t *name,*translatedName,*val;
 //  int wasChange;
 // };
 //struct TsortItem
 // {
 // private:
 //  const int *infos;
 // public:
 //  TsortItem(const int *Iinfos):infos(Iinfos) {}
 //  bool operator()(const Titem &i1,const Titem &i2) {return i1.index<i2.index;}
 // };
 //typedef std::vector<Titem> Titems;
 //Titems infoitems;
 typedef std::pair<DWORD,const char_t*> Tmerit;
 typedef std::vector<Tmerit> Tmerits;
 Tmerits merits;
 static bool sortMerits(const Tmerit &m1,const Tmerit &m2);
 void merit2dlg(void);
 bool isMerit,meritset;
 void blacklist2dlg(void);
 void comp_dlg2cfg(int IDC_ED_target,int IDFF_target,const char_t *delimitResult);
 void onEditCompatibilitylist(void);
 void onEditBlacklist(void);
 void onEditCompI(int IDFF_target);
 void onChbIsCompMgr(void);
protected:
 //virtual const int* getInfos(void)=0;
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 //virtual void onFrame(void);
public:
 static void convertDelimit(const char_t* instr,const char_t *inDelimit,ffstring &outstr,const char_t *outDelimit,const char_t *newFileName=NULL);
 static bool strCaseCmp(const ffstring &s1,const ffstring &s2);
 TdirectshowControlPageDec(TffdshowPageDec *Iparent);
 virtual ~TdirectshowControlPageDec();
 virtual void cfg2dlg(void);
 virtual void init(void);
 virtual void applySettings(void);
 virtual void translate(void);
};

#endif

