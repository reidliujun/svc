/*
 * Subtitle reader with format autodetection
 *
 * Written by laaz
 * Some code cleanup & realloc() by A'rpi/ESP-team
 * dunnowhat sub format by szabi
 */

#include "stdafx.h"
#include "TsubreaderMplayer.h"
#include "TsubtitlesSettings.h"
#include "Tstream.h"
#include "Tconfig.h"
#include "ffdebug.h"

//========================================= TsubtitleParser =========================================
template<class tchar> TsubtitleParser<tchar>::TsubtitleParser(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):
 TsubtitleParserBase(Iformat,Ifps),
 ffcfg(Iffcfg),
 textfix(Icfg,Iffcfg),
 subreader(Isubreader),
 textformat(Iffcfg->getHtmlColors())
{
 cfg = *Icfg;
}

template<class tchar> int TsubtitleParser<tchar>::eol(tchar p) {
    return (p=='\r' || p=='\n' || p=='\0');
}

/* Remove leading and trailing space */
template<class tchar> void TsubtitleParser<tchar>::trail_space(tchar *s) {
        int i = 0;
        while (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)s[i])) ++i;
        if (i) strcpy(s, s + i);
        i = (int)strlen(s) - 1;
        while (i > 0 && tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)s[i])) s[i--] = '\0';
}

template<class tchar> Tsubtitle* TsubtitleParser<tchar>::store(TsubtitleTextBase<tchar> &sub)
{
 sub.defProps.extendedTags=cfg.extendedTags;
 sub.format(textformat);
 sub.prepareKaraoke();
 sub.fix(textfix);
 subreader->push_back(new TsubtitleTextBase<tchar>(sub));
 return subreader->back();
}

