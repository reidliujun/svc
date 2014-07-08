/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cgraph.h"
#include "TencStats.h"
#include "TffPict.h"

void TgraphPage::init(void)
{
 hg=GetDlgItem(m_hwnd,IDC_BMP_GRAPH);
 RECT r;GetWindowRect(hg,&r);
 gx=r.right-r.left;gy=r.bottom-r.top;stride=(gx/4+1)*4;
 graphBits=(unsigned char*)calloc(stride,gy+1);
 memset(&graphBmp.bmiHeader,0,sizeof(graphBmp.bmiHeader));
 graphBmp.bmiHeader.biSize=sizeof(graphBmp.bmiHeader);
 graphBmp.bmiHeader.biWidth=stride;
 graphBmp.bmiHeader.biHeight=gy;
 graphBmp.bmiHeader.biPlanes=1;
 graphBmp.bmiHeader.biBitCount=8;
 graphBmp.bmiHeader.biCompression=BI_RGB;
 graphBmp.bmiHeader.biXPelsPerMeter=100;
 graphBmp.bmiHeader.biYPelsPerMeter=100;
 memset(graphBmp.bmiColors,0,256*sizeof(RGBQUAD)); //0 - background
 graphBmp.bmiColors[1].rgbRed=155;graphBmp.bmiColors[1].rgbGreen=110;graphBmp.bmiColors[1].rgbBlue=  0; //1 - grid
 graphBmp.bmiColors[2].rgbRed=255;graphBmp.bmiColors[2].rgbGreen=255;graphBmp.bmiColors[2].rgbBlue=255; //2 - quant
 graphBmp.bmiColors[3].rgbRed=  0;graphBmp.bmiColors[3].rgbGreen=  0;graphBmp.bmiColors[3].rgbBlue=255; //3 - p-frames
 graphBmp.bmiColors[4].rgbRed=255;graphBmp.bmiColors[4].rgbGreen=  0;graphBmp.bmiColors[4].rgbBlue=  0; //4 - i-frames
 graphBmp.bmiColors[5].rgbRed=128;graphBmp.bmiColors[5].rgbGreen=128;graphBmp.bmiColors[5].rgbBlue=128; //5 - b-frames
 graphBmp.bmiColors[6].rgbRed=255;graphBmp.bmiColors[6].rgbGreen=255;graphBmp.bmiColors[6].rgbBlue=255; //6 - text

 frames=new Tframe[gx];
 shift=0;
 oldframescount=0;t1=0;
 oldmaxframesize=1;
 SetTimer(m_hwnd,IDC_TMR_GRAPH,1000,NULL);
 drawCompleteGraph();
 enable((filterMode&IDFF_FILTERMODE_VFW)==0,IDC_CHB_ADDTOROT);
}

void TgraphPage::cfg2dlg(void)
{
 debug2dlg();
 graph2dlg();
 int working=cfgGet(IDFF_enc_working);
 if (working)
  {
   enable(sup_PSNR(codecId) && !working,IDC_CHB_STATS_PSNR);
   setCheck(IDC_CHB_STATS_PSNR,cfgGet(IDFF_enc_psnr));
  }
 else
  {
   setCheck(IDC_CHB_STATS_PSNR,cfgGet(IDFF_enc_psnr));
   enable(sup_PSNR(codecId) && !working,IDC_CHB_STATS_PSNR);
  }
 setCheck(IDC_CHB_ADDTOROT,cfgGet(IDFF_addToROT));
}

void TgraphPage::debug2dlg(void)
{
 setCheck(IDC_CHB_OUTPUTDEBUG,cfgGet(IDFF_outputdebug));
 setCheck(IDC_CHB_OUTPUTDEBUGFILE,cfgGet(IDFF_outputdebugfile));
 setDlgItemText(m_hwnd,IDC_ED_OUTPUTDEBUGFILE,cfgGetStr(IDFF_debugfile));
 static const int idDebugFile[]={IDC_ED_OUTPUTDEBUGFILE,IDC_BT_OUTPUTDEBUGFILE,0};
 enable(cfgGet(IDFF_outputdebugfile),idDebugFile);
 setCheck(IDC_CHB_ERRORBOX,cfgGet(IDFF_errorbox));
}

