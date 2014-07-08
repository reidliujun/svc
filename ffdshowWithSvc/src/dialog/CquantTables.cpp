/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "CquantTables.h"
#include "Tconfig.h"

//========================================== TquantTablesPageBase ==========================================
const char_t* TquantTablesPageBase::matrices[]=
{
 _l("8x8"),
 _l("4x4 luma"),
 _l("4x4 chroma"),
 NULL
};

const int TquantTablesPageBase::idIntra8[]=
{
 IDC_ED_QUANT_TABLE_INTRA1 ,IDC_ED_QUANT_TABLE_INTRA2 ,IDC_ED_QUANT_TABLE_INTRA3 ,IDC_ED_QUANT_TABLE_INTRA4 ,IDC_ED_QUANT_TABLE_INTRA5 ,IDC_ED_QUANT_TABLE_INTRA6 ,IDC_ED_QUANT_TABLE_INTRA7 ,IDC_ED_QUANT_TABLE_INTRA8 ,
 IDC_ED_QUANT_TABLE_INTRA9 ,IDC_ED_QUANT_TABLE_INTRA10,IDC_ED_QUANT_TABLE_INTRA11,IDC_ED_QUANT_TABLE_INTRA12,IDC_ED_QUANT_TABLE_INTRA13,IDC_ED_QUANT_TABLE_INTRA14,IDC_ED_QUANT_TABLE_INTRA15,IDC_ED_QUANT_TABLE_INTRA16,
 IDC_ED_QUANT_TABLE_INTRA17,IDC_ED_QUANT_TABLE_INTRA18,IDC_ED_QUANT_TABLE_INTRA19,IDC_ED_QUANT_TABLE_INTRA20,IDC_ED_QUANT_TABLE_INTRA21,IDC_ED_QUANT_TABLE_INTRA22,IDC_ED_QUANT_TABLE_INTRA23,IDC_ED_QUANT_TABLE_INTRA24,
 IDC_ED_QUANT_TABLE_INTRA25,IDC_ED_QUANT_TABLE_INTRA26,IDC_ED_QUANT_TABLE_INTRA27,IDC_ED_QUANT_TABLE_INTRA28,IDC_ED_QUANT_TABLE_INTRA29,IDC_ED_QUANT_TABLE_INTRA30,IDC_ED_QUANT_TABLE_INTRA31,IDC_ED_QUANT_TABLE_INTRA32,
 IDC_ED_QUANT_TABLE_INTRA33,IDC_ED_QUANT_TABLE_INTRA34,IDC_ED_QUANT_TABLE_INTRA35,IDC_ED_QUANT_TABLE_INTRA36,IDC_ED_QUANT_TABLE_INTRA37,IDC_ED_QUANT_TABLE_INTRA38,IDC_ED_QUANT_TABLE_INTRA39,IDC_ED_QUANT_TABLE_INTRA40,
 IDC_ED_QUANT_TABLE_INTRA41,IDC_ED_QUANT_TABLE_INTRA42,IDC_ED_QUANT_TABLE_INTRA43,IDC_ED_QUANT_TABLE_INTRA44,IDC_ED_QUANT_TABLE_INTRA45,IDC_ED_QUANT_TABLE_INTRA46,IDC_ED_QUANT_TABLE_INTRA47,IDC_ED_QUANT_TABLE_INTRA48,
 IDC_ED_QUANT_TABLE_INTRA49,IDC_ED_QUANT_TABLE_INTRA50,IDC_ED_QUANT_TABLE_INTRA51,IDC_ED_QUANT_TABLE_INTRA52,IDC_ED_QUANT_TABLE_INTRA53,IDC_ED_QUANT_TABLE_INTRA54,IDC_ED_QUANT_TABLE_INTRA55,IDC_ED_QUANT_TABLE_INTRA56,
 IDC_ED_QUANT_TABLE_INTRA57,IDC_ED_QUANT_TABLE_INTRA58,IDC_ED_QUANT_TABLE_INTRA59,IDC_ED_QUANT_TABLE_INTRA60,IDC_ED_QUANT_TABLE_INTRA61,IDC_ED_QUANT_TABLE_INTRA62,IDC_ED_QUANT_TABLE_INTRA63,IDC_ED_QUANT_TABLE_INTRA64,0
};
const int TquantTablesPageBase::idInter8[]=
{
 IDC_ED_QUANT_TABLE_INTER1 ,IDC_ED_QUANT_TABLE_INTER2 ,IDC_ED_QUANT_TABLE_INTER3 ,IDC_ED_QUANT_TABLE_INTER4 ,IDC_ED_QUANT_TABLE_INTER5 ,IDC_ED_QUANT_TABLE_INTER6 ,IDC_ED_QUANT_TABLE_INTER7 ,IDC_ED_QUANT_TABLE_INTER8 ,
 IDC_ED_QUANT_TABLE_INTER9 ,IDC_ED_QUANT_TABLE_INTER10,IDC_ED_QUANT_TABLE_INTER11,IDC_ED_QUANT_TABLE_INTER12,IDC_ED_QUANT_TABLE_INTER13,IDC_ED_QUANT_TABLE_INTER14,IDC_ED_QUANT_TABLE_INTER15,IDC_ED_QUANT_TABLE_INTER16,
 IDC_ED_QUANT_TABLE_INTER17,IDC_ED_QUANT_TABLE_INTER18,IDC_ED_QUANT_TABLE_INTER19,IDC_ED_QUANT_TABLE_INTER20,IDC_ED_QUANT_TABLE_INTER21,IDC_ED_QUANT_TABLE_INTER22,IDC_ED_QUANT_TABLE_INTER23,IDC_ED_QUANT_TABLE_INTER24,
 IDC_ED_QUANT_TABLE_INTER25,IDC_ED_QUANT_TABLE_INTER26,IDC_ED_QUANT_TABLE_INTER27,IDC_ED_QUANT_TABLE_INTER28,IDC_ED_QUANT_TABLE_INTER29,IDC_ED_QUANT_TABLE_INTER30,IDC_ED_QUANT_TABLE_INTER31,IDC_ED_QUANT_TABLE_INTER32,
 IDC_ED_QUANT_TABLE_INTER33,IDC_ED_QUANT_TABLE_INTER34,IDC_ED_QUANT_TABLE_INTER35,IDC_ED_QUANT_TABLE_INTER36,IDC_ED_QUANT_TABLE_INTER37,IDC_ED_QUANT_TABLE_INTER38,IDC_ED_QUANT_TABLE_INTER39,IDC_ED_QUANT_TABLE_INTER40,
 IDC_ED_QUANT_TABLE_INTER41,IDC_ED_QUANT_TABLE_INTER42,IDC_ED_QUANT_TABLE_INTER43,IDC_ED_QUANT_TABLE_INTER44,IDC_ED_QUANT_TABLE_INTER45,IDC_ED_QUANT_TABLE_INTER46,IDC_ED_QUANT_TABLE_INTER47,IDC_ED_QUANT_TABLE_INTER48,
 IDC_ED_QUANT_TABLE_INTER49,IDC_ED_QUANT_TABLE_INTER50,IDC_ED_QUANT_TABLE_INTER51,IDC_ED_QUANT_TABLE_INTER52,IDC_ED_QUANT_TABLE_INTER53,IDC_ED_QUANT_TABLE_INTER54,IDC_ED_QUANT_TABLE_INTER55,IDC_ED_QUANT_TABLE_INTER56,
 IDC_ED_QUANT_TABLE_INTER57,IDC_ED_QUANT_TABLE_INTER58,IDC_ED_QUANT_TABLE_INTER59,IDC_ED_QUANT_TABLE_INTER60,IDC_ED_QUANT_TABLE_INTER61,IDC_ED_QUANT_TABLE_INTER62,IDC_ED_QUANT_TABLE_INTER63,IDC_ED_QUANT_TABLE_INTER64,0
};

