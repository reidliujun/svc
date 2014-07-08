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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "char_t.h"

int allowDPRINTF=0;

void DPRINTFvaA(const char *fmt,va_list valist)
{
 char buf[1024];
 int len;

 if (!allowDPRINTF)
  return;
 len=_vsnprintf(buf,1023,fmt,valist);
 if (len>0)
  {
   if (buf[len-1]!='\n')
    {
     buf[len]='\n';
     buf[len+1]='\0';
    }
   OutputDebugStringA(buf);
  }
}
void DPRINTFvaW(const wchar_t *fmt,va_list valist)
{
 wchar_t buf[1024];
 int len;

 if (!allowDPRINTF)
  return;
 len=_vsnwprintf(buf,1023,fmt,valist);
 if (len>0)
  {
   if (buf[len-1]!='\n')
    {
     buf[len]='\n';
     buf[len+1]='\0';
    }
   OutputDebugStringW(buf);
  }
}
void DPRINTFva(const char_t *fmt,va_list valist)
{
 if (!allowDPRINTF)
  return;
#ifdef UNICODE
 DPRINTFvaW(fmt,valist);
#else
 DPRINTFvaA(fmt,valist);
#endif
}
void DPRINTF(const char_t *fmt,...)
{
 va_list args;

 if (!allowDPRINTF)
  return;
 va_start(args, fmt);
 DPRINTFva(fmt,args);
 va_end(args);
}
void DPRINTFA(const char *fmt,...)
{
 va_list args;

 if (!allowDPRINTF)
  return;
 va_start(args, fmt);
 DPRINTFvaA(fmt,args);
 va_end(args);
}
void DPRINTFW(const wchar_t *fmt,...)
{
 va_list args;

 if (!allowDPRINTF)
  return;
 va_start(args, fmt);
 DPRINTFvaW(fmt,args);
 va_end(args);
}