template<class tchar> Tsubtitle* TsubtitleParserSami<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
    tchar text[this->LINE_LEN+1], *p=NULL,*q;
    int state;


    /* read the first line */
    if (!s)
     if ((s = fd.fgets(line, this->LINE_LEN))==NULL) return NULL;

    TsubtitleTextBase<tchar> current(this->format);
    current.start = current.stop = 0;
    state = 0;

    do {
        switch (state) {

        case 0: /* find "START=" or "Slacktime:" */
            slacktime_s = stristr (s, _L("Slacktime:"));
	    if (slacktime_s)
                sub_slacktime = strtol (slacktime_s+10, NULL, 0) / 10;

            s = (tchar*)stristr (s, _L("Start="));
            if (s) {
                int sec1000=strtol (s + 6, &s, 0);
                current.start = this->hmsToTime(0,0,sec1000/1000,(sec1000%1000)/10);
                /* eat '>' */
                for (; *s != '>' && *s != '\0'; s++);
                s++;
                state = 1; continue;
            }
            break;

	case 1: /* find (optionnal) "<P", skip other TAGs */
	    for  (; *s == ' ' || *s == '\t'; s++); /* strip blanks, if any */
	    if (*s == '\0') break;
	    if (*s != '<') { state = 3; p = text; continue; } /* not a TAG */
	    s++;
	    if (*s == 'P' || *s == 'p') { s++; state = 2; continue; } /* found '<P' */
	    for (; *s != '>' && *s != '\0'; s++); /* skip remains of non-<P> TAG */
	    if (s == '\0')
	      break;
	    s++;
            continue;

        case 2: /* find ">" */
            if ((s = strchr (s, '>'))!=NULL) { s++; state = 3; p = text; continue; }
            break;

        case 3: /* get all text until '<' appears */
            if (*s == '\0') break;
            else if (!_strnicmp (s, _L("<br>"), 4)) {
                *p = '\0'; p = text; trail_space (text);
                if (text[0] != '\0')
                    current.add(text);
                s += 4;
            }
	    else if ((*s == '{') && !sub_no_text_pp) { state = 5; ++s; continue; }
            else if (*s == '<') { state = 4; }
            else if (!_strnicmp (s, _L("&nbsp;"), 6)) { *p++ = ' '; s += 6; }
            else if (*s == '\t') { *p++ = ' '; s++; }
            else if (*s == '\r' || *s == '\n') { s++; }
            else *p++ = *s++;

            /* skip duplicated space */
            if (p > text + 2) if (*(p-1) == ' ' && *(p-2) == ' ') p--;

            continue;

	case 4: /* get current->end or skip <TAG> */
	    q = (tchar*)stristr (s, _L("Start="));
            if (q) {
                int sec1000=strtol (q + 6, &q, 0);
                current.stop = this->hmsToTime(0,0, sec1000/1000,(sec1000%1000)/10-1);
                *p = '\0'; trail_space (text);
                if (text[0] != '\0')
                    current.add(text);
                if (current.size() > 0) { state = 99; break; }
                state = 0; continue;
            }
            s = strchr (s, '>');
            if (s) { s++; state = 3; continue; }
            break;
       case 5: /* get rid of {...} text, but read the alignment code */
	    if ((*s == '\\') && (*(s + 1) == 'a') && !sub_no_text_pp) {
               if (stristr(s, _L("\\a1")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_BOTTOMLEFT;
                   s = s + 3;
               }
               if (stristr(s, _L("\\a2")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_BOTTOMCENTER;
                   s = s + 3;
               } else if (stristr(s, _L("\\a3")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_BOTTOMRIGHT;
                   s = s + 3;
               } else if ((stristr(s, _L("\\a4")) != NULL) || (stristr(s, _L("\\a5")) != NULL) || (stristr(s, _L("\\a8")) != NULL)) {
                   //current->alignment = SUB_ALIGNMENT_TOPLEFT;
                   s = s + 3;
               } else if (stristr(s, _L("\\a6")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_TOPCENTER;
                   s = s + 3;
               } else if (stristr(s, _L("\\a7")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_TOPRIGHT;
                   s = s + 3;
               } else if (stristr(s, _L("\\a9")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_MIDDLELEFT;
                   s = s + 3;
               } else if (stristr(s, _L("\\a10")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_MIDDLECENTER;
                   s = s + 4;
               } else if (stristr(s, _L("\\a11")) != NULL) {
                   //current->alignment = SUB_ALIGNMENT_MIDDLERIGHT;
                   s = s + 4;
               }
	    }
	    if (*s == '}') state = 3;
	    ++s;
	    continue;
        }

        /* read next line */
        if (state != 99 && (s = fd.fgets (line, this->LINE_LEN))==NULL) {
            if (current.start > 0) {
                break; // if it is the last subtitle
            } else {
                return NULL;
            }
        }

    } while (state != 99);

    // For the last subtitle
    if (current.stop <= 0) {
        current.stop = current.start + this->hmsToTime(0,0,sub_slacktime/1000,(sub_slacktime%1000)/10);
        *p = '\0'; trail_space (text);
        if (text[0] != '\0')
            current.add(text);
    }
    return store(current);
}


template<class tchar> const tchar* TsubtitleParser<tchar>::sub_readtext(const tchar *source, TsubtitleTextBase<tchar> &sub) {
    int len=0;
    const tchar *p=source;

//    printf("src=%p  dest=%p  \n",source,dest);

    while ( !eol(*p) && *p!= '|' ) {
        p++,len++;
    }

    sub.add(source,len);
    while (*p=='\r' || *p=='\n' || *p=='|') p++;

    if (*p) return p;  // not-last text field
    else return NULL;  // last text field
}

template<class tchar> Tsubtitle* TsubtitleParserMicrodvd<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME, REFERENCE_TIME) {
    tchar line[this->LINE_LEN+1];
    tchar line2[this->LINE_LEN+1];
    const tchar *p, *next;

    int start=0,stop=0;
    bool skip;
    do {
      do {
          if (!fd.fgets (line, this->LINE_LEN)) return NULL;
      } while ((tchar_traits<tchar>::sscanf() (line,
                        _L("{%ld}{}%[^\r\n]"),
                        &start, line2) < 2) &&
               (tchar_traits<tchar>::sscanf() (line,
                        _L("{%ld}{%ld}%[^\r\n]"),
                        &start, &stop, line2) < 3));
      skip=false;
      if (start==1 && stop==1)
       {
        tchar *e;double newfps;
        if ((newfps=strtod(line2,&e))>0 && !*e)
         {
          this->fps=newfps;
          skip=true;
         }
       }
    } while (skip);

    TsubtitleTextBase<tchar> current(this->format);
    current.start=this->frameToTime(start);
    current.stop =this->frameToTime(stop );

    p=line2;

    next=p;
    while ((next =sub_readtext (next, current))!=NULL)
     ;
    return store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserSubrip<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
    tchar line[this->LINE_LEN+1];
    int a1,a2,a3,a4,b1,b2,b3,b4;
    tchar *p=NULL, *q=NULL;
    int len;
    TsubtitleTextBase<tchar> current(this->format);
    while (1) {
        if (!fd.fgets (line, this->LINE_LEN)) return NULL;
        if (flags&this->PARSETIME)
         {
          if (tchar_traits<tchar>::sscanf() (line, _L("%d:%d:%d.%d,%d:%d:%d.%d"),&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4) < 8) continue;
          current.start = this->hmsToTime(a1,a2,a3,a4);
          current.stop  = this->hmsToTime(b1,b2,b3,b4);
          if (!fd.fgets (line, this->LINE_LEN)) return NULL;
         }

        p=q=line;
        for (;;) {
            for (q=p,len=0; *p && *p!='\r' && *p!='\n' && *p!='|' && strncmp(p,_L("[br]"),4); p++,len++);
            current.add(q,len);
            if (!*p || *p=='\r' || *p=='\n') break;
            if (*p=='|') p++;
            else while (*p++!=']');
        }
        break;
    }

    return store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserSubviewer<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
    tchar line[this->LINE_LEN+1];
    int a1,a2,a3,a4,b1,b2,b3,b4;
    tchar *p=NULL;
    int len;
    TsubtitleTextBase<tchar> current(this->format);
    TsubtitleParser<tchar>::textformat.resetProps();
    while (!current.size()) {
        if (flags&this->PARSETIME)
         {
          if (!fd.fgets (line, this->LINE_LEN)) return NULL;
          int li;
	  if ((len=tchar_traits<tchar>::sscanf() (line, _L("%d:%d:%d%[,.:]%d --> %d:%d:%d%[,.:]%d"),&a1,&a2,&a3,&li,&a4,&b1,&b2,&b3,&li,&b4)) < 10)
           continue;
          current.start = this->hmsToTime(a1,a2,a3,a4/10);
          current.stop  = this->hmsToTime(b1,b2,b3,b4/10);
         }
        for (;;) {
            if (!fd.fgets (line, this->LINE_LEN)) goto end;//break;
            len=0;
            for (p=line; *p!='\n' && *p!='\r' && *p; p++,len++);
            if (len) {
                int j=0,skip=0;
		tchar *curptr0,*curptr=curptr0=(tchar*)_alloca((len+1)*sizeof(tchar));
                for(; j<len; j++) {
		    /* let's filter html tags ::atmos */
                    /*
		    if(line[j]=='>') {
			skip=0;
			continue;
		    }
		    if(line[j]=='<') {
			skip=1;
			continue;
		    }*/
		    if(skip) {
			continue;
		    }
		    *curptr=line[j];
		    curptr++;
		}
		*curptr='\0';
                current.add(curptr0);
            } else {
                break;
            }
        }
    }
end:
    return current.empty()?NULL:store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserSubviewer2<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
    tchar line[this->LINE_LEN+1];
    int a1,a2,a3,a4;
    tchar *p=NULL;
    int len;
    TsubtitleTextBase<tchar> current(this->format);
    while (!current.size()) {
        if (!fd.fgets (line, this->LINE_LEN)) return NULL;
        if (line[0]!='{')
            continue;
        if ((len=tchar_traits<tchar>::sscanf() (line, _L("{T %d:%d:%d:%d"),&a1,&a2,&a3,&a4)) < 4)
            continue;
        current.start = this->hmsToTime(a1,a2,a3,a4);
        for (/*i=0*/;;) {
            if (!fd.fgets (line, this->LINE_LEN)) goto end;//break;
            if (line[0]=='}') break;
            len=0;
            for (p=line; *p!='\n' && *p!='\r' && *p; ++p,++len);
            if (len) {
                current.add(line,len);
            } else {
                break;
            }
        }
        //current->lines=i;
    }
  end:
    return current.empty()?NULL:store(current);
}


template<class tchar> Tsubtitle* TsubtitleParserVplayer<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
        tchar line[this->LINE_LEN+1];
        int a1,a2,a3,a4;
        const tchar *p=NULL, *next;tchar separator1,separator2;
        int plen;
        TsubtitleTextBase<tchar> current(this->format);
        while (current.empty()) {
                if (!fd.fgets (line, this->LINE_LEN)) return NULL;
                int ret=tchar_traits<tchar>::sscanf() (line, T_L<tchar>("%d:%d:%d%c%d%c%n",L"%d:%d:%d%c%d%lc%n"),&a1,&a2,&a3,&separator1,&a4,&separator2,&plen);
                if (ret!=6)
                 {
                  a4=0;
                  ret=tchar_traits<tchar>::sscanf() (line, T_L<tchar>("%d:%d:%d%c%n",L"%d:%d:%d%lc%n"),&a1,&a2,&a3,&separator1,&plen);
                  if (ret!=4)
                   continue;
                 }

                if ((current.start = this->hmsToTime(a1,a2,a3,a4*10))==NULL)
                        continue;
                /* removed by wodzu
                p=line;
                // finds the body of the subtitle
                for (i=0; i<3; i++){
                   p=strchr(p,':');
                   if (p==NULL) break;
                   ++p;
                }
                if (p==NULL) {
                    printf("SUB: Skipping incorrect subtitle line!\n");
                    continue;
                }
                */
                // by wodzu: hey! this time we know what length it has! what is
                // that magic for? it can't deal with space instead of third
                // colon! look, what simple it can be:
                p = &line[ plen ];

                if (*p!='|') {
                        //
                        next = p;
                        while ((next =sub_readtext (next, current))!=NULL)
                         ;
                }
        }

        return store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserRt<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
        //TODO: This format uses quite rich (sub/super)set of xhtml
        // I couldn't check it since DTD is not included.
        // WARNING: full XML parses can be required for proper parsing
    tchar line[this->LINE_LEN+1];
    int a1,a2,a3,a4,b1,b2,b3,b4;
    const tchar *p=NULL,*next=NULL;
    int plen;
    TsubtitleTextBase<tchar> current(this->format);
    while (current.empty()) {
        if (!fd.fgets (line, this->LINE_LEN)) return NULL;
        //TODO: it seems that format of time is not easily determined, it may be 1:12, 1:12.0 or 0:1:12.0
        //to describe the same moment in time. Maybe there are even more formats in use.
        //if ((len=tchar_traits<tchar>::sscanf() (line, "<Time Begin=\"%d:%d:%d.%d\" End=\"%d:%d:%d.%d\"",&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4)) < 8)
        plen=a1=a2=a3=a4=b1=b2=b3=b4=0;
        if (
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d.%d\" %*[Ee]nd=\"%d.%d\"%*[^<]<clear/>%n"),&a3,&a4,&b3,&b4,&plen) < 4) &&
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d.%d\" %*[Ee]nd=\"%d:%d.%d\"%*[^<]<clear/>%n"),&a3,&a4,&b2,&b3,&b4,&plen) < 5) &&
        (tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d\" %*[Ee]nd=\"%d:%d\"%*[^<]<clear/>%n"),&a2,&a3,&b2,&b3,&plen) < 4) &&
        (tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d\" %*[Ee]nd=\"%d:%d.%d\"%*[^<]<clear/>%n"),&a2,&a3,&b2,&b3,&b4,&plen) < 5) &&