void TgraphPage::graph2dlg(void)
{
 int showGraph=cfgGet(IDFF_enc_showGraph);
 setCheck(IDC_CHB_GRAPH,showGraph);
 show(showGraph,IDC_LBL_GRAPH_MAXKB);
 deciE->setHgraph(showGraph?m_hwnd:NULL);
 clearFrames();
 drawCompleteGraph();
}

void TgraphPage::clearFrames(void)
{
 for (int i=0;i<gx;i++)
  frames[i].frametype=frames[i].size=frames[i].quant=0;
}

INT_PTR TgraphPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (graphBits) free(graphBits);graphBits=NULL;
    if (frames) delete []frames;frames=NULL;
    deciE->setHgraph(NULL);
    KillTimer(m_hwnd,IDC_TMR_GRAPH);
    break;
   case WM_DRAWITEM:
    if (wParam==IDC_BMP_GRAPH)
     {
      LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
      SetDIBitsToDevice(dis->hDC,0,0,gx,gy,0,0,0,gy,graphBits,(BITMAPINFO*)&graphBmp,DIB_RGB_COLORS);
      return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_OUTPUTDEBUGFILE:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t debugfile[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_OUTPUTDEBUGFILE,debugfile,MAX_PATH);
         cfgSet(IDFF_debugfile,debugfile);
         return TRUE;
        }
       break;
     }
    break;
   case WM_TIMER:
    if (wParam==IDC_TMR_GRAPH)
     {
      if (!cfgGet(IDFF_enc_working)) return TRUE;
      TencStats *stats=NULL;
      if (FAILED(deciE->getEncStats(&stats))) return TRUE;
      //if (oldframescount>stats.count) t1=0;
      clock_t t2=clock();
      setText(IDC_LBL_STATS_FPS,_l("%s %5.2f"),_(IDC_LBL_STATS_FPS),(t1==0)?0:float(CLOCKS_PER_SEC*(stats->count-oldframescount)/float(t2-t1)));
      t1=t2;oldframescount=stats->count;

      setText(IDC_LBL_STATS_NUMFRAMES,_l("%s %i"),_(IDC_LBL_STATS_NUMFRAMES),stats->count,FALSE);
      setText(IDC_LBL_STATS_AVGQUANT,_l("%s %5.2f"),_(IDC_LBL_STATS_AVGQUANT),stats->count?float(stats->sumQuants)/stats->count:0.0f);
      setText(IDC_LBL_STATS_KBPS,_l("%s %i"),_(IDC_LBL_STATS_KBPS),stats->count?int((8ULL*stats->sumFramesize*cfgGet(IDFF_enc_fpsRate))/(1000ULL*stats->count*cfgGet(IDFF_enc_fpsScale))):0);
      char_t psnrYs[30],psnrUs[30],psnrVs[30];
      if (sup_PSNR(codecId) && cfgGet(IDFF_enc_psnr))
       {
        double psnrY,psnrU,psnrV;
        stats->calcPSNR(&psnrY,&psnrU,&psnrV);
        if (psnrY>0) tsprintf(psnrYs,_l("%6.2f"),psnrY); else strcpy(psnrYs,_(IDC_LBL_STATS_PSNR,_l("exact")));
        if (psnrU>0) tsprintf(psnrUs,_l("%6.2f"),psnrU); else strcpy(psnrUs,_(IDC_LBL_STATS_PSNR,_l("exact")));
        if (psnrV>0) tsprintf(psnrVs,_l("%6.2f"),psnrV); else strcpy(psnrVs,_(IDC_LBL_STATS_PSNR,_l("exact")));
        setText(IDC_LBL_STATS_PSNR,_l("%s %s,%s,%s"),_(IDC_LBL_STATS_PSNR),psnrYs,psnrUs,psnrVs);
       }
      else
       setText(IDC_LBL_STATS_PSNR,_l("%s %s"),_(IDC_LBL_STATS_PSNR),_(IDC_LBL_STATS_PSNR,_l("unknown")));

      if (cfgGet(IDFF_enc_showGraph)) repaint(hg);
      return TRUE;
     }
    break;
   case TencStats::MSG_FF_FRAME:
    {
     memmove(frames,frames+1,sizeof(Tframe)*(gx-1));
     frames[gx-1].set(wParam,lParam);
     unsigned int maxframesize=1;
     for (int i=0;i<gx;i++)
      if (frames[i].size>maxframesize)
       maxframesize=frames[i].size;
     if (oldmaxframesize!=maxframesize)
      {
       oldmaxframesize=maxframesize;
       SetDlgItemInt(m_hwnd,IDC_LBL_GRAPH_MAXKB,maxframesize/1024,FALSE);
       for (int i=0;i<gx;i++)
        shiftGraph(frames[i]);
      }
     else
      shiftGraph(frames[gx-1]);
     return TRUE;
    }
   case TencStats::MSG_FF_CLEAR:
    t1=0;oldframescount=0;
    graph2dlg();
    return TRUE;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}
