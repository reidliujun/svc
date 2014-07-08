/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "reg.h"
#include "Tstream.h"
#include "char_t.h"

//================================== TregOpRegRead  =================================
void TregOpRegRead::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 DWORD size=(DWORD)(buflen*sizeof(char_t));
 if ((!hKey || RegQueryValueEx(hKey,X,0,0,(LPBYTE)Y,&size)!=ERROR_SUCCESS) && Z)
  {
   ff_strncpy(Y, Z, buflen);
   return;
  }
 if (multipleLines)
  {
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * (buflen + 1));
   memcpy(bufMULTI_SZ, Y, buflen);
   char_t *buf = Y;
   char_t *bufend = Y + buflen;
   while (*bufMULTI_SZ && buf<bufend)
    {
     size_t len = strlen(bufMULTI_SZ);
     ff_strncpy(buf, bufMULTI_SZ, bufend - buf);
     bufMULTI_SZ += len + 1;
     buf += len;
     if (buf +2 < bufend)
      strcpy(buf,_l("\r\n"));
     buf += 2;
    }
   if (buf < bufend)
    *buf = 0;
   else
    Y[buflen - 1] = 0;
  }
}

//================================== TregOpRegWrite =================================
void TregOpRegWrite::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 if (!hKey) return;
 if (multipleLines)
  {
   // from _l("\r\n") to double null terminated strings.
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * (buflen + 1));
   ff_strncpy(bufMULTI_SZ, Y, buflen);
   bufMULTI_SZ[buflen] = 0;
   strings lines;
   strtok(bufMULTI_SZ, _l("\r\n"), lines);
   memset(bufMULTI_SZ, 0, sizeof(char_t) * (buflen + 1));
   char_t *bufMULTI_SZpos = bufMULTI_SZ;
   char_t *bufMULTI_SZend = bufMULTI_SZ + (buflen>0 ? buflen - 1 : 0);
   for (strings::const_iterator l = lines.begin() ; l != lines.end() && bufMULTI_SZpos < bufMULTI_SZend ; l++)
    {
     ff_strncpy(bufMULTI_SZpos, l->c_str(), bufMULTI_SZend - bufMULTI_SZpos);
     bufMULTI_SZpos += l->size() + 1;
     if (bufMULTI_SZend <= bufMULTI_SZpos) break;
    }
   RegSetValueEx(hKey,X,0,REG_MULTI_SZ,(LPBYTE)bufMULTI_SZ,DWORD((bufMULTI_SZpos - bufMULTI_SZ + 1)*sizeof(char_t)));
  }
 else
  {
   RegSetValueEx(hKey,X,0,REG_SZ,(LPBYTE)Y,DWORD((strlen(Y)+1)*sizeof(char_t)));
  }
}


//============================= TregOpFileStreamReadBase   ============================
void TregOpFileStreamReadBase::processMultipleLines(char_t *Y, size_t buflen, char_t *bufMULTI_SZ, size_t buflenMULTI_SZ)
{
 bufMULTI_SZ[buflenMULTI_SZ - 1]=0;
 size_t count = 0;
 while (*bufMULTI_SZ && count < buflen)
  {
   if (*bufMULTI_SZ == '%')
    {
     bufMULTI_SZ++;
     if (*bufMULTI_SZ == 'r')
      {
       Y[count] = '\r';
       count++;
       bufMULTI_SZ++;
      }
     else if (*bufMULTI_SZ == 'n')
      {
       Y[count] = '\n';
       count++;
       bufMULTI_SZ++;
      }
     else if (*bufMULTI_SZ == '%')
      {
       Y[count] = '%';
       count++;
       bufMULTI_SZ++;
      }
     else
      {
       Y[count] = '%';
       count++;
       if (count < buflen)
        Y[count] = *bufMULTI_SZ;
       else
        break;
       count++;
       bufMULTI_SZ++;
      }
    }
   else
    {
     Y[count] = *bufMULTI_SZ;
     count++;
     bufMULTI_SZ++;
    }
  }
 if (count < buflen)
  Y[count] = 0;
 Y[buflen - 1] = 0;
}