//      (tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d.%d\" %*[Ee]nd=\"%d:%d\"%*[^<]<clear/>%n"),&a2,&a3,&a4,&b2,&b3,&plen) < 5) &&
        (tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d.%d\" %*[Ee]nd=\"%d:%d.%d\"%*[^<]<clear/>%n"),&a2,&a3,&a4,&b2,&b3,&b4,&plen) < 6) &&
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d:%d.%d\" %*[Ee]nd=\"%d:%d:%d.%d\"%*[^<]<clear/>%n"),&a1,&a2,&a3,&a4,&b1,&b2,&b3,&b4,&plen) < 8) &&
	//now try it without end time
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d.%d\"%*[^<]<clear/>%n"),&a3,&a4,&plen) < 2) &&
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d\"%*[^<]<clear/>%n"),&a2,&a3,&plen) < 2) &&
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d.%d\"%*[^<]<clear/>%n"),&a2,&a3,&a4,&plen) < 3) &&
	(tchar_traits<tchar>::sscanf() (line, _L("<%*[tT]ime %*[bB]egin=\"%d:%d:%d.%d\"%*[^<]<clear/>%n"),&a1,&a2,&a3,&a4,&plen) < 4)
        )
            continue;
        current.start = this->hmsToTime(a1,a2,a3,a4);
        current.stop  = this->hmsToTime(a1,a2,a3,a4);
	if (b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0)
	  current.stop = current.start+this->frameToTime(200);
        p=line; p+=plen;
        // TODO: I don't know what kind of convention is here for marking multiline subs, maybe <br/> like in xml?
        next = strstr(line,_L("<clear/>"));
        if(next && strlen(next)>8){
          next+=8;
          while ((next =sub_readtext (next, current))!=NULL)
           ;
        }
    }

    return store(current);
}

template<class tchar>  TsubtitleParserSSA<tchar>::TsubtitleParserSSA(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader,bool isEmbedded0):
 TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader),
 inV4styles(0),inEvents(0),inInfo(0),
 playResX(0),playResY(0),wrapStyle(0),
 timer(1,1),
 isEmbedded(isEmbedded0)
{
 defprops.alignment = 2;
 defprops.outlineWidth = defprops.shadowDepth = 2;
 defprops.size = 26;
 strcpy(defprops.fontname, _l("Arial"));
 defprops.encoding = 0;
 defprops.isColor = true;
 defprops.marginR = defprops.marginL = defprops.marginV = 10;
 defprops.version = TsubtitleParserSSA::SSA;
}

template<class tchar> void TsubtitleParserSSA<tchar>::strToInt(const ffstring &str,int *i)
{
 if (!str.empty())
  {
   tchar *end;
   int val=strtol(str.c_str(),&end,10);
   if (*end=='\0' && val>=0) *i=val;
  }
}

template<class tchar> void TsubtitleParserSSA<tchar>::strToIntMargin(const ffstring &str,int *i)
{
 if (!str.empty() /*str.size()==4 && str.compare(_L("0000"))!=0*/)
  {
   tchar *end;
   int val=strtol(str.c_str(),&end,10);
   if (*end=='\0' && val>0) *i=val;
  }
}

template<class tchar> void TsubtitleParserSSA<tchar>::strToDouble(const ffstring &str,double *d)
{
 if (!str.empty())
  {
   tchar *end;
   double val=strtod(str.c_str(),&end);
   if (*end=='\0') *d=val;
  }
}

