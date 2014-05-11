/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Alain Maccari <amaccari@ens.insa-rennes.fr>               
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
   * License along with this not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/
#include <math.h>







#ifdef MMXi_

#include "clip.h"
#include "edge_filter.h"

//mbl test with Pocket_PC		
void weak_horizontal_luma_MMX(unsigned char pix[], const int xstride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0){

	__m64 mp2 = _mm_set_pi16(pix[-3*xstride + 3], pix[-3*xstride + 2], pix[-3*xstride + 1], pix[-3*xstride]);
	__m64 mp1 = _mm_set_pi16(pix[-2*xstride + 3], pix[-2*xstride + 2], pix[-2*xstride + 1], pix[-2*xstride]); 
	__m64 mp0 = _mm_set_pi16(pix[-1*xstride + 3], pix[-1*xstride + 2], pix[-1*xstride + 1], pix[-1*xstride]); 
	__m64 mq0 = _mm_set_pi16(pix[3], pix[2], pix[1], pix[0]);
	__m64 mq1 = _mm_set_pi16(pix[xstride + 3], pix[xstride + 2], pix[xstride + 1], pix[xstride]);
	__m64 mq2 = _mm_set_pi16(pix[2*xstride + 3], pix[2*xstride + 2], pix[2*xstride + 1], pix[2*xstride]);
	
	__m64 mrshift = _mm_set_pi16(0,0,0,1);
	__m64 maddp0_q0 =  _mm_avg_pu8(mp0,mq0); //addp0_q0 = (p0 + q0 + 1) >> 1;
	__m64 maddp2 = _mm_add_pi16(maddp0_q0,mp2); //addp2 = (p2 + addp0_q0);
	__m64 maddq2 = _mm_add_pi16(maddp0_q0,mq2); //addp2 = (p2 + addp0_q0);
	__m64 maddp2_s = _mm_srl_pi16(maddp2,mrshift); //addp2 = (p2 + addp0_q0) >> 1;
	__m64 maddq2_s = _mm_srl_pi16(maddq2,mrshift); //addp2 = (p2 + addp0_q0) >> 1;
	__m64 mp1_c = _mm_sub_pi16(maddp2_s, mp1); //addp2 - p1
	__m64 mq1_c = _mm_sub_pi16(maddq2_s, mq1); // addq2 - q1



	//To calculate the mask
 	__m64 malpha = _mm_set_pi16(alpha,alpha,alpha,alpha);
	__m64 malphab = _mm_set_pi16(-alpha,-alpha,-alpha,-alpha);
	__m64 mbeta = _mm_set_pi16(beta,beta,beta,beta);
	__m64 mbetab = _mm_set_pi16(-beta,-beta,-beta,-beta);
	

	__m64 mdiff_p0_q0 = _mm_sub_pi16(mq0,mp0); //abs(q0 - p0)
	__m64 mdiff_p1_p0 = _mm_sub_pi16(mp0,mp1); //abs(p1 - p0)
	__m64 mdiff_q1_q0 = _mm_sub_pi16(mq0, mq1); //abs(q1 - q0)
	__m64 mdiff_p2_p0 = _mm_sub_pi16(mp2,mp0); //abs(p2 - p0 ))
	__m64 mdiff_q2_q0 = _mm_sub_pi16(mq2,mq0); //abs(q2 - q0)

 	__m64 mask0 = _mm_and_si64( _mm_cmpgt_pi16(malpha, mdiff_p0_q0), _mm_cmpgt_pi16(mdiff_p0_q0,malphab));
	__m64 mask1 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_p1_p0), _mm_cmpgt_pi16(mdiff_p1_p0,mbetab));
	__m64 mask2 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_q1_q0), _mm_cmpgt_pi16(mdiff_q1_q0,mbetab));
	__m64 mask3 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_p2_p0), _mm_cmpgt_pi16(mdiff_p2_p0,mbetab));
	__m64 mask4 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_q2_q0), _mm_cmpgt_pi16(mdiff_q2_q0,mbetab));
	
	__m64 first_mask = _mm_and_si64 (_mm_and_si64 (mask0,mask1),mask2); //(abs(q0 - p0) < alpha) && (abs(p1 - p0) < beta) && (abs(q1 - q0) < beta)
	__m64 second_mask = _mm_and_si64 (first_mask,mask3);
	__m64 third_mask = _mm_and_si64 (first_mask,mask4);


	__m64 mdiff_q0_p0 = _mm_sub_pi16(mq0,mp0); //(q0 - p0) 
	__m64 mlshift = _mm_set_pi16(0,0,0,2);
	__m64 minter_1 = _mm_sll_pi16(mdiff_q0_p0, mlshift);//inter_1 = (q0 - p0 ) << 2;
	__m64 minter_2 = _mm_sub_pi16(mp1, mq1);//(p1 - q1)
	__m64 madd4 = _mm_set_pi16(4,4,4,4);
	__m64 minter_3 = _mm_add_pi16(minter_2, madd4);//inter_2 = (p1 - q1) + 4;
	__m64 minter_4 = _mm_add_pi16(minter_3,minter_1); //(inter_1 +  inter_2)
	__m64 mrshift3 = _mm_set_pi16(0,0,0,3);
	__m64 minter5 = _mm_sra_pi16(minter_4, mrshift3);



	//Clip3
	__m64 m_tc0 = _mm_set_pi16(tc0,tc0,tc0,tc0);
	__m64 m_tcb0 = _mm_set_pi16(-tc0,-tc0,-tc0,-tc0);
	__m64 mres_c1 = _mm_min_pi16(_mm_max_pi16(mp1_c,m_tcb0),m_tc0); //CLIP3(-tc0, tc0, addp2 - p1 );
	__m64 mres_c2 = _mm_min_pi16(_mm_max_pi16(mq1_c,m_tcb0),m_tc0); //CLIP3(-tc0, tc0, addq2 - q1 );
	__m64 merror0 = _mm_and_si64 (mres_c1,second_mask);
	__m64 merror1 = _mm_and_si64 (mres_c2,third_mask);


	__m64 m_1 = _mm_set_pi16(1,1,1,1);
	__m64 m_and1 = _mm_and_si64 (mask3, m_1); //tc++; if abs( p2 - p0 ) < beta 
	__m64 m_and2 = _mm_and_si64 (mask4, m_1); //tc++; if abs( q2 - q0  ) < beta 
	__m64 m_tc = _mm_add_pi16(m_and2,_mm_add_pi16(m_tc0,m_and1));
	__m64 m_tcn =_mm_sub_pi16(_mm_sub_pi16(m_tcb0,m_and1),m_and2);
	__m64 mres_c3 = _mm_min_pi16(_mm_max_pi16(minter5,m_tcn),m_tc); //CLIP3(-tc0, tc0, addp2 - p1 );
	__m64 merror2 = _mm_and_si64 (mres_c3,first_mask);


	__m64 result_p1 = _mm_add_pi16(merror0,mp1); //_mm_shuffle_pi16(_mm_add_pi16(merror0,mp1), 0x1B);
	__m64 result_q1 = _mm_add_pi16(merror1,mq1); //_mm_shuffle_pi16(_mm_add_pi16(merror1,mq1), 0x1B);
	__m64 result_p0 = _mm_add_pi16(merror2,mp0); //_mm_shuffle_pi16(_mm_add_pi16(merror2,mq1), 0x1B);
	__m64 result_q0 = _mm_sub_pi16(mq0, merror2);//_mm_shuffle_pi16(_mm_sub_pi16(mq1, merror2), 0x1B);

	*((unsigned int* )(&pix[-2*xstride])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_p1,mrshift));
	*((unsigned int* )(&pix[-xstride])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_p0,mrshift));
	*((unsigned int* )(&pix[0])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_q0,mrshift));
	*((unsigned int* )(&pix[xstride])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_q1,mrshift));

	empty();  
}




