/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Tavisynth.h"
#include "dsutil.h"
#include "TsampleFormat.h"
#include "IffdshowBase.h"

//=================================== Tavisynth ==================================
const char_t* Tavisynth::dllname=_l("avisynth.dll");

Tavisynth::Tavisynth(void):Tdll(dllname,NULL)
{
 loadFunction(CreateScriptEnvironment,"CreateScriptEnvironment");
}

bool Tavisynth::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 bool res=false;
 Tavisynth *dl=new Tavisynth;
 if (dl->ok)
  {
   IScriptEnvironment *env=dl->CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
   try
    {
     char script[]="VersionString";
     AVSValue eval_args[]={script,"ffdshow_version_avisynth_script"};
     AVSValue val=env->Invoke("Eval",AVSValue(eval_args,2));
     vers=val.AsString();
     license=_l("(c) 2000-2003 Ben Rudiak-Gold and all subsequent developers");
     res=true;
    }
   catch (AvisynthError &err)
    {
     vers=text<char_t>(err.msg);
    }
   delete env;
  }
 else
  {
   vers=_l("not found");
   license.clear();
  }
 delete dl;
 return res;
}

const char* Tavisynth::getScriptVideo(size_t hdrsize,const char *extradata,size_t extradatasize)
{
 return getScriptAudio(hdrsize,extradata,extradatasize,4+1);
}
const char* Tavisynth::getScriptAudio(size_t hdrsize,const char *extradata,size_t extradatasize,size_t d)
{
 //#define CUSTOM_MAKEAVIS
 char *script;
 switch (extradata[0]&127)
  {
   case 1:
    script=(char*)malloc(extradatasize);
    memcpy(script,extradata+1+d,extradatasize-(1+d));
    #ifdef CUSTOM_MAKEAVIS
     #include "../../makeAVIS_custom/customMakeAVIS.h"
     getScript(script,(const unsigned char*)script,extradatasize-(1+d),hdrsize);
    #endif
    break;
   case 2:
    {
     char filename[MAX_PATH];memcpy(filename,extradata+1+d,extradatasize-(1+d));
     #ifdef CUSTOM_MAKEAVIS
      #include "../../makeAVIS_custom/customMakeAVIS.h"
      getScript(filename,(const unsigned char*)filename,extradatasize-(1+d),hdrsize);
     #endif
     FILE *f=fopen(extradata+1+d,"rb");
     if (!f) return NULL;
     int len=_filelength(fileno(f));
     script=(char*)malloc(len+1);
     fread(script,1,len,f);
     script[len]='\0';
     fclose(f);
     break;
    }
   default:
    return NULL;
  }
 return script;
}

//================================== Tavisynth_c =================================
const char_t* Tavisynth_c::dllname=_l("avisynth.dll");

Tavisynth_c::Tavisynth_c(void):Tdll(dllname,NULL)
{
 loadFunction(avs_create_script_environment,"avs_create_script_environment");
}

bool Tavisynth_c::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 bool res=false;
 Tavisynth_c *dl=new Tavisynth_c;
 if (dl->ok)
  {
   IScriptEnvironment *env=dl->CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
   try
    {
     char script[]="VersionString";
     AVSValue eval_args[]={script,"ffdshow_version_avisynth_script"};
     AVSValue val=env->Invoke("Eval",AVSValue(eval_args,2));
     vers=val.AsString();
     license=_l("(c) 2000-2003 Ben Rudiak-Gold and all subsequent developers");
     res=true;
    }
   catch (AvisynthError &err)
    {
     vers=text<char_t>(err.msg);
    }
   delete env;
  }
 else
  {
   vers=_l("not found");
   license.clear();
  }
 delete dl;
 return res;
}

//================================ TavisynthAudio ================================
TavisynthAudio::TavisynthAudio(const CMediaType &mt,TsampleFormat &fmt,IffdshowBase *deci,const char *scriptName):
 script(NULL),
 env(NULL),clip(NULL)
{
 if (ok)
  {
   env=CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
   if (env)
    {
     Textradata extradata(mt);
     script=::Tavisynth::getScriptAudio(1+extradata.size+1,(const char*)extradata.data,extradata.size);
     if (!script) {ok=false;return;}
     AVSValue eval_args[]={script,scriptName};
     try
      {
       AVSValue val=env->Invoke("Eval",AVSValue(eval_args,2));
       if (val.IsClip())
        {
         clip=new PClip(val,env);
         //*clip=val.AsClip();
         vi=&(*clip)->GetVideoInfo();
         switch (vi->SampleType())
          {
           case SAMPLE_INT16:fmt.sf=TsampleFormat::SF_PCM16;break;
           case SAMPLE_INT24:fmt.sf=TsampleFormat::SF_PCM24;break;
           case SAMPLE_INT32:fmt.sf=TsampleFormat::SF_PCM32;break;
           case SAMPLE_FLOAT:fmt.sf=TsampleFormat::SF_FLOAT32;break;
          }
         ok=true;
        }
      }
     catch (AvisynthError &err)
      {
       if (deci) deci->dbgError(text<char_t>(err.msg));
       ok=false;
      }
    }
  }
}
TavisynthAudio::~TavisynthAudio()
{
 if (clip) delete clip;
 if (env) delete env;
 if (script) free((void*)script);
}