template<class tchar> bool TsubtitleParserSSA<tchar>::Tstyle::toCOLORREF(const ffstring& colourStr,COLORREF &colour,int &alpha)
{
 if (colourStr.empty()) return false;
 int radix;
 ffstring s1,s2;
 s1=colourStr;
 s1.ConvertToUpperCase();
 if (s1.compare(0,2,_L("&H"),2)==0)
  {
   s1.erase(0,2);
   radix=16;
  }
 else
  radix=10;
 s2=s1;
 if (s1.size()>6)
  {
   s1.erase(s1.size()-6,6);
   s2.erase(0,s2.size()-6);
  }
 else
  s1.clear();

 int msb=0;
 if (!s1.empty())
  {
   const tchar *alphaS=s1.c_str();
   tchar *endalpha;
   long a=strtol(alphaS,&endalpha,radix);
   if (*endalpha=='\0')
    msb=a;
  }
 if (s2.empty()) return false;
 const tchar *colorS=s2.c_str();
 tchar *endcolor;
 COLORREF c=strtol(colorS,&endcolor,radix);
 if (*endcolor=='\0')
  {
   DWORD result=msb * (radix==16 ? 0x1000000 : 1000000) + c;
   colour=result & 0xffffff;
   alpha=256-(result>>24);
   return true;
  }
 return false;
}

template<class tchar> void TsubtitleParserSSA<tchar>::Tstyle::toProps(void)
{
 if (fontname)
  text<char_t>(fontname.c_str(), -1, props.fontname, countof(props.fontname));
 if (int size=atoi(fontsize.c_str()))
  props.size=size;
 bool isColor=toCOLORREF(primaryColour,props.color,props.colorA);
 isColor|=toCOLORREF(secondaryColour,props.SecondaryColour,props.SecondaryColourA);
 isColor|=toCOLORREF(tertiaryColour,props.TertiaryColour,props.TertiaryColourA);
 isColor|=toCOLORREF(outlineColour,props.OutlineColour,props.OutlineColourA);
 if (version==TsubtitleParserSSA::SSA)
  {
   isColor|=toCOLORREF(backgroundColour,props.OutlineColour,props.OutlineColourA);
   props.ShadowColour=props.OutlineColour;
   props.ShadowColourA=128;
  }
 else
  isColor|=toCOLORREF(backgroundColour,props.ShadowColour,props.ShadowColourA);
 props.isColor=isColor;
 if (bold==_L("-1"))
  props.bold=1;
 else
  props.bold=0;
 if (italic==_L("-1")) props.italic=true;
 if (underline==_L("-1")) props.underline=true;
 if (strikeout==_L("-1")) props.strikeout=true;
 strToInt(encoding,&props.encoding);
 strToDouble(spacing,&props.spacing);
 strToInt(fontScaleX,&props.scaleX);
 strToInt(fontScaleY,&props.scaleY);
 strToInt(alignment,&props.alignment);
 strToInt(marginLeft,&props.marginL);
 strToInt(marginRight,&props.marginR);
 strToInt(marginV,&props.marginV);
 strToInt(marginTop,&props.marginTop);
 strToInt(marginBottom,&props.marginBottom);
 strToInt(borderStyle,&props.borderStyle);
 strToDouble(outlineWidth,&props.outlineWidth);
 strToDouble(shadowDepth,&props.shadowDepth);
 if (alignment && this->version != SSA)
  props.alignment=TSubtitleProps::alignASS2SSA(props.alignment);
}
template<class tchar> void TsubtitleParserSSA<tchar>::Tstyles::add(Tstyle &s)
{
 s.toProps();
 insert(std::make_pair(s.name,s));
}
template<class tchar> const TSubtitleProps* TsubtitleParserSSA<tchar>::Tstyles::getProps(const ffstring &style)
{
 typename std::map<ffstring,Tstyle,ffstring_iless>::const_iterator si=this->find(style);
 if (si!=this->end())
  return &si->second.props;

 typename std::map<ffstring,Tstyle,ffstring_iless>::const_iterator iDefault=this->find(ffstring(_L("Default")));
 if (iDefault!=this->end())
  return &iDefault->second.props;

 iDefault=this->find(ffstring(_L("*Default")));

 if (iDefault!=this->end())
  return &iDefault->second.props;
 else
  return NULL;
}

