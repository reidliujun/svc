#ifndef _INIFILES_H_
#define _INIFILES_H_

struct Tstream;
class Tinifile
{
private:
 char_t filename[MAX_PATH];
 struct PROFILEKEY
  {
   char_t       *value;
   PROFILEKEY  *next;
   char_t       name[1];
  };
 struct PROFILESECTION
  {
   PROFILEKEY       *key;
   PROFILESECTION   *next;
   char_t                    name[1];
  };
 struct PROFILE
  {
   BOOL             changed;
   PROFILESECTION  *section;
   char_t           *filename;
   FILETIME LastWriteTime;
  };
 PROFILE *CurProfile;
 static void PROFILE_WriteMarker(Tstream & hFile);
 static void PROFILE_WriteLine(Tstream & hFile, char_t * szLine, size_t len);
 static void PROFILE_Save( Tstream & hFile, const PROFILESECTION *section);
 BOOL PROFILE_FlushFile(void);
 static void PROFILE_Free( PROFILESECTION *section );
 void PROFILE_ReleaseFile(void);
 static const char_t* PROFILE_GetLine(const char_t * szStart, const char_t * szEnd);
 static int PROFILE_isspace(char_t c);
 static const char_t *memrchr (const char_t *block, char_t c, size_t size);
 static void PROFILE_DetectTextEncoding(const void * buffer, size_t * len);
 PROFILESECTION* PROFILE_Load(HANDLE hFile);
 static void PROFILE_CopyEntry( LPTSTR buffer, LPCTSTR value, size_t len, BOOL strip_quote );
 INT PROFILE_GetSection( PROFILESECTION *section, LPCTSTR section_name, LPTSTR buffer, size_t len, BOOL return_values );
 BOOL PROFILE_Open(void);
 static BOOL PROFILE_DeleteSection( PROFILESECTION **section, LPCTSTR name );
 static BOOL PROFILE_DeleteKey( PROFILESECTION **section,LPCTSTR section_name, LPCTSTR key_name );
 static PROFILEKEY *PROFILE_Find( PROFILESECTION **section, LPCTSTR section_name, LPCTSTR key_name, BOOL create, BOOL create_always );
 BOOL PROFILE_SetString( LPCTSTR section_name, LPCTSTR key_name, LPCTSTR value, BOOL create_always );
 INT PROFILE_GetSectionNames( LPTSTR buffer, size_t len );
 INT PROFILE_GetString( LPCTSTR section, LPCTSTR key_name, LPCTSTR def_val, LPTSTR buffer, size_t len );
 int PROFILE_GetPrivateProfileString( LPCTSTR section, LPCTSTR entry,LPCTSTR def_val, LPTSTR buffer, size_t len, BOOL allow_section_name_copy );
public:
 Tinifile(const char_t *flnm,int Iencoding=-1);
 ~Tinifile();
 int encoding;
 DWORD getPrivateProfileSection(const char_t *lpAppName,char_t *lpReturnedString,DWORD nSize);
 BOOL writePrivateProfileString(const char_t *lpAppName,const char_t *lpKeyName,const char_t *lpString);
 DWORD getPrivateProfileSectionNames(char_t *lpszReturnBuffer,DWORD nSize);
 DWORD getPrivateProfileString(const char_t *lpAppName,const char_t *lpKeyName,const char_t *lpDefault,char_t *lpReturnedString,DWORD nSize);
 int getPrivateProfileInt(const char_t *lpAppName,const char_t *lpKeyName,int lpDefault);
};

#endif
