
/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

//AVC
#include "type.h"
#include "data.h"
#include "init_data.h"
#include "write_back.h"
#include "fill_caches.h"

//SVC
#include "svc_data.h"
#include "WriteBackSVC.h"




/**
Load motion vectors and reference indexes into local tables.
*/
void write_back_motion_cache_B_full_ref_C( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, 
										  short *ai_tiMv_l0, short *ai_tiMv_l1, short ai_tiMv_cache_l0[ ][2], 
										  short ai_tiMv_cache_l1[ ][2], short *ai_tiRef_l0, short *ai_tiRef_l1, 
										  short *aio_tiRef_cache_l0, short *aio_tiRef_cache_l1, int mb_stride)
{
	
	//Updating the adress of the macroblock which belong to the neighbourhood.
    const int top_xy = ai_iMb_xy - mb_stride  ;
    const int topleft_xy = top_xy - 1 ;
	const int topright_xy = top_xy >= 0 ? top_xy + 1:-1 ;
    const int left_xy = ai_iMb_xy - 1 ;


	short *MvdL0 = ai_tiMv_l0;
	short *MvdL1 = ai_tiMv_l1;
	short *RefL0 = ai_tiRef_l0;
	short *RefL1 = ai_tiRef_l1;
	short y;

	for(y = 0; y < 4; y++, RefL0 += ai_iB8_stride, RefL1 += ai_iB8_stride){
		FillFullRefCacheCurrPosition(&aio_tiRef_cache_l0[12 + 8 * y], RefL0);
		FillFullRefCacheCurrPosition(&aio_tiRef_cache_l1[12 + 8 * y], RefL1);
	}
		
		

	for(y = 0; y < 4; y++, MvdL0 += ai_iB_stride, MvdL1 += ai_iB_stride){
		FillMvCacheCurrPosition(&ai_tiMv_cache_l0[8 * y], MvdL0);
		FillMvCacheCurrPosition(&ai_tiMv_cache_l1[8 * y], MvdL1);
	}


	//In case of a skipped or a P macroblock
    if(top_xy >= 0)   {
		FillMvCacheTopPosition(ai_tiMv_cache_l0, &ai_tiMv_l0[- ai_iB_stride]);
		FillMvCacheTopPosition(ai_tiMv_cache_l1, &ai_tiMv_l1[- ai_iB_stride]);
		FillFullRefCacheCurrPosition(&aio_tiRef_cache_l0[4], &ai_tiRef_l0[- ai_iB8_stride]);
		FillFullRefCacheCurrPosition(&aio_tiRef_cache_l1[4], &ai_tiRef_l1[- ai_iB8_stride]);
    }
    else    {
        FillRefCacheTopPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	    FillRefCacheTopPosition(aio_tiRef_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
    }

	//In case of a skipped or a P macroblock
    if(left_xy >= 0)    {
		FillMvCacheLeftPosition(ai_tiMv_cache_l0, &ai_tiMv_l0[-2], ai_iB_stride);
		FillMvCacheLeftPosition(ai_tiMv_cache_l1, &ai_tiMv_l1[-2], ai_iB_stride);

        aio_tiRef_cache_l0[11] = ai_tiRef_l0[-1];
		aio_tiRef_cache_l0[19] = ai_tiRef_l0[-1 + ai_iB8_stride];
		aio_tiRef_cache_l0[27] = ai_tiRef_l0[-1 + 2 * ai_iB8_stride];
		aio_tiRef_cache_l0[35] = ai_tiRef_l0[-1 + 3 * ai_iB8_stride];

		aio_tiRef_cache_l1[11] = ai_tiRef_l1[-1];
		aio_tiRef_cache_l1[19] = ai_tiRef_l1[-1 + ai_iB8_stride];
		aio_tiRef_cache_l1[27] = ai_tiRef_l1[-1 + 2 * ai_iB8_stride];
		aio_tiRef_cache_l1[35] = ai_tiRef_l1[-1 + 3 * ai_iB8_stride];


    }
    else     {
		FillRefCacheLeftPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	    FillRefCacheLeftPosition(aio_tiRef_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
    }


	//In case of a skipped or a P macroblock
    if(topleft_xy >= 0){
        ai_tiMv_cache_l0 [12 - 1 - 8][0] = ai_tiMv_l0[-2 - ai_iB_stride];
		ai_tiMv_cache_l0 [12 - 1 - 8][1] = ai_tiMv_l0[-1 - ai_iB_stride];

		ai_tiMv_cache_l1 [12 - 1 - 8][0] = ai_tiMv_l1[-2 - ai_iB_stride];
		ai_tiMv_cache_l1 [12 - 1 - 8][1] = ai_tiMv_l1[-1 - ai_iB_stride];

		aio_tiRef_cache_l0[12 - 1 - 8] = ai_tiRef_l0[-1 - ai_iB8_stride];
		aio_tiRef_cache_l1[12 - 1 - 8] = ai_tiRef_l1[-1 - ai_iB8_stride];
    }
	else     {
        aio_tiRef_cache_l0[12 - 1 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
    }


	//In case of a skipped or a P macroblock
    if(topright_xy >= 0)    {
        ai_tiMv_cache_l0 [12 + 4 - 8][0] = ai_tiMv_l0[8 - ai_iB_stride];
		ai_tiMv_cache_l0 [12 + 4 - 8][1] = ai_tiMv_l0[9 - ai_iB_stride];

		ai_tiMv_cache_l1 [12 + 4 - 8][0] = ai_tiMv_l1[8 - ai_iB_stride];
		ai_tiMv_cache_l1 [12 + 4 - 8][1] = ai_tiMv_l1[9 - ai_iB_stride];

        aio_tiRef_cache_l0[12 + 4 - 8] = ai_tiRef_l0[2 - ai_iB8_stride];
        aio_tiRef_cache_l1[12 + 4 - 8] = ai_tiRef_l1[2 - ai_iB8_stride];
    }
    else     {
        aio_tiRef_cache_l0[12 + 4 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
    }
}


#ifdef TI_OPTIM
void write_back_motion_cache_B_full_ref_TI( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, 
										  short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short  ai_tiMv_cache_l0[ ][2], 
										  short  ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], 
										  short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ],int mb_stride)
{
	
	//Updating the adress of the macroblock which belong to the neighbourhood.
    const int top_xy = ai_iMb_xy - mb_stride  ;
    const int topleft_xy = top_xy - 1 ;
	const int topright_xy = top_xy >= 0 ? top_xy + 1:-1 ;
    const int left_xy = ai_iMb_xy - 1 ;


	int ai_iB8_stride_2 = ai_iB8_stride<<1;
	int ai_iB8_stride_3 = ai_iB8_stride_2 + ai_iB8_stride;
	int ai_iB_stride_2 = ai_iB_stride<<1;
	int ai_iB_stride_3 = ai_iB_stride_2 + ai_iB_stride;
	//short y;
	
	
	//Iteration 0
	_mem8((int*)aio_tiRef_cache_l0+6)=_itoll(_pack2(_mem4(ai_tiRef_l0+3),_mem4(ai_tiRef_l0+2)),_pack2(_mem4(ai_tiRef_l0+1),_mem4(ai_tiRef_l0)));
	_mem8((int*)aio_tiRef_cache_l0+10)=_itoll(_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride+3),_mem4(ai_tiRef_l0+ai_iB8_stride+2)),_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride+1),_mem4(ai_tiRef_l0+ai_iB8_stride)));
	_mem8((int*)aio_tiRef_cache_l0+14)=_itoll(_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride_2+3),_mem4(ai_tiRef_l0+ai_iB8_stride_2+2)),_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride_2+1),_mem4(ai_tiRef_l0+ai_iB8_stride_2)));
	_mem8((int*)aio_tiRef_cache_l0+18)=_itoll(_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride_3+3),_mem4(ai_tiRef_l0+ai_iB8_stride_3+2)),_pack2(_mem4(ai_tiRef_l0+ai_iB8_stride_3+1),_mem4(ai_tiRef_l0+ai_iB8_stride_3)));
	

	_mem8((int*)aio_tiRef_cache_l1+6)=_itoll(_pack2(_mem4(ai_tiRef_l1+3),_mem4(ai_tiRef_l1+2)),_pack2(_mem4(ai_tiRef_l1+1),_mem4(ai_tiRef_l1)));
	_mem8((int*)aio_tiRef_cache_l1+10)=_itoll(_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride+3),_mem4(ai_tiRef_l1+ai_iB8_stride+2)),_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride+1),_mem4(ai_tiRef_l1+ai_iB8_stride)));
	_mem8((int*)aio_tiRef_cache_l1+14)=_itoll(_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride_2+3),_mem4(ai_tiRef_l1+ai_iB8_stride_2+2)),_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride_2+1),_mem4(ai_tiRef_l1+ai_iB8_stride_2)));
	_mem8((int*)aio_tiRef_cache_l1+18)=_itoll(_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride_3+3),_mem4(ai_tiRef_l1+ai_iB8_stride_3+2)),_pack2(_mem4(ai_tiRef_l1+ai_iB8_stride_3+1),_mem4(ai_tiRef_l1+ai_iB8_stride_3)));
	
	//iteracion1
	_mem8((int*)ai_tiMv_cache_l0+12)=_itoll(_pack2(_mem4(ai_tiMv_l0+3),_mem4(ai_tiMv_l0+2)),_pack2(_mem4(ai_tiMv_l0+1),_mem4(ai_tiMv_l0)));
	_mem8((int*)ai_tiMv_cache_l0+14)=_itoll(_pack2(_mem4(ai_tiMv_l0+7),_mem4(ai_tiMv_l0+6)),_pack2(_mem4(ai_tiMv_l0+5),_mem4(ai_tiMv_l0+4)));
	
	_mem8((int*)ai_tiMv_cache_l1+12)=_itoll(_pack2(_mem4(ai_tiMv_l1+3),_mem4(ai_tiMv_l1+2)),_pack2(_mem4(ai_tiMv_l1+1),_mem4(ai_tiMv_l1)));
	_mem8((int*)ai_tiMv_cache_l1+14)=_itoll(_pack2(_mem4(ai_tiMv_l1+7),_mem4(ai_tiMv_l1+6)),_pack2(_mem4(ai_tiMv_l1+5),_mem4(ai_tiMv_l1+4)));

	//iteracion2
	_mem8((int*)ai_tiMv_cache_l0+20)=_itoll(_pack2(_mem4(ai_tiMv_l0+ai_iB_stride+3),_mem4(ai_tiMv_l0+ai_iB_stride+2)),_pack2(_mem4(ai_tiMv_l0+ai_iB_stride+1),_mem4(ai_tiMv_l0+ai_iB_stride)));
	_mem8((int*)ai_tiMv_cache_l0+22)=_itoll(_pack2(_mem4(ai_tiMv_l0+ai_iB_stride+7),_mem4(ai_tiMv_l0+ai_iB_stride+6)),_pack2(_mem4(ai_tiMv_l0+ai_iB_stride+5),_mem4(ai_tiMv_l0+ai_iB_stride+4)));
	
	_mem8((int*)ai_tiMv_cache_l1+20)=_itoll(_pack2(_mem4(ai_tiMv_l1+ai_iB_stride+3),_mem4(ai_tiMv_l1+ai_iB_stride+2)),_pack2(_mem4(ai_tiMv_l1+ai_iB_stride+1),_mem4(ai_tiMv_l1+ai_iB_stride)));
	_mem8((int*)ai_tiMv_cache_l1+22)=_itoll(_pack2(_mem4(ai_tiMv_l1+ai_iB_stride+7),_mem4(ai_tiMv_l1+ai_iB_stride+6)),_pack2(_mem4(ai_tiMv_l1+ai_iB_stride+5),_mem4(ai_tiMv_l1+ai_iB_stride+4)));

	//iteracion3
	_mem8((int*)ai_tiMv_cache_l0+28)=_itoll(_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_2)+3),_mem4(ai_tiMv_l0+(ai_iB_stride_2)+2)),_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_2)+1),_mem4(ai_tiMv_l0+(ai_iB_stride_2))));
	_mem8((int*)ai_tiMv_cache_l0+30)=_itoll(_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_2)+7),_mem4(ai_tiMv_l0+(ai_iB_stride_2)+6)),_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_2)+5),_mem4(ai_tiMv_l0+(ai_iB_stride_2)+4)));
	
	_mem8((int*)ai_tiMv_cache_l1+28)=_itoll(_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_2)+3),_mem4(ai_tiMv_l1+(ai_iB_stride_2)+2)),_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_2)+1),_mem4(ai_tiMv_l1+(ai_iB_stride_2))));
	_mem8((int*)ai_tiMv_cache_l1+30)=_itoll(_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_2)+7),_mem4(ai_tiMv_l1+(ai_iB_stride_2)+6)),_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_2)+5),_mem4(ai_tiMv_l1+(ai_iB_stride_2)+4)));

	//iteracion4
	_mem8((int*)ai_tiMv_cache_l0+36)=_itoll(_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_3)+3),_mem4(ai_tiMv_l0+(ai_iB_stride_3)+2)),_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_3)+1),_mem4(ai_tiMv_l0+(ai_iB_stride_3))));
	_mem8((int*)ai_tiMv_cache_l0+38)=_itoll(_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_3)+7),_mem4(ai_tiMv_l0+(ai_iB_stride_3)+6)),_pack2(_mem4(ai_tiMv_l0+(ai_iB_stride_3)+5),_mem4(ai_tiMv_l0+(ai_iB_stride_3)+4)));
	
	_mem8((int*)ai_tiMv_cache_l1+36)=_itoll(_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_3)+3),_mem4(ai_tiMv_l1+(ai_iB_stride_3)+2)),_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_3)+1),_mem4(ai_tiMv_l1+(ai_iB_stride_3))));
	_mem8((int*)ai_tiMv_cache_l1+38)=_itoll(_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_3)+7),_mem4(ai_tiMv_l1+(ai_iB_stride_3)+6)),_pack2(_mem4(ai_tiMv_l1+(ai_iB_stride_3)+5),_mem4(ai_tiMv_l1+(ai_iB_stride_3)+4)));



	//In case of a skipped or a P macroblock
    if(top_xy >= 0)   {


		
		
		_mem8((int*)ai_tiMv_cache_l0+4)=_itoll(_pack2(_mem4((&ai_tiMv_l0[- ai_iB_stride])+3),_mem4((&ai_tiMv_l0[- ai_iB_stride])+2)),_pack2(_mem4((&ai_tiMv_l0[- ai_iB_stride])+1),_mem4((&ai_tiMv_l0[- ai_iB_stride]))));
		_mem8((int*)ai_tiMv_cache_l0+6)=_itoll(_pack2(_mem4((&ai_tiMv_l0[- ai_iB_stride])+7),_mem4((&ai_tiMv_l0[- ai_iB_stride])+6)),_pack2(_mem4((&ai_tiMv_l0[- ai_iB_stride])+5),_mem4((&ai_tiMv_l0[- ai_iB_stride])+4)));
		_mem8((int*)ai_tiMv_cache_l1+4)=_itoll(_pack2(_mem4((&ai_tiMv_l1[- ai_iB_stride])+3),_mem4((&ai_tiMv_l1[- ai_iB_stride])+2)),_pack2(_mem4((&ai_tiMv_l1[- ai_iB_stride])+1),_mem4((&ai_tiMv_l1[- ai_iB_stride]))));
		_mem8((int*)ai_tiMv_cache_l1+6)=_itoll(_pack2(_mem4((&ai_tiMv_l1[- ai_iB_stride])+7),_mem4((&ai_tiMv_l1[- ai_iB_stride])+6)),_pack2(_mem4((&ai_tiMv_l1[- ai_iB_stride])+5),_mem4((&ai_tiMv_l1[- ai_iB_stride])+4)));
		_mem8((int*)aio_tiRef_cache_l0+2)=_itoll(_pack2(_mem4((&ai_tiRef_l0[- ai_iB8_stride])+3),_mem4((&ai_tiRef_l0[- ai_iB8_stride])+2)),_pack2(_mem4((&ai_tiRef_l0[- ai_iB8_stride])+1),_mem4(&ai_tiRef_l0[- ai_iB8_stride])));
		_mem8((int*)aio_tiRef_cache_l1+2)=_itoll(_pack2(_mem4((&ai_tiRef_l1[- ai_iB8_stride])+3),_mem4((&ai_tiRef_l1[- ai_iB8_stride])+2)),_pack2(_mem4((&ai_tiRef_l1[- ai_iB8_stride])+1),_mem4((&ai_tiRef_l1[- ai_iB8_stride]))));

	
	  

    }
    else
    {
        
		_mem8((int*)aio_tiRef_cache_l0+4)=0xFFFEFFFEFFFEFFFE;
		_mem8((int*)aio_tiRef_cache_l1+4)=0xFFFEFFFEFFFEFFFE;
    }

	//In case of a skipped or a P macroblock
    if(left_xy >= 0)    
    {

		_mem4(ai_tiMv_cache_l0+11)=_pack2(_mem4(&ai_tiMv_l0[-2]+1),_mem4(&ai_tiMv_l0[-2]));
		_mem4(ai_tiMv_cache_l0+19)=_pack2(_mem4(&ai_tiMv_l0[-2]+ai_iB_stride+1),_mem4(&ai_tiMv_l0[-2]+ai_iB_stride));
		_mem4(ai_tiMv_cache_l0+27)=_pack2(_mem4(&ai_tiMv_l0[-2]+(ai_iB_stride_2)+1),_mem4(&ai_tiMv_l0[-2]+(ai_iB_stride_2)));
		_mem4(ai_tiMv_cache_l0+35)=_pack2(_mem4(&ai_tiMv_l0[-2]+(ai_iB_stride_3)+1),_mem4(&ai_tiMv_l0[-2]+(ai_iB_stride_3)));
		
		//cache_L1
		_mem4(ai_tiMv_cache_l1+11)=_pack2(_mem4(&ai_tiMv_l1[-2]+1),_mem4(&ai_tiMv_l1[-2]));
		_mem4(ai_tiMv_cache_l1+19)=_pack2(_mem4(&ai_tiMv_l1[-2]+ai_iB_stride+1),_mem4(&ai_tiMv_l1[-2]+ai_iB_stride));
		_mem4(ai_tiMv_cache_l1+27)=_pack2(_mem4(&ai_tiMv_l1[-2]+(ai_iB_stride_2)+1),_mem4(&ai_tiMv_l1[-2]+(ai_iB_stride_2)));
		_mem4(ai_tiMv_cache_l1+35)=_pack2(_mem4(&ai_tiMv_l1[-2]+(ai_iB_stride_3)+1),_mem4(&ai_tiMv_l1[-2]+(ai_iB_stride_3)));
		

		aio_tiRef_cache_l0[11] = (short) ai_tiRef_l0[-1];
		aio_tiRef_cache_l0[19] = (short) ai_tiRef_l0[-1  + ai_iB8_stride];
		aio_tiRef_cache_l0[27] = (short) ai_tiRef_l0[-1  + ai_iB8_stride_2];
		aio_tiRef_cache_l0[35] = (short) ai_tiRef_l0[-1  + ai_iB8_stride_3];

		aio_tiRef_cache_l1[11] = (short) ai_tiRef_l1[-1];
		aio_tiRef_cache_l1[19] = (short) ai_tiRef_l1[-1  + ai_iB8_stride];
		aio_tiRef_cache_l1[27] = (short) ai_tiRef_l1[-1  + ai_iB8_stride_2];
		aio_tiRef_cache_l1[35] = (short) ai_tiRef_l1[-1  + ai_iB8_stride_3];


    }
    else 
    {
        //ESB No Optimizable
        aio_tiRef_cache_l0[12 - 1 + 16] = 
        aio_tiRef_cache_l0[12 - 1 + 24] = 
        aio_tiRef_cache_l0[12 - 1] = 
        aio_tiRef_cache_l0[12 - 1 + 8] = PART_NOT_AVAILABLE;
		
		//cache L1
		aio_tiRef_cache_l1[12 - 1 + 16] = 
		aio_tiRef_cache_l1[12 - 1 + 24] = 
        aio_tiRef_cache_l1[12 - 1] = 
        aio_tiRef_cache_l1[12 - 1 + 8] = PART_NOT_AVAILABLE;
    }


	//In case of a skipped or a P macroblock
    if(topleft_xy >= 0)
    {

		_mem4(ai_tiMv_cache_l0+3)=_pack2(_mem4(&ai_tiMv_l0[-2 - ai_iB_stride]),_mem4(&ai_tiMv_l0[-1 - ai_iB_stride]));
		//Cache l1
		_mem4(ai_tiMv_cache_l1+3)=_pack2(_mem4(&ai_tiMv_l1[-2 - ai_iB_stride]),_mem4(&ai_tiMv_l1[-1 - ai_iB_stride]));

		aio_tiRef_cache_l0[12 - 1 - 8] = (short) ai_tiRef_l0[-1 - ai_iB8_stride];
		aio_tiRef_cache_l1[12 - 1 - 8] = (short) ai_tiRef_l1[-1 - ai_iB8_stride];
    }
	else 
    {
        aio_tiRef_cache_l0[12 - 1 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
    }


	//In case of a skipped or a P macroblock
    if(topright_xy >= 0)    
    {

		_mem4(ai_tiMv_cache_l0+8)=_pack2(_mem4(&ai_tiMv_l0[9 - ai_iB_stride]),_mem4(&ai_tiMv_l0[8 - ai_iB_stride]));

		_mem4(ai_tiMv_cache_l1+8)=_pack2(_mem4(&ai_tiMv_l1[9 - ai_iB_stride]),_mem4(&ai_tiMv_l1[8 - ai_iB_stride]));

        aio_tiRef_cache_l0[12 + 4 - 8] = (short) ai_tiRef_l0[2 - ai_iB8_stride];
        aio_tiRef_cache_l1[12 + 4 - 8] = (short) ai_tiRef_l1[2 - ai_iB8_stride];
    }
    else 
    {
        aio_tiRef_cache_l0[12 + 4 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
    }
}
#endif

