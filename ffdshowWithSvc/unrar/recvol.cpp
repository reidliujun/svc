#include "rar.hpp"

#define RECVOL_BUFSIZE  0x800

RecVolumes::RecVolumes()
{
  Buf.Alloc(RECVOL_BUFSIZE*256);
  memset(SrcFile,0,sizeof(SrcFile));
}


RecVolumes::~RecVolumes()
{
  for (int I=0;I<sizeof(SrcFile)/sizeof(SrcFile[0]);I++)
    delete SrcFile[I];
}




bool RecVolumes::Restore(RAROptions *Cmd,const char *Name,
                         const wchar *NameW,bool Silent)
{
 return false;
}