const int TquantTablesPageBase::idIntra4[]=
{
 IDC_ED_QUANT_TABLE_INTRA1 ,IDC_ED_QUANT_TABLE_INTRA2 ,IDC_ED_QUANT_TABLE_INTRA3 ,IDC_ED_QUANT_TABLE_INTRA4 ,
 IDC_ED_QUANT_TABLE_INTRA9 ,IDC_ED_QUANT_TABLE_INTRA10,IDC_ED_QUANT_TABLE_INTRA11,IDC_ED_QUANT_TABLE_INTRA12,
 IDC_ED_QUANT_TABLE_INTRA17,IDC_ED_QUANT_TABLE_INTRA18,IDC_ED_QUANT_TABLE_INTRA19,IDC_ED_QUANT_TABLE_INTRA20,
 IDC_ED_QUANT_TABLE_INTRA25,IDC_ED_QUANT_TABLE_INTRA26,IDC_ED_QUANT_TABLE_INTRA27,IDC_ED_QUANT_TABLE_INTRA28,0
};
const int TquantTablesPageBase::idInter4[]=
{
 IDC_ED_QUANT_TABLE_INTER1 ,IDC_ED_QUANT_TABLE_INTER2 ,IDC_ED_QUANT_TABLE_INTER3 ,IDC_ED_QUANT_TABLE_INTER4 ,
 IDC_ED_QUANT_TABLE_INTER9 ,IDC_ED_QUANT_TABLE_INTER10,IDC_ED_QUANT_TABLE_INTER11,IDC_ED_QUANT_TABLE_INTER12,
 IDC_ED_QUANT_TABLE_INTER17,IDC_ED_QUANT_TABLE_INTER18,IDC_ED_QUANT_TABLE_INTER19,IDC_ED_QUANT_TABLE_INTER20,
 IDC_ED_QUANT_TABLE_INTER25,IDC_ED_QUANT_TABLE_INTER26,IDC_ED_QUANT_TABLE_INTER27,IDC_ED_QUANT_TABLE_INTER28,0
};