//============================ TregOpFileStreamWriteBase   ============================
void TregOpFileStreamWriteBase::processMultipleLines(char_t *Y, size_t buflen, char_t *bufMULTI_SZ, size_t buflenMULTI_SZ)
{
 ff_strncpy(bufMULTI_SZ, Y, buflenMULTI_SZ - 1);
 bufMULTI_SZ[buflenMULTI_SZ - 1] = 0;
 strings lines1,lines2;
 strtok(bufMULTI_SZ, _l("%"), lines1,true);
 ffstring line1;
 for (strings::const_iterator l = lines1.begin() ; l != lines1.end() ; l++)
  {
   if (line1.size())
    line1 += _l("%%") + *l;
   else
    line1 = *l;
  }
 strtok(line1.c_str(), _l("\r\n"), lines2,true);
 for (strings::const_iterator l = lines2.begin() ; l != lines2.end() ; l++)
  {
   if (outString.size())
    outString += _l("%r%n") + *l;
   else
    outString = *l;
  }
}

//================================== TregOpFileRead   =================================
void TregOpFileRead::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 if (multipleLines)
  {
   size_t buflenMULTI_SZ = buflen * 2 + 1;
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
   ini.getPrivateProfileString(section, X, Z, bufMULTI_SZ, (DWORD)buflen);
   processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
  }
 else
  {
   ini.getPrivateProfileString(section,X,Z,Y,(DWORD)buflen);
   Y[buflen-1]='\0';
  }
}

//================================== TregOpFileWrite  =================================
void TregOpFileWrite::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *, bool multipleLines)
{
 if (multipleLines)
  {
   size_t buflenMULTI_SZ = buflen + 1;
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
   processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
   ini.writePrivateProfileString(section,X,outString.c_str());
  }
 else
  ini.writePrivateProfileString(section,X,Y);
}

//================================== TregOpStreamRead =================================
TregOpStreamRead::TregOpStreamRead(const void *buf,size_t len,char_t sep,bool Iloaddef):loaddef(Iloaddef)
{
 for (const char_t *cur=(const char_t*)buf,*end=(const char_t*)buf+len;cur<end;)
  {
   while (*cur==' ' && cur<=end)
    cur++;
   const char_t *s=(const char_t*)memchr(cur,sep,end-cur);
   if (!s)
    s=end;
   char_t line[256];
   if (s-cur > countof(line) -1) return;
   _tcsncpy_s(line, countof(line), cur, s-cur);
   char_t *ir=strchr(line,'=');
   if (ir)
    {
     *ir='\0';
     strs.insert(std::make_pair(ffstring(line),ffstring(ir+1)));
    }
   cur=s+1;
  }
}
void TregOpStreamRead::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 Tstrs::const_iterator i=strs.find(X);
 if (i==strs.end())
  {
   if (loaddef)
    ff_strncpy(Y,Z,buflen);
  }
 else
  {
   if (multipleLines)
    {
     size_t buflenMULTI_SZ = buflen * 2 + 1;
     char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
     ff_strncpy(bufMULTI_SZ, i->second.c_str(), buflenMULTI_SZ);
     processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
    }
   else
    {
     ff_strncpy(Y,i->second.c_str(),buflen);
    }
  }
 Y[buflen-1]='\0';
}

//================================ TregOpStreamWrite ================================
void TregOpStreamWrite::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 // not used

 if (multipleLines)
  {
   size_t buflenMULTI_SZ = buflen + 1;
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
   processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
   ffstring pomS = ffstring(X) + _l("=") + outString;
   buf.append(pomS.c_str(), pomS.size() * sizeof(char_t));
   buf.append(sep);
  }
 else
  {
   ffstring pomS = ffstring(X) + _l("=") + ffstring(Y);
   buf.append(pomS.c_str(), pomS.size() * sizeof(char_t));
   buf.append(sep);
  }
}