void weak_vertical_luma_MMX(unsigned char pix[], const int ystride,const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{
    
	
	__m64 mp2 = _mm_set_pi16(pix[-3 + 3 * ystride], pix[-3 + 2 * ystride], pix[-3 + ystride], pix[-3]);
	__m64 mp1 = _mm_set_pi16(pix[-2 + 3 * ystride], pix[-2 + 2 * ystride], pix[-2 + ystride], pix[-2]); 
	__m64 mp0 = _mm_set_pi16(pix[-1 + 3 * ystride], pix[-1 + 2 * ystride], pix[-1 + ystride], pix[-1]); 
	__m64 mq0 = _mm_set_pi16(pix[3 * ystride], pix[2 * ystride], pix[ystride], pix[0]);
	__m64 mq1 = _mm_set_pi16(pix[1 + 3 * ystride], pix[1 + 2 * ystride], pix[1 + ystride], pix[1]);
	__m64 mq2 = _mm_set_pi16(pix[2 + 3 * ystride], pix[2 + 2 * ystride], pix[2 + ystride], pix[2]);



	__m64 mrshift = _mm_set_pi16(0,0,0,1);
	__m64 maddp0_q0 =  _mm_avg_pu8(mp0,mq0); //addp0_q0 = (p0 + q0 + 1) >> 1;
	__m64 maddp2 = _mm_add_pi16(maddp0_q0,mp2); //addp2 = (p2 + addp0_q0);
	__m64 maddq2 = _mm_add_pi16(maddp0_q0,mq2); //addp2 = (p2 + addp0_q0);
	__m64 maddp2_s = _mm_srl_pi16(maddp2,mrshift); //addp2 = (p2 + addp0_q0) >> 1;
	__m64 maddq2_s = _mm_srl_pi16(maddq2,mrshift); //addp2 = (p2 + addp0_q0) >> 1;
	__m64 mp1_c = _mm_sub_pi16(maddp2_s, mp1); //addp2 - p1
	__m64 mq1_c = _mm_sub_pi16(maddq2_s, mq1); // addq2 - q1



	//To calculate the mask
 	__m64 malpha = _mm_set_pi16(alpha,alpha,alpha,alpha);
	__m64 malphab = _mm_set_pi16(-alpha,-alpha,-alpha,-alpha);
	__m64 mbeta = _mm_set_pi16(beta,beta,beta,beta);
	__m64 mbetab = _mm_set_pi16(-beta,-beta,-beta,-beta);
	

	__m64 mdiff_p0_q0 = _mm_sub_pi16(mq0,mp0); //abs(q0 - p0)
	__m64 mdiff_p1_p0 = _mm_sub_pi16(mp0,mp1); //abs(p1 - p0)
	__m64 mdiff_q1_q0 = _mm_sub_pi16(mq0, mq1); //abs(q1 - q0)
	__m64 mdiff_p2_p0 = _mm_sub_pi16(mp2,mp0); //abs(p2 - p0 ))
	__m64 mdiff_q2_q0 = _mm_sub_pi16(mq2,mq0); //abs(q2 - q0)

 	__m64 mask0 = _mm_and_si64( _mm_cmpgt_pi16(malpha, mdiff_p0_q0), _mm_cmpgt_pi16(mdiff_p0_q0,malphab));
	__m64 mask1 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_p1_p0), _mm_cmpgt_pi16(mdiff_p1_p0,mbetab));
	__m64 mask2 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_q1_q0), _mm_cmpgt_pi16(mdiff_q1_q0,mbetab));
	__m64 mask3 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_p2_p0), _mm_cmpgt_pi16(mdiff_p2_p0,mbetab));
	__m64 mask4 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_q2_q0), _mm_cmpgt_pi16(mdiff_q2_q0,mbetab));
	
	__m64 first_mask = _mm_and_si64 (_mm_and_si64 (mask0,mask1),mask2); //(abs(q0 - p0) < alpha) && (abs(p1 - p0) < beta) && (abs(q1 - q0) < beta)
	__m64 second_mask = _mm_and_si64 (first_mask,mask3);
	__m64 third_mask = _mm_and_si64 (first_mask,mask4);


	__m64 mdiff_q0_p0 = _mm_sub_pi16(mq0,mp0); //(q0 - p0) 
	__m64 mlshift = _mm_set_pi16(0,0,0,2);
	__m64 minter_1 = _mm_sll_pi16(mdiff_q0_p0, mlshift);//inter_1 = (q0 - p0 ) << 2;
	__m64 minter_2 = _mm_sub_pi16(mp1, mq1);//(p1 - q1)
	__m64 madd4 = _mm_set_pi16(4,4,4,4);
	__m64 minter_3 = _mm_add_pi16(minter_2, madd4);//inter_2 = (p1 - q1) + 4;
	__m64 minter_4 = _mm_add_pi16(minter_3,minter_1); //(inter_1 +  inter_2)
	__m64 mrshift3 = _mm_set_pi16(0,0,0,3);
	__m64 minter5 = _mm_sra_pi16(minter_4, mrshift3);



	//Clip3
	__m64 m_tc0 = _mm_set_pi16(tc0,tc0,tc0,tc0);
	__m64 m_tcb0 = _mm_set_pi16(-tc0,-tc0,-tc0,-tc0);
	__m64 mres_c1 = _mm_min_pi16(_mm_max_pi16(mp1_c,m_tcb0),m_tc0); //CLIP3(-tc0, tc0, addp2 - p1 );
	__m64 mres_c2 = _mm_min_pi16(_mm_max_pi16(mq1_c,m_tcb0),m_tc0); //CLIP3(-tc0, tc0, addq2 - q1 );
	__m64 merror0 = _mm_and_si64 (mres_c1,second_mask);
	__m64 merror1 = _mm_and_si64 (mres_c2,third_mask);


	__m64 m_1 = _mm_set_pi16(1,1,1,1);
	__m64 m_and1 = _mm_and_si64 (mask3, m_1); //tc++; if abs( p2 - p0 ) < beta 
	__m64 m_and2 = _mm_and_si64 (mask4, m_1); //tc++; if abs( q2 - q0  ) < beta 
	__m64 m_tc = _mm_add_pi16(m_and2,_mm_add_pi16(m_tc0,m_and1));
	__m64 m_tcn =_mm_sub_pi16(_mm_sub_pi16(m_tcb0,m_and1),m_and2);
	__m64 mres_c3 = _mm_min_pi16(_mm_max_pi16(minter5,m_tcn),m_tc); //CLIP3(-tc0, tc0, addp2 - p1 );
	__m64 merror2 = _mm_and_si64 (mres_c3,first_mask);


	__m64 result_p1 = _mm_add_pi16(merror0,mp1); 
	__m64 result_q1 = _mm_add_pi16(merror1,mq1); 
	__m64 result_p0 = _mm_add_pi16(merror2,mp0); 
	__m64 result_q0 = _mm_sub_pi16(mq0, merror2);

	mp1 = _mm_packs_pu16(_mm_set_pi16(_mm_extract_pi16(result_q1,0),_mm_extract_pi16(result_q0,0),_mm_extract_pi16(result_p0,0),_mm_extract_pi16(result_p1,0)),mrshift);
	mp0 = _mm_packs_pu16(_mm_set_pi16(_mm_extract_pi16(result_q1,1),_mm_extract_pi16(result_q0,1),_mm_extract_pi16(result_p0,1),_mm_extract_pi16(result_p1,1)),mrshift);
	mq0 = _mm_packs_pu16(_mm_set_pi16(_mm_extract_pi16(result_q1,2),_mm_extract_pi16(result_q0,2),_mm_extract_pi16(result_p0,2),_mm_extract_pi16(result_p1,2)),mrshift);
	mq1 = _mm_packs_pu16(_mm_set_pi16(_mm_extract_pi16(result_q1,3),_mm_extract_pi16(result_q0,3),_mm_extract_pi16(result_p0,3),_mm_extract_pi16(result_p1,3)),mrshift);

	*((unsigned int* )(&pix[-2])) = _mm_cvtsi64_si32(mp1);
	*((unsigned int* )(&pix[-2 + ystride])) = _mm_cvtsi64_si32(mp0);
	*((unsigned int* )(&pix[-2 + (2 * ystride)])) = _mm_cvtsi64_si32(mq0);
	*((unsigned int* )(&pix[-2 + (3 * ystride)])) = _mm_cvtsi64_si32(mq1);

	empty();
}






