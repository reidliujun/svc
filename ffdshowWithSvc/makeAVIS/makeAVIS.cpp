/*
 * Copyright (c) 2003,2004 Milan Cutka
 * AVS to Wave output by Kurosu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define _STLP_NEW_PLATFORM_SDK
#define _STLP_NO_OWN_IOSTREAMS 1

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/inttypes.h"
//#include "../src/imgFilters/avisynth/avisynth.h"
#include <io.h>
#include <vfw.h>
#include "../src/imgFilters/avisynth/avisynth_c.h"
#include "resource.h"
#include <commctrl.h>
#include "../src/ffdshow_mediaguids.h"
#include <stdio.h>
#include <fcntl.h>

#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING 0x00800000
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#if 0
#  define DBG(...) fprintf(stderr, __VA_ARGS__); fflush(stderr);
#else
 static inline void DBG(const char*fmt,...) {}
#endif

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

// Merge with Tdll ?
// Pass that around rather than having it global ?
struct avs_t
{
  AVS_ScriptEnvironment *env;
  AVS_Value handler;
  AVS_Clip* clip;
  AVS_ScriptEnvironment* (AVSC_CC *CreateScriptEnvironment)(int version);
  struct AVS_Value (AVSC_CC *Invoke)(AVS_ScriptEnvironment *, const char * name,
                      AVS_Value args, const char** arg_names);
  void (AVSC_CC *Release_Value)(AVS_Value val);
  AVS_Clip* (AVSC_CC *Take_Clip)(AVS_Value val, AVS_ScriptEnvironment *env);
  const AVS_VideoInfo* (AVSC_CC *Get_Video_Info)(AVS_Clip* clip);
  void (AVSC_CC *Release_Clip)(AVS_Clip* clip);
  int (AVSC_CC *Get_Audio)(AVS_Clip *, void * buf, INT64 start, INT64 count);
  const char* (AVSC_CC *Get_Clip_Error)(AVS_Clip *);
} avs =
  {
    NULL, avs_void, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
  };

class Tdll
{
private:
  HMODULE hdll;
  void loadDll(const char *dllName);
public:
  bool ok;
  Tdll(const char *dllName1)
  {
    hdll=LoadLibrary(dllName1);
    if (!hdll)
    {
      char name[MAX_PATH],ext[MAX_PATH];
      _splitpath(dllName1,NULL,NULL,name,ext);
      char dllName2[MAX_PATH];
      _makepath(dllName2,NULL,NULL,name,ext);
      hdll=LoadLibrary(dllName2);
    }
    ok=(hdll!=NULL);
  }

  ~Tdll()
  {
    if (hdll)
      FreeLibrary(hdll);
  }

  void loadFunction(void **fnc,const char *name)
  {
    *fnc=(void*)GetProcAddress(hdll,name);
    ok&=(*fnc!=NULL);
  }
};

#define MAKEAVIS_REG_CLASS  "config"
#define MAKEAVIS_REG "Software\\GNU\\makeAVIS"
struct TregOp
{
public:
  virtual ~TregOp() {}
  virtual void _REG_OP_N(const char *X,int &Y,const int Z)=0;
  virtual void _REG_OP_S(const char *X,char *Y,const char *Z)=0;
};
struct TregOpRegRead :public TregOp
{
private:
  HKEY hKey;
public:
  TregOpRegRead(HKEY hive,const char *key)
  {
    hKey=NULL;
    RegOpenKeyEx(hive,key,0,KEY_READ,&hKey);
  }
  virtual ~TregOpRegRead()
  {
    if (hKey) RegCloseKey(hKey);
  }
  virtual void _REG_OP_N(const char *X,int &Y,const int Z)
  {
    DWORD size=sizeof(int);
    if (!hKey || RegQueryValueEx(hKey,X,0,0,(LPBYTE)&Y,&size)!=ERROR_SUCCESS)
      Y=Z;
  }
  virtual void _REG_OP_S(const char *X,char *Y,const char *Z)
  {
    DWORD size=MAX_PATH;
    if ((!hKey || RegQueryValueEx(hKey,X,0,0,(unsigned char*)Y,&size)!=ERROR_SUCCESS) && Z)
      strcpy(Y,Z);
  }
};
struct TregOpRegWrite :public TregOp
{
private:
  HKEY hKey;
public:
  TregOpRegWrite(HKEY hive,const char *key)
  {
    DWORD dispo;
    if (RegCreateKeyEx(hive,key,0,MAKEAVIS_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)!=ERROR_SUCCESS) hKey=NULL;
  }
  virtual ~TregOpRegWrite()
  {
    if (hKey) RegCloseKey(hKey);
  }
  virtual void _REG_OP_N(const char *X,int &Y,const int)
  {
    if (hKey) RegSetValueEx(hKey,X,0,REG_DWORD,(LPBYTE)&Y,sizeof(int));
  }
  virtual void _REG_OP_S(const char *X,char *Y,const char *Z)
  {
    if (hKey) RegSetValueEx(hKey,X,0,REG_SZ,(unsigned char*)Y,DWORD(strlen(Y)+1));
  }
};


HWND m_hwnd;
HINSTANCE hInstance=NULL;
HICON hIcon;
bool bUseMMI, bVideo = true;
int UseAudio = 1;
int num_frames;
#define		AUDIO_IN_WAVE		0
#define		AUDIO_IN_AVI		1
#define		AUDIO_NO_AUDIO		2


bool dlgGetFile(bool save,const char *capt,const char *filter,const char *defext,char *flnm,DWORD flags)
{
  OPENFILENAME ofn;
  memset(&ofn,0,sizeof(ofn));
  ofn.lStructSize    =sizeof(OPENFILENAME);
  ofn.hwndOwner      =m_hwnd;
  ofn.lpstrFilter    =filter;
  ofn.lpstrInitialDir=".";
  ofn.lpstrFile      =flnm;
  ofn.lpstrTitle     =capt;
  ofn.lpstrDefExt    =defext;
  ofn.nMaxFile       =MAX_PATH;
  ofn.Flags          =flags|(save?OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT:OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST)|OFN_ENABLESIZING|OFN_HIDEREADONLY;
  return (save?GetSaveFileName(&ofn):GetOpenFileName(&ofn))?true:false;
}

bool fileexists(const char *flnm)
{
  return GetFileAttributes(flnm)!=INVALID_FILE_ATTRIBUTES;
}

char avsflnm[MAX_PATH],aviflnm[MAX_PATH];
int mode;
int isForceoutcsp,forceoutcsp;
int isForceIlace,forceilace;
int rawaudio;
bool verbose;

#define ILACE_PROGRESSIVE 1
#define ILACE_INTERLACED  2

void reg_op(TregOp &t)
{
  t._REG_OP_S("avsflnm",avsflnm,"C:\\1.avs");
  t._REG_OP_S("aviflnm",aviflnm,"C:\\1.avi");
  t._REG_OP_N("mode",mode,2);
  t._REG_OP_N("isForceoutcsp",isForceoutcsp,0);
  t._REG_OP_N("forceoutcsp",forceoutcsp,FOURCC_YUY2);
  t._REG_OP_N("isForceIlace",isForceIlace,0);
  t._REG_OP_N("forceilace",forceilace,ILACE_PROGRESSIVE);
  t._REG_OP_N("rawaudio",rawaudio,0);
}

int cbxGetItemCount(int id)
{
  return (int)SendDlgItemMessage(m_hwnd,id,CB_GETCOUNT,0,0);
}
int cbxFindItemData(int id,int data)
{
  int cnt=cbxGetItemCount(id);
  for (int i=0;i<cnt;i++)
    if (SendDlgItemMessage(m_hwnd,id,CB_GETITEMDATA,i,0)==data)
      return i;
  return CB_ERR;
}

bool getCheck(int id)
{
  return SendDlgItemMessage(m_hwnd,id,BM_GETCHECK,0,0)==BST_CHECKED?true:false;
}
void setCheck(int id,int set)
{
  SendDlgItemMessage(m_hwnd,id,BM_SETCHECK,set?BST_CHECKED:0,0);
}
void enable(int is,int id)
{
  EnableWindow(GetDlgItem(m_hwnd,id),is);
}

void cfg2dlg(void)
{
  SetDlgItemText(m_hwnd,IDC_ED_AVS,avsflnm);
  SetDlgItemText(m_hwnd,IDC_ED_AVI,aviflnm);
  char ext[MAX_PATH];
  _splitpath(aviflnm,NULL,NULL,NULL,ext);
  int iswav=stricmp(ext,".wav")==0;
  setCheck(IDC_RBT_OUTPUTFORMAT_AVI,!iswav);
  setCheck(IDC_RBT_OUTPUTFORMAT_WAV,iswav);

  SendDlgItemMessage(m_hwnd,IDC_RBT_STORE_FILENAME,BM_SETCHECK,mode==2?BST_CHECKED:BST_UNCHECKED,0);
  SendDlgItemMessage(m_hwnd,IDC_RBT_STORE_SCRIPT  ,BM_SETCHECK,mode==1?BST_CHECKED:BST_UNCHECKED,0);
  SendDlgItemMessage(m_hwnd,IDC_CHB_FORCECSP,BM_SETCHECK,isForceoutcsp?BST_CHECKED:BST_UNCHECKED,0);
  int ii=cbxFindItemData(IDC_CBX_FORCECSP,forceoutcsp);
  SendDlgItemMessage(m_hwnd,IDC_CBX_FORCECSP,CB_SETCURSEL,ii,0);
  enable(isForceoutcsp,IDC_CBX_FORCECSP);
  SendDlgItemMessage(m_hwnd,IDC_CHB_FORCEILACE,BM_SETCHECK,isForceIlace?BST_CHECKED:BST_UNCHECKED,0);
  ii=cbxFindItemData(IDC_CBX_FORCEILACE,forceilace);
  SendDlgItemMessage(m_hwnd,IDC_CBX_FORCEILACE,CB_SETCURSEL,ii,0);
  enable(isForceIlace,IDC_CBX_FORCEILACE);
  setCheck(IDC_CHB_AUDIO_RAW,rawaudio);
}

const int ids[]={IDC_LBL_AVS,IDC_ED_AVS,IDC_BT_AVS,IDC_LBL_AVI,IDC_ED_AVI,
                 IDC_BT_AVI,IDC_RBT_STORE_FILENAME,IDC_RBT_STORE_SCRIPT,
                 IDC_BT_GO,IDCLOSE,IDC_CHB_FORCECSP,IDC_CBX_FORCECSP,
                 IDC_CHB_FORCEILACE,IDC_CBX_FORCEILACE,IDC_CHB_VIDEO,
                 IDC_CHB_AUDIO,IDC_LBL_OUTPUTFORMAT,IDC_RBT_OUTPUTFORMAT_AVI,
                 IDC_RBT_OUTPUTFORMAT_WAV,IDC_CHB_AUDIO_RAW,0};

const char *errors[]=
{
  "AviSynth AVI creation succeeded",
  "Can't open AviSynth script file",
  "",
  "Error in AVIFileOpen",
  "Error in AVIFileCreateStream",
  "Error in AVIStreamSetFormat",
  "Error in AVIStreamWrite",
  "Error in AVIFileCreateStream (audio)",
  "Error in AVIStreamSetFormat (audio)",
  "Error in AVIStreamWrite (audio)",
};

struct EmakeAVIS
{
  EmakeAVIS(int Icode):code(Icode) {}
  int code;
};

struct EEval
{
  EEval(const char* e) : err(e) {}
  const char* err;
};

//XXX Bad exception handling + stack corruption
int createClip(const char *script) throw(EmakeAVIS)
{
  AVS_Value code    = avs_void;
  AVS_Value param   = avs_void;
  AVS_Value handler = avs_void;
  code = avs_new_value_string(script);
  //assert(script != avs_void.);
  param = avs_new_value_array(&code, 1);
  //assert(param != avs_void);

  DBG("Invoking with:\n%s\n", script);
  avs.handler=avs.Invoke(avs.env, "Eval", param, NULL);
  DBG("Done\n");

  //Free values
  avs.Release_Value(code);
  avs.Release_Value(param);

  if (avs_is_error(avs.handler))
  {
    DBG("If no MessageBox or one with 'unknow error' after this, stack corruption\n");
    return 2;
  }

  if (avs_is_clip(avs.handler))
  {
    DBG("Taking clip...\n");
    avs.clip = avs.Take_Clip(avs.handler, avs.env);
    DBG("Took it: 0x%08X\n", avs.clip);
    if (!avs.clip || avs.Get_Clip_Error(avs.clip))
    {
      DBG("But clip has error\n");
      return 1;
    }
    return 0;
  }

  DBG("Not a clip\n");
  return 1;
}

void updateProgress(int frame)
{
  if (bUseMMI)
    SendDlgItemMessage(m_hwnd,IDC_PBR_GO,PBM_SETPOS,frame,0);
  else
    if (verbose)
      fprintf(stderr,"\r%6.2f%%",100.0f*frame/num_frames);
}

void convertToWav(const AVS_VideoInfo* vi,AVS_Clip *clip,size_t len,const char *script)
{
  void *rawaudiobuf=NULL;
  WAVEFORMATEX *wfx=(WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
  //FIXME: we allocate a fixed size here because we don't actually
  //       use wfx and don't need to append anything to it
  wfx->cbSize=0;
  wfx->nChannels=(WORD)avs_audio_channels(vi);
  wfx->nSamplesPerSec=avs_samples_per_second(vi);
  wfx->wBitsPerSample=WORD( (avs_bytes_per_audio_sample(vi)/
                             avs_audio_channels(vi)) << 3);

  try
  {
    FILE *wavefile;

    if (strcmp(aviflnm, "-")!=0)
    {
      wavefile = fopen(aviflnm, "wb");
      if (wavefile==NULL) throw EmakeAVIS(7);
    }
    else
    {
      wavefile = stdout;
      if (_setmode(_fileno(wavefile), _O_BINARY)==-1) throw EmakeAVIS(7);
    }

    /* See http://www.borg.com/~jglatt/tech/wave.htm */
    /* RIFF Wave chunk */
    fwrite("RIFF", 4, 1, wavefile);
    WORD wFormatTag;
    unsigned short wChannels, wBlockAlign, wBitsPerSample, wAdditionalBytes = 0;
    unsigned long dwSamplesPerSec, dwAvgBytesPerSec;
    DWORD dwFileSize,dwChunkSize = sizeof(wFormatTag) +
                                   sizeof(wChannels) + sizeof(dwSamplesPerSec) +
                                   sizeof(dwAvgBytesPerSec) + sizeof(wBlockAlign) +
                                   sizeof(wBitsPerSample);
    if (!rawaudio)
    {
      switch (mode)
      {
      case 1:
        /* To store script text */
        wAdditionalBytes = (unsigned short)(len+1);
        break;
      case 2:
        /* To store script filename */
        wAdditionalBytes = (unsigned short)(strlen(avsflnm)+1);
        break;
      default:throw EmakeAVIS(7);
      }
      dwChunkSize += wAdditionalBytes+3; /* Real chunk size */
      dwFileSize = vi->num_frames * sizeof(int64_t)*2 + 8/* data chunk */+
                   12 /* Fact chunk */;
      wfx->wFormatTag = WAVE_FORMAT_AVIS;
      wfx->nBlockAlign=16;
      wfx->nAvgBytesPerSec=16*vi->fps_numerator/vi->fps_denominator;
    }
    else
    {
      dwFileSize = DWORD(vi->num_audio_samples * avs_bytes_per_audio_sample(vi) + 8)/* data chunk */;
      wfx->wFormatTag=vi->sample_type == 16 ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
      wfx->nBlockAlign=WORD(avs_bytes_per_audio_sample(vi));
      wfx->nAvgBytesPerSec=avs_samples_per_second(vi)*wfx->nBlockAlign;
      dwChunkSize += sizeof(wfx->cbSize);
    }
    /* dwFileSize is in fact the RIFF chunksize, ie real filesize
       without bytes from "RIFF" header and dwFileSize */
    dwFileSize += /*"WAVE"*/ 4  + /* Format chunk */
                  4 /*"fmt "*/ + sizeof(dwChunkSize) + dwChunkSize ;
    fwrite(&dwFileSize, 1, sizeof(dwFileSize), wavefile);
    fwrite("WAVE", 4, 1, wavefile);

    /* Format chunk */
    fwrite("fmt ", 4, 1, wavefile);
    fwrite(&dwChunkSize, 1, sizeof(dwChunkSize), wavefile);

    fwrite(&wfx->wFormatTag, 1, sizeof(wfx->wFormatTag), wavefile);
    fwrite(&wfx->nChannels, 1, sizeof(wfx->nChannels), wavefile);
    fwrite(&wfx->nSamplesPerSec, 1, sizeof(wfx->nSamplesPerSec), wavefile);
    fwrite(&wfx->nAvgBytesPerSec, 1, sizeof(wfx->nAvgBytesPerSec), wavefile);
    fwrite(&wfx->nBlockAlign, 1, sizeof(wfx->nBlockAlign), wavefile);
    fwrite(&wfx->wBitsPerSample, 1, sizeof(wfx->wBitsPerSample), wavefile);

    // Write compression information data
    if (!rawaudio)
    {
      wAdditionalBytes++;
      fwrite(&wAdditionalBytes, 1, sizeof(wAdditionalBytes), wavefile);
      switch (mode)
       {
        case 1:fputc(0x01, wavefile);fwrite(script, 1, len+1, wavefile); break;
        case 2:fputc(0x02, wavefile);fwrite(avsflnm, 1, (len=strlen(avsflnm))+1, wavefile); break;
        default:throw EmakeAVIS(7);
       }

      /* Fact chunk: in case we write AVIS audio */
      if (len&1) //padding required?
       fputc(0,wavefile);
      fwrite("fact", 4, 1, wavefile);
      dwChunkSize = 4L;
      fwrite(&dwChunkSize, 1, sizeof(dwChunkSize), wavefile);
      /* Not sure of the value "size in sample points of the
         uncompressed data */
      unsigned long dwSizeInSamplePoints = (DWORD)vi->num_audio_samples;
      fwrite(&dwSizeInSamplePoints, 1, sizeof(dwSizeInSamplePoints), wavefile);
    }
    else
    {
      fwrite(&wfx->cbSize, 1, sizeof(wfx->cbSize), wavefile);
    }

    /* data chunk */
    fwrite("data", 1, 4, wavefile);
    DWORD dwNum;
    if (!rawaudio) dwNum = vi->num_frames * 16;
    else dwNum = DWORD(vi->num_audio_samples * avs_bytes_per_audio_sample(vi));
    fwrite(&dwNum, 1, sizeof(dwNum), wavefile);

    rawaudiobuf=calloc(4, LONG((avs_audio_samples_from_frames(vi, 1)-
                                avs_audio_samples_from_frames(vi, 0))*
                               avs_bytes_per_audio_sample(vi)));
    for (int frame=0; frame<vi->num_frames; frame++)
    {
      if ((frame&255)==0)
        updateProgress(frame);
      int64_t asample[2]={avs_audio_samples_from_frames(vi, frame),
                          avs_audio_samples_from_frames(vi, frame+1)-1};
      if (!rawaudio)
      {
        fwrite(&asample, 1, sizeof(asample), wavefile);
      }
      else if (asample[1]<=vi->num_audio_samples)
      {
        LONG rawaudiolen=LONG((asample[1]-asample[0]+1)*avs_bytes_per_audio_sample(vi));
        //No need for env ?
        avs.Get_Audio(clip, rawaudiobuf,asample[0],asample[1]-asample[0]+1);
        fwrite(rawaudiobuf, 1, rawaudiolen, wavefile);
      }
      else throw EmakeAVIS(7);
    }
    if (bUseMMI)
      SendDlgItemMessage(m_hwnd,IDC_PBR_GO,PBM_SETPOS,vi->num_frames,0);
    if (strcmp(aviflnm, "-")!=0) fclose(wavefile);
    throw EmakeAVIS(0);
  }
  catch (EmakeAVIS e)
  {
    DBG("Caught EmakeAVIS\n");
    if (e.code != 0) printf("Caught exception %i\n", e.code);
    if (wfx) free(wfx);
    if (rawaudiobuf) free(rawaudiobuf);
    throw e;
  }
}
void convertToAVI(const AVS_VideoInfo* vi,AVS_Clip *clip,int len,const char *script)
{
  IAVIFile *avifile=NULL;
  IAVIStream *avistreamV=NULL,*avistreamA=NULL;
  BITMAPINFOHEADER *bih=NULL;
  WAVEFORMATEX *wfx=NULL;
  void *rawaudiobuf=NULL;

  char aviflnm2[MAX_PATH];bool tempavi=false;
  try
  {
    char aviext[MAX_PATH];_splitpath(aviflnm,NULL,NULL,NULL,aviext);
    if (_stricmp(aviext,".avi")!=0)
    {
      _makepath(aviflnm2,NULL,NULL,aviflnm,".avi");
      tempavi=true;
    }
    else
    {
      strncpy(aviflnm2, aviflnm, MAX_PATH);
      aviflnm2[MAX_PATH - 1] = 0;
      tempavi=false;
    }
    if (FAILED(AVIFileOpen(&avifile,aviflnm2,OF_CREATE|OF_WRITE,NULL))) throw EmakeAVIS(3);

    if (avs_has_video(vi) && bVideo)
    {
      AVISTREAMINFOW asi;
      asi.fccType=streamtypeVIDEO;
      asi.fccHandler=mmioFOURCC('A','V','I','S');
      asi.dwFlags=0;
      asi.dwCaps=0;
      asi.wPriority=0;
      asi.wLanguage=0;
      asi.dwScale=vi->fps_denominator;
      asi.dwRate=vi->fps_numerator;
      asi.dwStart=0;
      asi.dwLength=vi->num_frames*asi.dwRate/asi.dwScale;
      asi.dwInitialFrames=0;
      asi.dwSuggestedBufferSize=0;
      asi.dwQuality=DWORD(-1);
      asi.dwSampleSize=0;
      asi.rcFrame.left=0;asi.rcFrame.top=0;asi.rcFrame.right=vi->width;asi.rcFrame.bottom=vi->height;
      asi.dwEditCount=0;
      asi.dwFormatChangeCount=0;
      wcscpy(asi.szName,L"avisynth");
      if (FAILED(avifile->CreateStream(&avistreamV,&asi))) throw EmakeAVIS(4);

      //type    1   1=script, 2=filename
      //outcsp  4   forced output colorspace (0 if none)
      //ilace   1   0=not forced, 1=progressive, 2=interlaced
      //next        script or avs file name
      switch (mode)
      {
      case 1:
        bih=(BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER)+len+2+4+1);
        bih->biSize=sizeof(BITMAPINFOHEADER)+len+2+4+1;
        *((unsigned char*)bih+sizeof(BITMAPINFOHEADER))=1;
        memcpy((unsigned char*)bih+sizeof(BITMAPINFOHEADER)+1+4+1,script,len+1);
        break;
      case 2:
        bih=(BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER)+strlen(avsflnm)+2+4+1);
        bih->biSize=DWORD(sizeof(BITMAPINFOHEADER)+strlen(avsflnm)+2+4+1);
        *((unsigned char*)bih+sizeof(BITMAPINFOHEADER))=2;
        memcpy((unsigned char*)bih+sizeof(BITMAPINFOHEADER)+1+4+1,avsflnm,strlen(avsflnm)+1);
        break;
      }
      FOURCC fcc=isForceoutcsp?forceoutcsp:0;
      *(FOURCC*)((unsigned char*)bih+sizeof(BITMAPINFOHEADER)+1)=fcc;
      unsigned char ilace=(unsigned char)(isForceIlace?forceilace:0);
      ((unsigned char*)bih)[sizeof(BITMAPINFOHEADER)+5]=ilace;
      bih->biWidth=vi->width;
      bih->biHeight=vi->height;
      bih->biPlanes=1;
      bih->biBitCount=16;
      bih->biCompression=mmioFOURCC('A','V','I','S');
      bih->biSizeImage=vi->width*vi->height*2;
      bih->biXPelsPerMeter=0;
      bih->biYPelsPerMeter=0;
      bih->biClrUsed=0;
      bih->biClrImportant=0;
      if (FAILED(avistreamV->SetFormat(0,bih,bih->biSize))) throw EmakeAVIS(5);
    }

    if (avs_has_audio(vi) && UseAudio==AUDIO_IN_AVI)
    {
      AVISTREAMINFOW asi;
      asi.fccType=streamtypeAUDIO;
      asi.fccHandler=0;
      asi.dwFlags=0;
      asi.dwCaps=0;
      asi.wPriority=0;
      asi.wLanguage=0;
      if (!rawaudio)
      {
        asi.dwScale=vi->fps_denominator;
        asi.dwRate=vi->fps_numerator;
        asi.dwSampleSize=16;
        asi.dwLength=DWORD((vi->num_audio_samples*int64_t(asi.dwRate))/
                           (avs_samples_per_second(vi)*int64_t(asi.dwScale)));
        //vi->num_frames*asi.dwRate/asi.dwScale;
      }
      else
      {
        asi.dwScale=avs_bytes_per_audio_sample(vi);
        asi.dwRate=avs_samples_per_second(vi)*asi.dwScale;
        asi.dwSampleSize=avs_bytes_per_audio_sample(vi);
        asi.dwLength=DWORD(vi->num_audio_samples);
      }
      asi.dwStart=0;
      asi.dwInitialFrames=0;
      asi.dwSuggestedBufferSize=0;
      asi.dwQuality=DWORD(-1);
      asi.rcFrame.left=asi.rcFrame.top=asi.rcFrame.right=asi.rcFrame.bottom=0;
      asi.dwEditCount=0;
      asi.dwFormatChangeCount=0;
      wcscpy(asi.szName,L"avisynth audio");
      if (FAILED(avifile->CreateStream(&avistreamA,&asi))) throw EmakeAVIS(7);

      if (!rawaudio)
      {
        switch (mode)
        {
        case 1:
          wfx=(WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX)+1+len+1);
          wfx->cbSize=WORD(1+len+1);
          *((unsigned char*)wfx+sizeof(WAVEFORMATEX))=1;
          memcpy((unsigned char*)wfx+sizeof(WAVEFORMATEX)+1,script,len+1);
          break;
        case 2:
          wfx=(WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX)+1+strlen(avsflnm)+1);
          wfx->cbSize=WORD(1+strlen(avsflnm)+1);
          *((unsigned char*)wfx+sizeof(WAVEFORMATEX))=2;
          memcpy((unsigned char*)wfx+sizeof(WAVEFORMATEX)+1,avsflnm,strlen(avsflnm)+1);
          break;
        }
        wfx->wFormatTag=WAVE_FORMAT_AVIS;
        wfx->nBlockAlign=16;
        wfx->nAvgBytesPerSec=16*vi->fps_numerator/vi->fps_denominator;
      }
      else
      {
        wfx=(WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
        wfx->cbSize=0;
        wfx->wFormatTag=vi->sample_type == 16 ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
        wfx->nBlockAlign=WORD(avs_bytes_per_audio_sample(vi));
        wfx->nAvgBytesPerSec=avs_samples_per_second(vi)*wfx->nBlockAlign;
      }
      wfx->nChannels=WORD(avs_audio_channels(vi));
      wfx->nSamplesPerSec=avs_samples_per_second(vi);
      //wfx->wBitsPerSample=WORD((avs_samples_per_second(vi)/avs_audio_channels(vi))<<3);
      wfx->wBitsPerSample=WORD((avs_bytes_per_audio_sample(vi)/avs_audio_channels(vi))<<3);
      if (FAILED(avistreamA->SetFormat(0,wfx,sizeof(WAVEFORMATEX)+wfx->cbSize))) throw EmakeAVIS(8);
    }

    for (int frame=0;frame<vi->num_frames;frame++)
    {
      if ((frame&255)==0)
        updateProgress(frame);

      if (avistreamV)
      {
        LONG samplesWritten=0,bytesWritten=0;
        if (FAILED(avistreamV->Write(frame,1,&frame,4,AVIIF_KEYFRAME,
                                     &samplesWritten,&bytesWritten))) throw EmakeAVIS(6);
      }
      if (avistreamA)
      {
        LONG samplesWritten=0,bytesWritten=0;
        int64_t asample[2]={avs_audio_samples_from_frames(vi, frame),
                            avs_audio_samples_from_frames(vi, frame+1)-1};
        if (!rawaudio)
        {
          if (FAILED(avistreamA->Write(frame,1,asample,sizeof(asample),
                                       AVIIF_KEYFRAME,&samplesWritten,&bytesWritten)))
            throw EmakeAVIS(9);
        }
        else if (asample[1]<=vi->num_audio_samples)
        {
          //LONG rawaudiolen=LONG((asample[1]-asample[0]+1)*avs_samples_per_second(vi));
          LONG rawaudiolen=LONG((asample[1]-asample[0]+1)*avs_bytes_per_audio_sample(vi));
          if (!rawaudiobuf) rawaudiobuf=malloc(rawaudiolen*2);
          //No need for env ?
          avs.Get_Audio(clip, rawaudiobuf,asample[0],asample[1]-asample[0]+1);
          if (FAILED(avistreamA->Write(frame,1,rawaudiobuf,rawaudiolen,
                                       AVIIF_KEYFRAME,&samplesWritten,&bytesWritten)))
            throw EmakeAVIS(9);
        }
      }
    }
    throw EmakeAVIS(0);
  }
  catch (EmakeAVIS e)
  {
    if (avistreamV) avistreamV->Release();
    if (avistreamA) avistreamA->Release();
    if (avifile) avifile->Release();
    if (bih) free(bih);
    if (wfx) free(wfx);
    if (rawaudiobuf) free(rawaudiobuf);
    if (e.code==0 && tempavi)
    {
      DeleteFile(aviflnm);
      MoveFile(aviflnm2,aviflnm);
    }
    throw e;
  }
  DBG("Everything OK\n");
}