TquantTablesPageBase::TquantTablesPageBase(IffdshowBase *Ideci):Twindow(Ideci)
{
 dialogId=IDD_QUANT_TABLES_BASE;
 static const TbindButton<TquantTablesPageBase> bt[]=
  {
   IDC_BT_QUANT_TABLE_LOAD,&TquantTablesPageBase::onLoad,
   IDC_BT_QUANT_TABLE_SAVE,&TquantTablesPageBase::onSave,
   0,NULL
  };
 bindButtons(bt);
 static const TbindCombobox<TquantTablesPageBase> cbx[]=
  {
   IDC_CBX_QUANT_TABLE,0,BINDCBX_NONE,&TquantTablesPageBase::table2dlg,
   0
  };
 bindComboboxes(cbx);
}

void TquantTablesPageBase::create(HWND parent)
{
 createDialog(dialogId,parent);SetParent(m_hwnd,parent);
 setPos(0,0);
 show(true);
 matrixflnm[0]='\0';
 _makepath_s(matrixdir,MAX_PATH,NULL,config->pth,_l("custom matrices"),NULL);
 firstdir=true;
}

void TquantTablesPageBase::cfg2dlg(void)
{
 codecId=getCodecId();
   if (codecId==CODEC_ID_X264 || codecId==CODEC_ID_H264)
    enable(1,IDC_CBX_QUANT_TABLE);
   else
    {
    enable(0,IDC_CBX_QUANT_TABLE);
    cbxSetCurSel(IDC_CBX_QUANT_TABLE,0);
    }
    table2dlg();
}

