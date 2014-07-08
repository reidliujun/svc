#include "stdafx.h"
#include <windows.h>
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "isNotCalledFromBlackList.h"
#include "TcompatibilityManager.h"
#include "ffdebug.h"

// Explorer.exe loads ffdshow.ax and never releases.
// That causes annoying error on re-install that one have to log off.
// With this patch, ffdshow.ax avoids to be loaded by returning false on DllMain
// if the caller is included in BlackList("Don't use ffdshow in:").
bool isNotCalledFromBlackList(HINSTANCE hInstance)
{
 TcompatibilityManager::s_mode=0;
 bool result= true;
 strings blacklistList2;
 HKEY hKey= NULL;
 LONG regErr;

 // read from registry directly because it is difficult to initialize Tconfig in DllMain.(Because it loads module)
 regErr= RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT _l("\\") FFDSHOW, 0, KEY_READ, &hKey);
 if(regErr!=ERROR_SUCCESS)
  return true;

 TregOpRegRead tHKCU_global(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tHKCU_global._REG_OP_N(IDFF_allowDPRINTF,_l("allowDPRINTF"),allowDPRINTF,0);

 DWORD type;
 DWORD isBlacklist;
 DWORD isWhitelist;
 DWORD cbData=sizeof(isBlacklist);
 char_t blacklist[MAX_COMPATIBILITYLIST_LENGTH];
 char_t fileName[MAX_PATH+2];
 char_t cmdBuf[MAX_PATH+3];
 char_t* cmdCopy=cmdBuf;
 char_t* cmd;
 char_t* endOfCmd;

 /* get filename of calling application */
 cmd= GetCommandLine();
 cmdCopy[MAX_PATH+2]='\0';
 ff_strncpy(cmdCopy,cmd,MAX_PATH+2);
 if(cmdCopy[0]=='"')
  {
   cmdCopy++;
   if(cmdCopy[0]!=NULL)
    {
     endOfCmd= _tcschr(cmdCopy, '"');
     if(endOfCmd && endOfCmd <= &(cmdCopy[MAX_PATH+1]))
      *endOfCmd= '\0';
    }
  }
 extractfilename(cmdCopy,fileName);

 // FIXME: use TregOpRegRead
 regErr= RegQueryValueEx(hKey, _l("isBlacklist"), NULL, &type, (LPBYTE)&isBlacklist, &cbData);
 if(regErr==ERROR_SUCCESS && isBlacklist)
  {
   cbData= sizeof(blacklist);
   regErr= RegQueryValueEx(hKey, _l("blacklist"), NULL, &type, (LPBYTE)blacklist, &cbData);
   if (regErr==ERROR_SUCCESS)
    {
     strings blacklistList;
     strtok(blacklist,_l(";"),blacklistList);

     for (strings::const_iterator b=blacklistList.begin();b!=blacklistList.end();b++)
      {
       if (DwStrcasecmp(*b,_l("explorer.exe"))==0)
        {
         blacklistList2.push_back(_l("explorer.exe"));
         break;
        }
      }
    }
  }

 /* some applications/games that are always blacklisted */
 blacklistList2.push_back(_l("oblivion.exe"));
 blacklistList2.push_back(_l("morrowind.exe"));
 blacklistList2.push_back(_l("YSO_WIN.exe"));
 blacklistList2.push_back(_l("WORMS 4 MAYHEM.EXE"));
 blacklistList2.push_back(_l("sh3.exe"));
 blacklistList2.push_back(_l("fallout3.exe"));
 blacklistList2.push_back(_l("hl2.exe"));
 blacklistList2.push_back(_l("gta_sa.exe"));
 blacklistList2.push_back(_l("age3.exe"));
 blacklistList2.push_back(_l("pes2008.exe"));
 blacklistList2.push_back(_l("pes2009.exe"));
 
 for (strings::const_iterator b=blacklistList2.begin();b!=blacklistList2.end();b++)
  {
   if (DwStrcasecmp(*b,fileName)==0)
    {
     result= false;
     break;
    }
  }
 if (result && stricmp(fileName,_l("explorer.exe"))==0)
  {
   regErr= RegQueryValueEx(hKey, _l("isWhitelist"), NULL, &type, (LPBYTE)&isWhitelist, &cbData);
   if(regErr==ERROR_SUCCESS && isWhitelist)
    {
     result=false;
     cbData= sizeof(blacklist);
     regErr= RegQueryValueEx(hKey, _l("whitelist"), NULL, &type, (LPBYTE)blacklist, &cbData);
     if (regErr==ERROR_SUCCESS)
      {
       strings whitelistList;
       strtok(blacklist,_l(";"),whitelistList);

       for (strings::const_iterator b=whitelistList.begin();b!=whitelistList.end();b++)
        {
         if (DwStrcasecmp(*b,_l("explorer.exe"))==0)
          {
           result=true;
           break;
          }
        }
      }
    }
  }
 if(hKey)
  RegCloseKey(hKey);
 return result;
}