template<class tchar> Tsubtitle* TsubtitleParserSSA<tchar>::parse(Tstream &fd, int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
/*
 * Sub Station Alpha v4 (and v2?) scripts have 9 commas before subtitle
 * other Sub Station Alpha scripts have only 8 commas before subtitle
 * Reading the "ScriptType:" field is not reliable since many scripts appear
 * w/o it
 *
 * http://www.scriptclub.org is a good place to find more examples
 * http://www.eswat.demon.co.uk is where the SSA specs can be found
 */
 tchar line0[this->LINE_LEN+1];
 tchar *line=line0;
 while (fd.fgets(line,this->LINE_LEN))
  {
#if 0
   text<char_t> lineD0(line);
   const char_t* lineD1=(const char_t*)lineD0;
   DPRINTF(_l("%s"),lineD1);
#endif
   if (line[0]==';')
    continue;
   tchar *cr=strrchr(line,'\n');if (cr) *cr='\0';
   cr=strrchr(line,'\r');if (cr) *cr='\0';
   if (strnicmp(line,_L("[Script Info]"),13)==0)
    {
     inV4styles=0;
     inEvents=0;
     inInfo=1;
    }
   else if (inInfo && strnicmp(line,_L("PlayResX:"),8)==0)
    strToInt(line+9,&playResX);
   else if (inInfo && strnicmp(line,_L("PlayResY:"),8)==0)
    strToInt(line+9,&playResY);
   else if (inInfo && strnicmp(line,_L("Timer:"),6)==0)
    {
     tchar *end;
     double t=strtod(line+7,&end);
     if (*end=='\0' && t!=0)
      timer=Rational(t/100.0,INT32_MAX);
    }
   else if (inInfo && strnicmp(line,_L("WrapStyle:"),9)==0)
    {
     strToInt(line+10,&wrapStyle);
    }
   else if (strnicmp(line,_L("[V4 Styles]"),11)==0)
    {
     version=SSA;
     inV4styles=2;
     inEvents=0;
     inInfo=0;
    }
   else if (strnicmp(line,_L("[V4+ Styles]"),11)==0)
    {
     version=ASS;
     inV4styles=2;
     inEvents=0;
     inInfo=0;
    }
   else if (strnicmp(line,_L("[V4++ Styles]"),11)==0)
    {
     version=ASS2;
     inV4styles=2;
     inEvents=0;
     inInfo=0;
    }
   else if (strnicmp(line,_L("[Events]"),8)==0)
    {
     inV4styles=0;
     inEvents=2;
     inInfo=0;
    }
   else if (inV4styles==2 && strnicmp(line,_L("Format:"),7)==0)
    {
     strlwr(line);strrmchar(line,_L(' '));
     typedef std::vector<Tstrpart<tchar> > Tparts;
     Tparts fields;
     const tchar *l=line+7;
     strtok(l,_L(","),fields);
     styleFormat.clear();
     for (typename Tparts::const_iterator f=fields.begin();f!=fields.end();f++)
      {
       if (strnicmp(f->first,_L("name"),4)==0)
        styleFormat.push_back(&Tstyle::name);
       else if (strnicmp(f->first,_L("fontname"),8)==0)
        styleFormat.push_back(&Tstyle::fontname);
       else if (strnicmp(f->first,_L("fontsize"),8)==0)
        styleFormat.push_back(&Tstyle::fontsize);
       else if (strnicmp(f->first,_L("primaryColour"),13)==0)
        styleFormat.push_back(&Tstyle::primaryColour);
       else if (strnicmp(f->first,_L("SecondaryColour"),15)==0)
        styleFormat.push_back(&Tstyle::secondaryColour);
       else if (strnicmp(f->first,_L("TertiaryColour"),14)==0)
        styleFormat.push_back(&Tstyle::tertiaryColour);
       else if (strnicmp(f->first,_L("OutlineColour"),13)==0)
        styleFormat.push_back(&Tstyle::outlineColour);
       else if (strnicmp(f->first,_L("BackColour"),10)==0)
        styleFormat.push_back(&Tstyle::backgroundColour);
       else if (strnicmp(f->first,_L("bold"),4)==0)
        styleFormat.push_back(&Tstyle::bold);
       else if (strnicmp(f->first,_L("italic"),6)==0)
        styleFormat.push_back(&Tstyle::italic);
       else if (strnicmp(f->first,_L("Underline"),9)==0)
        styleFormat.push_back(&Tstyle::underline);
       else if (strnicmp(f->first,_L("Strikeout"),9)==0)
        styleFormat.push_back(&Tstyle::strikeout);
       else if (strnicmp(f->first,_L("ScaleX"),6)==0)
        styleFormat.push_back(&Tstyle::fontScaleX);
       else if (strnicmp(f->first,_L("ScaleY"),6)==0)
        styleFormat.push_back(&Tstyle::fontScaleY);
       else if (strnicmp(f->first,_L("Spacing"),7)==0)
        styleFormat.push_back(&Tstyle::spacing);
       else if (strnicmp(f->first,_L("outline"),7)==0)
        styleFormat.push_back(&Tstyle::outlineWidth);
       else if (strnicmp(f->first,_L("shadow"),6)==0)
        styleFormat.push_back(&Tstyle::shadowDepth);
       else if (strnicmp(f->first,_L("alignment"),9)==0)
        styleFormat.push_back(&Tstyle::alignment);
       else if (strnicmp(f->first,_L("encoding"),8)==0)
        styleFormat.push_back(&Tstyle::encoding);
       else if (strnicmp(f->first,_L("marginl"),7)==0)
        styleFormat.push_back(&Tstyle::marginLeft);
       else if (strnicmp(f->first,_L("marginr"),7)==0)
        styleFormat.push_back(&Tstyle::marginRight);
       else if (strnicmp(f->first,_L("marginv"),7)==0)
        styleFormat.push_back(&Tstyle::marginV);
       else if (strnicmp(f->first,_L("borderstyle"),11)==0)
        styleFormat.push_back(&Tstyle::borderStyle);
       else
        styleFormat.push_back(NULL);
      }
     inV4styles=1;
    }
   else if (inV4styles && strnicmp(line,_L("Style:"),6)==0)
    {
     if (inV4styles==2)
      {
       styleFormat.clear();
       if (version==ASS2)
        {
         styleFormat.push_back(&Tstyle::name);
         styleFormat.push_back(&Tstyle::fontname);
         styleFormat.push_back(&Tstyle::fontsize);
         styleFormat.push_back(&Tstyle::primaryColour);
         styleFormat.push_back(&Tstyle::secondaryColour);
         styleFormat.push_back(&Tstyle::tertiaryColour);
         styleFormat.push_back(&Tstyle::backgroundColour);
         styleFormat.push_back(&Tstyle::bold);
         styleFormat.push_back(&Tstyle::italic);
         if (version>=ASS) styleFormat.push_back(&Tstyle::underline);
         if (version>=ASS) styleFormat.push_back(&Tstyle::strikeout);
         if (version>=ASS) styleFormat.push_back(&Tstyle::fontScaleX);
         if (version>=ASS) styleFormat.push_back(&Tstyle::fontScaleY);
         if (version>=ASS) styleFormat.push_back(&Tstyle::spacing);
         if (version>=ASS) styleFormat.push_back(&Tstyle::angleZ);
         styleFormat.push_back(&Tstyle::borderStyle);
         styleFormat.push_back(&Tstyle::outlineWidth);
         styleFormat.push_back(&Tstyle::shadowDepth);
         styleFormat.push_back(&Tstyle::alignment);
         styleFormat.push_back(&Tstyle::marginLeft);
         styleFormat.push_back(&Tstyle::marginRight);
         styleFormat.push_back(&Tstyle::marginTop);
         if (version>=ASS2) styleFormat.push_back(&Tstyle::marginBottom);
         styleFormat.push_back(&Tstyle::encoding);
         if (version<=SSA) styleFormat.push_back(&Tstyle::alpha);
         styleFormat.push_back(&Tstyle::relativeTo);
        }
       inV4styles=1;
      }
     strings fields;
     strtok(line+7,_L(","),fields);
     Tstyle style(playResX,playResY,version,wrapStyle);
     for (size_t i=0;i<fields.size() && i<styleFormat.size();i++)
       if (styleFormat[i])
        style.*(styleFormat[i])=fields[i];
     styles.add(style);
    }
   else if (inEvents==2 && strnicmp(line,_L("Format:"),7)==0)
    {
     strlwr(line);strrmchar(line,_L(' '));
     typedef std::vector<Tstrpart<tchar> > Tparts;
     Tparts fields;
     const tchar *l=line+7;
     strtok(l,_L(","),fields);
     eventFormat.clear();
     for (typename Tparts::const_iterator f=fields.begin();f!=fields.end();f++)
      {
       if (strnicmp(f->first,_L("marked"),6)==0)
        eventFormat.push_back(&Tevent::marked);
       else if (strnicmp(f->first,_L("start"),5)==0)
        {
         if (!isEmbedded) eventFormat.push_back(&Tevent::start);
        }
       else if (strnicmp(f->first,_L("end"),3)==0)
        eventFormat.push_back(&Tevent::end);
       else if (strnicmp(f->first,_L("style"),5)==0)
        eventFormat.push_back(&Tevent::style);
       else if (strnicmp(f->first,_L("name"),4)==0)
        eventFormat.push_back(&Tevent::name);
       else if (strnicmp(f->first,_L("marginL"),7)==0)
        eventFormat.push_back(&Tevent::marginL);
       else if (strnicmp(f->first,_L("marginR"),7)==0)
        eventFormat.push_back(&Tevent::marginR);
       else if (strnicmp(f->first,_L("marginV"),7)==0)
        eventFormat.push_back(&Tevent::marginV);
       else if (strnicmp(f->first,_L("marginR"),7)==0)
        eventFormat.push_back(&Tevent::marginR);
       else if (strnicmp(f->first,_L("effect"),6)==0)
        eventFormat.push_back(&Tevent::effect);
       else if (strnicmp(f->first,_L("text"),4)==0)
        eventFormat.push_back(&Tevent::text);
       else
        eventFormat.push_back(NULL);
      }
     inEvents=1;
    }
   else if ((flags&this->SSA_NODIALOGUE) || (inEvents==1 && strnicmp(line,_L("Dialogue:"),8)==0))
    {
     if (eventFormat.empty())
      {
       if (!(flags&this->SSA_NODIALOGUE))
        {
         if (version<=SSA) eventFormat.push_back(&Tevent::marked);
         if (version>=ASS) eventFormat.push_back(&Tevent::layer);
         eventFormat.push_back(&Tevent::start);
         eventFormat.push_back(&Tevent::end);
        }
       else
        {
         eventFormat.push_back(&Tevent::readorder);
         eventFormat.push_back(&Tevent::layer);
        }
       eventFormat.push_back(&Tevent::style);
       eventFormat.push_back(&Tevent::actor);
       eventFormat.push_back(&Tevent::marginL);
       eventFormat.push_back(&Tevent::marginR);
       eventFormat.push_back(&Tevent::marginT);
       if (version>=ASS2) eventFormat.push_back(&Tevent::marginB);
       eventFormat.push_back(&Tevent::effect);
       eventFormat.push_back(&Tevent::text);
      }
     strings fields;
     strtok(line+(flags&this->SSA_NODIALOGUE?0:9),_L(",") ,fields,true,eventFormat.size());
     Tevent event;
     event.dummy=""; // avoid being optimized.
     for (size_t i=0;i<fields.size() && i<eventFormat.size();i++/*,it++*/)
      {
       const tchar *str=fields[i].data();
       if (eventFormat[i])
        event.*(eventFormat[i]/* *it*/)=fields[i];
      }
     if (event.text)
      {
#if 0
       text<char_t> lineD2(event.text.c_str());
       const char_t* lineD3=(const char_t*)lineD2;
       DPRINTF(_l("%s"),lineD3);
#endif
       int hour1=0,min1=0,sec1=0,hunsec1=0;
       int hour2=0,min2=0,sec2=0,hunsec2=0;
       if (!(flags&this->PARSETIME) ||
           (tchar_traits<tchar>::sscanf()(event.start.c_str(),_L("%d:%d:%d.%d"),&hour1, &min1, &sec1, &hunsec1)==4 &&
            tchar_traits<tchar>::sscanf()(event.end.c_str()  ,_L("%d:%d:%d.%d"),&hour2, &min2, &sec2, &hunsec2)==4))
        {
         const TSubtitleProps *props=styles.getProps(event.style);
         TsubtitleTextBase<tchar> current(this->format,props?*props:defprops);
         strToIntMargin(event.marginL,&current.defProps.marginL);
         strToIntMargin(event.marginR,&current.defProps.marginR);
         strToIntMargin(event.marginV,&current.defProps.marginV);
         if (flags&this->PARSETIME)
          {
           current.start=timer.den*this->hmsToTime(hour1,min1,sec1,hunsec1)/timer.num;
           current.stop =timer.den*this->hmsToTime(hour2,min2,sec2,hunsec2)/timer.num;
          }
         else if (start != REFTIME_INVALID && stop != REFTIME_INVALID)
          {
           current.start = start;
           current.stop=stop;
          }
         current.defProps.tStart = current.defProps.karaokeStart = current.start;
         current.defProps.tStop = current.stop;

         // FIXME
         // \h removal : \h is hard space, so it should be replaced HARD sapce, soft space for band-aid.
         for (size_t i=0 ; i<event.text.size() ; i++)
         {
          if (event.text[i]=='\\' && event.text[i+1]=='h')
           {
            event.text[i]=0x20; // ' '
            event.text.erase(i+1,1);
           }
         }

         const tchar *line2=event.text.c_str();
         do
         {
          const tchar *tmp,*tmp1;
          int lineBreakReason = 0;
          do
           {
            tmp=strstr(line2, _L("\\n"));
            tmp1=strstr(line2, _L("\\N"));
            if (tmp == NULL && tmp1 == NULL)
             break;
            if (tmp && tmp1)
             tmp = std::min(tmp,tmp1);
            if (tmp == NULL)
             tmp = tmp1;
            current.addSSA(line2, tmp-line2, lineBreakReason);
            lineBreakReason = tmp[1] == 'n' ? 1 : 2;
            line2=tmp+2;
           } while (1);
          current.addSSA(line2, lineBreakReason);
         } while (flags&this->SSA_NODIALOGUE && fd.fgets((tchar*)(line2=line),this->LINE_LEN));
         return store(current);
        }
      }
    }
  }
 return NULL;
}

