/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "Cfir.h"
#include "firfilter.h"
#include "TfirSettings.h"
#include "IaudioFilterFIR.h"

void TfirPage::init(void)
{
 deciD->queryFilterInterface(IID_IaudioFilterFir,(void**)&filter);
 hg=GetDlgItem(m_hwnd,IDC_BMP_FIR);
 RECT r;GetClientRect(hg,&r);
 gx=r.right-r.left;gy=r.bottom-r.top;stride=(gx/4+1)*4;
 bitsFIR=(unsigned char*)calloc(stride,gy);bitsFFT=(unsigned char*)calloc(stride,gy);
 fir=(unsigned int*)calloc(gx,sizeof(unsigned int));
 memset(&bmpFIR.bmiHeader,0,sizeof(bmpFIR.bmiHeader));
 bmpFIR.bmiHeader.biSize=sizeof(bmpFIR.bmiHeader);
 bmpFIR.bmiHeader.biWidth=stride;
 bmpFIR.bmiHeader.biHeight=-1*int(gy);
 bmpFIR.bmiHeader.biPlanes=1;
 bmpFIR.bmiHeader.biBitCount=8;
 bmpFIR.bmiHeader.biCompression=BI_RGB;
 bmpFIR.bmiHeader.biXPelsPerMeter=100;
 bmpFIR.bmiHeader.biYPelsPerMeter=100;
 memset(bmpFIR.bmiColors,0,256*sizeof(RGBQUAD));                                                  //0 - fir
 bmpFIR.bmiColors[1].rgbRed=255;bmpFIR.bmiColors[1].rgbGreen=255;bmpFIR.bmiColors[1].rgbBlue=255;
 bmpFIR.bmiColors[2].rgbRed=192;bmpFIR.bmiColors[2].rgbGreen=192;bmpFIR.bmiColors[2].rgbBlue=192; //1 - fft
 bmpFFT=bmpFIR;
 isFFT=false;
 if ((filterMode&IDFF_FILTERMODE_PLAYER)==0)
  {
   enable(0,IDC_CHB_FIR_MAXFREQ);
   setCheck(IDC_CHB_FIR_MAXFREQ,1);
  }
}

void TfirPage::cfg2dlg(void)
{
 int taps=cfgGet(IDFF_firTaps);
 SetDlgItemInt(m_hwnd,IDC_ED_FIR_TAPS,taps,FALSE);
 type2dlg();
 window2dlg();
 drawFir();
 setCheck(IDC_CHB_FIR_SHOWFFT,cfgGet(IDFF_showCurrentFFT));
 maxfreq2dlg();
}
void TfirPage::type2dlg(void)
{
 int type=cfgGet(IDFF_firType);
 cbxSetCurSel(IDC_CBX_FIR_TYPE,type);
 SetDlgItemInt(m_hwnd,IDC_ED_FIR_FREQ,cfgGet(IDFF_firFreq),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_FIR_WIDTH,cfgGet(IDFF_firWidth),FALSE);
 static const int idWidth[]={IDC_LBL_FIR_WIDTH,IDC_ED_FIR_WIDTH,0};
 enable(type==TfirSettings::BANDPASS || type==TfirSettings::BANDSTOP,idWidth);
}
void TfirPage::window2dlg(void)
{
 int window=cfgGet(IDFF_firWindow);
 cbxSetCurSel(IDC_CBX_FIR_WINDOW,window);
 static const int idKaiser[]={IDC_LBL_FIR_KAISER_BETA,IDC_ED_FIR_KAISER_BETA,0};
 setText(IDC_ED_FIR_KAISER_BETA,_l("%.4g"),cfgGet(IDFF_firKaiserBeta)/1000.0f);
 enable(window==TfirSettings::WINDOW_KAISER,idKaiser);
}
void TfirPage::maxfreq2dlg(void)
{
 int user=cfgGet(IDFF_firIsUserDisplayMaxFreq) || (filterMode&IDFF_FILTERMODE_PLAYER)==0;
 setCheck(IDC_CHB_FIR_MAXFREQ,user);
 enable(user,IDC_ED_FIR_MAXFREQ);
 SetDlgItemInt(m_hwnd,IDC_ED_FIR_MAXFREQ,getFirFreq(),FALSE);
}

unsigned int TfirPage::getFirFreq(void)
{
 if ((filterMode&IDFF_FILTERMODE_PLAYER)==0 || cfgGet(IDFF_firIsUserDisplayMaxFreq))
  return cfgGet(IDFF_firUserDisplayMaxFreq);
 else
  {
   int freq=filter?filter->getFFTfreq()/2:0;
   return freq?freq:cfgGet(IDFF_firUserDisplayMaxFreq);
  }
}

