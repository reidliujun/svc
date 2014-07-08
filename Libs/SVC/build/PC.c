



#include "type.h"
#include "main_data.h"
#include "svc_type.h"







/* 2011-09-19 17:28:55, application decoder_svc, processor PC type=pentiumOS */

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long
#define uint unsigned int
#define prec_synchro int
#define stream unsigned char
#define image_type unsigned char
#define dpb_type unsigned char







image_type Display_1_Extract_1_Image_Y_o[3279368];
image_type Display_1_Extract_Image_Y_o[3279368];
NAL DqIdNextNal_Nal_o[1];
NALSTREAMER GetNalBytesAu_NalStreamer[1];
PPS GetNalBytesAu_PPS[255];
SPS GetNalBytesAu_SPS[32];
ConfigSVC GetNalBytesAu_StreamType[1];
int GetNalBytesAu_rbsp_o_size[1];
stream GetNalBytesAu_rbsp_o[1622016];
uchar ReadAU_DataFile_o[1622016];
int ReadAU_ReadBytes[1];
int ReadAU_pos_o[1];
short decoder_svc_MvBuffer_1_Mv[11489280];
short decoder_svc_MvBuffer_1_Ref[5744640];
short decoder_svc_MvBuffer_Mv[11489280];
short decoder_svc_MvBuffer_Ref[5744640];
int decoder_svc_NalUnit_NalRefIdc_io[1];
int decoder_svc_NalUnit_NalUnitType_io[1];
int decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos[1];
int decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[1];
PPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id[1];
W_TABLES decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif[1];
SPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id[1];
int decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[1];
short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o[261120];
short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o[261120];
short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o[32640];
short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o[32640];
int decoder_svc_Nal_Compute_SetPos_Pos[1];
int decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[1];
PPS decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id[1];
W_TABLES decoder_svc_Nal_Compute_SliceHeaderIDR_quantif[1];
SPS decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id[1];
int decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o[1];
SEI decoder_svc_Nal_Compute_sei_rbsp_Sei[1];
int decoder_svc_Nal_Compute_seq_parameter_set_IdOfsps_o[1];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp[2088960];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp[2088960];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16[1920];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16[1088];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o[261120];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o[261120];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o[32640];
short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o[32640];
int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[1];
int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[1];
PPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id[1];
W_TABLES decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif[1];
SPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id[1];
short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o[261120];
short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o[261120];
short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o[32640];
short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o[32640];
uchar decoder_svc_PictureBuffer_RefU[12570880];
uchar decoder_svc_PictureBuffer_RefV[12570880];
uchar decoder_svc_PictureBuffer_RefY[50283520];
short decoder_svc_ResiduBuffer_RefU[2186240];
short decoder_svc_ResiduBuffer_RefV[2186240];
short decoder_svc_ResiduBuffer_RefY[8744960];
DATA decoder_svc_Residu_Block[8160];
LIST_MMO decoder_svc_Residu_Current_pic[1];
uchar decoder_svc_Residu_MbToSliceGroupMap[8160];
MMO decoder_svc_Residu_Mmo[1];
PPS decoder_svc_Residu_PPS[255];
LIST_MMO decoder_svc_Residu_RefL0[16];
LIST_MMO decoder_svc_Residu_RefL1[16];
RESIDU decoder_svc_Residu_Residu[32640];
SPS decoder_svc_Residu_SPS[32];
SLICE decoder_svc_Residu_Slice[1];
uchar decoder_svc_Residu_SliceGroupId[8160];
uchar decoder_svc_Residu_SliceTab[8160];
SVC_VECTORS decoder_svc_Svc_Vectors_PC_Svc_Vectors[1];
int decoder_svc_VideoParameter_Crop[1];
int decoder_svc_VideoParameter_EndOfSlice[1];
int decoder_svc_VideoParameter_ImgToDisplay[1];
int decoder_svc_VideoParameter_address_pic_o[1];
int decoder_svc_VideoParameter_xsize_o[1];
int decoder_svc_VideoParameter_ysize_o[1];
VLC_TABLES decoder_svc_VlcTab_PC_o[1];
MAIN_STRUCT_PF decoder_svc_slice_main_vector_PC_Main_vector_o[1];
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
int *decoder_svc_Nal_Compute_CondO7_o = decoder_svc_VideoParameter_ysize_o;
int *decoder_svc_Nal_Compute_CondO6_o = decoder_svc_VideoParameter_xsize_o;
int *decoder_svc_Nal_Compute_CondO5_o = decoder_svc_VideoParameter_address_pic_o;
uchar *decoder_svc_Nal_Compute_CondO4_o = decoder_svc_PictureBuffer_RefY;
uchar *decoder_svc_Nal_Compute_CondO3_o = decoder_svc_PictureBuffer_RefV;
uchar *decoder_svc_Nal_Compute_CondO2_o = decoder_svc_PictureBuffer_RefU;
int *decoder_svc_Nal_Compute_CondO1_o = decoder_svc_VideoParameter_ImgToDisplay;
int *decoder_svc_Nal_Compute_CondO0_o = decoder_svc_VideoParameter_Crop;