void weak_horizontal_chroma_MMX(unsigned char pix[], const int xstride, const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{
	
	__m64 mp1 = _mm_set_pi16( 0,0,pix[-2*xstride + 1], pix[-2*xstride]); 
	__m64 mp0 = _mm_set_pi16( 0,0,pix[-1*xstride + 1], pix[-1*xstride]); 
	__m64 mq0 = _mm_set_pi16( 0,0,pix[1], pix[0]);
	__m64 mq1 = _mm_set_pi16( 0,0,pix[xstride + 1], pix[xstride]);
	

	__m64 mdiff_p0_q0 = _mm_sub_pi16(mq0,mp0); //abs(q0 - p0)
	__m64 mdiff_p1_p0 = _mm_sub_pi16(mp0,mp1); //abs(p1 - p0)
	__m64 mdiff_q1_q0 = _mm_sub_pi16(mq0, mq1); //abs(q1 - q0)

	//To calculate the mask
 	__m64 malpha = _mm_set_pi16(0,0,alpha,alpha);
	__m64 malphab = _mm_set_pi16(0,0,-alpha,-alpha);
	__m64 mbeta = _mm_set_pi16(0,0,beta,beta);
	__m64 mbetab = _mm_set_pi16(0,0,-beta,-beta);

	__m64 mask0 = _mm_and_si64( _mm_cmpgt_pi16(malpha, mdiff_p0_q0), _mm_cmpgt_pi16(mdiff_p0_q0,malphab));
	__m64 mask1 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_p1_p0), _mm_cmpgt_pi16(mdiff_p1_p0,mbetab));
	__m64 mask2 = _mm_and_si64( _mm_cmpgt_pi16(mbeta, mdiff_q1_q0), _mm_cmpgt_pi16(mdiff_q1_q0,mbetab));
	__m64 first_mask = _mm_and_si64 (_mm_and_si64 (mask0,mask1),mask2);


	__m64 mdiff_q0_p0 = _mm_sub_pi16(mq0,mp0); //(q0 - p0) 
	__m64 mlshift = _mm_set_pi16(0,0,0,2);
	__m64 minter_1 = _mm_sll_pi16(mdiff_q0_p0, mlshift);//inter_1 = (q0 - p0 ) << 2;
	__m64 minter_2 = _mm_sub_pi16(mp1, mq1);//(p1 - q1)
	__m64 madd4 = _mm_set_pi16(4,4,4,4);
	__m64 minter_3 = _mm_add_pi16(minter_2, madd4);//inter_2 = (p1 - q1) + 4;
	__m64 minter_4 = _mm_add_pi16(minter_3,minter_1); //(inter_1 +  inter_2)
	__m64 mrshift3 = _mm_set_pi16(0,0,0,3);
	__m64 minter5 = _mm_sra_pi16(minter_4, mrshift3);



	//Clip3
	__m64 m_tc0 = _mm_set_pi16(0,0,tc0,tc0);
	__m64 m_tcb0 = _mm_set_pi16(0,0,-tc0,-tc0);
	__m64 mres_c3 = _mm_min_pi16(_mm_max_pi16(minter5,m_tcb0),m_tc0); //CLIP3(-tc0, tc0, addp2 - p1 );
	__m64 merror2 = _mm_and_si64 (mres_c3,first_mask);

	__m64 result_p0 = _mm_add_pi16(merror2,mp0); //_mm_shuffle_pi16(_mm_add_pi16(merror2,mq1), 0x1B);
	__m64 result_q0 = _mm_sub_pi16(mq0, merror2);//_mm_shuffle_pi16(_mm_sub_pi16(mq1, merror2), 0x1B);
	__m64 mrshift = _mm_set_pi16(0,0,0,1);

	*((unsigned short* )(&pix[-xstride])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_p0,mrshift));
	*((unsigned short* )(&pix[0])) = _mm_cvtsi64_si32(_mm_packs_pu16(result_q0,mrshift));


	empty();
}


#endif