void TquantTablesPageBase::table2dlg(void)
{
 uint8_t *intra8,*inter8,*intra4Y,*inter4Y,*intra4C,*inter4C;getCustomQuantMatrixes(&intra8,&inter8,&intra4Y,&inter4Y,&intra4C,&inter4C);
 const int *idIntra,*idInter;uint8_t *intra,*inter;int cnt;
 switch (cbxGetCurSel(IDC_CBX_QUANT_TABLE))
  {
   case 1:idIntra=idIntra4;idInter=idInter4;intra=intra4Y;inter=inter4Y;cnt=16;break;
   case 2:idIntra=idIntra4;idInter=idInter4;intra=intra4C;inter=inter4C;cnt=16;break;
   default:idIntra=idIntra8;idInter=idInter8;intra=intra8;inter=inter8;cnt=64;break;
  }
 enable(0,idIntra8);
 enable(0,idInter8);
 for (int i=0;i<cnt;i++)
  {
   if (intra)
    {
     edLimitText(idIntra[i],3);
     SetDlgItemInt(m_hwnd,idIntra[i],intra[i],FALSE);
     ((TwidgetMatrix*)Twidget::getDlgItem(GetDlgItem(m_hwnd,idIntra[i])))->set(true ,i+1);
    }
   else
    edReadOnly(idIntra[i],true);
   if (inter)
    {
     edLimitText(idInter[i],3);
     SetDlgItemInt(m_hwnd,idInter[i],inter[i],FALSE);
     ((TwidgetMatrix*)Twidget::getDlgItem(GetDlgItem(m_hwnd,idInter[i])))->set(false,i+1);
    }
   else
    edReadOnly(idInter[i],true);
  }
 enable(1,cnt==64?idIntra8:idIntra4);
 enable(1,cnt==64?idInter8:idInter4);
 enable(codecId==CODEC_ID_X264 || codecId==CODEC_ID_H264,IDC_ED_QUANT_TABLE_INTRA1);
}


const char_t* TquantTablesPageBase::parse_jmlist(char_t *buf,const char_t *name,uint8_t *cqm,const uint8_t *jvt,int length)
{
 char_t *p=strstr(buf,name);
 if (!p)
  {
   memset(cqm,16,length);
   return NULL;
  }

 p+=strlen( name );
 if (*p=='U' || *p=='V')
  p++;

 char_t *nextvar=strstr(p,_l("INT"));
 int i;
 for(i=0;i<length && (p=strpbrk(p,_l(" \t\n,") ))!=NULL && (p=strpbrk(p,_l("0123456789")))!=NULL;i++)
  {
   int coef=-1;
   tsscanf(p,_l("%d"),&coef);
   if (i==0 && coef==0)
    {
     memcpy(cqm,jvt,length);
     return NULL;
    }
   if (coef<1 || coef>255)
    return _l("Bad coefficient in list.");
   cqm[i]=(uint8_t)coef;
  }
 if ((nextvar && p>nextvar) || i!=length)
  return _l("Not enough coefficients in list.");
 return NULL;
}

