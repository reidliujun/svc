/*
 * Profile functions
 *
 * Copyright 1993 Miguel de Icaza
 * Copyright 1996 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "inifiles.h"
#include "ffdebug.h"
#include "Tstream.h"

Tinifile::Tinifile(const char_t *Iflnm,int Iencoding):CurProfile(NULL)
{
 ff_strncpy(filename, Iflnm, countof(filename));
 encoding=Iencoding==-1?(sizeof(char_t)==sizeof(char)?Tstream::ENC_ASCII:Tstream::ENC_LE16):Iencoding;
}
Tinifile::~Tinifile()
{
 if (CurProfile)
  {
   PROFILE_ReleaseFile();
   free(CurProfile);
  }
}


void Tinifile::PROFILE_WriteMarker(Tstream &hFile)
{
}

void Tinifile::PROFILE_WriteLine( Tstream &hFile, char_t * szLine, size_t len)
{
    //DWORD dwBytesWritten;
    //WriteFile(hFile, szLine, (DWORD)len*sizeof(char_t), &dwBytesWritten, NULL);
    hFile.fputs(szLine);
}

void Tinifile::PROFILE_Save( Tstream &hFile, const PROFILESECTION *section)
{
    PROFILEKEY *key;
    char_t *buffer, *p;

    PROFILE_WriteMarker(hFile);

    for ( ; section; section = section->next)
    {
        size_t len = 0;

        if (section->name[0]) len += strlen(section->name) + 6;

        for (key = section->key; key; key = key->next)
        {
            len += strlen(key->name) + 2;
            if (key->value) len += strlen(key->value) + 1;
        }

        buffer = (char_t*)alloca(len * sizeof(char_t));
        if (!buffer) return;

        p = buffer;
        if (section->name[0])
        {
            //*p++ = '\r';
            *p++ = '\n';
            *p++ = '[';
            strcpy( p, section->name );
            p += strlen(p);
            *p++ = ']';
            //*p++ = '\r';
            *p++ = '\n';
        }
        for (key = section->key; key; key = key->next)
        {
            strcpy( p, key->name );
            p += strlen(p);
            if (key->value)
            {
                *p++ = '=';
                strcpy( p, key->value );
                p += strlen(p);
            }
            //*p++ = '\r';
            *p++ = '\n';
        }
        *p='\0';
        PROFILE_WriteLine( hFile, buffer, len);
    }
}

BOOL Tinifile::PROFILE_FlushFile(void)
{
    HANDLE hFile = NULL;
    FILETIME LastWriteTime;

    if(!CurProfile)
    {
        return FALSE;
    }

    if (!CurProfile->changed) return TRUE;

    TstreamFile *file=new TstreamFile(CurProfile->filename,false,true,(Tstream::ENCODING)encoding);
    PROFILE_Save( *file, CurProfile->section);
    delete file;
    hFile = CreateFile(CurProfile->filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;
    if(GetFileTime(hFile, NULL, NULL, &LastWriteTime))
       CurProfile->LastWriteTime=LastWriteTime;
    CloseHandle( hFile );
    CurProfile->changed = FALSE;
    return TRUE;
}

void Tinifile::PROFILE_Free( PROFILESECTION *section )
{
    PROFILESECTION *next_section;
    PROFILEKEY *key, *next_key;

    for ( ; section; section = next_section)
    {
        for (key = section->key; key; key = next_key)
        {
            next_key = key->next;
            if (key->value) free(key->value );
            free(key );
        }
        next_section = section->next;
        free(section );
    }
}

void Tinifile::PROFILE_ReleaseFile(void)
{
    PROFILE_FlushFile();
    PROFILE_Free( CurProfile->section );
    if (CurProfile->filename) free(CurProfile->filename );
    CurProfile->changed = FALSE;
    CurProfile->section = NULL;
    CurProfile->filename  = NULL;
    ZeroMemory(&CurProfile->LastWriteTime, sizeof(CurProfile->LastWriteTime));
}

const char_t* Tinifile::PROFILE_GetLine(const char_t * szStart, const char_t * szEnd)
{
    return (char_t*)memchr(szStart, _l('\n'), szEnd - szStart);
}

int Tinifile::PROFILE_isspace(char_t c)
{
	if (tchar_traits<char_t>::isspace((tchar_traits<char_t>::uchar_t)c)) return 1;
	if (c=='\r' || c==0x1a) return 1;
	/* CR and ^Z (DOS EOF) are spaces too  (found on CD-ROMs) */
	return 0;
}

