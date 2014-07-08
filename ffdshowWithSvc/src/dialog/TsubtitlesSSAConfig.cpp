/*
 * Copyright (c) 2002-2008 Damien Bain-Thouverez
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
#include "TsubtitlesSSAConfig.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"
#include "Ttranslate.h"
#include "TflatButtons.h"

//================================= TsubtitlesSSAConfig ==================================
TsubtitlesSSAConfig::TsubtitlesSSAConfig(IffdshowBase *Ideci,HWND Iparent,int x,int y):TdlgWindow(IDD_SUBTITLES_SSA,Ideci),parent(Iparent),deciD(Ideci)
{
 this->x=x;this->y=y;
 static const TbindCheckbox<TsubtitlesSSAConfig> chb[]=
 {
	 IDC_CHB_SUB_SSA_OVERRIDE_POSITION,IDFF_subSSAOverridePlacement,NULL,
	 IDC_CHB_SUB_SSA_KEEP_TEXT_INSIDE,IDFF_subSSAMaintainInside,NULL,
	 IDC_CHB_SUB_SSA_USE_INPUT_DIMENSIONS,IDFF_subSSAUseMovieDimensions,NULL,
	 IDC_CHB_SUB_SSA_OVERRIDE_SCALING,IDFF_fontOverrideScale,NULL,
	 0,NULL,NULL
 };
 bindCheckboxes(chb);
}
void TsubtitlesSSAConfig::init(void)
{
 translate();
 const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
 if (capt && capt[0]) setWindowText(m_hwnd,capt);
 setCheck(IDC_CHB_SUB_SSA_OVERRIDE_POSITION,deci->getParam2(IDFF_subSSAOverridePlacement) == 1);
 setCheck(IDC_CHB_SUB_SSA_KEEP_TEXT_INSIDE ,deci->getParam2(IDFF_subSSAMaintainInside) == 1);
 setCheck(IDC_CHB_SUB_SSA_USE_INPUT_DIMENSIONS ,deci->getParam2(IDFF_subSSAUseMovieDimensions) == 1);
 setCheck(IDC_CHB_SUB_SSA_OVERRIDE_SCALING ,deci->getParam2(IDFF_fontOverrideScale) == 1);
 addHint(IDC_CHB_SUB_SSA_OVERRIDE_POSITION,_l("When a subtitle has no position defined, then apply the horizontal and vertical positions defined in the subtitles section."));
 addHint(IDC_CHB_SUB_SSA_KEEP_TEXT_INSIDE,_l("If some text comes partially or totally out of the screen, then correct its position so that the whole text is visible."));
 addHint(IDC_CHB_SUB_SSA_USE_INPUT_DIMENSIONS,_l("Use movie dimensions to calculate coordinates instead of ASS resolution references. This information is not always present or accurate in ASS scripts."));
 addHint(IDC_CHB_SUB_SSA_OVERRIDE_SCALING,_l("Override scaling by the H/V scaling defined in the font section."));
}
bool TsubtitlesSSAConfig::show(void)
{
 return !!dialogBox(dialogId,parent);
}

INT_PTR TsubtitlesSSAConfig::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    init();
    return TRUE;
   case WM_DESTROY:
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDCLOSE:
      case IDCANCEL:
       {
        bool wasChange=false;
        EndDialog(m_hwnd,wasChange);
        return TRUE;
       }
     }
    break;
/*
   case WM_NCACTIVATE:
    if (wParam==FALSE)
     {
      EndDialog(m_hwnd,0);
      return TRUE;
     }
    break;*/
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}
