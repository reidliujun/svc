/*
 * Copyright (c) 2008 h.yamagata
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Alternative implementation of ff_strncpy (subset of strncpy_s), strncat_s, vsnprintf_s, _splitpath_s, _makepath_s and _snprintf_s.
// This file is not used by MSVC8 or later. MSVC9 SP1 is the recommended compiler in this regard.
// This is a subset of the CRT to be used in ffdshow if it is compiled by MSVC7.1, does not support all features of the latest CRT.
// Calling of _invalid_parameter_handler is not supported.

#include "stdafx.h"
#include <errno.h>
#include <malloc.h>
#include "char_t.h"

static __forceinline wchar_t *strncpy(wchar_t *strDest, const wchar_t *strSource, size_t count){return wcsncpy(strDest, strSource, count);}
static __forceinline wchar_t *strncat(wchar_t *strDest, const wchar_t *strSource, size_t count){return wcsncat(strDest, strSource, count);}
static __forceinline void _splitpath(const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *fname, wchar_t *ext){return _wsplitpath(path, drive, dir, fname, ext);}
static __forceinline void _makepath(wchar_t *path, const wchar_t *drive, const wchar_t *dir, const wchar_t *fname, const wchar_t *ext){return _wmakepath(path, drive, dir, fname, ext);}

// returns INT_MAX if src does not terminate in count. Always check INT_MAX.
template<class tchar> static size_t strlen_limit(const tchar* src, size_t count)
{
 size_t i = 0;
 do{
  if (src[i]==0) return i;
  i++;
 }while(i < count);
 return INT_MAX;
}

template<class tchar> tchar* ff_strncpy(tchar *dst, const tchar *src, size_t count)
{
 if (!dst || !src || !count) return NULL;
 strncpy(dst, src, count);
 if (count <= strlen_limit(src, count))
  dst[count - 1] = 0;
 return dst;
}

template<class tchar> errno_t strncat_s(tchar *strDest, size_t numberOfElements, const tchar *strSource, size_t count)
{
 if (!strDest || !strSource) return EINVAL;
 if (!numberOfElements) return ERANGE;
 size_t dstlen = strlen(strDest);
 size_t srclen = count == _TRUNCATE ? strlen(strSource) : count;

 if (count != _TRUNCATE && dstlen + srclen >= numberOfElements)
  return ERANGE;

 tchar *temp = (tchar *)_alloca((dstlen + srclen + 1) * sizeof(tchar));
 strcpy(temp, strDest);
 strncat(temp, strSource, srclen);
 temp[dstlen + srclen] = 0;

 strncpy(strDest, temp, numberOfElements);
 if (dstlen + srclen >= numberOfElements)
  strDest[numberOfElements - 1] = 0;
 return 0;
}

template<class tchar> int vsnprintf_s(tchar *buffer, size_t sizeOfBuffer, size_t count, const tchar *format, va_list argptr)
{
 if (!buffer) return -1;
 if (!format || count == 0) return -1;
 tchar *temp = (tchar *)_alloca((sizeOfBuffer) * sizeof(tchar));
 int ret = _vsnprintf(temp, sizeOfBuffer - 1, format, argptr);
 if (count != _TRUNCATE && ret < 0)
  {
   buffer[0] = 0;
   return -1;
  }
 temp[sizeOfBuffer - 1] = 0;
 strcpy(buffer, temp);

 return ret;
}

template<class tchar> int tsnprintf_s(tchar *buffer, size_t sizeOfBuffer, size_t count, const tchar *format, ...)
{
 va_list argptr;
 va_start(argptr, format);
 return vsnprintf_s(buffer, sizeOfBuffer, count, format, argptr);
}

errno_t _splitpath_s(const char_t    *path,
                     char_t    *drive, size_t driveNumberOfElements,
                     char_t    *dir,   size_t dirNumberOfElements,
                     char_t    *fname, size_t nameNumberOfElements,
                     char_t    *ext,   size_t extNumberOfElements)
{
 if (!path) return EINVAL;
 if (!drive && driveNumberOfElements) return EINVAL;
 if (drive && !driveNumberOfElements) return EINVAL;
 if (!dir && dirNumberOfElements)     return EINVAL;
 if (dir && !dirNumberOfElements)     return EINVAL;
 if (!fname && nameNumberOfElements)  return EINVAL;
 if (fname && !nameNumberOfElements)  return EINVAL;
 if (!ext && extNumberOfElements)     return EINVAL;
 if (ext && !extNumberOfElements)     return EINVAL;

 size_t len = strlen(path);
 char_t *tmp_drive = (char_t *)_alloca((len + 1) * sizeof(char_t));
 char_t *tmp_dir   = (char_t *)_alloca((len + 1) * sizeof(char_t));
 char_t *tmp_fname = (char_t *)_alloca((len + 1) * sizeof(char_t));
 char_t *tmp_ext   = (char_t *)_alloca((len + 1) * sizeof(char_t));
 _splitpath(path, tmp_drive, tmp_dir, tmp_fname, tmp_ext);

 tmp_drive[len] = 0; // not necessary?
 tmp_dir  [len] = 0;
 tmp_fname[len] = 0;
 tmp_ext  [len] = 0;

 if (   (drive && strlen(tmp_drive) >= driveNumberOfElements)
     || (dir   && strlen(tmp_dir)   >= dirNumberOfElements)
     || (fname && strlen(tmp_fname) >= nameNumberOfElements)
     || (ext   && strlen(tmp_ext)   >= extNumberOfElements))
  {
   if (drive) drive[0] = 0;
   if (dir)   dir  [0] = 0;
   if (fname) fname[0] = 0;
   if (ext)   ext  [0] = 0;
   return ERANGE;
  }

 if (drive) strcpy(drive, tmp_drive);
 if (dir)   strcpy(dir,   tmp_dir);
 if (fname) strcpy(fname, tmp_fname);
 if (ext)   strcpy(ext,   tmp_ext);
 return 0;
}

errno_t _makepath_s(char_t    *path,
                    size_t sizeInCharacters,
                    const char_t    *drive,
                    const char_t    *dir,
                    const char_t    *fname,
                    const char_t    *ext)
{
 if (!path || sizeInCharacters <= 0) return EINVAL;
 size_t len = 20;
 if (drive) len += strlen(drive);
 if (dir)   len += strlen(dir);
 if (fname) len += strlen(fname);
 if (ext)   len += strlen(ext);

 char_t *temp = (char_t *)_alloca(len * sizeof(char_t));
 _makepath(temp, drive, dir, fname, ext);
 if (strlen(temp) >= sizeInCharacters)
  {
   path[0] = 0;
   return ERANGE;
  }
 strcpy(path, temp);
 return 0;
}

template char* ff_strncpy(char *dst, const char *src, size_t count);
template wchar_t* ff_strncpy(wchar_t *dst, const wchar_t *src, size_t count);
template errno_t strncat_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count);
template errno_t strncat_s(wchar_t *strDest, size_t numberOfElements, const wchar_t *strSource, size_t count);
template int vsnprintf_s(char *buffer, size_t sizeOfBuffer, size_t count, const char *format, va_list argptr);
template int vsnprintf_s(wchar_t *buffer, size_t sizeOfBuffer, size_t count, const wchar_t *format, va_list argptr);
template int tsnprintf_s(char *buffer, size_t sizeOfBuffer, size_t count, const char *format, ...);
template int tsnprintf_s(wchar_t *buffer, size_t sizeOfBuffer, size_t count, const wchar_t *format, ...);