void TquantTablesPageBase::onLoad(void)
{
 bool x264=codecId==CODEC_ID_X264 || codecId==CODEC_ID_H264;
 if (dlgGetFile(false,m_hwnd,_(-IDD_QUANT_TABLES,_l("Load quantization matrices")),x264?_l("JM matrices file (*.cfg)\0*.cfg\0All files (*.*)\0*.*"):_l("All matrix files (*.xcm;*.qmatrix;*.cqm;*.txt)\0*.xcm;*.qmatrix;*.txt\0XviD quantization matrices file (*.xcm)\0*.xcm\0ffdshow quantization matrices file (*.qmatrix;*.txt)\0*.qmatrix;*.txt\0Custom quantization matrix (*.cqm)\0*.cqm\0All files (*.*)\0*.*\0"),x264?_l("*.cfg"):_l("*.xcm"),matrixflnm,firstdir?matrixdir:_l("."),0))
  {
   firstdir=false;
   const char_t *error=NULL;size_t readed=0;
 if (x264)
  {
  char_t *buf=readTextFile(matrixflnm);
  if (buf)
   {
    static const uint8_t jvt4i[16] =
    {
    6,13,20,28,
    13,20,28,32,
    20,28,32,37,
    28,32,37,42
    };
    static const uint8_t jvt4p[16] =
    {
    10,14,20,24,
    14,20,24,27,
    20,24,27,30,
    24,27,30,34
    };
    static const uint8_t jvt8i[64] =
    {
    6,10,13,16,18,23,25,27,
    10,11,16,18,23,25,27,29,
    13,16,18,23,25,27,29,31,
    16,18,23,25,27,29,31,33,
    18,23,25,27,29,31,33,36,
    23,25,27,29,31,33,36,38,
    25,27,29,31,33,36,38,40,
    27,29,31,33,36,38,40,42
    };
    static const uint8_t jvt8p[64] =
    {
    9,13,15,17,19,21,22,24,
    13,13,17,19,21,22,24,25,
    15,17,19,21,22,24,25,27,
    17,19,21,22,24,25,27,28,
    19,21,22,24,25,27,28,30,
    21,22,24,25,27,28,30,32,
    22,24,25,27,28,30,32,33,
    24,25,27,28,30,32,33,35
    };
    uint8_t *intra8,*inter8,*intra4Y,*inter4Y,*intra4C,*inter4C;getCustomQuantMatrixes(&intra8,&inter8,&intra4Y,&inter4Y,&intra4C,&inter4C);
    if ((error=parse_jmlist(buf,_l("INTRA4X4_LUMA")  ,intra4Y,jvt4i,16))==NULL)
    if ((error=parse_jmlist(buf,_l("INTRA4X4_CHROMA"),intra4C,jvt4i,16))==NULL)
    if ((error=parse_jmlist(buf,_l("INTER4X4_LUMA")  ,inter4Y,jvt4p,16))==NULL)
    if ((error=parse_jmlist(buf,_l("INTER4X4_CHROMA"),inter4C,jvt4p,16))==NULL)
    if ((error=parse_jmlist(buf,_l("INTRA8X8_LUMA")  ,intra8,jvt8i,64))==NULL)
    if ((error=parse_jmlist(buf,_l("INTER8X8_LUMA")  ,inter8,jvt8p,64))==NULL)
    readed=1;
    free(buf);
    }
   }
   else
   {
    FILE *f=fopen(matrixflnm,x264?_l("rt"):_l("rb"));
     if (f)
      {
       uint8_t *qmatrix_intra_custom,*qmatrix_inter_custom;getCustomQuantMatrixes(&qmatrix_intra_custom,&qmatrix_inter_custom,NULL,NULL,NULL,NULL);
       if (qmatrix_intra_custom)
        readed+=fread(qmatrix_intra_custom,1,64,f);
       else
        {
         fseek(f,64,SEEK_CUR);
         readed+=64;
        }
       if (qmatrix_inter_custom)
        readed+=fread(qmatrix_inter_custom,1,64,f);
       else
        {
         fseek(f,64,SEEK_CUR);
         readed+=64;
        }
       fclose(f);
       if (readed!=128)
        error=_l("Error while loading quantization matrices");
      }
    }
   if (error)
    err(_(-IDD_QUANT_TABLES,error));
   else
    {
     if (readed)
      setChange();
     cfg2dlg();
    }
  }
}

void TquantTablesPageBase::writeMatrix(FILE *f,const char *section,const uint8_t *matrix,unsigned int len)
{
 fprintf(f,"%s =\n",section);
 int cnt=ff_sqrt(len);
 for (int i=0;i<cnt;i++)
  {
   for (int j=0;j<cnt;j++)
    fprintf(f,"%i%s",int(*matrix++),--len>0?",":"");
   fprintf(f,"\n");
  }
 fprintf(f,"\n");
}

