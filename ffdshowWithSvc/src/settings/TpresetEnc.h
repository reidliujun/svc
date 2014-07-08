#ifndef _TPRESETENC_H_
#define _TPRESETENC_H_

#include "reg.h"

struct TcoSettings;
struct TpresetEnc
{
private:
 struct TregOpCategory :public TregOp
  {
  protected:
   TpresetEnc *preset;
   TcoSettings *co;
   const int *propsIDs;
   bool isIn(int id);
  public:
   TregOpCategory(TpresetEnc *Ipreset,TcoSettings *Ico,const int *IpropsIDs):preset(Ipreset),co(Ico),propsIDs(IpropsIDs) {}
  };
public:
 char_t name[256];
 typedef std::map<const char_t*,Tval> Tvals;
 Tvals vals;
 void initBuiltin(void);
 void load(const char_t *name);
 void save(void);
 void apply(TcoSettings *co,const int *propsIDs);
 void store(TcoSettings *co,const int *propsIDs);
 void rename(const char_t *newname);
 void remove(void);
};

struct TpresetsEnc :public std::vector<TpresetEnc>
{
private:
 static const char_t *preset_lavc_vcd_pal,*preset_lavc_vcd_ntsc,*preset_lavc_svcd_pal,*preset_lavc_svcd_ntsc,*preset_lavc_dvd_pal,*preset_lavc_dvd_ntsc;
public:
 void init(void);
 void save(void);
 TpresetEnc* getPreset(const char_t *name);
 TpresetEnc* createPreset(const char_t *name);
 void remove(TpresetEnc *preset);
};

#endif
