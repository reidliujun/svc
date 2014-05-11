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



#ifndef SVC_TYPE_H
#define SVC_TYPE_H
#include "type.h"

//mbl reduce memory

typedef struct {
	
	int DqIdMax;		//Max DqId in the stream
	int SetLayer;		//DqID defined
	int TemporalCom;	//Command to define what to do with Temporal Id; 0: nothing, 1: Temporal down, 2: Temporal up, 3: Specific temporal Id
	int TemporalId;		//TemporalId wanted (only used in case 3)
	int NextDqId;		// Or Max
	char layer_id_table[8];
	char UsedSpatial[8][8];
	char NbSlicesLayer[8];

} ConfigSVC;

typedef struct {
	
	short RefIdc;

	//Must be in int
	int PartIdc;
	int MvIndex;
	int RefIndex;
} REFPART;


typedef struct {

	//Layer Managment
	int TempToDisplay;			//Temporal Id of the nal to display
	int TempWanted;				//Temporal Id requested by the user
	int TemporalId;				//Temporal Id of the current nal
	int LastTemporalId;			//Temporal_id of the last decoded nal
	int LastLayerId;			//number of layer of the last decoded nal
	int BaseLayerId;			//number of layer of the base nal
	unsigned char LayerId;		//number of layer of the current nal
	unsigned char LastDecodedId;//Last decoded ID
	int DependencyId;
	int QualityId;
	int DqId;					 //DqId of the current nal
	int LastDqId;				 //DqId of the last decoded nal
	//int NextDqId;				 //DqId of the next nal (only in the decoder, not players)
	int DqIdToDisplay;			 //DqId of the nal to display
	int DqIdWanted;				 //DqId requested by the user
	int PicToDecode;			 //Frame used for prediction. Several algorithms have to be done for upper layer.
	int PicToDisplay;			 //Frame have to be totally decoded (Motion ompensation, ...)
	int NalToSkip;				 //Nal have to be decoded. Used for temporal scalability 	
	
	//Stream configuration
	int MvCalx[8];
	int MvCaly[8];
	int pic_parameter_id[8];
	char layer_id_table[8];
	char UsedSpatial[8][8];
	char MaxNumLayer;	 //Number of different layer allowed by the decoder
	int DependencyIdMax; //Maximum dependency_Id found in the stream
	int MaxTemporalId;   //Maximum temporal_id found in the stream
	int DqIdMax;		 //Maximum DqId found in the AU
	int LastDqIdMax;	 //Maximum DqId found in the previous AU


	//Help to display
	unsigned char FlushRenderer;

	//Nal type
    int NalRefIdc;
    int NalUnitType;
	int IdrFlag;


	//SVC Nal configuration
    int layer_base_flag;
    int UseRefBasePicFlag;
	int no_inter_layer_pred_flag;
	int StoreRefBasePicFlag;
	int DiscardableFlag;
	int SpatialScalabilityType;
	int non_diadic;
	short SCoeffPrediction;
	short TCoeffPrediction;
	short SliceSkipFlag;


	//Frame memory addresses
	int LastMemoryAddress;			//Memory address of the frame decoded in the last nal
	int BaseMemoryAddress;			//Memory address of the base layer
	int BaseMvMemoryAddress;		//MV memory address of the base layer
	int MemoryAddress;				//Current frame memory address
	int MvMemoryAddress;			//Current frame Mv memory address


	//Frame crop
	int b_stride ;
	int b8_stride ;
	int is_cropping;
	int left_offset ;
	int top_offset;


	//Frame size
	short curr_Width;
	short curr_Height;
	short up_Width;
	short up_Height;
	short base_Width;
	short base_Height;
	short down_Width;
	short down_Height;
	

	//Loop Filter
	short SpatialScalability; // difference of macroblock
	short LoopFilterSpatialScalability; //the same size for the common area (crop)
	short LoopFilterForcedIdc;

	//Layer Command
	unsigned char ComTempId;	//Used to select the temporal layer to display
	unsigned char ComDqId;		//Used to select the DqId to display


	//Error Detection
	char ErrorDetected;
	char ErrorDetection;
	char NbSlicesLayer[8];
}   NAL ;





typedef void    (*Sub_P_svc)(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, short *mv_l0, short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);

typedef void    (*P_svc)( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
			, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
			, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
			, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
			, const NAL *Nal, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);


typedef void    (*Sub_B_svc)( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0,short *aio_tref_cache_l1	, const DATA *aio_pBlock
					, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartition[16]) ;

typedef void    (*B_svc)( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]) ;


 typedef struct {

	B_svc	  B_vectors[5];
	Sub_B_svc B_sub_vectors[13];
	P_svc	  P_vectors[5];
	Sub_P_svc P_sub_vectors[4];
 }SVC_VECTORS;


#endif