//=============================== TregOpIDstreamRead ================================
//
// To test. Use graphedit and add ffdshow encoder. Load graph.
//

TregOpIDstreamRead::TregOpIDstreamRead(const void *buf,size_t len,const void* *last)
{
 const char *p,*pEnd;
 for (p=(const char*)buf,pEnd=p+len;p<pEnd;)
  {
   short int id=*(short int*)p;
   p+=2;
   if (id>0)
    {
     vals.insert(std::make_pair(id,Tval(*(int*)p)));
     p+=4;
    }
   else if (id<0)
    {
     if ((uint8_t)(p[0])==0x0ff && (uint8_t)(p[1])==0xfe)
      {
       const wchar_t *pw=text<wchar_t>((const wchar_t*)(p+2));
       vals.insert(std::make_pair(-id,Tval(text<char_t>(pw))));
       p=(const char*)(strchr(pw,'\0')+1);
      }
     else
      {
       vals.insert(std::make_pair(-id,Tval(text<char_t>(p))));
       p=strchr(p,'\0')+1;
      }
    }
   else
    {
     p-=2;
     break;
    }
  }
 if (last) *last=p;
}
bool TregOpIDstreamRead::_REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
{
 Tvals::const_iterator i=vals.find(id);
 if (i==vals.end())
  {
   Y=Z;
   return false;
  }
 else
  {
   Y=i->second.i;
   return true;
  }
}
void TregOpIDstreamRead::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 Tvals::const_iterator i=vals.find(id);
 if (i == vals.end())
  {
   ff_strncpy(Y, Z, buflen);
   Y[buflen-1]='\0';
   return;
  }

 if (multipleLines)
  {
   // To test. Enable image processing in ffdshow encoder and enter AviSynth script (preset name=ffdshowenc).
   size_t buflenMULTI_SZ = buflen * 2 + 1;
   char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
   ff_strncpy(bufMULTI_SZ, i->second.s.c_str(), buflenMULTI_SZ);
   processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
  }
 else
  {
   ff_strncpy(Y, i->second.s.c_str(), buflen);
   Y[buflen-1]='\0';
  }
}

//=============================== TregOpIDstreamWrite ===============================
//
//  To test. Use graphedit and add ffdshow encoder. Save graph.
//

bool TregOpIDstreamWrite::_REG_OP_N(short int id,const char_t *X,int &Y,const int)
{
 if (id)
  {
   append(id);
   append(Y);
  }
 return true;
}
void TregOpIDstreamWrite::_REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines)
{
 if (id)
  {
   append((short int)-id);
   if (sizeof(char_t)==sizeof(wchar_t))
    {
     static const uint8_t marker[]={0xff,0xfe};
     append(marker,2);
    }
   if (multipleLines)
    {
     // To test. Enable image processing in ffdshow encoder and enter AviSynth script (preset name=ffdshowenc).
     size_t buflenMULTI_SZ = buflen + 1;
     char_t *bufMULTI_SZ = (char_t *)alloca(sizeof(char_t) * buflenMULTI_SZ);
     processMultipleLines(Y, buflen, bufMULTI_SZ, buflenMULTI_SZ);
     append(outString.c_str(), (outString.size() + 1) * sizeof(char_t));
    }
   else
    {
     append(Y,(strlen(Y)+1)*sizeof(char_t));
    }
  }
}

