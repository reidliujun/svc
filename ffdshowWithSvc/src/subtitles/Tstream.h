#ifndef _TSTREAM_H_
#define _TSTREAM_H_

struct Tstream
{
private:
 int ugetc(void) const;
 char *ugets(char *buf,int len) const;
protected:
 bool crln,utod;
 void detectUnicode(void);
 int unicode_offset;
 virtual char* fgets0(char *buf,int len) const=0;
 virtual int getc0(void) const=0;
public:
 enum ENCODING
  {
   ENC_AUTODETECT=0,
   ENC_ASCII=1,
   ENC_UTF8=2,
   ENC_LE16=4,
   ENC_BE16=8,
   ENC_UNICODE=ENC_UTF8|ENC_LE16|ENC_BE16
  };
 mutable ENCODING encoding;
 Tstream(ENCODING Iencoding=ENC_ASCII):encoding(Iencoding),unicode_offset(0),crln(false),utod(false) {}
 virtual ~Tstream() {}
 char* fgets(char *buf,int len) const
  {
   return encoding==ENC_ASCII?fgets0(buf,len):ugets(buf,len);
  }
 wchar_t* fgets(wchar_t *buf,int len) const;
 virtual int fputs(const char *buf) {return EOF;}
 virtual wint_t fputs(const wchar_t *buf) {return WEOF;}
 virtual void rewind(void)=0;
 int getc(void) const
  {
   return encoding==ENC_ASCII?getc0():ugetc();
  }
 virtual int  eof(void) const=0;
 virtual long tell(void) const=0;
 virtual int  seek(long offset,int origin)=0;
 virtual size_t read(void *buf,size_t size,size_t count) const=0;
 virtual long length(void) const=0;
 void stripEOLN(bool strip) {crln=!strip;}
 template<class tchar> int printf(const tchar *format,...);
};

struct TstreamFile :Tstream, safe_bool<TstreamFile>
{
private:
 bool ownf;
 FILE *f;
protected:
 virtual char* fgets0(char *buf,int len) const
  {
   return ::fgets(buf,len,f);
  }
 virtual int getc0(void) const
  {
   return ::getc(f);
  }
public:
 TstreamFile(FILE *If):f(If),ownf(false)
  {
   detectUnicode();
  }
 TstreamFile(const char_t *flnm,bool binary,bool write,ENCODING Iencoding=ENC_AUTODETECT);
 virtual ~TstreamFile()
  {
   if (ownf && f)
    fclose(f);
  }
 virtual void rewind(void)
  {
   ::rewind(f);
   fseek(f,unicode_offset,SEEK_SET);
  }
 virtual int eof(void) const
  {
   return feof(f); //feof is a macro
  }
 virtual long tell(void) const
  {
   return ::ftell(f)-unicode_offset;
  }
 virtual int seek(long offset,int origin)
  {
   return ::fseek(f,origin==SEEK_SET?unicode_offset+offset:offset,origin);
  }
 virtual size_t read(void *buf,size_t size,size_t count) const
  {
   return ::fread(buf,size,count,f);
  }
 virtual long length(void) const
  {
   return f?_filelength(fileno(f)):0;
  }
 bool boolean_test() const {return f!=NULL;}
 virtual int fputs(const char *buf);
 virtual wint_t fputs(const wchar_t *buf);
};

struct TstreamMem :Tstream
{
private:
 const unsigned char *buf;size_t bufsize;
 mutable size_t bufpos;
 void handleUnicode(void)
  {
   if (encoding==ENC_AUTODETECT)
    detectUnicode();
   buf+=unicode_offset;bufsize-=unicode_offset;
  }
protected:
 TstreamMem(void):buf(NULL),bufsize(0),bufpos(0) {crln=true;}
 void setMem(const unsigned char *Ibuf,size_t Ibufsize)
  {
   buf=Ibuf;
   bufsize=Ibufsize;
   handleUnicode();
  }
 virtual char* fgets0(char *buf,int len) const;
 virtual int getc0(void) const
  {
   return (buf && bufpos<bufsize)?buf[bufpos++]:EOF;
  }
public:
 TstreamMem(const unsigned char *Ibuf,size_t Ibufsize,ENCODING Iencoding=ENC_ASCII):Tstream(Iencoding),buf(Ibuf),bufsize(Ibufsize),bufpos(0)
  {
   handleUnicode();
  }
 void convertUtoD(bool is)
  {
   utod=is;
  }
 virtual void rewind(void)
  {
   bufpos=0;
  }
 int eof(void) const
  {
   return buf?(bufpos<bufsize?0:1):1;
  }
 long tell(void) const
  {
   return buf?(long)bufpos:0;
  }
 int seek(long offset,int origin)
  {
   switch (origin)
    {
     case SEEK_SET:
      bufpos=limit<size_t>(offset,0,bufsize-1);
      return 0;
     case SEEK_CUR:
      bufpos=limit<size_t>(bufpos+offset,0,bufsize-1);
      return 0;
     case SEEK_END:
     default:
      return 0;
    }
  }
 virtual size_t read(void *buf,size_t size,size_t count) const
  {
   size_t copysize=std::min(bufsize-bufpos,size*count);
   if (copysize)
    {
     memcpy(buf,this->buf+bufpos,copysize);
     bufpos+=copysize;
    }
   return copysize/size;
  }
 virtual long length(void) const
  {
   return (long)bufsize;
  }
};

struct Tconfig;
struct TstreamRAR :TstreamMem
{
private:
 static int CALLBACK callback(UINT msg,LONG_PTR UserData,LONG_PTR P1,LONG P2);
 unsigned char *buf;size_t bufpos;
public:
 static const char_t *dllname;
 TstreamRAR(const char_t *rarflmn,const char_t *flnm,const Tconfig *config);
 virtual ~TstreamRAR()
  {
   if (buf) free(buf);
  }
};

#endif
