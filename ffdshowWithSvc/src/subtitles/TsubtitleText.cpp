/*
 * Copyright (c) 2003-2006 Milan Cutka
 * subtitles fixing code from SubRip by MJQ (subrip@divx.pl)
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
#include "TsubtitleText.h"
#include "Tfont.h"
#include "Tsubreader.h"
#include "TsubtitlesSettings.h"
#include "Tconfig.h"
#include "ThtmlColors.h"

//========================== TtextFixBase =========================
strings TtextFixBase::getDicts(const Tconfig *cfg)
{
 char_t msk[MAX_PATH];
 _makepath_s(msk, countof(msk), NULL, cfg->pth, _l("dict\\*"), _l("dic"));
 strings dicts;
 findFiles(msk,dicts,false);
 for (strings::iterator d=dicts.begin();d!=dicts.end();d++)
  d->erase(d->find('.'),MAX_PATH);
 return dicts;
}

//============================= TtextFix ==========================
template<class tchar> TtextFix<tchar>::TtextFix(const TsubtitlesSettings *scfg,const Tconfig *ffcfg):
 EndOfPrevSentence(true),
 inHearing(false)
{
 memcpy(&cfg,scfg,sizeof(TsubtitlesSettings));
 if (scfg->fix&fixOrtography)
  {
   char_t dictflnm[MAX_PATH];
   _makepath_s(dictflnm, countof(dictflnm), NULL, ffcfg->pth, (::ffstring(_l("dict\\"))+scfg->fixDict).c_str(), _l("dic"));
   TstreamFile fs(dictflnm,false,false);
   if (fs)
    {
     tchar ln[1000];
     while (fs.fgets(ln,1000))
      {
       tchar *eoln=strchr(ln,'\n');if (eoln) *eoln='\0';
       odict.push_back(ln);
      }
     if (odd(odict.size()))
      odict.erase(odict.end()-1);
     if (!odict.empty())
      for (typename strings::iterator s=odict.begin();s+1!=odict.end();s++)
       if (strstr((s+1)->c_str(),s->c_str()))
        {
         s=odict.erase(s);
         s=odict.erase(s);
        }
    }
  }
}

template<class tchar> bool TtextFix<tchar>::process(ffstring &text,ffstring &fixed)
{
 if (cfg.fix==0) return false;
 int W1;
 passtring<tchar> S1=text;

 if (cfg.fix&fixAP) //AP
  {
   S1=stringreplace(S1,_L("`")   , _L("'") ,rfReplaceAll);
   S1=stringreplace(S1,_L("\264"), _L("'") ,rfReplaceAll);
   S1=stringreplace(S1,_L("''")  , _L("\""),rfReplaceAll);
   S1=stringreplace(S1,_L("\"\""), _L("\""),rfReplaceAll);
  }

 if (cfg.fix&fixIl) // Il
  {
   #define TrChar(n) St1[W1+(n)]
   bool TakeI=false,Takel=false;
   passtring<tchar> St1=_L("  ")+S1+_L("  ");
   for (W1=1 + 2;W1<=St1.size() - 2;W1++)
    {
     //small L --> big i
     if (St1[W1] == 'l')
      {
       TakeI = false;
       //_lll trio
       if ((TrChar(-1)==' ' || TrChar(-1)=='#') && (TrChar(+1) == 'l') && (TrChar(+2) == 'l'))
        {
         //_lllx --> _Illx (Illinois, Illegal etc.) + _Ill-
         if (in(TrChar(+3),_L("-abehinopstuy"))) TakeI = true;
         //._Ill_ (=bedridden) + ._Ill. (Ill. = Illustriert, Illustration [German])
         //other than BoL cases not reflected
         if (in(TrChar(-2),_L(".[")) && (in(TrChar(+3),_L(" .")))) TakeI = true;
         //Godfather III / Godfather III.
         if (!(in(TrChar(-2),_L(".[")) && (in(TrChar(+3),_L(" .!?,")))))
          {
           TakeI = true;
           St1[W1 + 1] = 'I';
           St1[W1 + 2] = 'I';
          }
        }
       //_lgelit _lglu _lgnition _lgrafpapier _lguana
       if (in(TrChar(-1),_L(" [(#")) && (TrChar(+1) == 'g') && in(TrChar(+2),_L("elnru")))
        TakeI = true;
       //ME LOVES MAKARONl! (MY TO SLYSELl. (!?) [cz; no others affected])
       if (in(TrChar(+1),_L(".!?")) &&
           (TrChar(-1) == toupper(TrChar(-1))) &&
           (TrChar(-1) != '.') && //»p?En.l.« at EoL case
           (TrChar(-2) == toupper(TrChar(-2))))
        TakeI = true;
       //UNlVERSAL, lnternet, lnspektion, lnternational;  not changed - ln?Eý [cz; no others affected]
       //contain  : _l_ case [no Fr!], XlX, Xl_, _lX
       //           V.l. Lenin, l.V. Lenin (initials of names)
       //safely   : -lx OR -_lx; [lx OR [_lx; "Ix OR "_Ix
       //dangerous: .lxxx
       if (((TrChar(-1) == toupper(TrChar(-1))) &&
         (TrChar(-1) != '.') && //»p?En.l.« case [cz; no others affected]
         (TrChar(+1) == toupper(TrChar(+1))))
         ||
         ((in(TrChar(-1),_L(" #"))) && (TrChar(+1) == 'n') && (TrChar(+2) != '\354')))
        TakeI = true;
       //last sign at uniline OR space follows
       if ((TrChar(+1) == ' ') &&
         (TrChar(-1) == toupper(TrChar(-1))))
        TakeI = true;
       //._l | -l | -_l; contain: ._ll_ (italian: Il brutto, il nero)
       if (((TrChar(-2) != '.') && (TrChar(-1) == '.')) ||
           ((TrChar(-2) == ' ') && (TrChar(-1) == '-')) ||
           ((in(TrChar(-2),_L(".-"))) && (TrChar(-1) == ' ')) &&
           (TrChar(+1) != 'i')) TakeI = true;
       //_ll_ to _Il_ ...this is made only as prep for next
       if ((in(TrChar(-1),_L(" #"))) && (TrChar(+1) == 'l') &&
         (in(TrChar(+2),_L(" .,?!:")))) TakeI = true;
       //_Il_ to _II_ (-2 <> . or [) Godfather II & Ramses II
       if ((TrChar(-3) != '.') && (TrChar(-3) != '[') && (TrChar(-3) != '\0') &&
           (TrChar(-2) == ' ') && (TrChar(-1) == 'I') &&
           (in(TrChar(+1),_L(" .,?!:")))) TakeI = true;
       //"hey C.W. load the weapons" and "hey... load the weapons"
       //      432101                        432101
       if ((TrChar(-4) == '.') && (TrChar(-2) == '.') & (TrChar(-1) == ' ') &&
           (in(TrChar(+1),_L("aeiouy"))))
        TakeI = false;
       //_Ital...
       if ((in(TrChar(-1),_L(" #"))) && (TrChar(+1) == 't') && (TrChar(+2) == 'a'))
        TakeI = true;
       //only for international abnormalities
       switch (cfg.fixLang)
        {
         case 0:// StCorrectIl_En;
          if (St1[W1] == 'l')
           {
            //so shity due the styles
            if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 't' ) && (TrChar(+2) == '\'') && (TrChar(+3) == 's')) TakeI = true; //_lt's
            if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == '\'') && (TrChar(+2) == 'm' ) && (TrChar(+3) == ' ')) TakeI = true; //_l'm_
            if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == '\'') && (TrChar(+2) == 'd' ) && (TrChar(+3) == ' ')) TakeI = true; //_l'd_
            if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == '\'') && (TrChar(+2) == 'v' ) && (TrChar(+3) == 'e')) TakeI = true; //_l've
            if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == '\'') && (TrChar(+2) == 'l' ) && (TrChar(+3) == 'l')) TakeI = true; //_l'll
            if ((in(TrChar(-1),_L(" \"#"))) && (in(TrChar(+1),_L("fnst"))) && (TrChar(+2) == ' ')) TakeI = true; //_lf_, _ln_, _ls_, _lt_
            //no English word beginning with "lc", "ld", "lh", "ln", "lr", "ls"
            if ((in(TrChar(-1),_L(" \"#"))) && (in(TrChar(+1),_L("cdhnrs")))) TakeI = true;
           }
          break;
         case 1:// StCorrectIl_Fr;
          //*unknown*: I think that for french neither "I" alone nor "l" alone can exist so we don't check this. So the only thing we can write is that before an apostrophe there must be an "l".
          if (((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == ' ')) || //but Brain as Frenchman: lonesome I doesn't exist
              (TrChar(+1) == '\''))//*unknown*: An I followed by an apostrophe cannot exist. Must be an l.
           {
            TakeI = false;
            Takel = true;
           }
          break;

         case 2:// StCorrectIl_Ge;
          if (St1[W1] == 'l')
           //no German word beginning with "lc", "ld", "lh", "ln", "lr", "ls"
           if ((in(TrChar(-1),_L(" \"#"))) && (in(TrChar(+1),_L("cdhnrs")))) TakeI = true;
          break;
         case 3:// StCorrectIl_Cz;
          if (St1[W1] == 'l')
           //no Czech word beginning with "lc", "ld", "lr"
           if ((in(TrChar(-1),_L(" \"#"))) && (in(TrChar(+1),_L("cdr")))) TakeI = true;
          break;
         case 4:// StCorrectIl_It;
          //Hint: "i" alone CAN exist (even capital: after a full stop) while l alone can't, never
          //l' processing
          if ((TrChar(+1) == '\'')) //Ita always has l before apostrophe (if ' is not used as an accent for a capital letter! [rare, and followed by whitespace])
           if ((in(TrChar(+2),_L("aAeEiIloOuUhH\xe0\xe8\xe9\xef\xf2\xf9"))))
            {
             Takel = true;
             TakeI = false;
            }
           //else: unknown pattern!? Probably an English sentence within an Italian movie. We rely on the general algorithm.
          break;
         case 5:// StCorrectIl_Pl;
          if (St1[W1] == 'l')
           //no Czech word beginning with "lc", "ld", "lr"
           if ((in(TrChar(-1),_L("., \"#"))) && (in(TrChar(+1),_L("bcdfhjkrstz\346\263\361\237"))))
            {
             Takel = false;
             TakeI = true;
            }
          break;
        }
       if (TakeI)
        {
         St1[W1] = 'I';
         //'-' for I-I-I...
         if ((W1 > 4) && (St1[W1 - 2] == 'l') && (St1[W1 - 1] == '-'))
          if (St1[W1 - 3] == ' ') St1[W1 - 2] = 'I';
          else
           if ((St1[W1 - 4] == 'l') && (St1[W1 - 3] == '-'))
            {
             St1[W1 - 2] = 'I';
             St1[W1 - 4] = 'I';
            }
        }
      }
     //big i --> small L
     if (St1[W1] == 'I')
      {
       Takel = false;
       //_III to _Ill
       if ((in(TrChar(-1),_L(" #"))) && (TrChar(+1) == 'I') && (TrChar(+2) == 'I'))
        {
         //_IIIx --> _Illx (Illinois, Illegal etc.) + _Ill-
         if ((in(TrChar(+3),_L("-abehinopstuy")))
          ||
           //._Ill_ (=bedridden) + ._Ill. (Ill. = Illustriert, Illustration [German])
           //other than BoL cases not reflected
           ((in(TrChar(-2),_L(".["))) && (in(TrChar(+3),_L(" .")))))
          {
           St1[W1 + 1] = 'l';
           St1[W1 + 2] = 'l';
          }
           //Godfather III / Godfather III. *PASS*
        }
       //All, English, German
       if (TrChar(-1)>='a' && TrChar(-1)<='z' && in(TrChar(-1),_L("\344\366\374"))) Takel = true;
       else
        if (!((in(TrChar(+1),
          //All, English, German; 'w' - Iwao (Japan name), 'v' - Ivan
          _L("ABCDEFGHIJKLMNOPQRSTUVWXYZ\304\326\334 bcdghklmnoprstvwz\'.,!?"))) || //',' prevents PRCl, PRCl, PRCICKY
          //+East Europe - Czech, Slovak, Polish
          ((cfg.font.charset== EASTEUROPE_CHARSET) &&
          (in(TrChar(+1),_L("\301\311\314\315\323\332\331\335\310\317\322\330\212\215\216\324\305\274\243\217\257\323\245\312\214\306\321")))) ||
          //+Scandinavian
          ((cfg.font.charset== ANSI_CHARSET) &&
          in(TrChar(+1),_L("\xc6\xd8\xc5"))
          ) ||
          //+Italian: _Ieri_
          ((in(TrChar(-1),_L(" #"))) && (TrChar(+1) == 'e') &&
          (TrChar(+2) == 'r') && (cfg.fixLang == 4)
          )))
         Takel = true;
       //+East Europe - Czech, Slovak, Polish
       if ((cfg.font.charset== EASTEUROPE_CHARSET) &&
           (in(TrChar(-1),_L("\341\350\357\351\354\355\362\363\370\232\235\372\371\375\236\364\345\276\237\277\363\263\271\352\234\346\361"))))
        Takel = true;
       //+Scandinavian
       if ((cfg.font.charset== ANSI_CHARSET) &&
           (in(TrChar(-1),_L("\xe6\xf8\xe5"))))
        Takel = true;
       //_AIways AIden BIb BIouznit CIaire ÈIov?E PIný SIožit UItra ZIost
       if ((in(TrChar(-2),_L(" \"-c#"))) && //'c' for Mac/Mc (McCIoy)
           (in(TrChar(-1),_L("ABCDEFGHIJKLMNOPQRSTUVWXYZ\310\212\216\217\214"))) &&
           (in(TrChar(+1),_L("abcdefghijklmnopqrstuvwxyz\341\350\351\354\355\363\362\232\371\375"))))
        Takel = true;
       //Ihned leave, but Ihostejný --> lhostejný [cz; no others affected]
       if ((TrChar(+1) == 'h') && (TrChar(+2) != 'n')) Takel = true;
       //_AI_ to _Al_ - name (Artificial Intelligence (AI) not awaited :-)
       if ((in(TrChar(-2),_L(" \"#'"))) &&
           (TrChar(-1) == 'A') &&
           (in(TrChar(+1),_L(" \".,!?'"))))
        Takel = true;
       //._II_ to ._Il_ (Il nero on BoL italian)
       if ((in(TrChar(-3),_L(".[-"))) && (TrChar(-2) == ' ') &&
          (TrChar(-1) == 'I') && (TrChar(+1) == ' '))
        Takel = true;
       //(repd. from l-->I procedure too)  "EI gringo" to "El g." (BoL Spain)
       if ((TrChar(-1) == 'E') && (TrChar(+1) == ' ')) Takel = true;
       //'II_ --> 'll_
       if ((TrChar(-1) == '\'') && (TrChar(+1) == 'I') && (TrChar(+2) == ' '))
        {
         Takel = true;
         St1[W1 + 1] = 'l';
        }
       //_Iodic, Iolanthe/Iolite, Ion, Iowa, *Ioya* (but loyalty and more), Iota
       //vs Ioad Iocation Iook Iost Ioud Iove
       //Ioch Iogický/Iogo Ioket Iomcovák Iopata Iovit
       //. Io... (Italian)
       if (!(in(TrChar(-2),_L(".!?"))) &&
            (in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') &&
            !(in(TrChar(+2),_L("dlnwt .,!?"))))
        Takel = true;
       //Iodn?Eetc. vs Iodate Ioderma Iodic Iodoethanol [cz only, won't affect others]
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') && (TrChar(+2) == 'd') &&
            !(in(TrChar(+3),_L("aeio")) || TrChar(+3)=='\0'))
        Takel = true;
       //Iondýnský [cz only, won't affect others]
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') && (TrChar(+2) == 'n') &&
            (TrChar(+3) == 'd'))
        Takel = true;
       //_Iong
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') &&
            (TrChar(+2) == 'n') && (TrChar(+3) == 'g'))
        Takel = true;
       //_Ioni_ --> _loni_  [cz only, don't affect others]
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') && (TrChar(+2) == 'n') &&
            (TrChar(+3) == 'i') && (in(TrChar(+4),_L(" \".,!?:"))))
        Takel = true;
       //Iot_ etc. vs Iota
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') &&
            (TrChar(+2) == 't') && (TrChar(+3) != 'a'))
        Takel = true;
       //Iow_ etc. vs Iowa
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'o') &&
            (TrChar(+2) == 'w') && (TrChar(+3) != 'a'))
        Takel = true;
       //_AII_ etc.
       if ((in(TrChar(-2),_L(" \"-#"))) &&
            (in(TrChar(-1),_L("AEU"))) && (TrChar(+1) == 'I') &&
            (in(TrChar(+2),_L(" '-abcdefghijklmnopqrstuvwxyz"))))
        {
         Takel = true;
         St1[W1 + 1] = 'l';
        }
       //Eng only, no others affected: _If_, _If-, Iffy, Ifle, Ifor
       if ((in(TrChar(-1),_L(" \"#"))) && (TrChar(+1) == 'f') &&
          (in(TrChar(+2),_L(" -flo"))))
        Takel = false;
       //"hey C.W. Ioad the weapons" and "hey... Ioad the weapons"
       //      432101                        432101
       if ((TrChar(-4) == '.') && (TrChar(-2) == '.') && (TrChar(-1) == ' ') &&
            (in(TrChar(+1),_L("aeiouy"))))
        Takel = true;
       /* TODO
       //only for international abnormalities
       case RadioGroupCorrectIlLanguage.ItemIndex of
         //0: StCorrectIl_En;
         1: StCorrectIl_Fr;
         //2: StCorrectIl_Ge;
         //3: StCorrectIl_Cz;
         4: StCorrectIl_It;
       end;
       */
       if (Takel)
        {
         St1[W1] = 'l';
         //'-' for 'Uh, l-ladies and gentleman...' / 'just as you l-left it'
         if ((W1 > 4) && (St1[W1 - 2] == 'I') && (St1[W1 - 1] == '-'))
          if (St1[W1 - 3] == ' ') St1[W1 - 2] = 'l';
          else
           if ((St1[W1 - 4] == 'I') && (St1[W1 - 3] == '-'))
            {
             St1[W1 - 2] = 'l';
             St1[W1 - 4] = 'l';
            }
        }
      }
    }
   #undef TrChar
   S1=St1.copy(1 + 2, St1.size() - 4);
  }

 if (cfg.fix&fixPunctuation) //punctuation
  {
   static const tchar *punctinations[]=
    {
     _L("?."),_L("?"),
     _L("?,"),_L("?"),
     _L(" )"),_L(")"),
     _L("( "),_L("("),
     _L(" ]"),_L("]"),
     _L("[ "),_L("["),
     _L("!."),_L("!"),
     _L("!,"),_L("!"),
     _L(". . ."),_L("..."),
     _L(". .."),_L("..."),
     _L(".. ."),_L("..."),
     _L("--"),_L("..."),
     _L("...."),_L("..."),
     _L(" :"),_L(":"),
     _L(" ;"),_L(";"),
     _L(" %"),_L("%"),
     _L(" !"),_L("!"),
     _L(" ?"),_L("?"),
     _L(" ."),_L("."),
     _L(" ,"),_L(","),
     _L(".:"),_L(":"),
     _L(":."),_L(":"),
     _L("eVe"),_L("eve"),
     _L("    }"),_L("  }"),
     _L("  "),_L(" "),
     NULL,NULL
    };
   int i=1;
   //"--" at the beginning of dialogs (Spy Kids 2, Stolen Summer, Sweet Home Alabama)
   if (S1[i]=='-' && S1[i+1]=='-' && S1[i+2]!='-')
    S1.erase(i,1);

   //dict
   for (i=0;punctinations[2*i];i++)
    S1=stringreplace(S1,punctinations[2*i],punctinations[2*i+1],rfReplaceAll);

   //".." to "..."
   W1=S1.find(_L(".."));
   if (W1>0)
    while (S1.size()>W1)
     {
      if (((W1== 1) || (S1[W1 - 1] != '.')) &&
          (S1[W1] == '.') && (S1[W1 + 1] == '.') &&
          ((S1.size() == W1 + 1) || (S1[W1 + 2] != '.'))
         )
       S1.insert(W1,_L("."));
      W1++;
     }

   //"..."
   do
    {
     W1 = S1.find(_L("..."));
     if (W1 > 0)
      //Example "It is ..." -> "It is..."
      if ((W1 == S1.size() - 2) && (S1[W1 - 1] == ' '))
         S1 = S1.copy(1, S1.size() - 4) + _L("<><>");
       else
         //Example "... it is" -> "...it is"
         if ((W1 == 1) && (S1[W1 + 3] == ' '))
           S1 = _L("<><>") + S1.copy(5, S1.size() - 3);
         else
          //Example "It...is" -> "It... is"
          if ((W1 > 1) && (W1 < S1.size() - 2) &&
              !(S1[W1 - 2]=='?' || S1[W1-2]=='!'))
           S1 = S1.copy(1, W1 - 1) + _L("<><> ") + S1.copy(W1 + 3, S1.size() - (W1 + 2));
          else
           //Example "Who?... there..." -> "Who? ...there..."
           if ((W1 > 1) && (W1 < S1.size() - 2) &&
               (S1[W1 - 1] =='?' || S1[W1-1]=='!'))
            S1 = S1.copy( 1, W1 - 1) + _L(" <><>") + S1.copy( W1 + 4, S1.size() - (W1 + 3));
             else
            S1 = S1.copy( 1, W1 - 1) + _L("<><>") + S1.copy(W1 + 3, S1.size() - (W1 + 2));
    } while (W1!=0);
   S1 = stringreplace(S1, _L("?<><> "), _L("? <><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("<><> ?"), _L("<><>?"), rfReplaceAll);
   S1 = stringreplace(S1, _L("<><> !"), _L("<><>!"), rfReplaceAll);
   S1 = stringreplace(S1, _L("!<><> "), _L("! <><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("\"<><> "), _L("\"<><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("<><> \""), _L("<><>\""), rfReplaceAll);
   S1 = stringreplace(S1, _L("- <><> "), _L("<><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("- <><>"), _L("<><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("-<><> "), _L("<><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L("-<><>"), _L("<><>"), rfReplaceAll);
   S1 = stringreplace(S1, _L(" <><> "), _L("<><> "), rfReplaceAll);
   S1 = stringreplace(S1, _L(",<><>"), _L("<><>"), rfReplaceAll);

   //|"_| -> |"| or |_"| -> |"|  -  sometimes in safe cases
   int QoutCnt = 0;
   for (i=1;i<=S1.size();i++)
    if (S1[i]=='"')
     QoutCnt++;
   if (QoutCnt > 0)
    if (!odd(QoutCnt))
     {
      for (i=1;i<=S1.size();i++)
       if (S1[i] == '"')
        {
         if (odd(QoutCnt))
          {
           if ((i > 1) && (S1[i - 1] == ' ')) S1.erase(i - 1, 1);
          }
         else
          if ((i < S1.size()) && (S1[i + 1] == ' ')) S1.erase(i + 1, 1);
         QoutCnt--;
        }
     }
    else
     {
      //aftercheck 1: |"_| at position 1 (if only 1x " appear as beginning)
      if (S1.find(_L("\" ")) == 1) S1.erase(2, 1);
      //aftercheck 2: |_"| at last position (if only 1x " appear as end)
      if (S1.find(_L(" \"")) == S1.size() - 1) S1.erase(S1.size() - 1, 1);
     }

   //"-"
   do
    {
     W1 = S1.find('-');
     if (W1 > 0)
      {
       //Example "-It is" -> "- It is"
       if ((W1 == 1) && (S1[W1 + 1] != ' '))
        S1 = _L("[][] ") + S1.copy(2, S1.size() - 1);
       else
        //Example "Hi. -It is." -> "Hi. - It is." (works for styles: "<i>-It is")
        if (((in(S1[W1 - 2],_L(".!?>"))) || (S1[W1 - 1] == '>')) && (S1[W1 + 1] != ' '))
         S1 = S1.copy(1, W1 - 1) + _L("[][] ") + S1.copy(W1 + 1, S1.size() - (W1));
        else
         S1 = S1.copy(1, W1 - 1) + _L("[][]") + S1.copy(W1 + 1, S1.size() - (W1));
      }
    } while (W1 != 0);

   static const tchar *chars1[]={_L("."),_L(","),_L(":"),_L(";"),_L("%"),_L("$"),_L("!"),_L("?")};
   //Spaces after "," "." ":" ";" "$" "%"
   if ((S1.find(_L("www.")) == 0) && (S1.find(_L(".com ")) == 0) &&
       (S1.find('@') == 0) && (S1.find(_L("tp://")) == 0))
    for (i=0;i<6;i++)
     {
      do
       {
        W1 = S1.find(chars1[i]);
        if (W1 > 0)
         if ((W1 != S1.size() && !(S1[W1 + 1]==' ' || S1[W1 + 1]=='"' || S1[W1 + 1]=='\'' || S1[W1 + 1]=='?' || S1[W1 + 1]=='!' || S1[W1 + 1]==',' || S1[W1 + 1]==']' || S1[W1 + 1]=='}' || S1[W1 + 1]==')' || S1[W1 + 1]=='<'))
             && ((S1[W1 - 1] < '0') || (S1[W1 - 1] > '9')) && ((S1[W1 + 1] < '0') || (S1[W1 + 1] > '9'))
             && (S1[W1 + 2] != '.'))
          S1 = S1.copy(1, W1 - 1) + _L("()() ") + S1.copy(W1 + 1, S1.size() - (W1));
         else
          S1 = S1.copy(1, W1 - 1) + _L("()()") + S1.copy(W1 + 1, S1.size() - W1);
       } while (W1 != 0);
      S1 = stringreplace(S1, _L("()()"), chars1[i], rfReplaceAll);
     }
   //Spaces after "!" "?"
   for (i=6;i<=7;i++)
    {
     do
      {
       W1 = S1.find(chars1[i]);
       if (W1 > 0)
        if ((W1 != S1.size()) && (S1[W1 + 1] != ' ') && !(S1[W1 + 1]=='!' || S1[W1 + 1]=='?' || S1[W1 + 1]=='"' || S1[W1 + 1]==']' || S1[W1 + 1]=='}' || S1[W1 + 1]==')' || S1[W1 + 1]=='<')) //'<' styles)
         S1 = S1.copy(1, W1 - 1) + _L("()() ") + S1.copy( W1 + 1, S1.size() - (W1));
        else
         S1 = S1.copy(1, W1 - 1) + _L("()()") + S1.copy(W1 + 1, S1.size() - (W1));
      } while (W1 != 0);
     S1 = stringreplace(S1, _L("()()"), chars1[i], rfReplaceAll);
    }
   S1 = stringreplace(S1, _L("[][]"), _L("-"), rfReplaceAll);
   S1 = stringreplace(S1, _L("<><>"), _L("..."), rfReplaceAll);
  }

 if (cfg.fix&fixOrtography)
  for (typename strings::const_iterator s=odict.begin();s!=odict.end();)
   {
    const ffstring &oldstr=*s;s++;
    const ffstring &newstr=*s;s++;
    S1=stringreplace(S1,oldstr,newstr,rfReplaceAll);
   }

 if (cfg.fix&fixCapital)
  {
   if (cfg.fix&fixCapital2) S1.ConvertToLowerCase();

   //Change "..." to "<><>"
   S1 = stringreplace(S1, _L("..."), _L("<><>"), rfReplaceAll);

   static const tchar *chars2[]={_L("-"),_L("."),_L("!"),_L("?"),_L(":")};

   //Capital letters after ". " "? " "! " "- "
   for (unsigned int i=0;i<countof(chars2);i++)
    {
     do
      {
       W1 = S1.find(chars2[i]+ffstring(_L(" ")));
       if (W1 > 1) //take Pos from 2 and higher ("- " elsewhere)
        if (W1 != S1.size())
         if (((i == 1) /*'.'*/ && (S1[W1 - 1]>='0' && S1[W1-1]<='9')) || //prevents: 28. srpna -> 28. Srpna
             ((i == 0) /*'-'*/ && !(S1[W1 - 2]=='.' || S1[W1 - 2]=='!' || S1[W1 - 2]=='?')))  //UpperCase only after .!?
          S1 = S1.copy(1, W1 - 1) + _L("()()") + S1.copy(W1 + 1, S1.size() - W1);
         else
          {
           tchar up[2];
           up[0]=(tchar)toupper(S1[W1 + 2]);up[1]='\0';
           S1 = S1.copy(1, W1 - 1) + _L("()() ") + passtring<tchar>(up) + S1.copy(W1 + 3, S1.size() - (W1 + 2));
          }
        else
         S1 = S1.copy(1, W1 - 1) + _L("()()") + S1.copy(W1 + 1, S1.size() - W1);
      } while (W1 > 1);
     S1 = stringreplace(S1, _L("()()"), chars2[i], rfReplaceAll);
    }

   //Change "<><>" back to "..."
   S1 = stringreplace(S1, _L("<><>"), _L("..."), rfReplaceAll);

   //change the 1st letter case according to previous subtitle line
   if (EndOfPrevSentence && (S1[1] != '.')) //...bla
    {
     int J = 1;
     //while ((S1[J] == '<') && (S1[J + 1]=='b' || S1[J + 1]=='i' || S1[J + 1]=='u')) && //styles
     //       (S1[J + 2] == '>'))
     // j+=, 3);
     if (S1[J] != '.') //<i>...bla
      while (J <= S1.size())
       {
        //Alex: (battle cries) --> (Battle cries)
        //It's not expected behavior. It'll be much better to stay words in "()" intact.
        if (!(S1[J]=='[' || S1[J]=='"' || S1[J]=='\'' || S1[J]=='#' || S1[J]=='-' || S1[J]==' '))
         {
          S1[J] = (tchar)toupper(S1[J]);
          break;
         }
        J++;
       }
     }
   //prepare for next line
   passtring<tchar> Sstrip = S1; //styles
   unsigned int J = Sstrip.size();
   if (J==0) //in case of empty string (only styles)
    EndOfPrevSentence = false;
   else
    {
     while ((Sstrip[J]=='\'' || Sstrip[J]=='"' || Sstrip[J]==' ') && (J > 1))
      J--;
     EndOfPrevSentence = Sstrip[J]=='.' || Sstrip[J]=='!' || Sstrip[J]=='?' || Sstrip[J]==':' || Sstrip[J]==']' || Sstrip[J]==')';
     if ((Sstrip[J] == '.') && (Sstrip[J - 1] == '.') && (Sstrip[J - 2] == '.'))
      EndOfPrevSentence = false;
    }
  }

 if (cfg.fix&fixNumbers) //1 993 --> 1993 etc.
  for (int I=1;I<=S1.size()-2;I++)
   if ((S1.size() >= I + 2) /*Delete!*/ &&
       (S1[I]=='0' || S1[I]=='1' || S1[I]=='2' || S1[I]=='3' || S1[I]=='4' || S1[I]=='5' || S1[I]=='6' || S1[I]=='7' || S1[I]=='8' || S1[I]=='9' || S1[I]=='/') &&
       (S1[I + 1] == ' '))
    if ((S1[I + 2]=='0' || S1[I + 2]=='1' || S1[I + 2]=='2' || S1[I + 2]=='3' || S1[I + 2]=='4' || S1[I + 2]=='5' || S1[I + 2]=='6' || S1[I + 2]=='7' || S1[I + 2]=='8' || S1[I + 2]=='9' || S1[I + 2]==',' || S1[I + 2]=='.' || S1[I + 2]==':' || S1[I + 2]=='/') ||
        //5 -1-5
        ((S1[I + 2] == '-') &&
         (S1.size() >= I + 3) /*Delete!*/ &&
         (S1[I + 3]=='0' || S1[I + 3]=='1' || S1[I + 3]=='2' || S1[I + 3]=='3' || S1[I + 3]=='4' || S1[I + 3]=='5' || S1[I + 3]=='6' || S1[I + 3]=='7' || S1[I + 3]=='8' || S1[I + 3]=='9')))
     S1.erase(I + 1, 1);

 if (cfg.fix&fixHearingImpaired)
  for (int I=1;I<=S1.size();I++)
   {
    if (inHearing)
     {
      W1=S1.find(']',I);
      if (W1>0)
       {
        S1.erase(std::max(1,I-1),std::max(W1 - std::max(1,I-1) + 1, 0));
        if (S1 == passtring<tchar>(_L("- ")) || S1 == passtring<tchar>(_L("-")))
         S1 = _L("");
       }
      else
       {
        S1.erase(std::max(1,I-1),S1.size());
        break;
       }
      inHearing=false;
      I=W1+1;
     }
    else if (S1[I]=='[')
     inHearing=true;
   }
 bool useFixed=strcmp(text.c_str(),S1.c_str())!=0;
 if (useFixed)
  fixed=S1.c_str();
 return useFixed;
}

//============================ TsubtitleFormat =============================
template<class tchar> DwString<tchar> TsubtitleFormat::getAttribute(const tchar *start,const tchar *end,const tchar *attrname)
{
 if (const tchar *attr=strnistr(start,end-start+1,attrname))
  if (const tchar *eq=strnchr(attr,end-attr+1,'='))
   {
    eq++;
    bool in=false;
    for (int i=0;i<end-eq+1;i++)
     if (eq[i]=='"')
      in=!in;
     else if (!in && (eq[i]==' ' || eq[i]=='>' || eq[i]=='\0'))
      return stringreplace(DwString<tchar>(eq,i).Trim(),_L("\""),_L(""),rfReplaceAll);
   }
 return DwString<tchar>();
}

template<class tchar> void TsubtitleFormat::processHTMLTags(Twords &words, const tchar* &l, const tchar* &l1, const tchar* &l2)
{
if (_strnicmp(l2,_L("<i>"),3)==0) {words.add(l,l1,l2,props,3);props.italic=true;}
  else if (_strnicmp(l2,_L("</i>"),4)==0) {words.add(l,l1,l2,props,4);props.italic=false;}
  else if (_strnicmp(l2,_L("<u>"),3)==0) {words.add(l,l1,l2,props,3);props.underline=true;}
  else if (_strnicmp(l2,_L("</u>"),4)==0) {words.add(l,l1,l2,props,4);props.underline=false;}
  else if (_strnicmp(l2,_L("<b>"),3)==0) {words.add(l,l1,l2,props,3);props.bold=true;}
  else if (_strnicmp(l2,_L("</b>"),4)==0) {words.add(l,l1,l2,props,4);props.bold=false;}
  else if (_strnicmp(l2,_L("<font "),6)==0)
   {
    const tchar *end=strchr(l2,'>');
    if (end)
     {
      const tchar *start=l2+6;
      DwString<tchar> face=getAttribute(start,end,_L("face"));
      DwString<tchar> color=getAttribute(start,end,_L("color")).ConvertToLowerCase();
      DwString<tchar> size=getAttribute(start,end,_L("size"));
      words.add(l,l1,l2,props,end-l2+1);
      if (!face.empty()) ff_strncpy(props.fontname, (const char_t *)text<char_t>(face.c_str()),countof(props.fontname));
      if (!color.empty() && ((color[0]=='#' && tchar_traits<tchar>::sscanf()(color.c_str()+1,_L("%x"),&props.color)) || (htmlcolors->getColor(color,&props.color,0xffffff),true)))
       {
        std::swap(((uint8_t*)&props.color)[0],((uint8_t*)&props.color)[2]);
        props.isColor=true;
       }
      if (!size.empty())
       {
        tchar *ll;
        int s=strtol(size.c_str(),&ll,10);
        if (*ll=='\0') props.size=s;
       }
     }
   }
  else if (_strnicmp(l2,_L("</font>"),7)==0)
   {
    words.add(l,l1,l2,props,7);props.isColor=false;props.size=0;props.fontname[0]='\0';
   }
  else
   l2++;
}

template<class tchar> TsubtitleFormat::Twords TsubtitleFormat::processHTML(const TsubtitleLine<tchar> &line)
{
 Twords words;
 if (line.empty()) return words;
 const tchar *l=line[0];
 const tchar *l1=l,*l2=l;
 while (*l2)
   processHTMLTags(words, l, l1, l2);
 
 words.add(l,l1,l2,props,0);
 return words;
}

template<class tchar> void TsubtitleFormat::Tssa<tchar>::fontName(const tchar *start,const tchar *end)
{
 if (start!=end)
  {
   memset(props.fontname,0,sizeof(props.fontname));
   text<char_t>(start, int(end-start), (char_t*)props.fontname, countof(props.fontname)-1);
  }
 else
  ff_strncpy(props.fontname, defprops.fontname, countof(props.fontname));
}
template<class tchar> template<int TSubtitleProps::*offset,int min,int max> void TsubtitleFormat::Tssa<tchar>::intProp(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 tchar *bufend;
 int enc=strtol(buf,&bufend,10);
 if (buf!=bufend && *bufend=='\0' && isIn(enc,min,max))
  props.*offset=enc;
 else
  props.*offset=defprops.*offset;
}
template<class tchar> template<int TSubtitleProps::*offset,int min,int max> void TsubtitleFormat::Tssa<tchar>::intPropAn(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 tchar *bufend;
 int enc=strtol(buf,&bufend,10);
 if (buf!=bufend && *bufend=='\0' && isIn(enc,min,max))
  props.*offset=TSubtitleProps::alignASS2SSA(enc);
 else
  props.*offset=defprops.*offset;
}
template<class tchar> template<double TSubtitleProps::*offset,int min,int max> void TsubtitleFormat::Tssa<tchar>::doubleProp(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 tchar *bufend;
 double enc=strtod(buf,&bufend);
 if (buf!=bufend && *bufend=='\0' && isIn(enc,(double)min,(double)max))
  props.*offset=enc;
 else
  props.*offset=defprops.*offset;
}
template<class tchar> template<int TSubtitleProps::*offset1,int TSubtitleProps::*offset2,int min,int max> void TsubtitleFormat::Tssa<tchar>::pos(const tchar *start,const tchar *end)
{
 if (intProp2<offset1,offset2,min,max>(start,end))
  props.isPos=true;
}
template<class tchar> template<int TSubtitleProps::*offset1,int TSubtitleProps::*offset2,int min,int max> void TsubtitleFormat::Tssa<tchar>::fad(const tchar *start,const tchar *end)
{
 if (intProp2<offset1,offset2,min,max>(start,end))
  {
   props.isFad=true;
   props.fadeA1=0;
   props.fadeA2=255;
   props.fadeA3=0;
   props.fadeT1=props.tStart;
   props.fadeT2=props.fadeT1 + (props.tmpFadT1 * 10000);
   props.fadeT3=props.tStop - (props.tmpFadT2 * 10000);
   props.fadeT4=props.tStop;
  }
}

template<class tchar> void TsubtitleFormat::Tssa<tchar>::karaoke_kf(const tchar *start,const tchar *end)
{
 intProp<&TSubtitleProps::tmpFadT1, 0, INT_MAX>(start, end);
 props.karaokeDuration = (REFERENCE_TIME)props.tmpFadT1 * 100000;
 props.karaokeMode = TSubtitleProps::KARAOKE_kf;
 props.karaokeNewWord = true;
}
template<class tchar> void TsubtitleFormat::Tssa<tchar>::karaoke_ko(const tchar *start,const tchar *end)
{
 intProp<&TSubtitleProps::tmpFadT1, 0, INT_MAX>(start, end);
 props.karaokeDuration = (REFERENCE_TIME)props.tmpFadT1 * 100000;
 props.karaokeMode = TSubtitleProps::KARAOKE_ko;
 props.karaokeNewWord = true;
}
template<class tchar> void TsubtitleFormat::Tssa<tchar>::karaoke_k(const tchar *start,const tchar *end)
{
 intProp<&TSubtitleProps::tmpFadT1, 0, INT_MAX>(start, end);
 props.karaokeDuration = (REFERENCE_TIME)props.tmpFadT1 * 100000;
 props.karaokeMode = TSubtitleProps::KARAOKE_k;
 props.karaokeNewWord = true;
}

template<class tchar> void TsubtitleFormat::Tssa<tchar>::fade(const tchar *start,const tchar *end)
{
 // \fade(<a1>, <a2>, <a3>, <t1>, <t2>, <t3>, <t4>)
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 const tchar *bufstart=strchr(buf,'(');
 if(!bufstart) return;
 bufstart++;
 tchar *bufend;
 int val;

#define FADE2INT(v)                                        \
 val=strtol(bufstart,&bufend,10);                          \
 if (bufstart!=bufend) props.v=val;                        \
 if (*bufend=='\0') return;                                \
                                                           \
 bufstart=strchr(bufend,',');                              \
 if(!bufstart) return;                                     \
 bufstart++;

 FADE2INT(fadeA1);
 FADE2INT(fadeA2);
 FADE2INT(fadeA3);
 FADE2INT(fadeT1);
 FADE2INT(fadeT2);
 FADE2INT(fadeT3);
 val=strtol(bufstart,&bufend,10);
 if (bufstart!=bufend)
  props.fadeT4=val;
 else
  return;

 if (props.fadeA1 <  0)  props.fadeA1=0;
 if (props.fadeA1 > 255) props.fadeA1=255;
 if (props.fadeA2 <  0)  props.fadeA2=0;
 if (props.fadeA2 > 255) props.fadeA2=255;
 if (props.fadeA3 <  0)  props.fadeA3=0;
 if (props.fadeA3 > 255) props.fadeA3=255;
 props.fadeA1 = 256 - props.fadeA1;
 props.fadeA2 = 256 - props.fadeA2;
 props.fadeA3 = 256 - props.fadeA3;

 props.fadeT1 = props.tStart + props.fadeT1 * 10000;
 props.fadeT2 = props.tStart + props.fadeT2 * 10000;
 props.fadeT3 = props.tStart + props.fadeT3 * 10000;
 props.fadeT4 = props.tStart + props.fadeT4 * 10000;
 props.isFad = true;
#undef FADE2INT
}
template<class tchar> template<int TSubtitleProps::*offset1,int TSubtitleProps::*offset2,int min,int max> bool TsubtitleFormat::Tssa<tchar>::intProp2(const tchar *start,const tchar *end)
{
// (x,y) is expected.
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 const tchar *bufstart=strchr(buf,'(');
 if(!bufstart) return false;
 bufstart++;
 tchar *bufend;
 int val=strtol(bufstart,&bufend,10);
 if (bufstart!=bufend && isIn(val,min,max)) props.*offset1=val;
 if (*bufend=='\0') return false;

 bufstart=strchr(bufend,',');
 if(!bufstart) return false;
 bufstart++;
 val=strtol(bufstart,&bufend,10);
 if (bufstart!=bufend && isIn(val,min,max)) props.*offset2=val;
 return true;
}
template<class tchar> template<COLORREF TSubtitleProps::*offset> void TsubtitleFormat::Tssa<tchar>::color(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 int radix;
 if (strnicmp(buf,_L("&h"),2)==0)
  {
   radix=16;
   buf+=2;
  }
 else
  radix=10;
 tchar *endbuf;
 int c=strtol(buf,&endbuf,radix);
 if (*endbuf=='&')
  {
   props.*offset=c;
   props.isColor=true;
  }
 else
  {
   props.*offset=defprops.*offset;
   props.isColor=defprops.isColor;
  }
}
template<class tchar> template<int TSubtitleProps::*offset> void TsubtitleFormat::Tssa<tchar>::alpha(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 int radix;
 if (strnicmp(buf,_L("&h"),2)==0)
  {
   radix=16;
   buf+=2;
  }
 else
  radix=10;
 tchar *endbuf;
 int a=strtol(buf,&endbuf,radix);
 if (*endbuf=='&')
  {
   props.*offset=256-a;
   props.isColor=true;
  }
 else
  {
   props.*offset=defprops.*offset;
   props.isColor=defprops.isColor;
  }
}
template<class tchar> void TsubtitleFormat::Tssa<tchar>::alphaAll(const tchar *start,const tchar *end)
{
 tchar *buf=(tchar*)_alloca((end-start+1)*sizeof(tchar));memset(buf,0,(end-start+1)*sizeof(tchar));
 memcpy(buf,start,(end-start)*sizeof(tchar));
 int radix;
 if (strnicmp(buf,_L("&h"),2)==0)
  {
   radix=16;
   buf+=2;
  }
 else
  radix=10;
 tchar *endbuf;
 int a=strtol(buf,&endbuf,radix);
 if (*endbuf=='&')
  {
   props.colorA=256-a;
   props.OutlineColourA=256-a;
   props.ShadowColourA=256-a;
   props.isColor=true;
  }
 else
  {
   props.colorA=defprops.colorA;
   props.OutlineColourA=defprops.OutlineColourA;
   props.ShadowColourA=defprops.ShadowColourA;
   props.isColor=defprops.isColor;
  }
}

template<class tchar> template<bool TSubtitleProps::*offset> void TsubtitleFormat::Tssa<tchar>::boolProp(const tchar *start,const tchar *end)
{
 if (start!=end && start[0]=='1')
  props.*offset=true;
 else if (start!=end && start[0]=='0')
  props.*offset=false;
 else
  props.*offset=defprops.*offset;
}

template<class tchar> void TsubtitleFormat::Tssa<tchar>::reset(const tchar *start,const tchar *end)
{
 props=defprops;
}

// case sensitive version
template<class tchar> bool TsubtitleFormat::Tssa<tchar>::processTokenC(const tchar* &l2,const tchar *tok,TssaAction action)
{
 size_t toklen=strlen(tok);
 if (strncmp(l2,tok,toklen)==0)
  {
   const tchar *end1=strchr(l2+2,'\\');
   const tchar *end2=strchr(l2,'}');
   const tchar *end=(end1 && end1<end2)?end1:end2;
   if (end)
    {
     const tchar *start=l2+toklen;
     if (action)
      (this->*action)(start,end);
     l2=(end1 && end1<end2)?end1:end2+1;
    }
   return true;
  }
 else
  return false;
}

template<class tchar> bool TsubtitleFormat::Tssa<tchar>::processToken(const tchar* &l2,const tchar *tok,TssaAction action)
{
 size_t toklen=strlen(tok);
 if (_strnicmp(l2,tok,toklen)==0)
  {
   const tchar *end1=strchr(l2+2,'\\');
   const tchar *end2=strchr(l2,'}');
   const tchar *end=(end1 && end1<end2)?end1:end2;
   if (end)
    {
     const tchar *start=l2+toklen;
     if (action)
      (this->*action)(start,end);
     l2=(end1 && end1<end2)?end1:end2+1;
    }
   return true;
  }
 else
  return false;
}

template<class tchar> void TsubtitleFormat::Tssa<tchar>::processTokens(const tchar *l,const tchar* &l1,const tchar* &l2,const tchar *end)
{
 const tchar *l3=l2+1;
 words.add(l,l1,l2,props,end-l2+1);
 while (l3<end)
  {
   if (
       !processToken(l3,_L("\\1a"),&Tssa<tchar>::template alpha<&TSubtitleProps::colorA>) &&
       !processToken(l3,_L("\\2a"),&Tssa<tchar>::template alpha<&TSubtitleProps::SecondaryColourA>) &&
       !processToken(l3,_L("\\3a"),&Tssa<tchar>::template alpha<&TSubtitleProps::OutlineColourA>) &&
       !processToken(l3,_L("\\4a"),&Tssa<tchar>::template alpha<&TSubtitleProps::ShadowColourA>) &&
       !processToken(l3,_L("\\1c"),&Tssa<tchar>::template color<&TSubtitleProps::color>) && 
       !processToken(l3,_L("\\2c"),&Tssa<tchar>::template color<&TSubtitleProps::SecondaryColour>) &&
       !processToken(l3,_L("\\3c"),&Tssa<tchar>::template color<&TSubtitleProps::OutlineColour>) &&
       !processToken(l3,_L("\\4c"),&Tssa<tchar>::template color<&TSubtitleProps::ShadowColour>) &&
       !processToken(l3,_L("\\alpha"),&Tssa<tchar>::alphaAll) &&
       !processToken(l3,_L("\\an"),&Tssa<tchar>::template intPropAn<&TSubtitleProps::alignment,1,9>) &&
       !processToken(l3,_L("\\a"),&Tssa<tchar>::template intProp<&TSubtitleProps::alignment,1,11>) &&
       !processToken(l3,_L("\\bord"),&Tssa<tchar>::template doubleProp<&TSubtitleProps::outlineWidth,0,100>) &&
       !processToken(l3,_L("\\be"),&Tssa<tchar>::template boolProp<&TSubtitleProps::blur>) &&
       !processToken(l3,_L("\\b"),&Tssa<tchar>::template intProp<&TSubtitleProps::bold,0,1>) &&
       !processToken(l3,_L("\\clip"),NULL) &&
       !processToken(l3,_L("\\c"),&Tssa<tchar>::template color<&TSubtitleProps::color>) &&
       !processToken(l3,_L("\\fn"),&Tssa<tchar>::fontName) &&
       !processToken(l3,_L("\\fscx"),&Tssa<tchar>::template intProp<&TSubtitleProps::scaleX,1,1000>) &&
       !processToken(l3,_L("\\fscy"),&Tssa<tchar>::template intProp<&TSubtitleProps::scaleY,1,1000>) &&
       !processToken(l3,_L("\\fsp"),&Tssa<tchar>::template doubleProp<&TSubtitleProps::spacing,INT_MIN+1,INT_MAX>) &&
       !processToken(l3,_L("\\fs"),&Tssa<tchar>::template intProp<&TSubtitleProps::size,1,INT_MAX>) &&
       !processToken(l3,_L("\\fe"),&Tssa<tchar>::template intProp<&TSubtitleProps::encoding,0,255>) &&
       !processToken(l3,_L("\\i"),&Tssa<tchar>::template boolProp<&TSubtitleProps::italic>) &&
       !processToken(l3,_L("\\fade"),&Tssa<tchar>::fade) &&
       !processToken(l3,_L("\\fad"),&Tssa<tchar>::template fad<&TSubtitleProps::tmpFadT1,&TSubtitleProps::tmpFadT2,0,INT_MAX>) &&
       !processToken(l3,_L("\\pos"),&Tssa<tchar>::template pos<&TSubtitleProps::posx,&TSubtitleProps::posy,0,4096>) &&
       !processToken(l3,_L("\\q"),&Tssa<tchar>::template intProp<&TSubtitleProps::wrapStyle,0,3>) &&
       !processToken(l3,_L("\\r"),&Tssa<tchar>::reset) &&
       !processToken(l3,_L("\\shad"),&Tssa<tchar>::template doubleProp<&TSubtitleProps::shadowDepth,0,30>) &&
       !processToken(l3,_L("\\s"),&Tssa<tchar>::template boolProp<&TSubtitleProps::strikeout>) &&
       !processToken(l3,_L("\\u"),&Tssa<tchar>::template boolProp<&TSubtitleProps::underline>) &&
       !processToken(l3,_L("\\kf"),&Tssa<tchar>::karaoke_kf) &&
       !processToken(l3,_L("\\ko"),&Tssa<tchar>::karaoke_ko) &&
       !processTokenC(l3,_L("\\K"),&Tssa<tchar>::karaoke_kf) &&
       !processTokenC(l3,_L("\\k"),&Tssa<tchar>::karaoke_k)
      )
    l3++;
  }
}

template<class tchar> TsubtitleFormat::Twords TsubtitleFormat::processSSA(const TsubtitleLine<tchar> &line, int sfmt, TsubtitleTextBase<tchar> &parent)
{
 Twords words;
 if (line.empty()) return words;
 const tchar *l=line[0];
 props=parent.defProps;
 const tchar *l1=l,*l2=l;
 Tssa<tchar> ssa(props,parent.defProps,words);
 while (*l2)
  {
   if (l2[0]=='{' /*&& l2[1]=='\\'*/)
   {
    if (const tchar *end=strchr(l2+1,'}'))
     {
      ssa.processTokens(l,l1 ,l2,end);
      l2=end+1;
      continue;
     }
	l2++;
   }
   // Process HTML tags in SSA subs when extended tags option is checked
   else if (parent.defProps.extendedTags) // Add HTML support within SSA
	   processHTMLTags(words,l,l1,l2);
   else
	   l2++;
  }
 

 words.add(l,l1,l2,props,0);
 parent.defProps=props;
 return words;
}

template<class tchar> void TsubtitleFormat::processMicroDVD(TsubtitleTextBase<tchar> &parent,typename std::vector< TsubtitleLine<tchar> >::iterator it)
{
 if (it->empty()) return;
 const tchar *line0=(*it)[0],*line=line0;
 while (*line)
  if (line[0]=='}' || line[0]==' ') line++;
  else if (_strnicmp(line,_L("{y:"),3)==0)
   {
    const tchar *end=strchr(line+3,'}');
    if (end==NULL) break;
    bool all=!!tchar_traits<tchar>::isupper(line[1]);
    if (std::find_if(line+3,end,Tncasecmp<tchar,'i'>())!=end) parent.propagateProps(all?parent.begin():it,&TSubtitleProps::italic   ,true,all?parent.end():it+1);
    if (std::find_if(line+3,end,Tncasecmp<tchar,'b'>())!=end) parent.propagateProps(all?parent.begin():it,&TSubtitleProps::bold     ,1,all?parent.end():it+1);
    if (std::find_if(line+3,end,Tncasecmp<tchar,'u'>())!=end) parent.propagateProps(all?parent.begin():it,&TSubtitleProps::underline,true,all?parent.end():it+1);
    if (std::find_if(line+3,end,Tncasecmp<tchar,'s'>())!=end) parent.propagateProps(all?parent.begin():it,&TSubtitleProps::strikeout,true,all?parent.end():it+1);
    line=end+1;
   }
  else if (_strnicmp(line,_L("{s:"),3)==0)
   {
    int size;
    if (tchar_traits<tchar>::sscanf()(line,_L("{s:%i}"),&size) || tchar_traits<tchar>::sscanf()(line,_L("{S:%i}"),&size))
     {
      parent.propagateProps(tchar_traits<tchar>::isupper(line[1])?parent.begin():it,&TSubtitleProps::size,size,tchar_traits<tchar>::isupper(line[1])?parent.end():it+1);
      const tchar *r=strchr(line,'}');
      if (r)
       line=r+1;
     }
   }
  else if (_strnicmp(line,_L("{c:$"),4)==0)
   {
    COLORREF color;
    if (tchar_traits<tchar>::sscanf()(line,_L("{c:$%x}"),&color) || tchar_traits<tchar>::sscanf()(line,_L("{C:$%x}"),&color))
     {
      parent.propagateProps(tchar_traits<tchar>::isupper(line[1])?parent.begin():it,&TSubtitleProps::color,color,tchar_traits<tchar>::isupper(line[1])?parent.end():it+1);
      const tchar *r=strchr(line,'}');
      if (r)
       {
        parent.propagateProps(tchar_traits<tchar>::isupper(line[1])?parent.begin():it,&TSubtitleProps::isColor,true,tchar_traits<tchar>::isupper(line[1])?parent.end():it+1);
        line=r+1;
       }
     }
   }
  else
   break;
 (*it)[0].eraseLeft(line-line0);
}

template<class tchar> void TsubtitleFormat::processMPL2(TsubtitleLine<tchar> &line)
{
 if (line.empty() || !line[0]) return;
 if (line[0][0]=='/')
  {
   for (typename TsubtitleLine<tchar>::iterator w=line.begin();w!=line.end();w++)
    w->props.italic=true;
   line[0].eraseLeft(1);
  }
}

//================================= TsubtitleLine ==================================
template<class tchar> size_t TsubtitleLine<tchar>::strlen(void) const
{
 size_t len=0;
 for (typename Tbase::const_iterator w=this->begin();w!=this->end();w++)
  len+=::strlen(*w);
 return len;
}
template<class tchar> void TsubtitleLine<tchar>::applyWords(const TsubtitleFormat::Twords &words)
{
 bool karaokeNewWord = false;
 for (TsubtitleFormat::Twords::const_iterator w=words.begin();w!=words.end();w++)
  {
   karaokeNewWord |= w->props.karaokeNewWord;
   this->props=w->props;
   if (w->i1==w->i2)
    continue;
   if (w->i1==0 && w->i2==this->front().size())
    {
     this->front().props=w->props;
     return;
    }
   TsubtitleWord word(this->front()+w->i1,w->i2-w->i1);
   word.props=w->props;
   word.props.karaokeNewWord = karaokeNewWord;
   this->push_back(word);
   karaokeNewWord = false;
  }
 if (!this->empty())
  this->erase(this->begin());
}
template<class tchar> void TsubtitleLine<tchar>::format(TsubtitleFormat &format,int sfmt,TsubtitleTextBase<tchar> &parent)
{
 // Use SSA parser for SRT subs when extended tags option is checked
 // This option will be removed (and SSA parser applied to SUBVIEWER)
 // when the garble issue with Shift JIS (ANSI/DBCS) subs will be resovled
 if (sfmt==Tsubreader::SUB_SSA || (sfmt==Tsubreader::SUB_SUBVIEWER && parent.defProps.extendedTags))
  applyWords(format.processSSA(*this, sfmt, parent));
 else
  applyWords(format.processHTML(*this));
}template<class tchar> void TsubtitleLine<tchar>::fix(TtextFix<tchar> &fix)
{
 for (typename Tbase::iterator w=this->begin();w!=this->end();w++)
  w->fix(fix);
}

template size_t TsubtitleLine<char>::strlen(void) const;
template size_t TsubtitleLine<wchar_t>::strlen(void) const;

//================================= TsubtitleTextBase ==================================
template<class tchar> void TsubtitleTextBase<tchar>::format(TsubtitleFormat &format)
{
 int sfmt=subformat&Tsubreader::SUB_FORMATMASK;
 for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
  l->format(format,sfmt,*this);
 for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
  format.processMicroDVD(*this,l);
 if (sfmt==Tsubreader::SUB_MPL2)
  for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
   format.processMPL2(*l);
}

template<class tchar> void TsubtitleTextBase<tchar>::prepareKaraoke(void)
{
 int sfmt=subformat&Tsubreader::SUB_FORMATMASK;
 if (sfmt != Tsubreader::SUB_SSA)
  return;

 TsubtitleTextBase<tchar> temp(subformat, defProps);
 TsubtitleLine tempLine;
 int wrapStyle = 0;
 for (typename Tbase::iterator l = this->begin() ; l != this->end() ; l++)
  {
   if (l->props.wrapStyle == 2 || l->lineBreakReason == 2)
    {
     temp.push_back(tempLine);
     tempLine.clear();
    }
   else if (!tempLine.empty())
    {
     tempLine.back().addTailSpace();
    }

   tempLine.props = l->props;
   for (typename TsubtitleLine::iterator w = l->begin() ; w != l->end() ; w++)
    {
     wrapStyle = w->props.wrapStyle;
     tempLine.push_back(*w);
    }
  }
 if (!tempLine.empty())
  temp.push_back(tempLine);

 this->clear();
 this->insert(this->end(), temp.begin(),temp.end());

 REFERENCE_TIME karaokeStart = REFTIME_INVALID;
 REFERENCE_TIME karaokeDuration = 0;
 for (typename Tbase::iterator l = this->begin() ; l != this->end() ; l++)
  {
   if (karaokeStart != REFTIME_INVALID)
    karaokeStart += karaokeDuration;

   karaokeDuration = 0;
   for (typename TsubtitleLine::iterator w = l->begin() ; w != l->end() ; w++)
    {
     if (karaokeStart == REFTIME_INVALID)
      karaokeStart = w->props.karaokeStart;
     else
      w->props.karaokeStart = karaokeStart;
     
     if (w->props.karaokeNewWord)
      {
       karaokeStart += karaokeDuration;
       karaokeDuration = w->props.karaokeDuration;
      }
     w->props.karaokeDuration = karaokeDuration;
    }
  }
}

template<class tchar> void TsubtitleTextBase<tchar>::fix(TtextFix<tchar> &fix)
{
 for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
  l->fix(fix);
 if (stop == REFTIME_INVALID)
  {
   size_t len = 0;
   for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
    len += l->strlen();
   stop = start + len * 900000;
  }
}

template<class tchar> void TsubtitleTextBase<tchar>::print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const
{
 prefs.subformat=subformat;
 f.print(this,forceChange,prefs);
}
template<class tchar> Tsubtitle* TsubtitleTextBase<tchar>::copy(void)
{
 TsubtitleTextBase<tchar> *s2=new TsubtitleTextBase<tchar>(subformat);
 for (typename Tbase::iterator l=this->begin();l!=this->end();l++)
  s2->push_back(*l);
 return s2;
}

template class TtextFix<char>;
template struct TsubtitleTextBase<char>;

template class TtextFix<wchar_t>;
template struct TsubtitleTextBase<wchar_t>;