//===================================================================================
static const char_t* hivename(HKEY hive)
{
 if      (hive==HKEY_LOCAL_MACHINE      ) return _l("HKEY_LOCAL_MACHINE");
 else if (hive==HKEY_CURRENT_USER       ) return _l("HKEY_CURRENT_USER");
 else if (hive==HKEY_CLASSES_ROOT       ) return _l("HKEY_CLASSES_ROOT");
 else if (hive==HKEY_USERS              ) return _l("HKEY_USERS");
 else if (hive==HKEY_PERFORMANCE_DATA   ) return _l("HKEY_PERFORMANCE_DATA");
 else if (hive==HKEY_PERFORMANCE_TEXT   ) return _l("HKEY_PERFORMANCE_TEXT");
 else if (hive==HKEY_PERFORMANCE_NLSTEXT) return _l("HKEY_PERFORMANCE_NLSTEXT");
 else if (hive==HKEY_CURRENT_CONFIG     ) return _l("HKEY_CURRENT_CONFIG");
 else if (hive==HKEY_DYN_DATA           ) return _l("HKEY_DYN_DATA");
 else return _l("");
}

bool regExport(Tstream &f,HKEY hive,const char_t *key,bool unicode)
{
 HKEY hkey=NULL;
 RegOpenKey(hive,key,&hkey);
 if (!hkey) return false;
 char_t subkey[256];
 int i;
 for (i=0;RegEnumKey(hkey,i,subkey,256)==ERROR_SUCCESS;i++)
  {
   char_t key2[256];
   tsnprintf_s(key2, countof(key2), _TRUNCATE, _l("%s\\%s"),key,subkey);
   regExport(f,hive,key2,unicode);
  }
 char_t valuename[256];DWORD valuenamelen=256;
 DWORD type,datalen;
 for (i=0;(valuenamelen=256,RegEnumValue(hkey,i,valuename,&valuenamelen,NULL,&type,NULL,&datalen))==ERROR_SUCCESS;i++)
  {
   BYTE *data=(BYTE*)_alloca(datalen);
   valuenamelen++;
   RegEnumValue(hkey,i,valuename,&valuenamelen,NULL,&type,data,&datalen);
   if (i==0) f.printf(_l("[%s\\%s]\n"),hivename(hive),key);
   switch (type)
    {
     case REG_DWORD:
      f.printf(_l("\"%s\"=dword:%08x\n"),valuename,*((DWORD*)data));
      break;
     case REG_SZ:
      {
       ffstring dataS=(const char_t*)data;
       f.printf(_l("\"%s\"=\"%s\"\n"),valuename,stringreplace(dataS,_l("\\"),_l("\\\\"),rfReplaceAll).c_str());
       break;
      }
     case REG_MULTI_SZ:
      {
       ffstring outstring(_l("\""));
       outstring += ffstring(valuename) + _l("\"=hex(7):");
       const char_t *indata = (const char_t*)data;
       while (*indata)
        {
         size_t len = strlen(indata);
         if (unicode)
          {
           text<wchar_t> uni(indata);
           BYTE *bin = (BYTE*)((const wchar_t *)uni);
           while (*bin || *(bin + 1))
            {
             char_t s[20];
             wsprintf(s,_l("%02x,%02x,"),*bin,*(bin+1));
             outstring += s;
             bin += 2;
            }
           outstring += _l("00,00,");
          }
         else
          {
           text<char> ansi(indata);
           BYTE *bin = (BYTE*)((const char *)ansi);
           while (*bin)
            {
             char_t s[20];
             wsprintf(s,_l("%02x,"),*bin);
             outstring += s;
             bin++;
            }
           outstring += _l("00,");
          }
         indata += len + 1;
        }
       if (unicode)
        outstring += _l("00,00\n");
       else
        outstring += _l("00\n");

       size_t insCount = 76;
       while (insCount < outstring.size())
        {
         insCount =  outstring.find(_l(","), insCount);
         if (insCount == DwString<char_t>::npos)
          break;
         insCount++;
         outstring.insert(insCount, _l("\\\n  "));
         insCount += 78;
        };
       f.printf(_l("%s"),outstring.c_str());
       break;
      }
    }
  }
 if (i>0) f.printf(_l("\n"));
 RegCloseKey(hkey);
 return true;
}
