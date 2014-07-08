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
#include "Cnoise.h"

void TnoisePage::init(void)
{
 tbrSetRange(IDC_TBR_NOISESTRENGTH,0,255,16);
 tbrSetRange(IDC_TBR_NOISESTRENGTH_CHROMA,0,127,16);
 tbrSetRange(IDC_TBR_FLICKER_A,0,100);
 tbrSetRange(IDC_TBR_FLICKER_F,1,100);
 tbrSetRange(IDC_TBR_SHAKE_A,0,15);
 tbrSetRange(IDC_TBR_SHAKE_F,1,100);
 tbrSetRange(IDC_TBR_LINES_A,1,100);
 tbrSetRange(IDC_TBR_LINES_F,0,100);
 tbrSetRange(IDC_TBR_LINES_TRANSPARENCY,1,255);
 tbrSetRange(IDC_TBR_LINES_C,0,255);
 tbrSetRange(IDC_TBR_SCRATCHES_A,1,100);
 tbrSetRange(IDC_TBR_SCRATCHES_F,0,100);
 tbrSetRange(IDC_TBR_SCRATCHES_TRANSPARENCY,1,255);
 tbrSetRange(IDC_TBR_SCRATCHES_C,0,255);
}

void TnoisePage::cfg2dlg(void)
{
 noise2dlg();
 flicker2dlg();
 shake2dlg();
 lines2dlg();
 scratches2dlg();
}

void TnoisePage::noise2dlg(void)
{
 int method=cfgGet(IDFF_noiseMethod);
 setCheck(IDC_RBT_NOISE_MOJ ,method==0);
 setCheck(IDC_RBT_NOISE_AVIH,method==1);
 setCheck(IDC_RBT_NOISE_MP  ,method==2);
 setCheck(IDC_CHB_NOISE_UNIFORM,cfgGet(IDFF_uniformNoise ));
 enable(method!=0,IDC_CHB_NOISE_PATTERN);setCheck(IDC_CHB_NOISE_PATTERN,cfgGet(IDFF_noisePattern ));
 enable(method==2,IDC_CHB_NOISE_AVG    );setCheck(IDC_CHB_NOISE_AVG    ,cfgGet(IDFF_noiseAveraged));
 tbrSet(IDC_TBR_NOISESTRENGTH,cfgGet(IDFF_noiseStrength),IDC_LBL_NOISESTRENGTH);
 tbrSet(IDC_TBR_NOISESTRENGTH_CHROMA,cfgGet(IDFF_noiseStrengthChroma),IDC_LBL_NOISESTRENGTH_CHROMA);
}
void TnoisePage::flicker2dlg(void)
{
 int a=cfgGet(IDFF_noiseFlickerA),f=cfgGet(IDFF_noiseFlickerF);
 tbrSet(IDC_TBR_FLICKER_A,a);tbrSet(IDC_TBR_FLICKER_F,f);
 setText(IDC_LBL_FLICKER,_l("%s %s:%i, %s:%i"),_(IDC_LBL_FLICKER),_(IDC_LBL_FLICKER,_l("amplitude")),a,_(IDC_LBL_FLICKER,_l("frequency")),f);
}
void TnoisePage::shake2dlg(void)
{
 int a=cfgGet(IDFF_noiseShakeA),f=cfgGet(IDFF_noiseShakeF);
 tbrSet(IDC_TBR_SHAKE_A,a);tbrSet(IDC_TBR_SHAKE_F,f);
 setText(IDC_LBL_SHAKE,_l("%s %s:%i, %s:%i"),_(IDC_LBL_SHAKE),_(IDC_LBL_SHAKE,_l("amplitude")),a,_(IDC_LBL_SHAKE,_l("frequency")),f);
}
void TnoisePage::lines2dlg(void)
{
 int a=cfgGet(IDFF_noiseLinesA),f=cfgGet(IDFF_noiseLinesF),t=cfgGet(IDFF_noiseLinesTransparency),c=cfgGet(IDFF_noiseLinesC);
 tbrSet(IDC_TBR_LINES_A,a);tbrSet(IDC_TBR_LINES_F,f);tbrSet(IDC_TBR_LINES_TRANSPARENCY,t);tbrSet(IDC_TBR_LINES_C,c);
 setText(IDC_LBL_LINES,_l("%s %s:%i, %s:%i, %s:%i, %s:%s"),_(IDC_LBL_LINES),_(IDC_LBL_LINES,_l("frequency")),f,_(IDC_LBL_LINES,_l("duration")),a,_(IDC_LBL_LINES,_l("opacity")),t,_(IDC_LBL_LINES,_l("color")),nameGray(IDC_LBL_LINES,c));
}
void TnoisePage::scratches2dlg(void)
{
 int a=cfgGet(IDFF_noiseScratchesA),f=cfgGet(IDFF_noiseScratchesF),t=cfgGet(IDFF_noiseScratchesTransparency),c=cfgGet(IDFF_noiseScratchesC);
 tbrSet(IDC_TBR_SCRATCHES_A,a);tbrSet(IDC_TBR_SCRATCHES_F,f);tbrSet(IDC_TBR_SCRATCHES_TRANSPARENCY,t);tbrSet(IDC_TBR_SCRATCHES_C,c);
 setText(IDC_LBL_SCRATCHES,_l("%s %s:%i, %s:%i, %s:%i, %s:%s"),_(IDC_LBL_SCRATCHES),_(IDC_LBL_SCRATCHES,_l("frequency")),f,_(IDC_LBL_SCRATCHES,_l("size")),a,_(IDC_LBL_SCRATCHES,_l("opacity")),t,_(IDC_LBL_SCRATCHES,_l("color")),nameGray(IDC_LBL_SCRATCHES,c));
}
const char_t* TnoisePage::nameGray(int id,int c)
{
 switch (c)
  {
   case 0:
    return _(id,_l("black"));
   case 255:
    return _(id,_l("white"));
   default:
    tsnprintf_s(grayS, countof(grayS), _TRUNCATE, _l("%i%% %s"), 100*c/255, _(id,_l("gray")));
    return grayS;
  }
}