int convert(void)
{
  FILE *f=fopen(avsflnm,"rb");if (!f) return 1;
  int i;
  int ret=0;
  char *script=NULL;
  AVS_Clip *clip=NULL;

  if (bUseMMI)
    for (i=0;ids[i];i++) enable(FALSE,ids[i]);
  else
    if (verbose)
    {
      fprintf(stderr,"Input:  %s\n",avsflnm);
      fprintf(stderr,"Output: %s\n",(strcmp(aviflnm,"-")==0)?"standard output":aviflnm);
    }

  try
  {
    int len=filelength(fileno(f));
    script=(char*)malloc(len+1);
    fread(script,1,len,f);
    fclose(f);
    script[len]='\0';

    DBG("Creating convert clip...\n");
    ret = createClip(script);
    DBG("Created: 0x%08X\n", avs.clip);
    if (ret || !avs.clip)
    {
      DBG("Boom convert: %li\n", ret);
      if (ret == 2)
        MessageBox(m_hwnd, avs_as_error(avs.handler),
                 "AviSynth error", MB_ICONERROR|MB_OK);
      else
        MessageBox(m_hwnd, "Unknown error",
                 "AviSynth error", MB_ICONERROR|MB_OK);
       // if AVS threw an exception, this will corrupt stack I guess
      throw EmakeAVIS(2);
    }
    //Work around stack corruption anytime in createClip
    clip = avs.clip;

    const AVS_VideoInfo* vi=avs.Get_Video_Info(clip);
    num_frames=vi->num_frames;
    if (bUseMMI)
      SendDlgItemMessage(m_hwnd,IDC_PBR_GO,PBM_SETRANGE32,0,num_frames);

    if (!bUseMMI)
    {
      char ext[MAX_PATH];
      _splitpath(aviflnm,NULL,NULL,NULL,ext);
      if (_stricmp(ext,".avi")==0)
        convertToAVI(vi,clip,len,script);
      else
        convertToWav(vi,clip,len,script);
    }
    else
    {
      if (getCheck(IDC_RBT_OUTPUTFORMAT_AVI))
        convertToAVI(vi,clip,len,script);
      else
        convertToWav(vi,clip,len,script);
    }

    if (strcmp(aviflnm, "-")!=0) DeleteFile(aviflnm);
    if (bUseMMI)
      SendDlgItemMessage(m_hwnd,IDC_PBR_GO,PBM_SETPOS,num_frames,0);
  }
  catch (EmakeAVIS e)
  {
    if (e.code)
      DBG("Boom convert\n");
    ret=e.code;
  }

  if (script) free(script);
  if (avs.clip) avs.Release_Clip(avs.clip);
  avs.Release_Value(avs.handler);
  DBG("Ready\n");

  if (bUseMMI)
    for (i=0;ids[i];i++) enable(TRUE,ids[i]);
  return ret;
}

