



#include "type.h"
#include "main_data.h"
#include "svc_type.h"






/* 2010-07-29 17:43:25, application decoder_svc, processor DSP1 type=C64xOS */

#include <stdio.h>
#include <stdlib.h>
#include <std.h>
#include <tsk.h>
#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int
#define ulong unsigned long
#define prec_synchro int
#define stream uchar
#define image_type uchar








image_type Display_1_Extract_Image_Y_o[685832];

NAL DqIdNextNal_Nal_o[1];

int DqIdNextNal_set_layer_o[1];

int GetNalBytes_NalUnitBytes_o_buf[1];

ConfigSVC GetNalBytes_StreamType[1];

int GetNalBytes_rbsp_o_size[1];


stream GetNalBytes_rbsp_o[100000];

int NumBytesInNal_buf_1[1];

short decoder_svc_MvBuffer_1_Mv[3525120];

short decoder_svc_MvBuffer_1_Ref[1762560];

short decoder_svc_MvBuffer_Mv[3525120];

short decoder_svc_MvBuffer_Ref[1762560];

int decoder_svc_NalUnit_NalRefIdc_io[1];

int decoder_svc_NalUnit_NalUnitType_io[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[1];

PPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id[1];

W_TABLES decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif[1];

SPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[1];

int decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_entropy_coding_flag[1];

PPS decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_pps_id[1];

W_TABLES decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_quantif[1];

SPS decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id[1];

short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o[51840];

short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o[51840];

short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o[6480];

short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o[6480];

int decoder_svc_Nal_Compute_SetPos_Pos[1];

int decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[1];

PPS decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id[1];

W_TABLES decoder_svc_Nal_Compute_SliceHeaderIDR_quantif[1];

SPS decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id[1];

int decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o[1];

SEI decoder_svc_Nal_Compute_sei_rbsp_Sei[1];

int decoder_svc_Nal_Compute_seq_parameter_set_IdOfsps_o[1];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp[414720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py[576];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16[576];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16[576];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp[414720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py[576];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16[720];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16[576];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16[576];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop[1];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o[1];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o[1];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o[1];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o[51840];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o[51840];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o[6480];

short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o[6480];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[1];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[1];

PPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id[1];

W_TABLES decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif[1];

SPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id[1];

int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_entropy_coding_flag[1];

PPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_pps_id[1];

W_TABLES decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_quantif[1];

SPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id[1];

short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o[51840];

short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o[51840];

short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o[6480];

short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o[6480];

uchar decoder_svc_PictureBuffer_RefU[4000640];

uchar decoder_svc_PictureBuffer_RefV[4000640];

uchar decoder_svc_PictureBuffer_RefY[16002560];

short decoder_svc_ResiduBuffer_RefU[457216];

short decoder_svc_ResiduBuffer_RefV[457216];

short decoder_svc_ResiduBuffer_RefY[1828864];

DATA decoder_svc_Residu_Block[1620];

LIST_MMO decoder_svc_Residu_Current_pic[1];

uchar decoder_svc_Residu_MbToSliceGroupMap[1620];

MMO decoder_svc_Residu_Mmo[1];

PPS decoder_svc_Residu_PPS[255];

LIST_MMO decoder_svc_Residu_RefL0[16];

LIST_MMO decoder_svc_Residu_RefL1[16];

RESIDU decoder_svc_Residu_Residu[6480];

SPS decoder_svc_Residu_SPS[32];

SLICE decoder_svc_Residu_Slice[1];

uchar decoder_svc_Residu_SliceGroupId[1620];

uchar decoder_svc_Residu_SliceTab[1620];

int decoder_svc_SetZeor_Pos[1];

SVC_VECTORS decoder_svc_Svc_Vectors_DSP1_Svc_Vectors[1];

int decoder_svc_VideoParameter_Crop[1];

int decoder_svc_VideoParameter_EndOfSlice[1];

int decoder_svc_VideoParameter_ImgToDisplay[1];

int decoder_svc_VideoParameter_address_pic_o[1];

int decoder_svc_VideoParameter_xsize_o[1];

int decoder_svc_VideoParameter_ysize_o[1];

VLC_TABLES decoder_svc_VlcTab_DSP1_o[1];

MAIN_STRUCT_PF decoder_svc_slice_main_vector_DSP1_Main_vector_o[1];

uchar read_SVC_DataFile_o[100000];

int read_SVC_pos_o[1];
SLICE *decoder_svc_Nal_Compute_slice_layer_main_CondO9_o = decoder_svc_Residu_Slice;
short *decoder_svc_Nal_Compute_slice_layer_main_CondO8_o = decoder_svc_MvBuffer_1_Ref;
short *decoder_svc_Nal_Compute_slice_layer_main_CondO7_o = decoder_svc_MvBuffer_Ref;
short *decoder_svc_Nal_Compute_slice_layer_main_CondO6_o = decoder_svc_MvBuffer_1_Mv;
short *decoder_svc_Nal_Compute_slice_layer_main_CondO5_o = decoder_svc_MvBuffer_Mv;
uchar *decoder_svc_Nal_Compute_slice_layer_main_CondO4_o = decoder_svc_Residu_SliceTab;
RESIDU *decoder_svc_Nal_Compute_slice_layer_main_CondO3_o = decoder_svc_Residu_Residu;
DATA *decoder_svc_Nal_Compute_slice_layer_main_CondO0_o = decoder_svc_Residu_Block;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o = decoder_svc_Residu_SliceTab;
SLICE *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o = decoder_svc_Residu_Slice;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o = decoder_svc_VideoParameter_EndOfSlice;
DATA *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o = decoder_svc_Residu_Block;
RESIDU *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o = decoder_svc_Residu_Residu;
short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o = decoder_svc_MvBuffer_1_Mv;
short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o = decoder_svc_MvBuffer_1_Ref;
short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o = decoder_svc_MvBuffer_Ref;
short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o = decoder_svc_MvBuffer_Mv;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o = decoder_svc_PictureBuffer_RefU;
SLICE *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o = decoder_svc_Residu_Slice;
short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o = decoder_svc_MvBuffer_1_Ref;
short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o = decoder_svc_MvBuffer_Ref;
short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o = decoder_svc_MvBuffer_1_Mv;
short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o = decoder_svc_MvBuffer_Mv;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o = decoder_svc_Residu_SliceTab;
RESIDU *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o = decoder_svc_Residu_Residu;
int *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o = decoder_svc_VideoParameter_EndOfSlice;
DATA *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o = decoder_svc_Residu_Block;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o = decoder_svc_PictureBuffer_RefU;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_CondO7_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_CondO6_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o = decoder_svc_VideoParameter_address_pic_o;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o = decoder_svc_VideoParameter_address_pic_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o = decoder_svc_VideoParameter_address_pic_o;
int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o = decoder_svc_VideoParameter_address_pic_o;
int *decoder_svc_Nal_Compute_CondO5_o = decoder_svc_VideoParameter_address_pic_o;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_CondO4_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_CondO3_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o = decoder_svc_PictureBuffer_RefU;
uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o = decoder_svc_PictureBuffer_RefU;
uchar *decoder_svc_Nal_Compute_CondO2_o = decoder_svc_PictureBuffer_RefU;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o = decoder_svc_VideoParameter_Crop;
int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o = decoder_svc_VideoParameter_Crop;
int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o = decoder_svc_VideoParameter_Crop;
int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o = decoder_svc_VideoParameter_Crop;
int *decoder_svc_Nal_Compute_CondO0_o = decoder_svc_VideoParameter_Crop;
int *NumBytesInNal_o_1 = NumBytesInNal_buf_1;
int *GetNalBytes_NalUnitBytes_o = GetNalBytes_NalUnitBytes_o_buf;


void init_svc_vectors(SVC_VECTORS *svc);
void decode_init_vlc(VLC_TABLES *VLc );
void vector_main_init(MAIN_STRUCT_PF *pf);
void get_layer(int argc,uchar **argv,ConfigSVC *NumLayer);
void choose_layer(ConfigSVC *NumLayer);
void ParseFirstAU(int StreamSize,uchar *Buffer,ConfigSVC *StreamType);
int GetNalBytesInNalunit(uchar *data,stream *rbsp,int *NalInRbsp,ConfigSVC *StreamType);
void NextNalDqId(ConfigSVC *Buffer,NAL *NAL,int *SetLayer,int DqIdMax);
void init_nal_struct(NAL *nal,unsigned char NumOfLayer);
void init_mmo(int num_of_layers,MMO *mmo_stru);
void init_slice(SLICE *slice);
void init_pps(PPS *sps);
void init_sps(SPS *sps);
void InitListMmo(LIST_MMO *RefPicListL0);
void init_int(int  *tab);
void set_layer(int *num_of_layers);
int  init_dpb = 0;
void init_max_layer(int num_layer);
void initialize_layer(int num_of_layer);
void pic_parameter_set(stream *data,uchar *ao_slice_group_id,PPS *pps,SPS *sps,const int NalBytesInNalunit);
void decoderh264_init(const int pic_width,const int pic_height);
void slice_header_svc(int *Layer,const stream *data,SPS *sps_id,PPS *pps_id,int *entropy_coding_flag,W_TABLES *quantif,LIST_MMO *current_pic,SPS *sps,PPS *pps,int *position,SLICE *slice,MMO *mmo,LIST_MMO RefPicListL0[],LIST_MMO RefPicListL1[],NAL *nal,int *end_of_slice,int *ImgToDisplay,int *xsize,int *ysize,int *AddressPic,int *Crop);
void SliceHeaderSkip(const stream *data,SPS *sps_id,PPS *pps_id,int *entropy_coding_flag,W_TABLES *quantif,LIST_MMO *current_pic,SPS *sps,PPS *pps,int *position,SLICE *slice,MMO *mmo,LIST_MMO RefPicListL0[],LIST_MMO RefPicListL1[],NAL *nal,int *end_of_slice,int *ImgToDisplay,int *xsize,int *ysize,int *AddressPic,int *Crop);
void display_order_init(int argc,uchar **argv);
void DisplayOrderSvc(SPS *sps,LIST_MMO *current_pic,NAL *nal,int *address_pic,int *x_size,int *y_size,int *Crop,int *img_to_display,MMO *mmo);
void NalUnitSVC(stream *data_in,int *nal_unit_type,int *nal_ref_idc,NAL *Nal);
void init_int(int  *tab);
void svc_calculate_dpb(const int total_memory,const int mv_memory,int nb_of_layers,MMO *mmo_struct,SPS *sps);
void sei_rbsp(stream *data,int NalInRbsp,SPS *sps,SEI *Sei);
void seq_parameter_set(stream *data,SPS *sps);
void PrefixNalUnit(stream *data,int *NalinRbsp,NAL *nal,MMO *mmo,SPS *sps,int *DecRefPic,int *EndOfSlice);
void subset_sps(stream *data,int * NalInRbsp,SPS *sps,NAL *nal);
void NalUnitHeader(const stream *data,int *pos,int *do_ref_pic_marking_base,NAL *nal,int *EndOfSlice);
void slice_base_layer_cavlc(const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *Vlc,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *picture_residu,int *   aio_piEnd_of_slice,short *mv_io,short *mvl1_io,short *ref_io,short *refl1_io);
void slice_base_layer_cabac(uchar *data,int *position,int *NalBytesInNalunit,SPS *sps,PPS *pps,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice,short *mvl0_io,short *mb_l1_io,short *refl0_io,short *refl1_io);
void Decode_P_avc(const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const uchar *ai_tiSlice_table,const RESIDU *picture_residu,const STRUCT_PF *pf,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,NAL *Nal,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_avc(SPS *ai_stSps,PPS *ai_stPps ,SLICE *ai_stSlice,uchar *ai_tSlice_table,RESIDU *picture_residu,MAIN_STRUCT_PF *main_vector,LIST_MMO *ai_pRefPicListL0,LIST_MMO *ai_pRefPicListL1,LIST_MMO *ai_pCurrent_pic,W_TABLES *quantif,NAL *Nal,short *aio_tMv_l0,short *aio_tMv_l1,short *aio_tref_l0,short *aio_tref_l1,uchar *aio_tDpb_luma,uchar *aio_tDpb_Cb,uchar *aio_tDpb_Cr,short *Residu_img,short *Residu_Cb ,short *Residu_Cr);
void Decode_I_avc(SPS *sps,PPS *pps,SLICE *slice,uchar *slice_table,RESIDU *picture_residu,STRUCT_PF *pf,W_TABLES *quantif_tab,NAL *Nal,uchar *image,uchar *image_Cb,uchar *image_Cr);
void FinishFrameSVC(const int NbMb,NAL *Nal,SPS *Sps,PPS *Pps,LIST_MMO *Current_pic,SLICE *Slice,int EndOfSlice,uchar *SliceTab,DATA *TabBlbock,RESIDU *Residu,short *MvL0,short *MvL1,short *RefL0,short *RefL1,int *Crop,int *ImgToDisplay,int *AdressPic,MMO *Mmo,unsigned char *RefY,unsigned char *RefU,unsigned char *RefV,int *xsize,int *ysize);
void slice_data_in_scalable_extension_cavlc(const int size_mb,const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const NAL *nal,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *vlc,uchar *ai_slice_group_id,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *residu ,int * aio_piEnd_of_slice);
void SliceCabac(const int size_mb,uchar *data,int *position,int *NalBytesInNalunit,const NAL *Nal,SPS *sps,PPS *pps,uchar *ai_slice_group_id,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice);
void Decode_P_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *xk16,short *xp16,short *yk16,short* yp16,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,MAIN_STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstRefPicListL1,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *k16,short *p16,short *yk16,short *yp16,short *aio_tiMv_l0,short *aio_tMv_l1,short *aio_tiRef_l0,short *aio_tiRef_l1,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
//void Decode_I_svc(const int size,SPS *sps,PPS *pps,SLICE *slice,NAL *nal,uchar *slice_table,RESIDU *residu,STRUCT_PF *vector,LIST_MMO *Current_pic,W_TABLES *quantif,unsigned char *aio_tucImage,unsigned char *aio_tucImage_Cb,unsigned char  *aio_tucImage_Cr);void extract_picture(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,uchar *img_luma_out,uchar *img_Cb_out,uchar *img_Cr_out);
void Decode_I_svc(const int size,SPS *sps,PPS *pps,SLICE *slice,NAL *nal,uchar *slice_table,DATA *Block,RESIDU *residu,STRUCT_PF *vector,LIST_MMO *Current_pic,W_TABLES *quantif,unsigned char *aio_tucImage,unsigned char *aio_tucImage_Cb,unsigned char  *aio_tucImage_Cr);void extract_picture(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,uchar *img_luma_out,uchar *img_Cb_out,uchar *img_Cr_out);


/* External Variables */
extern far int ISRAM;   /* Generated within BIOS configuration */







void main(void)
{   
	/*CACHE_reset();
	CACHE_enableCaching(CACHE_EMIFA_CE00);
	CACHE_enableCaching(CACHE_EMIFA_CE01);
	CACHE_setL2Mode(CACHE_256KCACHE);

	CACHE_wbInvAllL2(CACHE_WAIT);	
{
    
    aTsk_MainTask.priority = 1;
    aTsk_MainTask.stack = NULL;
    aTsk_MainTask.stacksize = 0x2000;
    aTsk_MainTask.stackseg =ISRAM;
    aTsk_MainTask.environ = NULL;
    aTsk_MainTask.name = "Main_";
    aTsk_MainTask.exitflag = TRUE;


    if( (MainTask_handle = TSK_create((Fxn)MainTask, &aTsk_MainTask, 1)) == NULL)
    {

        while(1);
    }*/

}




//void MainTask(const unsigned int tskId){
void MainTask(){
  int argc; 	 							
  unsigned char** argv;   
  char outfilename [256];
  FILE* outfile= 0;							
  unsigned int frame_num= 0x0FFFFFFF;									

  #ifdef TI_OPTIM 
  unsigned int ini=0,fin=0;
  long long acumulado=0;
  #endif  


  init_DSP_TI(&argc, (unsigned char***) &argv,outfilename); 


  init_svc_vectors(decoder_svc_Svc_Vectors_DSP1_Svc_Vectors);
  decode_init_vlc(decoder_svc_VlcTab_DSP1_o);
  vector_main_init(decoder_svc_slice_main_vector_DSP1_Main_vector_o);
  
  get_layer(argc, argv, GetNalBytes_StreamType);
  
  
  SetLastDqId(0); //*GDEM
  SetArgLayer(0); //*GDEM


  #if  defined TI_OPTIM && ((defined CHIP_DM642) || defined (CHIP_DM6437) || defined (CHIP_DM648) || defined (CHIP_OMAP3530))
     readh264_init_TI(argc, (unsigned char**)argv, outfile,  &frame_num); 
  #elif
     readh264_init(argc, (unsigned char **)argv);
  #endif

  ParseFirstAU(100000, read_SVC_DataFile_o, GetNalBytes_StreamType);
  init_nal_struct(DqIdNextNal_Nal_o, 4);
  init_int(decoder_svc_SetZeor_Pos);
  init_slice(decoder_svc_Residu_Slice);
  init_sps(decoder_svc_Residu_SPS);
  init_pps(decoder_svc_Residu_PPS);
  InitListMmo(decoder_svc_Residu_RefL0);
  InitListMmo(decoder_svc_Residu_RefL1);
  InitListMmo(decoder_svc_Residu_Current_pic);
  init_mmo(4, decoder_svc_Residu_Mmo);
  init_int(decoder_svc_VideoParameter_EndOfSlice);
  init_int(decoder_svc_VideoParameter_ImgToDisplay);
  init_int(decoder_svc_VideoParameter_xsize_o);
  init_int(decoder_svc_VideoParameter_ysize_o);
  init_int(decoder_svc_VideoParameter_address_pic_o);
  init_int(decoder_svc_VideoParameter_Crop);
  
  init_max_layer(4);
  init_mmo(4, decoder_svc_Residu_Mmo);
  
  
  decoderh264_init(720, 576);
  
  
  
  init_dpb=0;
  read_SVC_pos_o[0]=0;   







  *NumBytesInNal_o_1=100000;
  
  
  for(;;){ /* loop_2 */ 
    
    readh264(100000, NumBytesInNal_o_1[0], read_SVC_pos_o, read_SVC_DataFile_o); //GDEM

	#ifdef TI_OPTIM 
    	ini = CLK_gethtime();
	#endif  


    choose_layer(GetNalBytes_StreamType);
    GetNalBytes_rbsp_o_size[0] = GetNalBytesInNalunit(read_SVC_DataFile_o, GetNalBytes_rbsp_o, GetNalBytes_NalUnitBytes_o, GetNalBytes_StreamType);
    NextNalDqId(GetNalBytes_StreamType, DqIdNextNal_Nal_o, DqIdNextNal_set_layer_o, -1);
    
    
    
    NalUnitSVC(GetNalBytes_rbsp_o, decoder_svc_NalUnit_NalUnitType_io, decoder_svc_NalUnit_NalRefIdc_io, DqIdNextNal_Nal_o);
    init_int(decoder_svc_VideoParameter_ImgToDisplay);
    if (init_dpb == 0){ 
        initialize_layer(DqIdNextNal_set_layer_o[0]);
        init_dpb = 1; 
    }
    set_layer(DqIdNextNal_set_layer_o);
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_3 */
      case 5 : {/* case_4 */
        svc_calculate_dpb((457216 * (16 + 16 + 4 - 1)), (720 * 576 / 8 * 4 * (16 + 1)), 4, decoder_svc_Residu_Mmo, decoder_svc_Residu_SPS);
      break; }/* case_4 */
      case 6 : {/* case_5 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        sei_rbsp(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size[0], decoder_svc_Residu_SPS, decoder_svc_Nal_Compute_sei_rbsp_Sei);
      break; }/* case_5 */
      case 7 : {/* case_6 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        seq_parameter_set(GetNalBytes_rbsp_o, decoder_svc_Residu_SPS);
      break; }/* case_6 */
      case 8 : {/* case_7 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
      break; }/* case_7 */
      case 14 : {/* case_8 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        PrefixNalUnit(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, DqIdNextNal_Nal_o, decoder_svc_Residu_Mmo, decoder_svc_Residu_SPS, decoder_svc_SetZeor_Pos, decoder_svc_VideoParameter_EndOfSlice);
      break; }/* case_8 */
      case 15 : {/* case_9 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        subset_sps(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Residu_SPS, DqIdNextNal_Nal_o);
      break; }/* case_9 */
      case 20 : {/* case_10 */
        NalUnitHeader(GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, decoder_svc_SetZeor_Pos, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice);
      break; }/* case_10 */
    } /* end switch_3 */
    
    
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_11 */
      case 1 : {/* case_12 */
        switch (decoder_svc_SetZeor_Pos[0]) { /* switch_13 */
          case 0 : {/* case_14 */
            *decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos = 8;
            slice_header_svc(DqIdNextNal_set_layer_o, GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
          break; }/* case_14 */
          case 3 : {/* case_15 */
            *decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos = 8;
            SliceHeaderSkip(GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_entropy_coding_flag, decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
            if (decoder_svc_VideoParameter_EndOfSlice[0]==1) { /* if_16 */
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o=decoder_svc_VideoParameter_ysize_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o=decoder_svc_VideoParameter_xsize_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o=decoder_svc_VideoParameter_address_pic_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o=decoder_svc_VideoParameter_Crop;
            } /* end if_16 */
          break; }/* case_15 */
        } /* end switch_13 */
      break; }/* case_12 */
      case 5 : {/* case_17 */
        *decoder_svc_Nal_Compute_SetPos_Pos = 8;
        slice_header_svc(DqIdNextNal_set_layer_o, GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag, decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
        switch (decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[0]) { /* switch_18 */
          case 0 : {/* case_19 */
            slice_base_layer_cavlc(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_VlcTab_DSP1_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_19 */
          case 1 : {/* case_20 */
            slice_base_layer_cabac(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_20 */
        } /* end switch_18 */
      break; }/* case_17 */
      case 8 : {/* case_21 */
        pic_parameter_set(GetNalBytes_rbsp_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_PPS, decoder_svc_Residu_SPS, GetNalBytes_rbsp_o_size[0]);
      break; }/* case_21 */
      case 20 : {/* case_22 */
        switch (decoder_svc_SetZeor_Pos[0]) { /* switch_23 */
          case 0 : {/* case_24 */
            slice_header_svc(DqIdNextNal_set_layer_o, GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
          break; }/* case_24 */
          case 3 : {/* case_25 */
            SliceHeaderSkip(GetNalBytes_rbsp_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_pps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_entropy_coding_flag, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
            switch (decoder_svc_VideoParameter_EndOfSlice[0]) { /* switch_26 */
              case 0 : {/* case_27 */
                DisplayOrderSvc(decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id, decoder_svc_Residu_Current_pic, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_Residu_Mmo);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop;
              break; }/* case_27 */
              case 1 : {/* case_28 */
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o=decoder_svc_VideoParameter_ysize_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o=decoder_svc_VideoParameter_xsize_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o=decoder_svc_VideoParameter_address_pic_o;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o=decoder_svc_VideoParameter_Crop;
              break; }/* case_28 */
            } /* end switch_26 */
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o;
          break; }/* case_25 */
        } /* end switch_23 */
      break; }/* case_22 */
    } /* end switch_11 */
    
    
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_29 */
      case 1 : {/* case_30 */
        switch (decoder_svc_SetZeor_Pos[0]) { /* switch_31 */
          case 0 : {/* case_32 */
            switch (decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[0]) { /* switch_33 */
              case 0 : {/* case_34 */
                slice_base_layer_cavlc(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_VlcTab_DSP1_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=decoder_svc_Residu_Slice;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=decoder_svc_MvBuffer_1_Ref;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=decoder_svc_MvBuffer_Ref;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=decoder_svc_MvBuffer_1_Mv;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=decoder_svc_MvBuffer_Mv;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=decoder_svc_Residu_SliceTab;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=decoder_svc_Residu_Residu;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=decoder_svc_VideoParameter_EndOfSlice;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=decoder_svc_Residu_Block;
              break; }/* case_34 */
              case 1 : {/* case_35 */
                slice_base_layer_cabac(GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=decoder_svc_Residu_Slice;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=decoder_svc_MvBuffer_1_Ref;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=decoder_svc_MvBuffer_Ref;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=decoder_svc_MvBuffer_1_Mv;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=decoder_svc_MvBuffer_Mv;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=decoder_svc_Residu_SliceTab;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=decoder_svc_Residu_Residu;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=decoder_svc_VideoParameter_EndOfSlice;
                decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=decoder_svc_Residu_Block;
              break; }/* case_35 */
            } /* end switch_33 */
            *decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o=decoder_svc_Residu_Slice[0].slice_type;
            switch (decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[0]) { /* switch_36 */
              case 0 : {/* case_37 */
                Decode_P_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Residu_RefL0, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, &decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o[decoder_svc_Residu_Current_pic->MvMemoryAddress], &decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o[decoder_svc_Residu_Current_pic->MvMemoryAddress >> 1], decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_37 */
              case 1 : {/* case_38 */
                Decode_B_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_38 */
              case 2 : {/* case_39 */
                Decode_I_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, &decoder_svc_PictureBuffer_RefY[decoder_svc_Residu_Current_pic->MemoryAddress], &decoder_svc_PictureBuffer_RefU[decoder_svc_Residu_Current_pic->MemoryAddress>>2], &decoder_svc_PictureBuffer_RefV[decoder_svc_Residu_Current_pic->MemoryAddress>>2]);
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_39 */
            } /* end switch_36 */
            FinishFrameSVC(720*576/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o[0], decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o=decoder_svc_VideoParameter_ysize_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o=decoder_svc_VideoParameter_xsize_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o=decoder_svc_VideoParameter_Crop;
          break; }/* case_32 */
          case 3 : {/* case_40 */
            if (decoder_svc_VideoParameter_EndOfSlice[0]==0) { /* if_41 */
              DisplayOrderSvc(decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id, decoder_svc_Residu_Current_pic, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o, decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o, decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o, decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_Residu_Mmo);
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
              decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop;
            } /* end if_41 */
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o=decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_40 */
        } /* end switch_31 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o;
      break; }/* case_30 */
      case 5 : {/* case_42 */
        Decode_I_avc(decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Nal_Compute_slice_layer_main_CondO9_o, decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, DqIdNextNal_Nal_o, &decoder_svc_PictureBuffer_RefY[decoder_svc_Residu_Current_pic->MemoryAddress], &decoder_svc_PictureBuffer_RefU[decoder_svc_Residu_Current_pic->MemoryAddress>>2], &decoder_svc_PictureBuffer_RefV[decoder_svc_Residu_Current_pic->MemoryAddress>>2]);
        FinishFrameSVC(720*576/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Residu_Slice, decoder_svc_VideoParameter_EndOfSlice[0], decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, decoder_svc_Nal_Compute_slice_layer_main_CondO0_o, decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, decoder_svc_Nal_Compute_slice_layer_main_CondO5_o, decoder_svc_Nal_Compute_slice_layer_main_CondO6_o, decoder_svc_Nal_Compute_slice_layer_main_CondO7_o, decoder_svc_Nal_Compute_slice_layer_main_CondO8_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
      break; }/* case_42 */
      case 6 : {/* case_43 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_43 */
      case 7 : {/* case_44 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_44 */
      case 8 : {/* case_45 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_45 */
      case 14 : {/* case_46 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_46 */
      case 15 : {/* case_47 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_47 */
      case 20 : {/* case_48 */
        switch (decoder_svc_SetZeor_Pos[0]) { /* switch_49 */
          case 0 : {/* case_50 */
            switch (decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[0]) { /* switch_51 */
              case 0 : {/* case_52 */
                slice_data_in_scalable_extension_cavlc(720*576/256, GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_VlcTab_DSP1_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=decoder_svc_Residu_SliceTab;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=decoder_svc_Residu_Slice;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=decoder_svc_VideoParameter_EndOfSlice;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=decoder_svc_Residu_Block;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=decoder_svc_Residu_Residu;
              break; }/* case_52 */
              case 1 : {/* case_53 */
                SliceCabac(720*576/256, GetNalBytes_rbsp_o, GetNalBytes_rbsp_o_size, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=decoder_svc_Residu_SliceTab;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=decoder_svc_Residu_Slice;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=decoder_svc_VideoParameter_EndOfSlice;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=decoder_svc_Residu_Block;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=decoder_svc_Residu_Residu;
              break; }/* case_53 */
            } /* end switch_51 */
            *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o[0].slice_type;
            switch (decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[0]) { /* switch_54 */
              case 0 : {/* case_55 */
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
                Decode_P_svc(720*576/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Residu_RefL0, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Svc_Vectors_DSP1_Svc_Vectors, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_55 */
              case 1 : {/* case_56 */
                Decode_B_svc(720*576/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Svc_Vectors_DSP1_Svc_Vectors, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_56 */
              case 2 : {/* case_57 */
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
               // Decode_I_svc(720*576/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV);
                Decode_I_svc(720*576/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_DSP1_Main_vector_o->baseline_vectors, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV);
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
                decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
              break; }/* case_57 */
            } /* end switch_54 */
            FinishFrameSVC(720*576/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o[0], decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o=decoder_svc_VideoParameter_ysize_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o=decoder_svc_VideoParameter_xsize_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o=decoder_svc_VideoParameter_Crop;
          break; }/* case_50 */
          case 3 : {/* case_58 */
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_58 */
        } /* end switch_49 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o;
      break; }/* case_48 */
    } /* end switch_29 */
    if (decoder_svc_Nal_Compute_CondO1_o[0]==1) { /* if_59 */
      {
         int XDIM = ((int *) Display_1_Extract_Image_Y_o)[0] = *decoder_svc_Nal_Compute_CondO6_o;
         int YDIM = ((int *) Display_1_Extract_Image_Y_o)[1] = *decoder_svc_Nal_Compute_CondO7_o;
         uchar *Y = Display_1_Extract_Image_Y_o + 8;
         uchar *U = Y + (XDIM + 32) * YDIM;
         uchar *V = U + (XDIM + 32) * YDIM/4;
         extract_picture(XDIM, YDIM, 16, decoder_svc_Nal_Compute_CondO0_o[0], decoder_svc_Nal_Compute_CondO4_o, decoder_svc_Nal_Compute_CondO2_o, decoder_svc_Nal_Compute_CondO3_o, decoder_svc_Nal_Compute_CondO5_o[0], Y, U, V);
      }
    } /* end if_59 */
    
    {int *NumBytesInNal_o_1_temp = NumBytesInNal_o_1;
    NumBytesInNal_o_1=GetNalBytes_NalUnitBytes_o;
    GetNalBytes_NalUnitBytes_o=NumBytesInNal_o_1_temp;}

	#ifdef TI_OPTIM 
	    fin = CLK_gethtime();
		if (fin>ini)
			acumulado+=(fin-ini);
	#endif  

    
    if (decoder_svc_Nal_Compute_CondO1_o[0]==1) { /* if_60 */
      {
         /*
         int XDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[0];
         int YDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[1];
	 	 int Xsize = XDIM + 32;
   	     const int offset_C = ((Xsize << 2) + 8 + (decoder_svc_Nal_Compute_CondO5_o[0] >> 2));
 	     uchar *Y = decoder_svc_Nal_Compute_CondO4_o + (Xsize << 4) + 16 + decoder_svc_Nal_Compute_CondO5_o[0];
	     uchar *U = decoder_svc_Nal_Compute_CondO2_o+offset_C;
	     uchar *V = decoder_svc_Nal_Compute_CondO3_o+offset_C;
         */
		// Codigo original del repositorio
         int XDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[0];
         int YDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[1];
         uchar *Y = Display_1_Extract_Image_Y_o + 8;
         uchar *U = Y + (XDIM + 32) * YDIM;
         uchar *V = U + (XDIM + 32) * YDIM/4 ;
         
               
      
         save_data(Y,U,V,XDIM,YDIM,outfilename,acumulado);
      
     }

    } /* end if_60 */
  } 
  













} /* end of main */