void TquantTablesPageBase::onSave(void)
{
 bool x264=codecId==CODEC_ID_X264 || codecId==CODEC_ID_H264;
 if (dlgGetFile(true,m_hwnd,_(-IDD_QUANT_TABLES,_l("Save quantization matrices")),x264?_l("JM matrices file (*.cfg)\0*.cfg\0All files (*.*)\0*.*"):_l("XviD quantization matrices file (*.xcm)\0*.xcm\0ffdshow quantization matrices file (*.qmatrix)\0*.qmatrix\0Custom quantization matrix (*.cqm)\0*.cqm\0All files (*.*)\0*.*\0"),x264?_l("*.cfg"):_l("*.xcm"),matrixflnm,firstdir?matrixdir:_l("."),0))
  {
   firstdir=false;
   FILE *f=fopen(matrixflnm,x264?_l("wt"):_l("wb"));
   if (f)
    {
      if (x264)
      {
       uint8_t *intra8,*inter8,*intra4Y,*inter4Y,*intra4C,*inter4C;getCustomQuantMatrixes(&intra8,&inter8,&intra4Y,&inter4Y,&intra4C,&inter4C);
      writeMatrix(f,"INTRA4X4_LUMA",intra4Y,16);
      writeMatrix(f,"INTRA4X4_CHROMAU",intra4C,16);
      writeMatrix(f,"INTRA4X4_CHROMAV",intra4C,16);
      writeMatrix(f,"INTER4X4_LUMA",inter4Y,16);
      writeMatrix(f,"INTER4X4_CHROMAU",inter4C,16);
      writeMatrix(f,"INTER4X4_CHROMAV",inter4C,16);
      writeMatrix(f,"INTRA8X8_LUMA",intra8,64);
      writeMatrix(f,"INTER8X8_LUMA",inter8,64);
      }
      else
      {
       uint8_t *qmatrix_intra_custom,*qmatrix_inter_custom;getCustomQuantMatrixes(&qmatrix_intra_custom,&qmatrix_inter_custom,NULL,NULL,NULL,NULL);
       if (qmatrix_intra_custom || qmatrix_inter_custom)
        {
         fwrite(qmatrix_intra_custom?qmatrix_intra_custom:qmatrix_inter_custom,1,64,f);
         fwrite(qmatrix_inter_custom?qmatrix_inter_custom:qmatrix_intra_custom,1,64,f);
        }
      }
     fclose(f);
    }
   else
    err(_(-IDD_QUANT_TABLES,_l("Error while saving quantization matrices")));
  }
}

INT_PTR TquantTablesPageBase::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
     {
      HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
      if (hed!=GetFocus()) return FALSE;
      repaint(hed);
      TwidgetMatrix *item=(TwidgetMatrix*)Twidget::getDlgItem(hed);
      if (item->id)
       {
        int valI;
        if (eval(hed,1,255,&valI))
         {
          uint8_t *intra8,*inter8,*intra4Y,*inter4Y,*intra4C,*inter4C;getCustomQuantMatrixes(&intra8,&inter8,&intra4Y,&inter4Y,&intra4C,&inter4C);
          uint8_t *intra,*inter;
          switch (cbxGetCurSel(IDC_CBX_QUANT_TABLE))
           {
            case 1:intra=intra4Y;inter=inter4Y;break;
            case 2:intra=intra4C;inter=inter4C;break;
            default:intra=intra8;inter=inter8;break;
           }
          (item->intra?intra:inter)[item->id-1]=(uint8_t)valI;
          setChange();
         }
       }
      return TRUE;
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (hwnd==GetDlgItem(m_hwnd,IDC_CBX_QUANT_TABLE)) break;
     bool ok=eval(hwnd,1,255);
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
    }
  }
 return Twindow::msgProc(uMsg,wParam,lParam);
}

Twidget* TquantTablesPageBase::createDlgItem(int id,HWND h)
{
 char_t cls[256]=_l("");
 GetClassName(h,cls,256);
 if (stricmp(cls,_l("EDIT"))==0)
  return new TwidgetMatrix(h,this);
 else
  return Twindow::createDlgItem(id,h);
}

void TquantTablesPageBase::translate(void)
{
 Twindow::translate();

 int sel=cbxGetCurSel(IDC_CBX_QUANT_TABLE);
 cbxClear(IDC_CBX_QUANT_TABLE);
 for (int i=0;matrices[i];i++)
  cbxAdd(IDC_CBX_QUANT_TABLE,_(IDC_CBX_QUANT_TABLE,matrices[i]));
 cbxSetCurSel(IDC_CBX_QUANT_TABLE,sel==CB_ERR?0:sel);
}