int cbxAdd(int id,const char *s,int data)
{
 int ii=(int)SendDlgItemMessage(m_hwnd,id,CB_ADDSTRING,0,LPARAM(s));
 SendDlgItemMessage(m_hwnd,id,CB_SETITEMDATA,ii,data);
 return ii;
}

struct Tcsp
{
 const char *name;
 FOURCC fcc;
} const csps[]=
{
 "YV12",FOURCC_YV12,
 "YUY2",FOURCC_YUY2,
 "YVYU",FOURCC_YVYU,
 "UYVY",FOURCC_UYVY,
 "I420",FOURCC_I420,
 "RGB555",FOURCC_RGB5,
 "RGB565",FOURCC_RGB6,
 "RGB24",FOURCC_RGB2,
 "RGB32",FOURCC_RGB3,
 NULL,0
};

const char *ilaces[]=
{
  "Progressive",
  "Interlaced",
  NULL
};

int cbxGetCurItemData(int id)
{
 int ii=(int)SendDlgItemMessage(m_hwnd,id,CB_GETCURSEL,0,0);
 return (int)SendDlgItemMessage(m_hwnd,id,CB_GETITEMDATA,ii,0);
}

int showProps(void)
{
  FILE *f=fopen(avsflnm,"rb");
  if (!f) return 0;
  int len=filelength(fileno(f));
  char *script=(char*)malloc(len+1);
  fread(script,1,len,f);
  fclose(f);
  script[len]='\0';
  AVS_Clip *clip=NULL;
  const AVS_VideoInfo* vi;
  int ret = 0;

  DBG("Creating convert clip...\n");
  ret = createClip(script);
  DBG("Created: 0x%08X\n", avs.clip);
  if (ret || !avs.clip)
  {
    DBG("Boom show: %i\n", ret);
    if (ret == 2)
      MessageBox(m_hwnd, avs_as_error(avs.handler),
                 "AviSynth error", MB_ICONERROR|MB_OK);
    else
      MessageBox(m_hwnd, "Unknown error",
                 "AviSynth error", MB_ICONERROR|MB_OK);
    goto end;
  }

  vi=avs.Get_Video_Info(avs.clip);
  char pomS[256];
  DBG("Checking if video\n");
  if (avs_has_video(vi))
  {
    sprintf(pomS,"Resolution: %ix%i",vi->width,vi->height);
    SetDlgItemText(m_hwnd,IDC_LBL_VIDEO_RES,pomS);
    sprintf(pomS,"FPS: %g",float(vi->fps_numerator)/vi->fps_denominator);
    SetDlgItemText(m_hwnd,IDC_LBL_VIDEO_FPS,pomS);
    const char *cspS;
    if (avs_is_rgb24(vi))      cspS="RGB24";
    else if (avs_is_rgb32(vi)) cspS="RGB32";
    else if (avs_is_yuy2(vi))  cspS="YUY2";
    else if (avs_is_yv12(vi))  cspS="YV12";
    else cspS="unknown";
    sprintf(pomS,"Colorspace: %s",cspS);
    SetDlgItemText(m_hwnd,IDC_LBL_VIDEO_CSP,pomS);
    sprintf(pomS,"Number of frames: %i",vi->num_frames);
    SetDlgItemText(m_hwnd,IDC_LBL_VIDEO_FRAMESNUM,pomS);
    int secs=vi->num_frames*vi->fps_denominator/vi->fps_numerator;
    sprintf(pomS,"Time: %02i:%02i:%02i",secs/3600,(secs/60)%60,secs%60);
    SetDlgItemText(m_hwnd,IDC_LBL_VIDEO_TIME,pomS);
  }
  else
    setCheck(IDC_CHB_VIDEO,0);

  DBG("Checking if audio\n");
  if (avs_has_audio(vi))
  {
    sprintf(pomS,"Sample rate: %i",vi->audio_samples_per_second);
    SetDlgItemText(m_hwnd,IDC_LBL_AUDIO_SAMPLERATE,pomS);
    sprintf(pomS,"Channels: %i",vi->nchannels);
    SetDlgItemText(m_hwnd,IDC_LBL_AUDIO_CHANNELS,pomS);
    //sprintf(pomS,"Bits per sample: %i",(avs_samples_per_second(vi)/avs_audio_channels(vi))<<3);
    sprintf(pomS,"Bits per sample: %i",(avs_bytes_per_audio_sample(vi)/avs_audio_channels(vi))<<3);
    SetDlgItemText(m_hwnd,IDC_LBL_AUDIO_BITSPERSAMPLE,pomS);
    int secs=int(vi->num_audio_samples/avs_samples_per_second(vi));
    sprintf(pomS,"Time: %02i:%02i:%02i",secs/3600,(secs/60)%60,secs%60);
    SetDlgItemText(m_hwnd,IDC_LBL_AUDIO_TIME,pomS);
  }
  else
    setCheck(IDC_CHB_AUDIO,0);
  ret = 1;

 end:
  DBG("Releasing show...");
  if (avs.clip) avs.Release_Clip(avs.clip);
  avs.Release_Value(avs.handler);
  if (script) free(script);
  DBG(" Done\n");
  return ret;
}

