#ifndef _CABOUTPAGE_H_
#define _CABOUTPAGE_H_

#include "TconfPageBase.h"
#include "TversionsDlg.h"

class TaboutPage :public TconfPageBase
{
private:
 char_t regflnm[MAX_PATH];
 void onExport(void),onAbout(void),onDonate(void),onLicense(void);
 char_t *lic;
protected:
 const char_t *capt;int helpId;
 int aboutStringID;
 virtual const TversionsDlg::TversionInfo* fcsVersions(void)=0;
 template<class T> static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
 TaboutPage(TffdshowPageBase *Iparent);
public:
 virtual ~TaboutPage();
 virtual void init(void);
 virtual void translate(void);
 static void exportReg(Twindow *w,char_t *regflnm);
};

class TaboutPageDecVideo :public TaboutPage
{
protected:
 virtual const TversionsDlg::TversionInfo* fcsVersions(void);
public:
 TaboutPageDecVideo(TffdshowPageBase *Iparent);
};

class TaboutPageDecAudio :public TaboutPage
{
protected:
 virtual const TversionsDlg::TversionInfo* fcsVersions(void);
public:
 TaboutPageDecAudio(TffdshowPageBase *Iparent);
};

class TaboutPageEnc :public TaboutPage
{
protected:
 virtual const TversionsDlg::TversionInfo* fcsVersions(void);
public:
 TaboutPageEnc(TffdshowPageBase *Iparent);
};

#endif

