#include "stdafx.h"
#include "TwordWrap.h"
#include "ffdebug.h"
#include <mbstring.h>

template TwordWrap<char>::TwordWrap(int Imode, const char *Istr, int *Ipwidths, int IsplitdxMax, bool assCompatibleMode);
template TwordWrap<wchar_t>::TwordWrap(int Imode, const wchar_t *Istr, int *Ipwidths, int IsplitdxMax, bool assCompatibleMode);
template<class tchar> TwordWrap<tchar>::TwordWrap(int Imode, const tchar *Istr, int *Ipwidths, int IsplitdxMax, bool IassCompatibleMode):
  mode(Imode),
  str(Istr),
  pwidths(Ipwidths),
  splitdxMax(IsplitdxMax),
  assCompatibleMode(IassCompatibleMode)
{
 if (splitdxMax<1) splitdxMax=1;
 if (mode==0)
  {
   int linesPredicted=pwidths[str.size()-1]/splitdxMax+1;
   splitdxMin=pwidths[str.size()-1]/linesPredicted;
   splitdxMin=std::min(splitdxMin,splitdxMax);
   int lc=smart();
   if (linesPredicted<lc)
    {
     splitdxMin=pwidths[str.size()-1]/lc;
     splitdxMin=std::min(splitdxMin,splitdxMax);
     smart();
    }
  }
 else if (mode==1)
  {
   splitdxMin=splitdxMax;
   smart();
  }
 else if (mode==2)
  {
   splitdxMin=splitdxMax=INT_MAX;
   smart();
  }
 else if (mode==3)
  {
   int linesPredicted=pwidths[str.size()-1]/splitdxMax+1;
   splitdxMin=pwidths[str.size()-1]/linesPredicted;
   splitdxMin=std::min(splitdxMin,splitdxMax);
   int lc=smartReverse();
   if (linesPredicted<lc)
    {
     splitdxMin=pwidths[str.size()-1]/lc;
     splitdxMin=std::min(splitdxMin,splitdxMax);
     smartReverse();
    }
  }
}

template<class tchar> int TwordWrap<tchar>::smart()
{
 int left=0;
 int strlenp=(int)str.size();
 bool skippingSpace=false;
 int xx=0;
 rightOfTheLines.clear();
 for (int x=0;x<strlenp;x++)
  {
   if (skippingSpace || (pwidths[x]-left>splitdxMin && pwidths[x]-left<=splitdxMax)) // ideal for line break.
    {
     if (skippingSpace)
      {
       if (!tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
        {
         skippingSpace=false;
         left=x>0 ? pwidths[x-1] : 0;
         xx=x;
        }
      }
     else
      {
       if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
        {
         rightOfTheLines.push_back(x>0 ? x-1 : 0);
         skippingSpace=true;
        }
      }
    }
   if (pwidths[x]-left>splitdxMax && !skippingSpace) // Not ideal, but we have to break before current word.
    {
     bool found=false;
     skippingSpace=false;
     for (;x>xx;x--)
      {
       if (pwidths[x]-left<=splitdxMax)
        {
         if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
          {
           rightOfTheLines.push_back(x>0 ? x-1 : 0);
           left=pwidths[x];
           xx=nextChar(x);
           found=true;
           break;
          }
        }
      }
     // fail over : no space to cut. Japanese subtitles often come here.
     if (!found)
      {
       if (assCompatibleMode)
        {
         for (;x<strlenp;x++)
          {
           if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
            {
             rightOfTheLines.push_back(x>0 ? x-1 : 0);
             skippingSpace=true;
             break;
            }
          }
        }
       else
        {
         for (;x<strlenp;x++)
          {
           if (pwidths[x]-left>splitdxMin)
            {
             if (x>xx && pwidths[x]-left>splitdxMax) x=prevChar(x);
             left=pwidths[x];
             xx=nextChar(x);
             rightOfTheLines.push_back(rightOfDBCS(x));
             break;
            }
          }
        }
      }
    }
  }
 rightOfTheLines.push_back(strlenp);
 return (int)rightOfTheLines.size();
}

template<class tchar> int TwordWrap<tchar>::smartReverse()
{
 int strlenp=(int)str.size();
 int right=pwidths[strlenp-1];
 bool skippingSpace=false;
 int xx=strlenp-1;
 rightOfTheLines.clear();
 rightOfTheLines.push_front(xx);
 for (int x=strlenp-1;x>=0;x--)
  {
   if (skippingSpace || (right-pwidthsLeft(x)>splitdxMin && right-pwidthsLeft(x)<=splitdxMax)) // ideal for line break.
    {
     if (skippingSpace)
      {
       if (!tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
        {
         skippingSpace=false;
         right=pwidths[x];
         xx=x;
         rightOfTheLines.push_front(xx);
        }
      }
     else
      {
       if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
        skippingSpace=true;
      }
    }
   if (right-pwidthsLeft(x)>splitdxMax && !skippingSpace) // Not ideal, but we have to break after current word.
    {
     bool found=false;
     skippingSpace=false;
     for (;x<xx;x++)
      {
       if (right-pwidths[x]<=splitdxMax)
        {
         if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
          {
           xx=x>0 ? x-1 : 0;
           rightOfTheLines.push_front(xx);
           right=pwidths[xx];
           found=true;
           break;
          }
        }
      }
     // fail over : no space to cut. Japanese subtitles often come here.
     if (!found)
      {
       if (assCompatibleMode)
        {
         for (;x>=0;x--)
          {
           if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)str.at(x)))
            {
             skippingSpace=true;
             break;
            }
          }
        }
       else
        {
         for (;x>=0;x--)
          {
           if (right-pwidths[x]>splitdxMin)
            {
             x=leftOfDBCS(x);
             if (x<leftOfDBCS(xx) && right-pwidthsLeft(x)>splitdxMax)
              x=nextChar(x);
             xx=rightOfDBCS(prevChar(x));
             rightOfTheLines.push_front(xx);
             right=pwidths[xx];
             break;
            }
          }
        }
      }
    }
  }
 return (int)rightOfTheLines.size();
}

