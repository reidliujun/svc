#ifndef _REG_H_
#define _REG_H_

#include "inifiles.h"
#include "malloc.h"

#define FFDSHOW_REG_PARENT _l("Software\\GNU")
#define FFDSHOW_REG_CLASS _l("config")

struct TregOp
{
public:
 virtual ~TregOp() {}
 virtual bool _REG_OP_N(short int id, const char_t *X, int &Y, const int Z)=0;
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)=0;
};
struct TregOpRegRead :public TregOp
{
private:
 HKEY hKey;
public:
 TregOpRegRead(HKEY hive,const char_t *key)
  {
   hKey=NULL;
   RegOpenKeyEx(hive,key,0,KEY_READ,&hKey);
  }
 virtual ~TregOpRegRead()
  {
   if (hKey) RegCloseKey(hKey);
  }
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
  {
   DWORD size=sizeof(int);
   if (!hKey || RegQueryValueEx(hKey,X,0,0,(LPBYTE)&Y,&size)!=ERROR_SUCCESS)
    {
     Y=Z;
     return false;
    }
   else
    return true;
  }
#ifndef REG_REG_ONLY
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
#else
 // for ff_acm. No multiple lines support.
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)
  {
   DWORD size=(DWORD)(buflen*sizeof(char_t));
   if ((!hKey || RegQueryValueEx(hKey,X,0,0,(LPBYTE)Y,&size)!=ERROR_SUCCESS) && Z)
    strcpy(Y,Z);
  }
#endif
};
struct TregOpRegWrite :public TregOp
{
private:
 HKEY hKey;
public:
 TregOpRegWrite(HKEY hive,const char_t *key)
  {
   DWORD dispo;
   if (RegCreateKeyEx(hive,key,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)!=ERROR_SUCCESS) hKey=NULL;
  }
 virtual ~TregOpRegWrite()
  {
   if (hKey) RegCloseKey(hKey);
  }
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int)
  {
   if (hKey)
    RegSetValueEx(hKey,X,0,REG_DWORD,(LPBYTE)&Y,sizeof(int));
   return true; // write always returns true
  }
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
};

#ifndef REG_REG_ONLY

struct TregOpFileStreamReadBase :public TregOp
{
protected:
 void processMultipleLines(char_t *Y, size_t buflen, char_t *bufMULTI_SZ, size_t buflenMULTI_SZ);
};

struct TregOpFileStreamWriteBase :public TregOp
{
protected:
 ffstring outString;
 void processMultipleLines(char_t *Y, size_t buflen, char_t *bufMULTI_SZ, size_t buflenMULTI_SZ);
};

struct TregOpFileRead :public TregOpFileStreamReadBase
{
private:
 Tinifile ini;
 char_t flnm[MAX_PATH],section[260];
 char_t pomS[256],propS[256];
public:
 TregOpFileRead(const char_t *Iflnm,const char_t *Isection):ini(Iflnm)
  {
   strcpy(section,Isection);
  }
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
  {
   ini.getPrivateProfileString(section,X,_itoa(Z,pomS,10),propS,255);
   Y=atoi(propS);
   return true; // TODO: detect if default has been used
  }
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
};

struct TregOpFileWrite :public TregOpFileStreamWriteBase
{
private:
 Tinifile ini;
 char_t section[260];
 char_t pomS[256];
public:
 TregOpFileWrite(const char_t *Iflnm,const char_t *Isection):ini(Iflnm)
  {
   strcpy(section,Isection);
  }
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int)
  {
   ini.writePrivateProfileString(section,X,_itoa(Y,pomS,10));
   return true;
  }
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *, bool multipleLines = false);
};

struct TregOpStreamRead :public TregOpFileStreamReadBase
{
private:
 typedef std::map<ffstring,ffstring,ffstring_iless> Tstrs;
 Tstrs strs;
 bool loaddef;
public:
 TregOpStreamRead(const void *buf,size_t len,char_t sep='\0',bool Iloaddef=true);
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
  {
   Tstrs::const_iterator i=strs.find(X);
   if (i==strs.end())
    {
     if (loaddef)
      Y=Z;
     return false;
    }
   else
    {
     Y=atoi(i->second.c_str());
     return true;
    }
  }
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
};

struct TregOpStreamWrite :public TregOpFileStreamWriteBase
{
private:
 char_t sep;
public:
 TregOpStreamWrite(char_t Isep='\0'):sep(Isep) {}
 TbyteBuffer buf;
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int)
  {
   char_t pomS[1024];
   tsnprintf_s(pomS,countof(pomS), _TRUNCATE, _l("%s=%i"), X, Y);
   buf.append(pomS,strlen(pomS)*sizeof(char_t));
   buf.append(sep);
   return true;
  }
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *, bool multipleLines = false);
 void end(void)
  {
   buf.append(sep);
  }
 void end(char_t c)
  {
   buf.append(c);
  }
};

struct Tval
{
 Tval(void) {}
 Tval(const char_t *Is):s(Is),i(0) {}
 Tval(int Ii):i(Ii) {}
 ffstring s;
 int i;
};

struct TregOpIDstreamRead: public TregOpFileStreamReadBase
{
 typedef std::hash_map<int,Tval> Tvals;
 Tvals vals;
public:
 TregOpIDstreamRead(const void *buf,size_t len,const void* *last=NULL);
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z);
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
};

struct TregOpIDstreamWrite :public TregOpFileStreamWriteBase, public TbyteBuffer
{
public:
 virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int);
 virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false);
};

struct Tstream;
bool regExport(Tstream &f,HKEY hive,const char_t *key,bool unicode);

#endif //REG_REG_ONLY

#endif