const char_t* Tinifile::memrchr (const char_t *block, char_t c, size_t size)
{
  for (const char_t *p = block + size; p != block; p--)
    if (*p == c)
      return p;
  return 0;
}
void Tinifile::PROFILE_DetectTextEncoding(const void * buffer, size_t * len)
{
    *len = 0;
}

Tinifile::PROFILESECTION* Tinifile::PROFILE_Load(HANDLE hFile0)
{
    uint8_t *pBuffer;
    //char_t * szFile;
    //const char_t *szLineStart, *szLineEnd;
    const char_t *szValueStart, *szNameEnd;//, *szEnd;
    int line = 0; size_t len;
    PROFILESECTION *section, *first_section;
    PROFILESECTION **next_section;
    PROFILEKEY *key, *prev_key, **next_key;
    DWORD dwFileSize;

    dwFileSize = GetFileSize(hFile0, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
        return NULL;

    pBuffer = (uint8_t*)malloc(dwFileSize);
    if (!pBuffer) return NULL;

    if (!ReadFile(hFile0, pBuffer, dwFileSize, &dwFileSize, NULL))
    {
        free(pBuffer);
        return NULL;
    }
    len = dwFileSize;

    TstreamMem hFile(pBuffer,len,Tstream::ENC_AUTODETECT);hFile.stripEOLN(true);
    encoding=hFile.encoding;

    //PROFILE_DetectTextEncoding(pBuffer, &len);
    /* len is set to the number of bytes in the character marker.
     * we want to skip these bytes */
    //pBuffer = (char_t*)((char *)pBuffer + len);
    //dwFileSize -= (DWORD)len;

        //szFile = (char_t *)pBuffer;
        //szEnd = (char_t *)((char *)pBuffer + dwFileSize);

    first_section = (PROFILESECTION*)malloc(sizeof(*section) );
    if(first_section == NULL)
    {
        //if (szFile != pBuffer)
            //free( szFile);
        free(pBuffer);
        return NULL;
    }
    first_section->name[0] = 0;
    first_section->key  = NULL;
    first_section->next = NULL;
    next_section = &first_section->next;
    next_key     = &first_section->key;
    prev_key     = NULL;
    //szLineEnd = szFile - 1; /* will be increased to correct value in loop */

    char_t *szLineStart0 = (char_t*)malloc((dwFileSize+30)*sizeof(char_t));
    char_t *szLineStart = szLineStart0;
    if (!szLineStart) return NULL;

    while (hFile.fgets(szLineStart,dwFileSize))
    {
        const char_t *szLineEnd=strchr(szLineStart,'\0');
        //szLineStart = szLineEnd + 1;
        //if (szLineStart >= szEnd)
            //break;
        //szLineEnd = PROFILE_GetLine(szLineStart, szEnd);
        //if (!szLineEnd)
            //szLineEnd = szEnd;
        //line++;

        while (*szLineStart && PROFILE_isspace(*szLineStart)) szLineStart++;

        if (!*szLineStart /*>= szLineEnd*/) continue;

        if (*szLineStart == '[')  /* section start */
        {
            const char_t * szSectionEnd;
            if ((szSectionEnd = memrchr( szLineStart, _l(']'), szLineEnd - szLineStart ))==NULL)
            {
                //WARN("Invalid section header at line %d: %s\n",line, debugstr_wn(szLineStart, (int)(szLineEnd - szLineStart)) );
            }
            else
            {
                szLineStart++;
                len = (int)(szSectionEnd - szLineStart);
                /* no need to allocate +1 for NULL terminating character as
                 * already included in structure */
                if ((section = (PROFILESECTION*)malloc(sizeof(*section) + len * sizeof(char_t) ))==NULL)
                    break;
                memcpy(section->name, szLineStart, len * sizeof(char_t));
                section->name[len] = '\0';
                section->key  = NULL;
                section->next = NULL;
                *next_section = section;
                next_section  = &section->next;
                next_key      = &section->key;
                prev_key      = NULL;

                //TRACE("New section: %s\n", debugstr_w(section->name));

                continue;
            }
        }

        /* get rid of white space at the end of the line */
        while ((szLineEnd > szLineStart) && ((*szLineEnd == '\n') || PROFILE_isspace(*szLineEnd))) szLineEnd--;

        /* line end should be pointing to character *after* the last wanted character */
        szLineEnd++;

        /* get rid of white space after the name and before the start
         * of the value */
        if ((szNameEnd = szValueStart = (const char_t*)memchr( szLineStart, _l('='), szLineEnd - szLineStart )) != NULL)
        {
            szNameEnd = szValueStart - 1;
            while ((szNameEnd > szLineStart) && PROFILE_isspace(*szNameEnd)) szNameEnd--;
            szValueStart++;
            while (szValueStart < szLineEnd && PROFILE_isspace(*szValueStart)) szValueStart++;
        }
        if (!szNameEnd)
            szNameEnd = szLineEnd - 1;
        /* name end should be pointing to character *after* the last wanted character */
        szNameEnd++;

        len = (int)(szNameEnd - szLineStart);

        if (len || !prev_key || *prev_key->name)
        {
            /* no need to allocate +1 for NULL terminating character as
             * already included in structure */
            if ((key = (PROFILEKEY*)malloc(sizeof(*key) + len * sizeof(char_t) ))==NULL) break;
            memcpy(key->name, szLineStart, len * sizeof(char_t));
            key->name[len] = '\0';
            if (szValueStart)
            {
                len = (int)(szLineEnd - szValueStart);
                key->value = (char_t*)malloc((len + 1) * sizeof(char_t) );
                memcpy(key->value, szValueStart, len * sizeof(char_t));
                key->value[len] = '\0';
            }
            else key->value = NULL;

           key->next  = NULL;
           *next_key  = key;
           next_key   = &key->next;
           prev_key   = key;

           //TRACE("New key: name=%s, value=%s\n", debugstr_w(key->name), key->value ? debugstr_w(key->value) : "(none)");
        }
    }
    //if (szFile != pBuffer)
        //free(szFile);
    free(pBuffer);
    free(szLineStart0);
    return first_section;
}

void Tinifile::PROFILE_CopyEntry( LPTSTR buffer, LPCTSTR value, size_t len, BOOL strip_quote )
{
    char_t quote = '\0';

    if(!buffer) return;

    if (strip_quote && ((*value == '\'') || (*value == '\"')))
    {
        if (value[1] && (value[strlen(value)-1] == *value)) quote = *value++;
    }

    _tcsncpy( buffer, value, len );
    if (quote && (len >= (int)strlen(value))) buffer[strlen(buffer)-1] = '\0';
}

INT Tinifile::PROFILE_GetSection( PROFILESECTION *section, LPCTSTR section_name, LPTSTR buffer, size_t len, BOOL return_values )
{
    PROFILEKEY *key;

    if(!buffer) return 0;

    while (section)
    {
        if (section->name[0] && !strcmp( section->name, section_name ))
        {
            size_t oldlen = len;
            for (key = section->key; key; key = key->next)
            {
                if (len <= 2) break;
                if (!*key->name) continue;  /* Skip empty lines */
                if (key->name[0] == ';') continue;  /* Skip comments */
                PROFILE_CopyEntry( buffer, key->name, len - 1, 0 );
                len -= strlen(buffer) + 1;
                buffer += strlen(buffer) + 1;
		if (len < 2)
		    break;
		if (return_values && key->value) {
			buffer[-1] = '=';
			PROFILE_CopyEntry ( buffer, key->value, len - 1, 0 );
			len -= strlen(buffer) + 1;
			buffer += strlen(buffer) + 1;
                }
            }
            *buffer = '\0';
            if (len <= 1)
                /*If either lpszSection or lpszKey is NULL and the supplied
                  destination buffer is too small to hold all the strings,
                  the last string is truncated and followed by two null characters.
                  In this case, the return value is equal to cchReturnBuffer
                  minus two. */
            {
		buffer[-1] = '\0';
                return INT(oldlen) - 2;
            }
            return INT(oldlen - len);
        }
        section = section->next;
    }
    buffer[0] = buffer[1] = '\0';
    return 0;
}

BOOL Tinifile::PROFILE_Open(void)
{
    char_t windirW[MAX_PATH];
    char_t buffer[MAX_PATH];
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME LastWriteTime;

    ZeroMemory(&LastWriteTime, sizeof(LastWriteTime));

    /* First time around */

    if(!CurProfile)
       {
          CurProfile=(PROFILE*)malloc(sizeof(PROFILE) );
          if(CurProfile == NULL) return FALSE;
          CurProfile->changed=FALSE;
          CurProfile->section=NULL;
          CurProfile->filename=NULL;
          ZeroMemory(&CurProfile->LastWriteTime, sizeof(FILETIME));
       }

    GetWindowsDirectory( windirW, MAX_PATH );
/*
    if ((RtlDetermineDosPathNameType_U(filename) == RELATIVE_PATH) &&
        !strchrW(filename, '\\') && !strchrW(filename, '/'))
    {
        static const CHAR wszSeparator[] = {'\\', 0};
        strcpy(buffer, windirW);
        strcat(buffer, wszSeparator);
        strcat(buffer, filename);
    }
    else*/
    {
        LPTSTR dummy;
        GetFullPathName(filename, sizeof(buffer)/sizeof(buffer[0]), buffer, &dummy);
    }

    hFile = CreateFile(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile == INVALID_HANDLE_VALUE) && (GetLastError() != ERROR_FILE_NOT_FOUND))
    {
        return FALSE;
    }
       if ((CurProfile->filename && !strcmp( buffer, CurProfile->filename )))
       {
          GetFileTime(hFile, NULL, NULL, &LastWriteTime);
          if(memcmp(&CurProfile->LastWriteTime, &LastWriteTime, sizeof(FILETIME)))
             ;//DPRINTF("(%s): already opened (mru=%d)\n", buffer,0);
          else
              ;//DPRINTF("(%s): already opened, needs refreshing (mru=%d)\n",buffer,0);
          CloseHandle(hFile);
          return TRUE;
        }
    /* Flush the old current profile */
    PROFILE_FlushFile();

    /* Make the oldest profile the current one only in order to get rid of it */
/*
    if(i==N_CACHED_PROFILES)
      {
       tempProfile=MRUProfile[N_CACHED_PROFILES-1];
       for(i=N_CACHED_PROFILES-1;i>0;i--)
          MRUProfile[i]=MRUProfile[i-1];
       CurProfile=tempProfile;
      }
    if(CurProfile->filename) PROFILE_ReleaseFile();
*/
    /* OK, now that CurProfile is definitely free we assign it our new file */
    CurProfile->filename  = (char_t*)malloc((strlen(buffer)+1) * sizeof(char_t) );
    strcpy( CurProfile->filename, buffer );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CurProfile->section = PROFILE_Load(hFile);
        GetFileTime(hFile, NULL, NULL, &CurProfile->LastWriteTime);
        CloseHandle(hFile);
    }
    else
    {
        /* Does not exist yet, we will create it in PROFILE_FlushFile */
    }
    return TRUE;
}