template<> int TwordWrap<char>::nextChar(int x)
{
 if (x+1>=(int)str.size())
  return x;
 return x+(int)_mbclen((const unsigned char *)str.c_str()+x);
}

template<> int TwordWrap<wchar_t>::nextChar(int x)
{
 if (x+1>=(int)str.size())
  return x;
 else
  return x+1;
}

template<> int TwordWrap<char>::prevChar(int x)
{
 if (x==0)
  return x;
 x--;
 if (x==0)
  return x;
 if (pwidths[x]==pwidths[x-1])
  x--;
 return x;
}

template<> int TwordWrap<wchar_t>::prevChar(int x)
{
 if (x==0)
  return x;
 else
  return x-1;
}

template<> int TwordWrap<char>::leftOfDBCS(int x)
{
 if (x==0)
  return x;
 if (pwidths[x-1]==pwidths[x])
  x--;
 return x;
}

template<> int TwordWrap<wchar_t>::leftOfDBCS(int x)
{
 return x;
}

template<> int TwordWrap<char>::rightOfDBCS(int x)
{
 if (x+1>=(int)str.size())
  return x;
 if (pwidths[x]==pwidths[x+1])
  x++;
 return x;
}

template<> int TwordWrap<wchar_t>::rightOfDBCS(int x)
{
 return x;
}

template<class tchar> int TwordWrap<tchar>::pwidthsLeft(int x)
{
 if (leftOfDBCS(x)==0) return 0;
 return pwidths[prevChar(x)];
}

template void TwordWrap<char>::debugprint();
template void TwordWrap<wchar_t>::debugprint();
template<class tchar> void TwordWrap<tchar>::debugprint()
{
 DPRINTF(_l("lineCount: %d"),rightOfTheLines.size());
 for(int l=1;l<=(int)rightOfTheLines.size();l++)
  {
   text<char_t> dbgstr0(str.c_str() + (l>=2 ? rightOfTheLines[l-2] : 0));
   const char_t *dbgstr=(const char_t*)dbgstr0;
   if (l>=2)
    {
     if (*dbgstr) dbgstr++;
     if (*dbgstr) dbgstr++;
    }
   DPRINTF(_l("%d:%d %s"),l,rightOfTheLines[l-1],dbgstr);
  }
}

template int TwordWrap<char>::getRightOfTheLine(int n);
template int TwordWrap<wchar_t>::getRightOfTheLine(int n);
template<class tchar> int TwordWrap<tchar>::getRightOfTheLine(int n)
{
 if (n<(int)rightOfTheLines.size())
  return rightOfTheLines[n];
 else
  return (int)str.size();
}
