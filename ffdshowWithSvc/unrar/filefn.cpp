#include "rar.hpp"

bool IsRemovable(const char *Name)
{
#ifdef _WIN_32
  char Root[NM];
  GetPathRoot(Name,Root);
  int Type=GetDriveType(*Root ? Root:NULL);
  return(Type==DRIVE_REMOVABLE || Type==DRIVE_CDROM);
#elif defined(_EMX)
  char Drive=toupper(Name[0]);
  return((Drive=='A' || Drive=='B') && Name[1]==':');
#else
  return(false);
#endif
}



bool FileExist(const char *Name,const wchar *NameW)
{
#ifdef _WIN_32
    if (WinNT() && NameW!=NULL && *NameW!=0)
      return(GetFileAttributesW(NameW)!=0xffffffff);
    else
      return(GetFileAttributes(Name)!=0xffffffff);
#elif defined(ENABLE_ACCESS)
  return(access(Name,0)==0);
#else
  struct FindData FD;
  return(FindFile::FastFind(Name,NameW,&FD));
#endif
}


bool WildFileExist(const char *Name,const wchar *NameW)
{
  if (IsWildcard(Name,NameW))
  {
    FindFile Find;
    Find.SetMask(Name);
    Find.SetMaskW(NameW);
    struct FindData fd;
    return(Find.Next(&fd));
  }
  return(FileExist(Name,NameW));
}


bool IsDir(uint Attr)
{
#if defined (_WIN_32) || defined(_EMX)
  return(Attr!=0xffffffff && (Attr & 0x10)!=0);
#endif
#if defined(_UNIX)
  return((Attr & 0xF000)==0x4000);
#endif
}


bool IsUnreadable(uint Attr)
{
#if defined(_UNIX) && defined(S_ISFIFO) && defined(S_ISSOCK) && defined(S_ISCHR)
  return(S_ISFIFO(Attr) || S_ISSOCK(Attr) || S_ISCHR(Attr));
#endif
  return(false);
}


bool IsLabel(uint Attr)
{
#if defined (_WIN_32) || defined(_EMX)
  return((Attr & 8)!=0);
#else
  return(false);
#endif
}


bool IsLink(uint Attr)
{
#ifdef _UNIX
  return((Attr & 0xF000)==0xA000);
#endif
  return(false);
}






bool IsDeleteAllowed(uint FileAttr)
{
#if defined(_WIN_32) || defined(_EMX)
  return((FileAttr & (FA_RDONLY|FA_SYSTEM|FA_HIDDEN))==0);
#else
  return((FileAttr & (S_IRUSR|S_IWUSR))==(S_IRUSR|S_IWUSR));
#endif
}



uint GetFileAttr(const char *Name,const wchar *NameW)
{
#ifdef _WIN_32
    if (WinNT() && NameW!=NULL && *NameW!=0)
      return(GetFileAttributesW(NameW));
    else
      return(GetFileAttributes(Name));
#elif defined(_DJGPP)
  return(_chmod(Name,0));
#else
  struct stat st;
  if (stat(Name,&st)!=0)
    return(0);
#ifdef _EMX
  return(st.st_attr);
#else
  return(st.st_mode);
#endif
#endif
}


void ConvertNameToFull(const char *Src,char *Dest)
{
#ifdef _WIN_32
#ifndef _WIN_CE
  char FullName[NM],*NamePtr;
  if (GetFullPathName(Src,sizeof(FullName),FullName,&NamePtr))
    strcpy(Dest,FullName);
  else
#endif
    if (Src!=Dest)
      strcpy(Dest,Src);
#else
  char FullName[NM];
  if (IsPathDiv(*Src) || IsDiskLetter(Src))
    strcpy(FullName,Src);
  else
  {
    getcwd(FullName,sizeof(FullName));
    AddEndSlash(FullName);
    strcat(FullName,Src);
  }
  strcpy(Dest,FullName);
#endif
}


#ifndef SFX_MODULE
void ConvertNameToFull(const wchar *Src,wchar *Dest)
{
  if (Src==NULL || *Src==0)
  {
    *Dest=0;
    return;
  }
#ifdef _WIN_32
#ifndef _WIN_CE
  if (WinNT())
#endif
  {
#ifndef _WIN_CE
    wchar FullName[NM],*NamePtr;
    if (GetFullPathNameW(Src,sizeof(FullName)/sizeof(FullName[0]),FullName,&NamePtr))
      strcpyw(Dest,FullName);
    else
#endif
      if (Src!=Dest)
        strcpyw(Dest,Src);
  }
#ifndef _WIN_CE
  else
  {
    char AnsiName[NM];
    WideToChar(Src,AnsiName);
    ConvertNameToFull(AnsiName,AnsiName);
    CharToWide(AnsiName,Dest);
  }
#endif
#else
  char AnsiName[NM];
  WideToChar(Src,AnsiName);
  ConvertNameToFull(AnsiName,AnsiName);
  CharToWide(AnsiName,Dest);
#endif
}
#endif


#ifndef SFX_MODULE
char *MkTemp(char *Name)
{
  int Length=strlen(Name);
  if (Length<=6)
    return(NULL);
  int Random=clock();
  for (int Attempt=0;;Attempt++)
  {
    sprintf(Name+Length-6,"%06u",Random+Attempt);
    Name[Length-4]='.';
    if (!FileExist(Name))
      break;
    if (Attempt==1000)
      return(NULL);
  }
  return(Name);
}
#endif