BOOL Tinifile::PROFILE_DeleteSection( PROFILESECTION **section, LPCTSTR name )
{
    while (*section)
    {
        if ((*section)->name[0] && !strcmp( (*section)->name, name ))
        {
            PROFILESECTION *to_del = *section;
            *section = to_del->next;
            to_del->next = NULL;
            PROFILE_Free( to_del );
            return TRUE;
        }
        section = &(*section)->next;
    }
    return FALSE;
}
BOOL Tinifile::PROFILE_DeleteKey( PROFILESECTION **section,LPCTSTR section_name, LPCTSTR key_name )
{
    while (*section)
    {
        if ((*section)->name[0] && !strcmp( (*section)->name, section_name ))
        {
            PROFILEKEY **key = &(*section)->key;
            while (*key)
            {
                if (!strcmp( (*key)->name, key_name ))
                {
                    PROFILEKEY *to_del = *key;
                    *key = to_del->next;
                    if (to_del->value) free(to_del->value);
                    free(to_del );
                    return TRUE;
                }
                key = &(*key)->next;
            }
        }
        section = &(*section)->next;
    }
    return FALSE;
}

Tinifile::PROFILEKEY* Tinifile::PROFILE_Find( PROFILESECTION **section, LPCTSTR section_name, LPCTSTR key_name, BOOL create, BOOL create_always )
{
    LPCTSTR p;
    size_t seclen, keylen;

    while (PROFILE_isspace(*section_name)) section_name++;
    p = section_name + strlen(section_name) - 1;
    while ((p > section_name) && PROFILE_isspace(*p)) p--;
    seclen = p - section_name + 1;

    while (PROFILE_isspace(*key_name)) key_name++;
    p = key_name + strlen(key_name) - 1;
    while ((p > key_name) && PROFILE_isspace(*p)) p--;
    keylen = p - key_name + 1;

    while (*section)
    {
        if ( ((*section)->name[0])
             && (!(strncmp( (*section)->name, section_name, seclen )))
             && (((*section)->name)[seclen] == '\0') )
        {
            PROFILEKEY **key = &(*section)->key;

            while (*key)
            {
                /* If create_always is FALSE then we check if the keyname
                 * already exists. Otherwise we add it regardless of its
                 * existence, to allow keys to be added more than once in
                 * some cases.
                 */
                if(!create_always)
                {
                    if ( (!(strncmp( (*key)->name, key_name, keylen )))
                         && (((*key)->name)[keylen] == '\0') )
                        return *key;
                }
                key = &(*key)->next;
            }
            if (!create) return NULL;
            if ((*key = (PROFILEKEY*)malloc(sizeof(PROFILEKEY) + strlen(key_name) * sizeof(char_t) ))==NULL)
                return NULL;
            strcpy( (*key)->name, key_name );
            (*key)->value = NULL;
            (*key)->next  = NULL;
            return *key;
        }
        section = &(*section)->next;
    }
    if (!create) return NULL;
    *section = (PROFILESECTION*)malloc(sizeof(PROFILESECTION) + strlen(section_name) * sizeof(char_t) );
    if(*section == NULL) return NULL;
    strcpy( (*section)->name, section_name );
    (*section)->next = NULL;
    if (((*section)->key  = (PROFILEKEY*)malloc(sizeof(PROFILEKEY) + strlen(key_name) * sizeof(char_t) ))==NULL)
    {
        free(*section);
        return NULL;
    }
    strcpy( (*section)->key->name, key_name );
    (*section)->key->value = NULL;
    (*section)->key->next  = NULL;
    return (*section)->key;
}