template<class tchar> Tsubtitle* TsubtitleParserDunnowhat<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME, REFERENCE_TIME) {
    tchar line[this->LINE_LEN+1];
    tchar text[this->LINE_LEN+1];

    if (!fd.fgets (line, this->LINE_LEN))
        return NULL;
    long start,stop;
    if (tchar_traits<tchar>::sscanf() (line, _L("%ld,%ld,\"%[^\"]"), &start,
                &stop, text) <3)
        return NULL;
    TsubtitleTextBase<tchar> current(this->format);
    current.start=this->frameToTime(start);
    current.stop =this->frameToTime(stop );
    current.add(text);

    return store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserMPsub<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
        tchar line[this->LINE_LEN+1];
        float a,b;
        int num=0;
        tchar *p, *q;

        do
        {
                if (!fd.fgets(line, this->LINE_LEN)) return NULL;
        } while (tchar_traits<tchar>::sscanf() (line, _L("%f %f"), &a, &b) !=2);
        TsubtitleTextBase<tchar> current(this->format);
        mpsub_position += a*(sub_uses_time ? 100.0 : 1.0);
        current.start=this->frameToTime(int(sub_uses_time?mpsub_position/100.0:mpsub_position));
        mpsub_position += b*(sub_uses_time ? 100.0 : 1.0);
        current.stop =this->frameToTime(int(sub_uses_time?mpsub_position/100.0:mpsub_position));

        while (1) {
                if (!fd.fgets (line, this->LINE_LEN)) {
                        if (num == 0) return NULL;
                        else return store(current);
                }
                p=line;
                while (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)*p)) p++;
                if (eol(*p) && num > 0) return store(current);
                if (eol(*p)) return NULL;

                for (q=p; !eol(*q); q++);
                *q='\0';
                if (strlen(p)) {
                        current.add(p);
                        ++num;
                } else {
                        if (num) return store(current);
                        else return NULL;
                }
        }
        return NULL; // we should have returned before if it's OK
}