TnoisePage::TnoisePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_NOISE;
 helpURL=_l("noise.html");
 static const TbindCheckbox<TnoisePage> chb[]=
  {
   IDC_CHB_NOISE_UNIFORM,IDFF_uniformNoise,NULL,
   IDC_CHB_NOISE_PATTERN,IDFF_noisePattern,NULL,
   IDC_CHB_NOISE_AVG,IDFF_noiseAveraged,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TnoisePage> htbr[]=
  {
   IDC_TBR_NOISESTRENGTH,IDFF_noiseStrength,&TnoisePage::noise2dlg,
   IDC_TBR_NOISESTRENGTH_CHROMA,IDFF_noiseStrengthChroma,&TnoisePage::noise2dlg,
   IDC_TBR_FLICKER_A,IDFF_noiseFlickerA,&TnoisePage::flicker2dlg,
   IDC_TBR_FLICKER_F,IDFF_noiseFlickerF,&TnoisePage::flicker2dlg,
   IDC_TBR_SHAKE_A,IDFF_noiseShakeA,&TnoisePage::shake2dlg,
   IDC_TBR_SHAKE_F,IDFF_noiseShakeF,&TnoisePage::shake2dlg,
   IDC_TBR_LINES_A,IDFF_noiseLinesA,&TnoisePage::lines2dlg,
   IDC_TBR_LINES_F,IDFF_noiseLinesF,&TnoisePage::lines2dlg,
   IDC_TBR_LINES_TRANSPARENCY,IDFF_noiseLinesTransparency,&TnoisePage::lines2dlg,
   IDC_TBR_LINES_C,IDFF_noiseLinesC,&TnoisePage::lines2dlg,
   IDC_TBR_SCRATCHES_A,IDFF_noiseScratchesA,&TnoisePage::scratches2dlg,
   IDC_TBR_SCRATCHES_F,IDFF_noiseScratchesF,&TnoisePage::scratches2dlg,
   IDC_TBR_SCRATCHES_TRANSPARENCY,IDFF_noiseScratchesTransparency,&TnoisePage::scratches2dlg,
   IDC_TBR_SCRATCHES_C,IDFF_noiseScratchesC,&TnoisePage::scratches2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TnoisePage> rbt[]=
  {
   IDC_RBT_NOISE_MOJ,IDFF_noiseMethod,0,&TnoisePage::cfg2dlg,
   IDC_RBT_NOISE_AVIH,IDFF_noiseMethod,1,&TnoisePage::cfg2dlg,
   IDC_RBT_NOISE_MP,IDFF_noiseMethod,2,&TnoisePage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
}