INT_PTR CALLBACK dlgProc(HWND m_hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_INITDIALOG:
    {
      ::m_hwnd=m_hwnd;
      SendMessage(m_hwnd,WM_SETICON,ICON_BIG,LPARAM(hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_AVISYNTH))));
      TregOpRegRead t(HKEY_CURRENT_USER,MAKEAVIS_REG);
      reg_op(t);
      char avidsk[MAX_PATH],avidir[MAX_PATH];
      _splitpath(aviflnm,avidsk,avidir,NULL,NULL);
      char avipath[MAX_PATH];
      _makepath(avipath,avidsk,avidir,NULL,NULL);
      SetCurrentDirectory(avipath);
      int i;
      for (i=0;csps[i].name;i++)
        cbxAdd(IDC_CBX_FORCECSP,csps[i].name,csps[i].fcc);
      for (i=0;ilaces[i];i++)
        cbxAdd(IDC_CBX_FORCEILACE,ilaces[i],i+1);
      SendDlgItemMessage(m_hwnd,IDC_CHB_VIDEO,BM_SETCHECK,BST_CHECKED,0);
      SendDlgItemMessage(m_hwnd,IDC_CHB_AUDIO,BM_SETCHECK,BST_CHECKED,0);
      cfg2dlg();
      DBG("Init\n");
      showProps();
      DBG("Init end\n");
      break;
    }
  case WM_DESTROY:
    DestroyIcon(hIcon);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDCANCEL:
      EndDialog(m_hwnd,wParam);
      return TRUE;
    case IDCLOSE:
      {
        TregOpRegWrite t(HKEY_CURRENT_USER,MAKEAVIS_REG);
        reg_op(t);
        EndDialog(m_hwnd,wParam);
        return TRUE;
      }
    case IDC_BT_AVS:
      if (dlgGetFile(false,"Select AviSynth script","AviSynth scripts (*.avs)\0*.avs\0All files (*.*)\0*.*\0\0","*.avs",avsflnm,0))
      {
        DBG("BT AVS\n");
        if (showProps())
        {
          DBG("OK\n");
          char aviext[MAX_PATH];
          _splitpath(aviflnm,NULL,NULL,NULL,aviext);
          char dsk[MAX_PATH],dir[MAX_PATH],name[MAX_PATH];
          _splitpath(avsflnm,dsk,dir,name,NULL);
          _makepath(aviflnm,dsk,dir,name,aviext);
          cfg2dlg();
        }
        else
        {
          DBG("Not\n");
          return FALSE;
        }
      }
      return TRUE;
    case IDC_BT_AVI:
      if (dlgGetFile(true,"Select name for output file","AVI files (*.avi)\0*.avi\0Wave files (*.wav)\0*.wav\0\0","*.avi",aviflnm,0))
      {
        char ext[MAX_PATH];
        _splitpath(aviflnm,NULL,NULL,NULL,ext);
        int iswav=stricmp(ext,".wav")==0;
        setCheck(IDC_RBT_OUTPUTFORMAT_AVI,!iswav);
        setCheck(IDC_RBT_OUTPUTFORMAT_WAV,iswav);
        cfg2dlg();
      }
      return TRUE;
    case IDC_RBT_STORE_FILENAME:
    case IDC_RBT_STORE_SCRIPT:
      if (getCheck(IDC_RBT_STORE_FILENAME)) mode=2;
      else if (getCheck(IDC_RBT_STORE_SCRIPT)) mode=1;
      return TRUE;
    case IDC_BT_GO:
      if (!fileexists(avsflnm))
        MessageBox(m_hwnd,"AviSynth script doesn't exists","Error",MB_ICONERROR|MB_OK);
      else
      {
        /* Set all parameters given by GUI */
        bVideo = getCheck(IDC_CHB_VIDEO);
        UseAudio = getCheck(IDC_CHB_AUDIO);

        const char *err=errors[convert()];
        if (err[0])
          MessageBox(m_hwnd,err,"Status",MB_ICONINFORMATION|MB_OK);
      }
      SendDlgItemMessage(m_hwnd,IDC_PBR_GO,PBM_SETPOS,0,0);
      cfg2dlg();
      return TRUE;
    case IDC_CHB_FORCECSP:
      isForceoutcsp=getCheck(IDC_CHB_FORCECSP);
      cfg2dlg();
      return TRUE;
    case IDC_CBX_FORCECSP:
      if (HIWORD(wParam)==CBN_SELCHANGE)
      {
        forceoutcsp=cbxGetCurItemData(IDC_CBX_FORCECSP);
        return TRUE;
      }
      break;
    case IDC_CHB_FORCEILACE:
      isForceIlace=getCheck(IDC_CHB_FORCEILACE);
      cfg2dlg();
      return TRUE;
    case IDC_CBX_FORCEILACE:
      if (HIWORD(wParam)==CBN_SELCHANGE)
      {
        forceilace=cbxGetCurItemData(IDC_CBX_FORCEILACE);
        return TRUE;
      }
      break;
    case IDC_ED_AVS:
      if (HIWORD(wParam)==EN_CHANGE)
      {
        GetDlgItemText(m_hwnd,IDC_ED_AVS,avsflnm,MAX_PATH);
        return TRUE;
      }
      break;
    case IDC_ED_AVI:
      if (HIWORD(wParam)==EN_CHANGE)
      {
        GetDlgItemText(m_hwnd,IDC_ED_AVI,aviflnm,MAX_PATH);
        return TRUE;
      }
      break;
    case IDC_CHB_AUDIO_RAW:
      rawaudio=getCheck(IDC_CHB_AUDIO_RAW);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