void init_svc_vectors(SVC_VECTORS *svc);
void decode_init_vlc(VLC_TABLES *VLc );
void vector_main_init(MAIN_STRUCT_PF *pf);
void readh264_init(int argc,char **argv);
void get_layer(int argc,char **argv,ConfigSVC *NumLayer);
void choose_layer(ConfigSVC *NumLayer);
void ParseFirstAU(int StreamSize,uchar *Buffer,ConfigSVC *StreamType);
int GetNalBytesAuSVC(uchar *data,stream *rbsp,int *NalInRbsp,ConfigSVC *StreamType,NALSTREAMER *NalSize,SPS *Sps,PPS *Pps);
void init_int(int  *tab);
void init_mmo(int num_of_layers,MMO *mmo_stru);
void init_slice(SLICE *slice);
void init_pps(PPS *sps);
void init_sps(SPS *sps);
void InitListMmo(LIST_MMO *RefPicListL0);
void slice_header_svc(int NbLayers,const stream *data,SPS *sps_id,PPS *pps_id,int *entropy_coding_flag,W_TABLES *quantif,LIST_MMO *current_pic,SPS *sps,PPS *pps,int *position,SLICE *slice,MMO *mmo,LIST_MMO RefPicListL0[],LIST_MMO RefPicListL1[],NAL *nal,int *end_of_slice,int *ImgToDisplay,int *xsize,int *ysize,int *AddressPic,int *Crop);
void pic_parameter_set(stream *data,uchar *ao_slice_group_id,PPS *pps,SPS *sps,const int NalBytesInNalunit);
void decoderh264_init(const int pic_width,const int pic_height);
void Init_SDL(int edge,int frame_width,int frame_height);
void SDL_Display(int edge,int frame_width,int frame_height,unsigned char *Y,unsigned char *V,unsigned char *U);
void CloseSDLDisplay();
void ReadAuH264(const int nb_octets_to_read,uchar *buffer,int ReadnewBytes,int *nb_octet_already_read);
void NextNalDqId(ConfigSVC *Buffer,NAL *NAL);
void init_nal_struct(NAL *nal,unsigned char NumOfLayer);
void NalUnitSVC(stream *data_in,int *nal_unit_type,int *nal_ref_idc,NAL *Nal);
void init_int(int  *tab);
void svc_calculate_dpb(const int total_memory,const int mv_memory,int nb_of_layers,MMO *mmo_struct,SPS *sps,NAL *Nal);
void sei_rbsp(stream *data,int NalInRbsp,SPS *sps,SEI *Sei);
void seq_parameter_set(stream *data,SPS *sps);
void FlushSVCFrame(SPS *sps,NAL *nal,MMO *mmo,int *address_pic,int *x_size,int *y_size,int *Crop,int *img_to_display);
void PrefixNalUnit(stream *data,int *NalinRbsp,NAL *nal,MMO *mmo,SPS *sps,int *EndOfSlice);
void subset_sps(stream *data,int * NalInRbsp,SPS *sps,NAL *nal);
void NalUnitHeader(const stream *data,int *pos,NAL *nal,int *EndOfSlice);
void slice_data_in_scalable_extension_cavlc(const int size_mb,const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const NAL *nal,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *vlc,uchar *ai_slice_group_id,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *residu ,int * aio_piEnd_of_slice);
void SliceCabac(const int size_mb,uchar *data,int *position,int *NalBytesInNalunit,const NAL *Nal,SPS *sps,PPS *pps,uchar *ai_slice_group_id,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice);
void slice_base_layer_cavlc(const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *Vlc,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *picture_residu,int *   aio_piEnd_of_slice,short *mv_io,short *mvl1_io,short *ref_io,short *refl1_io);
void slice_base_layer_cabac(uchar *data,int *position,int *NalBytesInNalunit,SPS *sps,PPS *pps,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice,short *mvl0_io,short *mb_l1_io,short *refl0_io,short *refl1_io);
void Decode_P_avc(const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const uchar *ai_tiSlice_table,const RESIDU *picture_residu,const STRUCT_PF *pf,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,NAL *Nal,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_avc(SPS *ai_stSps,PPS *ai_stPps ,SLICE *ai_stSlice,uchar *ai_tSlice_table,RESIDU *picture_residu,MAIN_STRUCT_PF *main_vector,LIST_MMO *ai_pRefPicListL0,LIST_MMO *ai_pRefPicListL1,LIST_MMO *ai_pCurrent_pic,W_TABLES *quantif,NAL *Nal,short *aio_tMv_l0,short *aio_tMv_l1,short *aio_tref_l0,short *aio_tref_l1,uchar *aio_tDpb_luma,uchar *aio_tDpb_Cb,uchar *aio_tDpb_Cr,short *Residu_img,short *Residu_Cb ,short *Residu_Cr);
void Decode_I_avc(SPS *sps,PPS *pps,SLICE *slice,uchar *slice_table,RESIDU *picture_residu,STRUCT_PF *pf,W_TABLES *quantif_tab,NAL *Nal,uchar *image,uchar *image_Cb,uchar *image_Cr);
void FinishFrameSVC(const int NbMb,NAL *Nal,SPS *Sps,PPS *Pps,LIST_MMO *Current_pic,SLICE *Slice,int EndOfSlice,uchar *SliceTab,DATA *TabBlbock,RESIDU *Residu,short *MvL0,short *MvL1,short *RefL0,short *RefL1,int *Crop,int *ImgToDisplay,int *AdressPic,MMO *Mmo,unsigned char *RefY,unsigned char *RefU,unsigned char *RefV,int *xsize,int *ysize);
void Decode_P_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *xk16,short *xp16,short *yk16,short* yp16,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,MAIN_STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstRefPicListL1,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *k16,short *p16,short *yk16,short *yp16,short *aio_tiMv_l0,short *aio_tMv_l1,short *aio_tiRef_l0,short *aio_tiRef_l1,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_I_svc(const int size,SPS *sps,PPS *pps,SLICE *slice,NAL *nal,uchar *slice_table,DATA *Block,RESIDU *residu,STRUCT_PF *vector,LIST_MMO *Current_pic,W_TABLES *quantif,unsigned char *aio_tucImage,unsigned char *aio_tucImage_Cb,unsigned char  *aio_tucImage_Cr);
void extract_picture(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,uchar *img_luma_out,uchar *img_Cb_out,uchar *img_Cr_out);
int main(int argc, char* argv[]) { /* for link with C runtime boot */
  init_svc_vectors(decoder_svc_Svc_Vectors_PC_Svc_Vectors);
  decode_init_vlc(decoder_svc_VlcTab_PC_o);
  vector_main_init(decoder_svc_slice_main_vector_PC_Main_vector_o);
  *ReadAU_ReadBytes = 1622016;
  readh264_init(argc, argv);
  get_layer(argc, argv, GetNalBytesAu_StreamType);
  ParseFirstAU(1622016, ReadAU_DataFile_o, GetNalBytesAu_StreamType);
  init_nal_struct(DqIdNextNal_Nal_o, 4);
  init_int(decoder_svc_VideoParameter_EndOfSlice);
  init_int(decoder_svc_VideoParameter_ImgToDisplay);
  init_int(decoder_svc_VideoParameter_xsize_o);
  init_int(decoder_svc_VideoParameter_ysize_o);
  init_int(decoder_svc_VideoParameter_address_pic_o);
  init_int(decoder_svc_VideoParameter_Crop);
  init_slice(decoder_svc_Residu_Slice);
  init_sps(decoder_svc_Residu_SPS);
  init_pps(decoder_svc_Residu_PPS);
  InitListMmo(decoder_svc_Residu_RefL0);
  InitListMmo(decoder_svc_Residu_RefL1);
  InitListMmo(decoder_svc_Residu_Current_pic);
  init_mmo(4, decoder_svc_Residu_Mmo);
  
  
  
  
  decoderh264_init(1920, 1088);
  
  
  
  
  
  Init_SDL(16, 1920, 1088);

  

  for(;;){ /* loop_2 */ 
    ReadAuH264(1622016, ReadAU_DataFile_o, ReadAU_ReadBytes[0], ReadAU_pos_o);
    choose_layer(GetNalBytesAu_StreamType);
    GetNalBytesAu_rbsp_o_size[0] = GetNalBytesAuSVC(ReadAU_DataFile_o, GetNalBytesAu_rbsp_o, ReadAU_ReadBytes, GetNalBytesAu_StreamType, GetNalBytesAu_NalStreamer, GetNalBytesAu_SPS, GetNalBytesAu_PPS);
    NextNalDqId(GetNalBytesAu_StreamType, DqIdNextNal_Nal_o);
    
    NalUnitSVC(GetNalBytesAu_rbsp_o, decoder_svc_NalUnit_NalUnitType_io, decoder_svc_NalUnit_NalRefIdc_io, DqIdNextNal_Nal_o);
    
    init_int(decoder_svc_VideoParameter_ImgToDisplay);
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_3 */
      case 1 : {/* case_4 */
        *decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos = 8;
        slice_header_svc(4, GetNalBytesAu_rbsp_o, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
      break; }/* case_4 */
      case 5 : {/* case_5 */
        svc_calculate_dpb((2186240 * (10 + 10 + 4 - 1)), (1920 * 1088 / 8 * 4 * (10 + 1)), 4, decoder_svc_Residu_Mmo, decoder_svc_Residu_SPS, DqIdNextNal_Nal_o);
        *decoder_svc_Nal_Compute_SetPos_Pos = 8;
      break; }/* case_5 */
      case 6 : {/* case_6 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        sei_rbsp(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size[0], decoder_svc_Residu_SPS, decoder_svc_Nal_Compute_sei_rbsp_Sei);
      break; }/* case_6 */
      case 7 : {/* case_7 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        seq_parameter_set(GetNalBytesAu_rbsp_o, decoder_svc_Residu_SPS);
      break; }/* case_7 */
      case 8 : {/* case_8 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
      break; }/* case_8 */
      case 11 : {/* case_9 */
        FlushSVCFrame(decoder_svc_Residu_SPS, DqIdNextNal_Nal_o, decoder_svc_Residu_Mmo, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o);
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
      break; }/* case_9 */
      case 14 : {/* case_10 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        PrefixNalUnit(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, DqIdNextNal_Nal_o, decoder_svc_Residu_Mmo, decoder_svc_Residu_SPS, decoder_svc_VideoParameter_EndOfSlice);
      break; }/* case_10 */
      case 15 : {/* case_11 */
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        subset_sps(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Residu_SPS, DqIdNextNal_Nal_o);
      break; }/* case_11 */
      case 20 : {/* case_12 */
        NalUnitHeader(GetNalBytesAu_rbsp_o, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice);
        slice_header_svc(4, GetNalBytesAu_rbsp_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
      break; }/* case_12 */
    } /* end switch_3 */
    
    
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_13 */
      case 5 : {/* case_14 */
        slice_header_svc(4, GetNalBytesAu_rbsp_o, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag, decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, decoder_svc_Residu_Current_pic, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Residu_Slice, decoder_svc_Residu_Mmo, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o, decoder_svc_VideoParameter_address_pic_o, decoder_svc_VideoParameter_Crop);
      break; }/* case_14 */
      case 8 : {/* case_15 */
        pic_parameter_set(GetNalBytesAu_rbsp_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_PPS, decoder_svc_Residu_SPS, GetNalBytesAu_rbsp_o_size[0]);
      break; }/* case_15 */
      case 20 : {/* case_16 */
        switch (decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[0]) { /* switch_17 */
          case 0 : {/* case_18 */
            slice_data_in_scalable_extension_cavlc(1920*1088/256, GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_VlcTab_PC_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=decoder_svc_VideoParameter_EndOfSlice;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=decoder_svc_Residu_Block;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=decoder_svc_Residu_Residu;
          break; }/* case_18 */
          case 1 : {/* case_19 */
            SliceCabac(1920*1088/256, GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=decoder_svc_VideoParameter_EndOfSlice;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=decoder_svc_Residu_Block;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=decoder_svc_Residu_Residu;
          break; }/* case_19 */
        } /* end switch_17 */
        *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o[0].slice_type;
      break; }/* case_16 */
    } /* end switch_13 */
    
    
    switch (decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_20 */
      case 1 : {/* case_21 */
        switch (decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[0]) { /* switch_22 */
          case 0 : {/* case_23 */
            slice_base_layer_cavlc(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_VlcTab_PC_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=decoder_svc_VideoParameter_EndOfSlice;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_23 */
          case 1 : {/* case_24 */
            slice_base_layer_cabac(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=decoder_svc_VideoParameter_EndOfSlice;
            decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_24 */
        } /* end switch_22 */
        *decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o=decoder_svc_Residu_Slice[0].slice_type;
        switch (decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[0]) { /* switch_25 */
          case 0 : {/* case_26 */
            Decode_P_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_PC_Main_vector_o->baseline_vectors, decoder_svc_Residu_RefL0, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, &decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o[decoder_svc_Residu_Current_pic->MvMemoryAddress], &decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o[decoder_svc_Residu_Current_pic->MvMemoryAddress >> 1], decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_26 */
          case 1 : {/* case_27 */
            Decode_B_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_PC_Main_vector_o, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_27 */
          case 2 : {/* case_28 */
            Decode_I_avc(decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_slice_main_vector_PC_Main_vector_o->baseline_vectors, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, DqIdNextNal_Nal_o, &decoder_svc_PictureBuffer_RefY[decoder_svc_Residu_Current_pic->MemoryAddress], &decoder_svc_PictureBuffer_RefU[decoder_svc_Residu_Current_pic->MemoryAddress>>2], &decoder_svc_PictureBuffer_RefV[decoder_svc_Residu_Current_pic->MemoryAddress>>2]);
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_28 */
        } /* end switch_25 */
        FinishFrameSVC(1920*1088/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o[0], decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o, decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
      break; }/* case_21 */
      case 5 : {/* case_29 */
        switch (decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[0]) { /* switch_30 */
          case 0 : {/* case_31 */
            slice_base_layer_cavlc(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_VlcTab_PC_o, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_31 */
          case 1 : {/* case_32 */
            slice_base_layer_cabac(GetNalBytesAu_rbsp_o, GetNalBytesAu_rbsp_o_size, decoder_svc_Nal_Compute_SetPos_Pos, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Residu_SliceGroupId, decoder_svc_Residu_Current_pic, decoder_svc_Residu_RefL1, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o, decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o, decoder_svc_Residu_Slice, decoder_svc_Residu_MbToSliceGroupMap, decoder_svc_Residu_SliceTab, decoder_svc_Residu_Block, decoder_svc_Residu_Residu, decoder_svc_VideoParameter_EndOfSlice, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref);
            decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=decoder_svc_Residu_Slice;
            decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=decoder_svc_Residu_SliceTab;
            decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=decoder_svc_Residu_Residu;
            decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=decoder_svc_Residu_Block;
          break; }/* case_32 */
        } /* end switch_30 */
        Decode_I_avc(decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Nal_Compute_slice_layer_main_CondO9_o, decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, decoder_svc_slice_main_vector_PC_Main_vector_o->baseline_vectors, decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, DqIdNextNal_Nal_o, &decoder_svc_PictureBuffer_RefY[decoder_svc_Residu_Current_pic->MemoryAddress], &decoder_svc_PictureBuffer_RefU[decoder_svc_Residu_Current_pic->MemoryAddress>>2], &decoder_svc_PictureBuffer_RefV[decoder_svc_Residu_Current_pic->MemoryAddress>>2]);
        FinishFrameSVC(1920*1088/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Residu_Slice, decoder_svc_VideoParameter_EndOfSlice[0], decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, decoder_svc_Nal_Compute_slice_layer_main_CondO0_o, decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, decoder_svc_Nal_Compute_slice_layer_main_CondO5_o, decoder_svc_Nal_Compute_slice_layer_main_CondO6_o, decoder_svc_Nal_Compute_slice_layer_main_CondO7_o, decoder_svc_Nal_Compute_slice_layer_main_CondO8_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
      break; }/* case_29 */
      case 6 : {/* case_33 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_33 */
      case 7 : {/* case_34 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_34 */
      case 8 : {/* case_35 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_35 */
      case 11 : {/* case_36 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_36 */
      case 14 : {/* case_37 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_37 */
      case 15 : {/* case_38 */
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_PictureBuffer_RefY;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_PictureBuffer_RefV;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_PictureBuffer_RefU;
      break; }/* case_38 */
      case 20 : {/* case_39 */
        switch (decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[0]) { /* switch_40 */
          case 0 : {/* case_41 */
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            Decode_P_svc(1920*1088/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_PC_Main_vector_o->baseline_vectors, decoder_svc_Residu_RefL0, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Svc_Vectors_PC_Svc_Vectors, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_41 */
          case 1 : {/* case_42 */
            Decode_B_svc(1920*1088/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_PC_Main_vector_o, decoder_svc_Residu_RefL0, decoder_svc_Residu_RefL1, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_Svc_Vectors_PC_Svc_Vectors, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16, decoder_svc_MvBuffer_Mv, decoder_svc_MvBuffer_1_Mv, decoder_svc_MvBuffer_Ref, decoder_svc_MvBuffer_1_Ref, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV, decoder_svc_ResiduBuffer_RefY, decoder_svc_ResiduBuffer_RefU, decoder_svc_ResiduBuffer_RefV);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_42 */
          case 2 : {/* case_43 */
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=decoder_svc_MvBuffer_1_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=decoder_svc_MvBuffer_Ref;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=decoder_svc_MvBuffer_1_Mv;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=decoder_svc_MvBuffer_Mv;
            Decode_I_svc(1920*1088/256, decoder_svc_Residu_SPS, decoder_svc_Residu_PPS, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_slice_main_vector_PC_Main_vector_o->baseline_vectors, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, decoder_svc_PictureBuffer_RefY, decoder_svc_PictureBuffer_RefU, decoder_svc_PictureBuffer_RefV);
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=decoder_svc_PictureBuffer_RefY;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=decoder_svc_PictureBuffer_RefV;
            decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=decoder_svc_PictureBuffer_RefU;
          break; }/* case_43 */
        } /* end switch_40 */
        FinishFrameSVC(1920*1088/256, DqIdNextNal_Nal_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, decoder_svc_Residu_Current_pic, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o[0], decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o, decoder_svc_VideoParameter_Crop, decoder_svc_VideoParameter_ImgToDisplay, decoder_svc_VideoParameter_address_pic_o, decoder_svc_Residu_Mmo, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o, decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o, decoder_svc_VideoParameter_xsize_o, decoder_svc_VideoParameter_ysize_o);
        decoder_svc_Nal_Compute_CondO7_o=decoder_svc_VideoParameter_ysize_o;
        decoder_svc_Nal_Compute_CondO6_o=decoder_svc_VideoParameter_xsize_o;
        decoder_svc_Nal_Compute_CondO5_o=decoder_svc_VideoParameter_address_pic_o;
        decoder_svc_Nal_Compute_CondO4_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o;
        decoder_svc_Nal_Compute_CondO3_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o;
        decoder_svc_Nal_Compute_CondO2_o=decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o;
        decoder_svc_Nal_Compute_CondO1_o=decoder_svc_VideoParameter_ImgToDisplay;
        decoder_svc_Nal_Compute_CondO0_o=decoder_svc_VideoParameter_Crop;
      break; }/* case_39 */
    } /* end switch_20 */
    switch (decoder_svc_Nal_Compute_CondO1_o[0]) { /* switch_44 */
      case 1 : {/* case_45 */
        {
           int XDIM = ((int *) Display_1_Extract_Image_Y_o)[0] = *decoder_svc_Nal_Compute_CondO6_o;
           int YDIM = ((int *) Display_1_Extract_Image_Y_o)[1] = *decoder_svc_Nal_Compute_CondO7_o;
           uchar *Y = Display_1_Extract_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4;
           extract_picture(XDIM, YDIM, 16, decoder_svc_Nal_Compute_CondO0_o[0], decoder_svc_Nal_Compute_CondO4_o, decoder_svc_Nal_Compute_CondO2_o, decoder_svc_Nal_Compute_CondO3_o, decoder_svc_Nal_Compute_CondO5_o[0], Y, U, V);
        }
      break; }/* case_45 */
      case 2 : {/* case_46 */
        {
           int XDIM = ((int *) Display_1_Extract_1_Image_Y_o)[0] = *decoder_svc_Nal_Compute_CondO6_o;
           int YDIM = ((int *) Display_1_Extract_1_Image_Y_o)[1] = *decoder_svc_Nal_Compute_CondO7_o;
           uchar *Y = Display_1_Extract_1_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4;
           extract_picture(XDIM, YDIM, 16, decoder_svc_Nal_Compute_CondO0_o[0], decoder_svc_Nal_Compute_CondO4_o, decoder_svc_Nal_Compute_CondO2_o, decoder_svc_Nal_Compute_CondO3_o, decoder_svc_Nal_Compute_CondO5_o[0], Y, U, V);
        }
      break; }/* case_46 */
    } /* end switch_44 */
    
    switch (decoder_svc_Nal_Compute_CondO1_o[0]) { /* switch_47 */
      case 1 : {/* case_48 */
        {
           int XDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[0];
           int YDIM = ((unsigned int *) Display_1_Extract_Image_Y_o)[1];
           uchar *Y = Display_1_Extract_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4 ;
           SDL_Display(16, XDIM, YDIM, Y, U, V);
        }

      break; }/* case_48 */
      case 2 : {/* case_49 */
        {
           int XDIM = ((unsigned int *) Display_1_Extract_1_Image_Y_o)[0];
           int YDIM = ((unsigned int *) Display_1_Extract_1_Image_Y_o)[1];
           uchar *Y = Display_1_Extract_1_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4 ;
           SDL_Display(16, XDIM, YDIM, Y, U, V);
        }

      break; }/* case_49 */
    } /* end switch_47 */
  } 
  
  
  
  
  
  
  
  
CloseSDLDisplay();

  


  return(0);
} /* end of main */