//============================================ TquantTablesPage ============================================
bool TquantTablesPage::enabled(void)
{
 return sup_customQuantTables(codecId) && cfgGet(IDFF_enc_quant_type)==QUANT::CUSTOM;
}

TquantTablesPage::TquantTablesPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent),base(new TquantTables(Iparent->deci))
{
 dialogId=IDD_QUANT_TABLES;
 static const int props[]={IDFF_enc_qmatrix_intra_custom0,IDFF_enc_qmatrix_intra_custom1,IDFF_enc_qmatrix_intra_custom2,IDFF_enc_qmatrix_intra_custom3,IDFF_enc_qmatrix_intra_custom4,IDFF_enc_qmatrix_intra_custom5,IDFF_enc_qmatrix_intra_custom6,IDFF_enc_qmatrix_intra_custom7,IDFF_enc_qmatrix_intra_custom8,IDFF_enc_qmatrix_intra_custom9,IDFF_enc_qmatrix_intra_custom10,IDFF_enc_qmatrix_intra_custom11,IDFF_enc_qmatrix_intra_custom12,IDFF_enc_qmatrix_intra_custom13,IDFF_enc_qmatrix_intra_custom14,IDFF_enc_qmatrix_intra_custom15,IDFF_enc_qmatrix_inter_custom0,IDFF_enc_qmatrix_inter_custom1,IDFF_enc_qmatrix_inter_custom2,IDFF_enc_qmatrix_inter_custom3,IDFF_enc_qmatrix_inter_custom4,IDFF_enc_qmatrix_inter_custom5,IDFF_enc_qmatrix_inter_custom6,IDFF_enc_qmatrix_inter_custom7,IDFF_enc_qmatrix_inter_custom8,IDFF_enc_qmatrix_inter_custom9,IDFF_enc_qmatrix_inter_custom10,IDFF_enc_qmatrix_inter_custom11,IDFF_enc_qmatrix_inter_custom12,IDFF_enc_qmatrix_inter_custom13,IDFF_enc_qmatrix_inter_custom14,IDFF_enc_qmatrix_inter_custom15,IDFF_enc_qmatrix_intra4x4Y_custom0,IDFF_enc_qmatrix_intra4x4Y_custom1,IDFF_enc_qmatrix_intra4x4Y_custom2,IDFF_enc_qmatrix_intra4x4Y_custom3,IDFF_enc_qmatrix_inter4x4Y_custom0,IDFF_enc_qmatrix_inter4x4Y_custom1,IDFF_enc_qmatrix_inter4x4Y_custom2,IDFF_enc_qmatrix_inter4x4Y_custom3,IDFF_enc_qmatrix_intra4x4C_custom0,IDFF_enc_qmatrix_intra4x4C_custom1,IDFF_enc_qmatrix_intra4x4C_custom2,IDFF_enc_qmatrix_intra4x4C_custom3,IDFF_enc_qmatrix_inter4x4C_custom0,IDFF_enc_qmatrix_inter4x4C_custom1,IDFF_enc_qmatrix_inter4x4C_custom2,IDFF_enc_qmatrix_inter4x4C_custom3,0};
 propsIDs=props;
 base->deciE=deciE;base->parent=parent;
}
TquantTablesPage::~TquantTablesPage()
{
 delete base;
}

void TquantTablesPage::init(void)
{
 base->create(m_hwnd);
}
void TquantTablesPage::cfg2dlg(void)
{
 base->cfg2dlg();
}
void TquantTablesPage::translate(void)
{
 TconfPageEnc::translate();
 base->translate();
}

int TquantTablesPage::TquantTables::getCodecId(void)
{
 return cfgGet(IDFF_enc_codecId);
}
HRESULT TquantTablesPage::TquantTables::getCustomQuantMatrixes(uint8_t* *intra8,uint8_t* *inter8,uint8_t* *intra4Y,uint8_t* *inter4Y,uint8_t* *intra4C,uint8_t* *inter4C)
{
 return deciE->getCustomQuantMatrixes(intra8,inter8,intra4Y,inter4Y,intra4C,inter4C);
}
void TquantTablesPage::TquantTables::setChange(void)
{
 parent->setChange();
}