template<class tchar> Tsubtitle* TsubtitleParserAqt<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME, REFERENCE_TIME) {
    tchar line[this->LINE_LEN+1];
    const tchar *next;
    //int i;

    long start;
    while (1) {
    // try to locate next subtitle
        if (!fd.fgets (line, this->LINE_LEN))
                return NULL;
        if (!(tchar_traits<tchar>::sscanf() (line, _L("-->> %ld"), &start) <1))
                break;
    }
    TsubtitleTextBase<tchar> current(this->format);
    current.start=this->frameToTime(start);
    if (previous != NULL)
        previous->stop = current.start-1;

    //previous_aqt_sub = current;

    if (!fd.fgets (line, this->LINE_LEN))
        return NULL;

    sub_readtext(line,current);
    current.stop = current.start; // will be corrected by next subtitle

    if (!fd.fgets (line, this->LINE_LEN))
     return previous=store(current);

    next = line;//,i=1;
    while ((next =sub_readtext (next,current))!=NULL)
     ;

    if (current.at(0)[0]=='\0' && current.at(1)[0]=='\0') {
        // void subtitle -> end of previous marked and exit
        previous = NULL;
        return NULL;
        }

    return previous=store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserSubrip09<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME start, REFERENCE_TIME stop) {
    tchar line[this->LINE_LEN+1];
    int a1,a2,a3;
    const tchar * next=NULL;
    int i;

    while (1) {
    // try to locate next subtitle
        if (!fd.fgets (line, this->LINE_LEN))
                return NULL;
        if (!(tchar_traits<tchar>::sscanf() (line, _L("[%d:%d:%d]"),&a1,&a2,&a3) < 3))
                break;
    }
    TsubtitleTextBase<tchar> current(this->format);
    current.start = this->hmsToTime(a1,a2,a3);
    if (previous != NULL)
        previous->stop = current.start-1;

    if (!fd.fgets (line, this->LINE_LEN))
        return NULL;

    next = line;i=0;

    //(*current)[0]=""; // just to be sure that string is clear

    while ((next =sub_readtext (next, current))!=NULL)
     i++;

    if (current.size()==0 || (current.at(0)[0]=='\0') && (i==0)) {
        // void subtitle -> end of previous marked and exit
        previous = NULL;
        return NULL;
        }

    return previous=store(current);
}

template<class tchar> Tsubtitle* TsubtitleParserMPL2<tchar>::parse(Tstream &fd,int flags, REFERENCE_TIME, REFERENCE_TIME) {
    tchar line[this->LINE_LEN+1];
    tchar line2[this->LINE_LEN+1];
    const tchar *p,*next;
    int start=0,end=0;

    do {
	if (!fd.fgets (line, this->LINE_LEN)) return NULL;
    } while ((tchar_traits<tchar>::sscanf() (line,
		      _L("[%ld][%ld]%[^\r\n]"),
		      &start, &end, line2) < 3));
    TsubtitleTextBase<tchar> current(this->format);
    current.start = (REFERENCE_TIME)start*1000000;
    current.stop = (REFERENCE_TIME)end*1000000;
    p=line2;
    next=p;
    while ((next =sub_readtext (next, current))!=NULL)
     ;
    return store(current);
}

template<class tchar> TsubtitleParserBase* TsubtitleParserBase::getParser0(int format,double fps,const TsubtitlesSettings *cfg,const Tconfig *ffcfg,Tsubreader *subreader,bool isEmbedded)
{
 switch (format&Tsubreader::SUB_FORMATMASK)
  {
   case Tsubreader::SUB_MICRODVD  :return new TsubtitleParserMicrodvd<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SUBRIP    :return new TsubtitleParserSubrip<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SUBVIEWER :return new TsubtitleParserSubviewer<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SAMI      :return new TsubtitleParserSami<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_VPLAYER   :return new TsubtitleParserVplayer<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_RT        :return new TsubtitleParserRt<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SSA       :return new TsubtitleParserSSA<tchar>(format,fps,cfg,ffcfg,subreader,isEmbedded);
   case Tsubreader::SUB_DUNNOWHAT :return new TsubtitleParserDunnowhat<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_MPSUB     :return new TsubtitleParserMPsub<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_AQTITLE   :return new TsubtitleParserAqt<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SUBVIEWER2:return new TsubtitleParserSubviewer2<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_SUBRIP09  :return new TsubtitleParserSubrip09<tchar>(format,fps,cfg,ffcfg,subreader);
   case Tsubreader::SUB_MPL2      :return new TsubtitleParserMPL2<tchar>(format,fps,cfg,ffcfg,subreader);
   default:return NULL;
  }
}
TsubtitleParserBase* TsubtitleParserBase::getParser(int format,double fps,const TsubtitlesSettings *cfg,const Tconfig *ffcfg,Tsubreader *subreader,bool utf8,bool isEmbedded)
{
 if (utf8 || Tsubreader::getSubEnc(format)&Tstream::ENC_UNICODE)
  return getParser0<wchar_t>(format,fps,cfg,ffcfg,subreader,isEmbedded);
 else
  return getParser0<char>(format,fps,cfg,ffcfg,subreader,isEmbedded);
}

//======================================= TsubreaderMplayer =======================================
TsubreaderMplayer::TsubreaderMplayer(Tstream &fd,int sub_format,double fps,const TsubtitlesSettings *cfg,const Tconfig *ffcfg,bool isEmbedded)
{
 TsubtitleParserBase *parser=TsubtitleParserBase::getParser(sub_format,fps,cfg,ffcfg,this,false,isEmbedded);
 if (!parser) return;

 fd.rewind();

 while (parser->parse(fd))
  ;
 delete parser;

 processDuration(cfg);

 if (cfg->timeoverlap && !empty())
  {
   for (iterator s=begin();s!=end()-1;s++)
    { // without these braces, processOverlap() is executed multiple times... (MSVC8)
     if ((*s)->stop<(*s)->start)
      (*s)->stop=(*(s+1))->start-1;
    }
   processOverlap();
  }
}

