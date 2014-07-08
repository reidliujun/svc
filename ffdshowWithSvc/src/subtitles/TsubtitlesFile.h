#ifndef _TSUBTITLESFILE_H_
#define _TSUBTITLESFILE_H_

#include "Tsubtitles.h"

DECLARE_INTERFACE(IcheckSubtitle)
{
 STDMETHOD (checkSubtitle)(const char_t *subflnm) PURE; // S_OK on accept
};

class TsubtitlesFile :public Tsubtitles
{
private:
 double fps;
 HANDLE hwatch;
 FILETIME lastwritetime;
 static void findPossibleSubtitles(const char_t *dir,strings &files);
 static const char_t *exts[];
protected:
 virtual void checkChange(const TsubtitlesSettings *cfg,bool *forceChange);
public:
 static const char_t *mask;
 static bool extMatch(const char_t *flnm);
 static void findPossibleSubtitles(const char_t *aviFlnm,const char_t *sdir,strings &files);
 static void findSubtitlesFile(const char_t *aviFlnm,const char_t *sdir,const char_t *sext,char_t *subFlnm,size_t buflen,int heuristic,IcheckSubtitle *checkSubtitle);

 TsubtitlesFile(IffdshowBase *Ideci);
 virtual ~TsubtitlesFile();
 bool init(const TsubtitlesSettings *cfg,const char_t *subFlnm,double Ifps,bool watch,int checkOnly);
 virtual void done(void);
 char_t subFlnm[MAX_PATH];
};

#endif