//============================================ TcurrentQuantDlg ============================================
TcurrentQuantDlg::TcurrentQuantDlg(HWND IhParent,IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra,TquantTables *Ibase):
 TdlgWindow(IDD_QUANT_CURRENT,Ideci),
 base(Ibase?Ibase:new TquantTables(Ideci,Iinter,Iintra)),
 hParent(IhParent)
{
}
TcurrentQuantDlg::~TcurrentQuantDlg()
{
 delete base;
}
void TcurrentQuantDlg::show(void)
{
 dialogBox(dialogId,hParent);
}
void TcurrentQuantDlg::init(void)
{
 base->create(m_hwnd);
 translate();
 base->cfg2dlg();
}
void TcurrentQuantDlg::translate(void)
{
 TdlgWindow::translate();
 base->translate();
}

INT_PTR TcurrentQuantDlg::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDCLOSE:
      case IDCANCEL:
       EndDialog(m_hwnd,LOWORD(wParam));
       return TRUE;
     }
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

TcurrentQuantDlg::TquantTables::TquantTables(IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra):
 inter(Iinter),intra(Iintra),
 TquantTablesPageBase(Ideci)
{
}
int TcurrentQuantDlg::TquantTables::getCodecId(void)
{
 return deci->getCurrentCodecId2();
}
HRESULT TcurrentQuantDlg::TquantTables::getCustomQuantMatrixes(uint8_t* *intra8,uint8_t* *inter8,uint8_t* *intra4Y,uint8_t* *inter4Y,uint8_t* *intra4C,uint8_t* *inter4C)
{
 if (intra8) *intra8=intra;
 if (inter8) *inter8=inter;
 return S_OK;
}
void TcurrentQuantDlg::TquantTables::cfg2dlg(void)
{
 TquantTablesPageBase::cfg2dlg();
 enable(0,IDC_BT_QUANT_TABLE_LOAD);
 for (int i=0;idIntra8[i];i++)
  edReadOnly(idIntra8[i],true);
 for (int i=0;idInter8[i];i++)
  edReadOnly(idInter8[i],true);
}

//============================================== TdctQuantDlg ==============================================
const int TdctQuantDlg::idffs[16]={IDFF_dctMatrix0,IDFF_dctMatrix1,IDFF_dctMatrix2,IDFF_dctMatrix3,IDFF_dctMatrix4,IDFF_dctMatrix5,IDFF_dctMatrix6,IDFF_dctMatrix7,IDFF_dctMatrix8,IDFF_dctMatrix9,IDFF_dctMatrix10,IDFF_dctMatrix11,IDFF_dctMatrix12,IDFF_dctMatrix13,IDFF_dctMatrix14,IDFF_dctMatrix15};

TdctQuantDlg::TdctQuantDlg(HWND hParent,IffdshowBase *Ideci):TcurrentQuantDlg(hParent,Ideci,NULL,NULL,new TdctQuantTables(Ideci,inter,NULL))
{
 uint8_t *m=inter;
 for (int i=0;i<16;i++,m+=4)
  {
   int32_t val=cfgGet(idffs[i]);
   memcpy(m,&val,sizeof(val));
  }
}

TdctQuantDlg::TdctQuantTables::TdctQuantTables(IffdshowBase *Ideci,uint8_t *Iinter,uint8_t *Iintra):TquantTables(Ideci,Iinter,Iintra)
{
}
void TdctQuantDlg::TdctQuantTables::cfg2dlg(void)
{
 TquantTablesPageBase::cfg2dlg();
}
void TdctQuantDlg::TdctQuantTables::setChange(void)
{
 uint8_t *m=inter;
 for (int i=0;i<16;i++,m+=4)
  {
   int32_t val;
   memcpy(&val,m,sizeof(val));
   cfgSet(idffs[i],val);
  }
}