BOOL Tinifile::PROFILE_SetString( LPCTSTR section_name, LPCTSTR key_name, LPCTSTR value, BOOL create_always )
{
    if (!key_name)  /* Delete a whole section */
    {
        CurProfile->changed |= PROFILE_DeleteSection( &CurProfile->section,
                                                      section_name );
        return TRUE;         /* Even if PROFILE_DeleteSection() has failed,
                                this is not an error on application's level.*/
    }
    else if (!value)  /* Delete a key */
    {
        CurProfile->changed |= PROFILE_DeleteKey( &CurProfile->section,
                                                  section_name, key_name );
        return TRUE;          /* same error handling as above */
    }
    else  /* Set the key value */
    {
        PROFILEKEY *key = PROFILE_Find(&CurProfile->section, section_name,
                                        key_name, TRUE, create_always );
        if (!key) return FALSE;

        /* strip the leading spaces. We can safely strip \n\r and
         * friends too, they should not happen here anyway. */
        while (PROFILE_isspace(*value)) value++;

        if (key->value)
        {
            if (!strcmp( key->value, value ))
            {
                //TRACE("  no change needed\n" );
                return TRUE;  /* No change needed */
            }
            //TRACE("  replacing %s\n", debugstr_w(key->value) );
            free(key->value );
        }
        else /*TRACE("  creating key\n" )*/;
        key->value = (char_t*)malloc((strlen(value)+1) * sizeof(char_t) );
        strcpy( key->value, value );
        CurProfile->changed = TRUE;
    }
    return TRUE;
}
INT Tinifile::PROFILE_GetSectionNames( LPTSTR buffer, size_t len )
{
    LPTSTR buf;
    size_t f,l;
    PROFILESECTION *section;

    if (!buffer || !len)
        return 0;
    if (len==1) {
        *buffer='\0';
        return 0;
    }

    f=len-1;
    buf=buffer;
    section = CurProfile->section;
    while ((section!=NULL)) {
        if (section->name[0]) {
            l = strlen(section->name)+1;
            if (l > f) {
                if (f>0) {
                    _tcsncpy(buf, section->name, f-1);
                    buf += f-1;
                    *buf++='\0';
                }
                *buf='\0';
                return INT(len)-2;
            }
            strcpy(buf, section->name);
            buf += l;
            f -= l;
        }
        section = section->next;
    }
    *buf='\0';
    return INT(buf-buffer);
}