void TfirPage::drawFir(void)
{
 unsigned int order=cfgGet(IDFF_firTaps);
 TfirFilter::_ftype_t freq[2];
 TfirFilter::_ftype_t allfreq=(TfirFilter::_ftype_t)getFirFreq();
 int type=cfgGet(IDFF_firType),window=cfgGet(IDFF_firWindow);
 if (type==TfirSettings::BANDPASS || type==TfirSettings::BANDSTOP)
  {
   int firFreq=cfgGet(IDFF_firFreq),firWidth=cfgGet(IDFF_firWidth);
   freq[0]=(firFreq-firWidth/2)/allfreq;
   freq[1]=(firFreq+firWidth/2)/allfreq;
  }
 else
  freq[0]=cfgGet(IDFF_firFreq)/allfreq;
 TfirFilter::_ftype_t *a=TfirFilter::design_fir(&order,freq,type,window,cfgGet(IDFF_firKaiserBeta)/1000.0f);
 memset(bitsFIR,1,stride*gy);
 if (!a) return;
 float *g=new float[gx+1];
 //   float theta, s, c, sac, sas;
 float gMax=-1000000.0f,gMin=100000.0f;
 float sc=10.0f/logf(10.0f);
 float t=float(M_PI)/gx;
 for (unsigned int i=0;i<=gx;i++)
  {
   float theta=i*t;
   float sac=0.0f,sas=0.0f;
   for (unsigned int k=0;k<order;k++)
    {
     float c=cosf(k*theta);
     float s=sinf(k*theta);
     sac+=float(c*a[k]);
     sas+=float(s*a[k]);
    }
   g[i]=-sc*logf(sac*sac+sas*sas);
   gMax=std::max(gMax,g[i]);
   gMin=std::min(gMin,g[i]);
  }
 //for (int i=0;i<=gx;i++) g[i]-=gMax;
 //float normFactor=powf(10.0,-0.05*gMax);
 //for (int i=0;i<=order;i++) a[i]*=normFactor;
 for (unsigned int i=0;i<gx;i++)
  bitsFIR[i+stride*(fir[i]=int((gy-1)*(g[i]-gMin)/(gMax-gMin)))]=0;
 delete []g;
 aligned_free(a);
 repaint(hg);
}
void TfirPage::drawFir2(void)
{
 drawFir();
 onFrame();
}
INT_PTR TfirPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (bitsFIR) free(bitsFIR);bitsFIR=NULL;
    if (bitsFFT) free(bitsFFT);bitsFFT=NULL;
    if (fir) free(fir);fir=NULL;
    break;
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BMP_FIR:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        if (isFFT)
         SetDIBitsToDevice(dis->hDC,0,0,gx,gy,0,0,0,gy,bitsFFT,(BITMAPINFO*)&bmpFFT,DIB_RGB_COLORS);
        else
         SetDIBitsToDevice(dis->hDC,0,0,gx,gy,0,0,0,gy,bitsFIR,(BITMAPINFO*)&bmpFIR,DIB_RGB_COLORS);
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

void TfirPage::onFrame(void)
{
 memset(bitsFFT,1,stride*gy);
 unsigned int fftnum=0;
 if (filter && cfgGet(IDFF_showCurrentFFT) && (fftnum=filter->getFFTdataNum())!=0)
  {
   float *fft=(float*)_alloca(fftnum*sizeof(float));
   filter->getFFTdata(fft);
   unsigned int fftnum2=fftnum*getFirFreq()/(filter->getFFTfreq()/2);
   for (unsigned int i=0,ffti=0,fftstep=65536*fftnum2/gx;i<gx && (ffti>>16)<fftnum2;i++,ffti+=fftstep)
    {
     if (ffti>>16<fftnum)
      {
       int yy=limit(int(gy-1)-int(fft[ffti>>16]),0,int(gy-1));
       for (int y=gy-1;y>=yy;y--)
        bitsFFT[i+stride*y]=2;
      }
     if (fir) bitsFFT[i+stride*fir[i]]=0;
    }
   isFFT=true;
   repaint(hg);
  }
 else
  {
   if (isFFT) repaint(hg);
   isFFT=false;
  }
}

void TfirPage::onUserDisplayMaxFreq(void)
{
 maxfreq2dlg();
 drawFir();
 onFrame();
}
void TfirPage::onFirType(void)
{
 drawFir();
 type2dlg();
}
void TfirPage::onFirWindow(void)
{
 drawFir();
 window2dlg();
}

void TfirPage::translate(void)
{
 TconfPageDecAudio::translate();

 cbxTranslate(IDC_CBX_FIR_TYPE,TfirSettings::types);
 cbxTranslate(IDC_CBX_FIR_WINDOW,TfirSettings::windows);
}

TfirPage::TfirPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff),filter(NULL)
{
 resInter=IDC_CHB_FIR;
 bitsFIR=bitsFFT=NULL;fir=NULL;
 static const TbindCheckbox<TfirPage> chb[]=
  {
   IDC_CHB_FIR_SHOWFFT,IDFF_showCurrentFFT,NULL,
   IDC_CHB_FIR_MAXFREQ,IDFF_firIsUserDisplayMaxFreq,&TfirPage::onUserDisplayMaxFreq,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TfirPage> edInt[]=
  {
   IDC_ED_FIR_TAPS,8,1024,IDFF_firTaps,&TfirPage::drawFir,
   IDC_ED_FIR_FREQ,1,192000,IDFF_firFreq,&TfirPage::drawFir,
   IDC_ED_FIR_WIDTH,1,192000/2,IDFF_firWidth,&TfirPage::drawFir,
   IDC_ED_FIR_MAXFREQ,1,192000/2,IDFF_firUserDisplayMaxFreq,&TfirPage::drawFir2,
   0
  };
 bindEditInts(edInt);
 static const TbindEditReal<TfirPage> edReal[]=
  {
   IDC_ED_FIR_KAISER_BETA,1.0,20.0,IDFF_firKaiserBeta,1000.0,NULL,
   0
  };
 bindEditReals(edReal);
 static const TbindCombobox<TfirPage> cbx[]=
  {
   IDC_CBX_FIR_TYPE,IDFF_firType,BINDCBX_SEL,&TfirPage::onFirType,
   IDC_CBX_FIR_WINDOW,IDFF_firWindow,BINDCBX_SEL,&TfirPage::onFirWindow,
   0
  };
 bindComboboxes(cbx);
}