void help()
{
  printf("MakeAVIS options:\n"
         "  -i <file>   Input AviSynth script\n"
         "  -v <file>   Output video file (- = stdout)\n"
         "  -a <file>   Output audio file (- = stdout)\n"
         "  -c <int>    Colorspace (0:YV12/1:YUY2/2:YVYU/3:I420)\n"
         "      Default: original  (4:RGB555/6:RGB565/7:RGB24/8:RGB32)\n"
         "  -f <int>    Interlacing (0:Interlaced/1:Progressive), default: original\n"
         "  -p          Store PCM uncompressed audio\n"
         "  -s <int>    Store script: 1:by filename inside script/2: in output file\n"
         "  -V          Write progress info\n");
}

int parsecl(int argc, char **argv)
{
  int i;
  bool bHasOut = false, bHasIn = false;

  /* Defauls */
  isForceoutcsp = 0;
  isForceIlace = 0;
  rawaudio = false;
  mode = 1;
  verbose = false;

  /* Parse */
  try
  {
    for (i=1; i<argc; i++)
    {
      if (argv[i][0] == '-')
      {
        switch(argv[i][1])
        {
        case 'i':
         i++;
         strncpy(avsflnm, argv[i], MAX_PATH);
         avsflnm[MAX_PATH - 1] = 0;
         bHasIn = true; break;
        case 'v':
         i++;
         strncpy(aviflnm, argv[i], MAX_PATH);
         aviflnm[MAX_PATH - 1] = 0;
         bHasOut = true;
         break;
        case 'a':
         i++;
         strncpy(aviflnm, argv[i], MAX_PATH);
         aviflnm[MAX_PATH - 1] = 0;
         bHasOut = true;
         break;
        case 'c':
          i++;
          isForceoutcsp = 1;
          forceoutcsp = atoi(argv[i]);
          if (forceoutcsp<0 || forceoutcsp>8)
          {
            fprintf(stderr, "Incorrect colorspace\n");
            return -1;
          }
          forceoutcsp = csps[forceoutcsp].fcc;
          break;
        case 'f':
          isForceIlace = true;
          forceilace = atoi(argv[++i])-68;
          if (forceilace != 0 && forceilace != 1) return -1;
          break;
        case 'p': rawaudio = true; break;
        case 's':
          mode = atoi(argv[++i]);
          if (mode != 1 && mode != 2) return -1;
          break;
        case 'V':
          verbose = true; break;
        default: fprintf(stderr, "Unknown option \"%s\"\n", argv[i]); return -1;
        }
      }
      else
      {
        fprintf(stderr, "Unknown parameter \"%s\"\n", argv[i]);
        return -1;
      }
    }
  }
  catch (...)
  {
    fprintf(stderr, "Malformed command-line\n");
    return -1;
  }
  if (!bHasOut || !bHasIn) return -1;
  return 0;
}

