#ifndef _TTRANSLATE_H_
#define _TTRANSLATE_H_

class Tinifile;
class Ttranslate
{
private:
 int refcount;
 int translateEncoding;

 HINSTANCE hi;
 const char_t *pth;
 char_t curlang[MAX_PATH];
 char_t curflnm[MAX_PATH];
 struct Tfont
  {
  public:
   Tfont(void);
   void init(Tinifile &ini);
   char_t typeface[256];
   int pointsize;static const int pointsizeDef=8;
   uint8_t italic;static const uint8_t italicDef=0;
   int horizontalScale;static const int horizontalScaleDef=100;
   //int charset;
  };
 mutable bool initfont;
 struct Tstrs :public std::hash_map<ffstring,ffstring>
  {
  public:
   void init(Tinifile &ini);
   const char_t *translate(const char_t *s) const;
   void newTranslation(const ffstring &oldstring,const ffstring &newstring,const char_t *flnm,int encoding);
  } strs;
 struct Tdialog :public std::hash_map<int,ffstring>
  {
  private:
   static BOOL CALLBACK transDialogEnum(HWND hwnd,LPARAM lParam);
   void createTranslationTemplate(HWND hdlg,int dlgId);
  public:
   Tdialog(void) {}
   Tdialog(Tinifile &ini,const char_t *sect);
   Tdialog(HINSTANCE hi,HWND hwnd);
   void translate(int dlgId,HWND hwnd,Tdialog &builtin);
   const char_t* translate(int dlgId,int item,const char_t *def,Tdialog *builtin);
   void newTranslation(int dlgId,int item,const ffstring &newtrans,const char_t *flnm,int encoding);
  };
 typedef std::hash_map<int,Tdialog> Tdialogs;
 mutable Tdialogs dlgs,dlgsBuiltin;
 static int lastHorizontalScale;
 
 friend class Twindow;
public:
 Ttranslate(HINSTANCE Ihi,const char_t *Ipth);
 ~Ttranslate() {}
 void addref(void)
  {
   refcount++;
  }
 void release(void)
  {
   if (--refcount<0)
    delete this;
  }

 struct Tlanglist :public std::map<ffstring,ffstring,ffstring_iless>
  {
  private:
   const char_t *pth;
  public:
   Tlanglist(const char_t *Ipth);
   void getLangName(const_iterator &lang,char_t *buf,size_t buflen);
  } langlist;

 void init(const char_t *lang,int ItranslateMode);
 void done(void);

 void translate(HWND hwnd,int dlgId);
 const char_t *translate(HWND hwnd,int dialogId,int item,const char_t *def) const;
 const char_t *translate(const char_t *s) const
  {
   return strs.translate(s);
  }
 const char_t *translate(int dlgId);

 Tfont font;bool isCustomFont(void);
 DLGTEMPLATE *translateDialogTemplate(int dialogId) const;

 void newTranslation(int dialogId,int item,const ffstring &newtrans)
  {
   if (stricmp(curlang,_l("en"))!=0)
    dlgs[dialogId].newTranslation(dialogId,item,newtrans,curflnm,translateEncoding);
  }
 void newTranslation(const ffstring &oldstring,const ffstring &newstring)
  {
   if (stricmp(curlang,_l("en"))!=0)
    strs.newTranslation(oldstring,newstring,curflnm,translateEncoding);
  }

 static int lang2int(const char_t *lang)
  {
   int res=0;
   memcpy(&res,text<char>(lang),std::min(strlen(lang),size_t(4)));
   char *resC=(char*)&res;
   resC[0]=(char)toupper(resC[0]);
   resC[1]=(char)toupper(resC[1]);
   resC[2]=(char)toupper(resC[2]);
   resC[3]=(char)toupper(resC[3]);
   return res;
  }
 static ffstring eolnFile2string(const ffstring &s)
  {
   return stringreplace(stringreplace(s,_l("\\n"),_l("\n"),rfReplaceAll),_l("\\r"),_l("\r"),rfReplaceAll);
  }
 static ffstring eolnString2file(const ffstring &s)
  {
   return stringreplace(stringreplace(s,_l("\n"),_l("\\n"),rfReplaceAll),_l("\r"),_l("\\r"),rfReplaceAll);
  }
 int translateMode;
};

#endif