void Tsubreader::processOverlap(void)
{
 if (empty()) return;
 if (!at(0)->isText()) return;
 static const int SUB_MAX_TEXT=INT_MAX/2;
 int sub_orig = size();
 int n_first = size();
 int sub_num =0;
 std::vector<Tsubtitle*> newsubs;
 for (int sub_first = 0; sub_first < n_first; ++sub_first)
  {
   REFERENCE_TIME global_start = at(sub_first)->start,global_end = at(sub_first)->stop, local_start, local_end;
   int lines_to_add = at(sub_first)->numlines(), sub_to_add = 0;
   int **placeholder = NULL, higher_line = 0, counter, start_block_sub = sub_num;
   char real_block = 1;

   // here we find the number of subtitles inside the 'block'
   // and its span interval. this works well only with sorted
   // subtitles
   while ((sub_first + sub_to_add + 1 < n_first) && (at(sub_first + sub_to_add + 1)->start < global_end))
    {
     ++sub_to_add;
     lines_to_add += at(sub_first + sub_to_add)->numlines();
     if (at(sub_first + sub_to_add)->start < global_start)
      global_start = at(sub_first + sub_to_add)->start;
     if (at(sub_first + sub_to_add)->stop > global_end)
      global_end = at(sub_first + sub_to_add)->stop;
    }

   // we need a structure to keep trace of the screen lines
   // used by the subs, a 'placeholder'
   counter = 2 * sub_to_add + 1;  // the maximum number of subs derived from a block of sub_to_add+1 subs
   placeholder = (int **) malloc(sizeof(int *) * counter);
   for (int i = 0; i < counter; ++i)
    {
     placeholder[i] = (int *) malloc(sizeof(int) * lines_to_add);
     for (int j = 0; j < lines_to_add; ++j)
      placeholder[i][j] = -1;
    }

   counter = 0;
   local_end = global_start - 1;
   do
    {
     // here we find the beginning and the end of a new subtitle in the block
     local_start = local_end + 1;
     local_end   = global_end;
     for (int j = 0; j <= sub_to_add; ++j)
      {
       if ((at(sub_first + j)->start - 1 > local_start) && (at(sub_first + j)->start - 1 < local_end))
        local_end = at(sub_first + j)->start - 1;
       else if ((at(sub_first + j)->stop > local_start) && (at(sub_first + j)->stop < local_end))
        local_end = at(sub_first + j)->stop;
      }

     // here we allocate the screen lines to subs we must
     // display in current local_start-local_end interval.
     // if the subs were yet presents in the previous interval
     // they keep the same lines, otherwise they get unused lines
     for (int j = 0; j <= sub_to_add; ++j)
      {
       if ((at(sub_first + j)->start <= local_end) && (at(sub_first + j)->stop > local_start))
        {
         unsigned long sub_lines=at(sub_first + j)->numlines(), fragment_length = lines_to_add + 1,tmp=0;
         char boolean = 0;
         int fragment_position = -1;

         // if this is not the first new sub of the block
         // we find if this sub was present in the previous
         // new sub
         if (counter)
          for (int i = 0; i < lines_to_add; ++i)
           if (placeholder[counter - 1][i] == sub_first + j)
            {
             placeholder[counter][i] = sub_first + j;
             boolean = 1;
            }
         if (boolean)
          continue;
         // we are looking for the shortest among all groups of
         // sequential blank lines whose length is greater than or
         // equal to sub_lines. we store in fragment_position the
         // position of the shortest group, in fragment_length its
         // length, and in tmp the length of the group currently
         // examined
         int i;
         for (i = 0; i < lines_to_add; ++i)
          {
           if (placeholder[counter][i] == -1)
            {
             ++tmp; // placeholder[counter][i] is part of the current group of blank lines
            }
           else
            {
             if (tmp == sub_lines)
              {
               // current group's size fits exactly the one we
               // need, so we stop looking
               fragment_position = i - tmp;
               tmp = 0;
               break;
              }
             if ((tmp) && (tmp > sub_lines) && (tmp < fragment_length))
              {
               // current group is the best we found till here,
               // but is still bigger than the one we are looking
               // for, so we keep on looking
               fragment_length = tmp;
               fragment_position = i - tmp;
               tmp = 0;
              }
             else
              {
               tmp = 0; // current group doesn't fit at all, so we forget it
              }
            }
          }
         if (tmp)
          {
           if ((tmp >= sub_lines) && (tmp < fragment_length))  // last screen line is blank, a group ends with it
            fragment_position = i - tmp;
          }
         if (fragment_position == -1)
          {
           // it was not possible to find free screen line(s) for a subtitle,
           // usually this means a bug in the code; however we do not overlap
           //mp_msg(MSGT_SUBREADER, MSGL_WARN, "SUB: we could not find a suitable position for an overlapping subtitle\n");
           higher_line = SUB_MAX_TEXT + 1;
           break;
          }
         else
          for (tmp = 0; tmp < sub_lines; ++tmp)
           placeholder[counter][fragment_position + tmp] = sub_first + j;
        }
      }
     for (int j = higher_line + 1; j < lines_to_add; ++j)
      {
       if (placeholder[counter][j] != -1)
        higher_line = j;
       else
        break;
      }

     // we read the placeholder structure and create the new subs.
     Tsubtitle *second=at(0)->create();
     second->start = local_start;
     second->stop  = local_end;
     for (int i = 0, j = 0; j < lines_to_add ; ++j)
      {
       if (placeholder[counter][j] != -1)
        {
         at(placeholder[counter][j])->copyProps(second);
         int lines = at(placeholder[counter][j])->numlines();
         for (int ls = 0; ls < lines; ++ls){
          at(placeholder[counter][j])->copyLine(second,ls);}
         j+=lines-1;
        }
      }
     newsubs.push_back(second);
     ++sub_num;
     ++counter;
    } while (local_end < global_end);
   counter = 2 * sub_to_add + 1;
   for (int i = 0; i < counter; ++i)
    free(placeholder[i]);
   free(placeholder);
   sub_first += sub_to_add;
  }
 clear();
 for (std::vector<Tsubtitle*>::const_iterator s=newsubs.begin();s!=newsubs.end();s++)
  push_back(*s);
 IsProcessOverlapDone=true;
}