INT Tinifile::PROFILE_GetString( LPCTSTR section, LPCTSTR key_name, LPCTSTR def_val, LPTSTR buffer, size_t len )
{
    PROFILEKEY *key = NULL;
    static const char_t empty_str[] = { 0 };

    if(!buffer) return 0;

    if (!def_val) def_val = empty_str;
    if (key_name)
    {
	if (!key_name[0])
        {
            /* Win95 returns 0 on keyname "". Tested with Likse32 bon 000227 */
            return 0;
        }
        key = PROFILE_Find( &CurProfile->section, section, key_name, FALSE, FALSE);
        PROFILE_CopyEntry( buffer, (key && key->value) ? key->value : def_val,
                           len, TRUE );
        return (INT)strlen( buffer );
    }
    /* no "else" here ! */
    if (section && section[0])
    {
        INT ret = PROFILE_GetSection(CurProfile->section, section, buffer, len, FALSE);
        if (!buffer[0]) /* no luck -> def_val */
        {
            PROFILE_CopyEntry(buffer, def_val, len, TRUE);
            ret = (INT)strlen(buffer);
        }
        return ret;
    }
    buffer[0] = '\0';
    return 0;
}

int Tinifile::PROFILE_GetPrivateProfileString( LPCTSTR section, LPCTSTR entry,LPCTSTR def_val, LPTSTR buffer, size_t len, BOOL allow_section_name_copy )
{
    int		ret;
    LPCTSTR	pDefVal = NULL;

    if (!filename)
	    return 0;

    /* strip any trailing ' ' of def_val. */
    if (def_val)
    {
        LPCTSTR p = &def_val[strlen(def_val)]; /* even "" works ! */

	while (p > def_val)
	{
	    p--;
	    if ((*p) != ' ')
		break;
	}
	if (*p == ' ') /* ouch, contained trailing ' ' */
	{
	    int l = (int)(p - def_val);
            LPTSTR pp;

	    pp = (LPTSTR)malloc((l + 1) * sizeof(char_t));
	    _tcsncpy(pp, def_val, l);
	    pp[l] = '\0';
            pDefVal = pp;
	}
    }
    if (!pDefVal)
	pDefVal = /*(LPCTSTR)*/def_val;

    if (PROFILE_Open()) {
	if ((allow_section_name_copy) && (section == NULL))
            ret = PROFILE_GetSectionNames(buffer, len);
	else
	    /* PROFILE_GetString already handles the 'entry == NULL' case */
            ret = PROFILE_GetString( section, entry, pDefVal, buffer, len );
    } else {
       _tcsncpy( buffer, pDefVal, len );
       ret = (INT)strlen( buffer );
    }

    if (pDefVal != def_val) /* allocated */
	free((void*)pDefVal);


    return ret;
}