void TgraphPage::onDebugoutputfile(void)
{
 char_t debugfile[MAX_PATH];cfgGet(IDFF_debugfile,debugfile,MAX_PATH);
 if (dlgGetFile(true,m_hwnd,_(-IDD_GRAPH,_l("Select file for debug info")),_l("Log files (*.log)\0*.log\0All files (*.*)\0*.*\0"),_l("log"),debugfile,_l("."),0))
  {
   cfgSet(IDFF_debugfile,debugfile);
   debug2dlg();
  }
}

void TgraphPage::putPixel(int x,int y,unsigned char c)
{
 graphBits[y*stride+x]=c;
}
void TgraphPage::lineY(int x,int y1,int y2,unsigned char c)
{
 unsigned char *dst=graphBits+y1*stride+x;
 for (int y=y1;y<y2;y++,dst+=stride)
  *dst=c;
}
void TgraphPage::lineX(int x1,int x2,int y,unsigned char c)
{
 memset(graphBits+y*stride+x1,c,x2-x1+1);
}
void TgraphPage::drawCompleteGraph(void)
{
 memset(graphBits,0,stride*gy);
 for (int x=0;x<gx;x++)
  {
   shift++;
   if (shift==30)
    {
     lineY(x,0,gy,1);
     shift=0;
    }
  }
 for (int y=0;y<gy;y+=30) lineX(0,gx,y,1);
 repaint(hg);
}
void TgraphPage::shiftLeft(void)
{
 shift++;
 memmove(graphBits,graphBits+1,stride*gy-1);
 if (shift==30)
  {
   lineY(gx-1,0,gy-1,1);
   shift=0;
  }
 else
  lineY(gx-1,0,gy-1,0);
 for (int y=0;y<gy;y+=30) putPixel(gx-1,y,1);
}
void TgraphPage::shiftGraph(Tframe &frame)
{
 shiftLeft();
 unsigned char color;
 switch (frame.frametype)
  {
   case FRAME_TYPE::I:color=4;break;
   case FRAME_TYPE::B:color=5;break;
   default:
   case FRAME_TYPE::P:color=3;break;
  }
 if (frame.size) lineY(gx-1,2,(gy-4)*frame.size/oldmaxframesize,color);
 if (isIn(frame.quant,(unsigned int)parent->qmin,(unsigned int)parent->qmax)) putPixel(gx-1,(gy-5)*(frame.quant-parent->qmin)/(parent->qmax-parent->qmin+1)+2,2);
}

TgraphPage::TgraphPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GRAPH;
 static const TbindCheckbox<TgraphPage> chb[]=
  {
   IDC_CHB_GRAPH,IDFF_enc_showGraph,&TgraphPage::graph2dlg,
   IDC_CHB_OUTPUTDEBUG,IDFF_outputdebug,NULL,
   IDC_CHB_OUTPUTDEBUGFILE,IDFF_outputdebugfile,&TgraphPage::debug2dlg,
   IDC_CHB_ERRORBOX,IDFF_errorbox,NULL,
   IDC_CHB_STATS_PSNR,IDFF_enc_psnr,NULL,
   IDC_CHB_ADDTOROT,IDFF_addToROT,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindButton<TgraphPage> bt[]=
  {
   IDC_BT_OUTPUTDEBUGFILE,&TgraphPage::onDebugoutputfile,
   0,NULL
  };
 bindButtons(bt);
}