int main(int argc, char **argv)
{
  int ret = 0;
  if (argc==1)
    FreeConsole();
  hInstance=GetModuleHandle(NULL);
  Tdll avisynth("avisynth.dll");
  if (!avisynth.ok)
   {
    fprintf(stderr,"AviSynth not installed\n");
    return -100;
   }
  AVIFileInit();

  //Load functions
#define LOAD(func, string) avisynth.loadFunction((void**)&(avs.func), string)

  LOAD(CreateScriptEnvironment, "avs_create_script_environment");
  LOAD(Invoke, "avs_invoke");
  LOAD(Release_Value, "avs_release_value");
  LOAD(Take_Clip, "avs_take_clip");
  LOAD(Get_Video_Info, "avs_get_video_info");
  LOAD(Release_Clip, "avs_release_clip");
  LOAD(Get_Audio, "avs_get_audio");
  LOAD(Get_Clip_Error, "avs_clip_get_error");

  avs.env=avs.CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
  if (avisynth.ok && avs.env)
  {
    if (argc==1)
    {
      bUseMMI=true;
      DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,(DLGPROC)dlgProc);
      ret=0;
    }
    else
    {
      bUseMMI=false;
      if (parsecl(argc,argv) != -1)
        ret=convert();
      else
      {
        help();
        ret=-1;
      }
    }

    //XXX How are we supposed to release env ?
  }
  else
  {
    ret=-1;
    if (__argc==1) MessageBox(NULL,"Failed to initialize AviSynth","Error",MB_ICONERROR|MB_OK);
  }

  AVIFileExit();
  return ret;
}