DWORD Tinifile::getPrivateProfileSection(const char_t *section, char_t *buffer,DWORD len)
{
 DWORD	ret=0;
 if (PROFILE_Open())
  ret=PROFILE_GetSection(CurProfile->section,section,buffer,len,TRUE);
 return ret;
}
BOOL Tinifile::writePrivateProfileString( const char_t * section, const char_t *entry, const char_t *string)
{
    BOOL ret = FALSE;

    if (PROFILE_Open( ))
    {
        if (!section && !entry && !string) /* documented "file flush" case */
        {
            PROFILE_FlushFile();
            PROFILE_ReleaseFile();  /* always return FALSE in this case */
        }
	else {
	    if (!section) {
	    } else {
		ret = PROFILE_SetString( section, entry, string, FALSE);
		PROFILE_FlushFile();
	    }
	}
    }
    return ret;
}

DWORD Tinifile::getPrivateProfileSectionNames( char_t *buffer, DWORD size)
{
    DWORD ret = 0;

    if (PROFILE_Open( ))
        ret = PROFILE_GetSectionNames(buffer, size);
    return ret;
}

DWORD Tinifile::getPrivateProfileString( const char_t *section, const char_t * entry, const char_t * def_val,  char_t * buffer, DWORD len)
{
    return PROFILE_GetPrivateProfileString( section, entry, def_val,
                                            buffer, len, TRUE );
}
int Tinifile::getPrivateProfileInt(const char_t *section,const char_t *entry,int def_val)
{
 char_t def_valS[60],retS[256];
 getPrivateProfileString(section,entry,_itoa(def_val,def_valS,10),retS,256);
 return atoi(retS);
}
