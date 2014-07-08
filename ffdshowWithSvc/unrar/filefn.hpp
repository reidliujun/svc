#ifndef _RAR_FILEFN_
#define _RAR_FILEFN_

enum MKDIR_CODE {MKDIR_SUCCESS,MKDIR_ERROR,MKDIR_BADPATH};

bool IsRemovable(const char *Name);
bool FileExist(const char *Name,const wchar *NameW=NULL);
bool WildFileExist(const char *Name,const wchar *NameW=NULL);
bool IsDir(uint Attr);
bool IsUnreadable(uint Attr);
bool IsLabel(uint Attr);
bool IsLink(uint Attr);
void SetSFXMode(const char *FileName);
bool IsDeleteAllowed(uint FileAttr);
uint GetFileAttr(const char *Name,const wchar *NameW=NULL);
void ConvertNameToFull(const char *Src,char *Dest);
void ConvertNameToFull(const wchar *Src,wchar *Dest);
char* MkTemp(char *Name);


uint CalcFileCRC(File *SrcFile,Int64 Size=INT64ERR);

#endif
