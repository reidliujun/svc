# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c"
# 1 "/home/masnec/workspace/svc/jni//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c"
# 36 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h" 1
# 65 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
#pragma warning (disable : 177)
# 128 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
typedef struct
{

 int offset_for_non_ref_pic;
 int offset_for_top_to_bottom_field;
 int offset_for_ref_frame[255];

 short delta_pic_order_cnt_bottom;
 short delta_pic_order_cnt[2];
 short max_pic_num;


 short pic_width_in_mbs ;
 short pic_height_in_map_units ;
 short PicSizeInMbs;
 short b_stride ;
 short b8_stride;
 short scaled_base_left_offset;
 short scaled_base_top_offset;
 short scaled_base_right_offset;
 short scaled_base_bottom_offset;



 short CropLeft;
    short CropRight;
    short CropTop;
    short CropBottom;


 short scaling_matrix_present;
 short scaling_matrix4[6][16];
 short scaling_matrix8[2][64];

 unsigned char profile_idc;
 unsigned char level_idc;
 unsigned char field_pic_flag ;
 unsigned char pic_order_cnt_type ;
 unsigned char frame_mbs_only_flag ;
 unsigned char delta_pic_order_always_zero_flag ;
 unsigned char log2_max_frame_num ;
 unsigned char log2_max_pic_order_cnt_lsb;
 unsigned char MbAdaptiveFrameFieldFlag ;
 unsigned char direct_8x8_inference_flag;
 unsigned char num_ref_frames_in_pic_order_cnt_cycle;
 unsigned char num_ref_frames;
 unsigned char nb_img_disp;



 unsigned char nal_hrd_parameters_present_flag;
 unsigned char vlc_hrd_parameters_present_flag;
 unsigned char cpb_cnt;
 unsigned char cpb_removal_delay_length;
 unsigned char pic_present_flag;
 unsigned char time_offset_length;
 unsigned char initial_cpb_removal_delay_length;





 unsigned char bit_depth_luma;
 unsigned char bit_depth_chroma;



 unsigned char extended_spatial_scalability;
 char slice_header_restriction_flag;
 unsigned char InterLayerDeblockingFilterFlag;
 unsigned char adaptive_tcoeff_level_prediction_flag;
 unsigned char tcoeff_level_prediction_flag;
 signed char chroma_phase_x;
 signed char chroma_phase_y;
 unsigned char chroma_format_idc;


 unsigned char MaxSpsId;

} SPS ;
# 216 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
typedef struct {


 short run_length_minus1 [8];
 short bottom_right_tab[8];
 short top_left_tab[8];


 short scaling_matrix8[2][64];
 short scaling_matrix4[6][16];
 short slice_group_change_rate_minus1 ;


 unsigned char seq_parameter_set_id ;
 unsigned char pic_init_qp;
 char chroma_qp_index_offset ;
 unsigned char num_slice_groups_minus1 ;
 unsigned char pic_order_present_flag ;
 unsigned char redundant_pic_cnt_present_flag ;
 unsigned char WeightedPredFlag ;
 unsigned char WeightedBipredIdc ;
 unsigned char deblocking_filter_control_present_flag ;
 unsigned char slice_group_map_type ;
 unsigned char entropy_coding_mode_flag ;
 unsigned char num_RefIdxL0_active_minus1 ;
 unsigned char num_RefIdxL1_active_minus1 ;

 unsigned char slice_group_change_direction_flag ;
 unsigned char constrained_intra_pred_flag ;



 unsigned char transform_8x8_mode_flag;
 char second_chroma_qp_index_offset;
 char alpha_offset;
 char beta_offset;


 unsigned char constrained_intra_upsampling_flag;
 char alpha_offset_upsapmpling;
 char beta_offset_upsapmpling;


 unsigned char MaxPpsId;
} PPS ;





typedef struct {
 short dist_scale_factor[16];
 short MapColList[32];


 short b_stride ;
 short b8_stride ;
 short mb_xy;
 short mb_stride ;
 short first_mb_in_slice ;
 short slice_group_change_cycle ;

 unsigned char slice_num ;
 char slice_qp_delta ;
 unsigned char slice_type ;
 unsigned char cabac_init_idc;

 unsigned char num_RefIdxL0_active_minus1;
 unsigned char num_RefIdxL1_active_minus1;
 unsigned char disable_deblocking_filter_idc;
 unsigned char direct_spatial_mv_pred_flag;


 unsigned char AdaptiveBaseModeFlag;
 unsigned char DefaultBaseModeFlag;
 unsigned char AdaptivePredictionFlag;
 unsigned char AdaptiveResidualPredictionFlag;
 unsigned char DefaultResidualPredictionFlag;
 unsigned char AdaptiveMotionPredictionFlag;
 unsigned char DefaultMotionPredictionFlag;
 unsigned char notDefaultScanIdx;
 unsigned char InterLayerDeblockingFilterIdc;
# 308 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
} SLICE;


typedef struct
{

 int MvMemoryAddress;
 int MemoryAddress;


 short poc;
 short frame_num;
 short pic_id;

 short ref_poc_l0[16];
 short ref_poc_l1[16];

 unsigned char slice_type;
 unsigned char long_term;
 unsigned char displayed;
 char reference;
 char ref_count_l0;
 char ref_count_l1;



 unsigned char KeyFrame;
 unsigned char PicToDisplay;

} LIST_MMO;





typedef struct
{
 int Crop;

 short x_size;
 short y_size;

 unsigned char is_ghost;
 unsigned char LayerId;

 LIST_MMO Picture [1];
} POC_PARAMS;



typedef struct MMCO{

 short short_frame_num;
 short long_term_frame_idx;
 unsigned char opcode;
} MMCO;



typedef struct
{

 int MvMemoryAddress[32];


 short frame_num_offset;
 short prev_frame_num;
 short prev_frame_num_offset;
 short frame_num;
 short pic_order_cnt_lsb;
 short prevPicOrderCntMsb;
 short prevPicOrderCntLsb;
 int num_decoded_frame;

 LIST_MMO LongRef[16];
 LIST_MMO ShortRef[16];

 MMCO Mmco[66];

 unsigned char index;
 unsigned char ShortRefCount;
 unsigned char LongRefCount;
 unsigned char RefCountL0;
 unsigned char RefCountL1;
 char FreeMvMemory;

} LAYER_MMO;



typedef struct
{
 int AuMemoryAddress[8 * 2];
 int FreeMemoryAddress[32];

 POC_PARAMS poc_params[24];
 LAYER_MMO LayerMMO[8];

 short last_poc;

 short PrevPicOrderCntLsb;
 short PrevPicOrderCntMsb;
 short LastDecodedPoc;



 short MaxHeight;
 short MaxWidth;


 char NumFreeAddress;
 char AuMemory;
 char num_free_poc_address;
 char nb_img_display;
 unsigned char LayerId;



 POC_PARAMS Prev[1];
 unsigned char low_delay;
 unsigned char has_b_frames;
 unsigned char NumDelayedPics;




 unsigned char MemoryAllocation;
} MMO;
# 444 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
typedef struct {
 short MvdL0 [16][2];
 short MvdL1 [16][2];


 short xBase;
 short yBase;
 short MbBaseAddr;
 short xBasePart;
 short yBasePart;


 unsigned char NumMbPart ;
 unsigned char MbPartPredMode [2];


 char RefIdxL0 [4];
 char RefIdxL1 [4];

 unsigned char Transform8x8;


 char MotionPredL0[4];
 char MotionPredL1[4];


 unsigned char NonZeroCount[48];

} DATA ;




typedef struct {
 int Addr ;
 int AvailA ;
    int AvailB ;
    int AvailC ;
    int AvailD ;
 int AvailE ;
    int AvailF ;
    int AvailG ;
    int AvailH ;
} CURRENT_BLOCK ;



typedef struct {
 int xN ;
 int yN ;
 int Avail ;
} NEIGHBOUR ;





typedef struct {
 int MbType ;
 int Mode;
 int Intra16x16PredMode ;
 int intra_chroma_pred_mode ;
 int SubMode;
 int SubMbType [4];
 int AvailMask;
 int SliceNum;

 int Cbp;
 int Qp;
 int QpLf;
 int blk4x4;
 int blk4x4_Res;
 int Transform8x8;
 int residual_prediction_flag;
 int base_mode_flag;
 int Is_Intra4x4;
 int in_crop_window;



 short Intra16x16DCLevel [16];
 short LumaLevel [16 * 16];
 short ChromaDCLevel_Cb [4];
 short ChromaDCLevel_Cr [4];
 short ChromaACLevel_Cb [4 * 16];
 short ChromaACLevel_Cr [4 * 16];
} RESIDU ;




typedef struct {
 short code ;
 short len ;
} VLC_TYPE ;

typedef struct {
 VLC_TYPE Chroma_dc_coeff_token_vlc [256];
 VLC_TYPE Chroma_dc_total_zeros_vlc [24];
 VLC_TYPE Coeff_token_vlc [1388];
 VLC_TYPE Total_zeros_vlc [7680];
 VLC_TYPE Run_vlc [48];
 VLC_TYPE Run7_vlc [96];
 int Coeff_token_vlc_adress [4];

} VLC_TABLES ;

typedef struct W_TABLES{
 short W4x4_intra [6][16] ;
 short W4x4_Cb_intra [6][16] ;
 short W4x4_Cr_intra [6][16] ;
 short W4x4_inter [6][16] ;
 short W4x4_Cb_inter [6][16] ;
 short W4x4_Cr_inter [6][16] ;
 short W8x8_intra [6][64] ;
 short W8x8_inter [6][64] ;
} W_TABLES;

typedef struct PMbInfo{
 unsigned char name;
    unsigned char type_0;
 unsigned char type_1;
 unsigned char partcount;
    unsigned char Mode;
} PMbInfo;



typedef struct IMbInfo{
    unsigned char type;
    unsigned char pred_mode;
    unsigned char Cbp;
} IMbInfo;



typedef struct {
# 662 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h"
 int pr_num_dId_minus1;
} SEI;


typedef void (*interpol_4x4)(unsigned char image_luma [], unsigned char refPicLXl[], const short PicWidthSamples, const short stride);

typedef void (*interpol_8x8)(unsigned char image_luma [], unsigned char refPicLXl[], const short PicWidthSamples, const short stride);


typedef void (*mb4x4_mode_pf)( unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy) ;
typedef void (*mb16x16_mode_pf) (unsigned char * predict_sample, const short PicWidthInPix, int Mask);
typedef void (*mbChroma_mode_pf)(unsigned char * predict_sample, const short PicWidthInPix, int Mask);


typedef struct {
 mb4x4_mode_pf Ptr_mb4x4_mode[9];
 mb16x16_mode_pf Ptr_mb16x16_mode[4];
 mbChroma_mode_pf Ptr_mbChroma_mode_pf[4];
 interpol_4x4 Ptr_interpol_4x4[16];
 interpol_8x8 Ptr_interpol_8x8[16];
} STRUCT_PF;
# 37 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h" 1
# 38 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h"
# 1 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/string.h" 1
# 31 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/string.h"
# 1 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/sys/cdefs.h" 1
# 65 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/sys/cdefs.h"
# 1 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/sys/cdefs_elf.h" 1
# 66 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/sys/cdefs.h" 2
# 32 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/string.h" 2
# 1 "/android/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stddef.h" 1 3 4
# 149 "/android/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 211 "/android/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stddef.h" 3 4
typedef unsigned int size_t;
# 323 "/android/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stddef.h" 3 4
typedef unsigned char wchar_t;
# 33 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/string.h" 2
# 1 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/malloc.h" 1
# 32 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/malloc.h"
# 1 "/android/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stddef.h" 1 3 4
# 33 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/malloc.h" 2



extern __attribute__((malloc)) void* malloc(size_t);
extern __attribute__((malloc)) void* calloc(size_t, size_t);
extern __attribute__((malloc)) void* realloc(void *, size_t);
extern void free(void *);

extern void* memalign(size_t alignment, size_t bytesize);
extern void* valloc(size_t bytesize);
extern void* pvalloc(size_t bytesize);
extern int mallopt(int param_number, int param_value);
extern size_t malloc_footprint(void);
extern size_t malloc_max_footprint(void);

struct mallinfo {
    size_t arena;
    size_t ordblks;
    size_t smblks;
    size_t hblks;
    size_t hblkhd;
    size_t usmblks;
    size_t fsmblks;
    size_t uordblks;
    size_t fordblks;
    size_t keepcost;
};

extern struct mallinfo mallinfo(void);
# 78 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/malloc.h"
extern size_t malloc_usable_size(void* block);
# 99 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/malloc.h"
extern void malloc_stats(void);


# 34 "/android/android-ndk-r7/platforms/android-8/arch-arm/usr/include/string.h" 2



extern void* memccpy(void *, const void *, int, size_t);
extern void* memchr(const void *, int, size_t);
extern void* memrchr(const void *, int, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void* memcpy(void *, const void *, size_t);
extern void* memmove(void *, const void *, size_t);
extern void* memset(void *, int, size_t);
extern void* memmem(const void *, size_t, const void *, size_t);
extern void memswap(void *, void *, size_t);

extern char* index(const char *, int);
extern char* rindex(const char *, int);
extern char* strchr(const char *, int);
extern char* strrchr(const char *, int);

extern size_t strlen(const char *);
extern int strcmp(const char *, const char *);
extern char* strcpy(char *, const char *);
extern char* strcat(char *, const char *);

extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
extern char* strdup(const char *);

extern char* strstr(const char *, const char *);
extern char* strcasestr(const char *haystack, const char *needle);
extern char* strtok(char *, const char *);
extern char* strtok_r(char *, const char *, char**);

extern char* strerror(int);
extern int strerror_r(int errnum, char *buf, size_t n);

extern size_t strnlen(const char *, size_t);
extern char* strncat(char *, const char *, size_t);
extern char* strndup(const char *, size_t);
extern int strncmp(const char *, const char *, size_t);
extern char* strncpy(char *, const char *, size_t);

extern size_t strlcat(char *, const char *, size_t);
extern size_t strlcpy(char *, const char *, size_t);

extern size_t strcspn(const char *, const char *);
extern char* strpbrk(const char *, const char *);
extern char* strsep(char **, const char *);
extern size_t strspn(const char *, const char *);

extern char* strsignal(int sig);

extern int strcoll(const char *, const char *);
extern size_t strxfrm(char *, const char *, size_t);


# 39 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/type.h" 1
# 40 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/symbol.h" 1
# 41 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h" 2


void init_non_zero_count_cache ( unsigned char non_zero_count_cache [], unsigned char val );
void init_intra_pred( int *ao_pstIntra);
void init_int_tab (const int ai_iSize, int ao_tiPicture_mb_type [ ] );
void InitUcharTab (const int ai_iSize, unsigned char ao_tiPicture_mb_type [ ] );
void init_data_block (const int ai_iSize, DATA ao_tstBlock[ ]);
void init_data_residu (const int ai_iSize, RESIDU *Block);
# 57 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h"
static __inline void init_ref_cache(short *ref_cache){

 memset(ref_cache,-1, 48 * sizeof(short));
}







static __inline void init_mv_cache(short mv_cache[][2]){

 memset(mv_cache,0, 96 * sizeof(short));
}
# 80 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h"
static __inline void init_block (DATA *ao_pstBlock){

 memset (ao_pstBlock,'\0',sizeof(DATA));
}


static __inline void init_residu(RESIDU *Residu){

 memset (Residu,'\0',sizeof(RESIDU));
}
# 98 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h"
static __inline void init_P_skip_mb(RESIDU *residu, int LastQp, SLICE *Slice)
{
 residu -> Qp = LastQp;
 residu -> Mode = 4;
 residu -> MbType = 255;
 residu -> SliceNum = Slice -> slice_num;
 residu -> residual_prediction_flag = Slice -> DefaultResidualPredictionFlag;
}
# 116 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/init_data.h"
static __inline void init_B_skip_mb( RESIDU *residu, int LastQp, SLICE *Slice)
{
 residu -> Mode = 0;
 residu -> MbType = 254;
 residu -> Qp = LastQp;
 residu -> SliceNum = Slice -> slice_num;
 residu -> residual_prediction_flag = Slice -> DefaultResidualPredictionFlag;
}
# 38 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/write_back.h" 1
# 55 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/write_back.h"
void write_back_motion_cache( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, short ai_tiMv[ ], short ai_tiMv_cache[ ][2]
, short ai_tiRef[ ], short aio_tiRef_cache[ ], int mb_stride);

void NonZeroCountManagement( const unsigned char Non_zero_count_cache [], const short iMb_x, const short iMb_y, const int PicWidthInMbs
       , const int PicHeightInMbs,DATA *aio_tstBlock, RESIDU *CurrentResidu);
void WriteBackMotionCacheCurrPosition(const int ai_iB_stride , const int ai_iB8_stride, short ai_tiMv[ ], short ai_tiMv_cache[ ][2], short ai_tiRef[ ], short aio_tiRef_cache[ ]);
void write_back_motion_cache_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv[ ], short ai_tiMv_cache[ ][2], short ai_tiRef[ ], short aio_tiRef_cache[ ]);
void write_back_motion_C( const int ai_iB_stride, const int ai_iB8_stride , short MvdL0[ ], short ai_tiMv_cache[ ][2], short ao_tiRef[ ], short ai_tiRef_cache[]);
void write_back_P_skip_motion( const int ai_iB_stride, const int ai_iB8_stride, short MvdL0[ ], short ai_tiMv_cache[2], short ao_tiRef[ ], const short ai_tiRef_cache);


static __inline void write_back_cbp_8x8(unsigned char *tab, RESIDU *residu){

 if(tab[12]){
  if (!(residu -> blk4x4 & 1))
   residu -> blk4x4 += 1;
  if (!(residu -> blk4x4 & 2))
   residu -> blk4x4 += 2;
  if (!(residu -> blk4x4 & 16))
   residu -> blk4x4 += 16;
  if (!(residu -> blk4x4 & 32))
   residu -> blk4x4 += 32;
 }

 if(tab[14]){
  if (!(residu -> blk4x4 & 4))
   residu -> blk4x4 += 4;
  if (!(residu -> blk4x4 & 8))
   residu -> blk4x4 += 8;
  if (!(residu -> blk4x4 & 64))
   residu -> blk4x4 += 64;
  if (!(residu -> blk4x4 & 128))
   residu -> blk4x4 += 128;
 }


 if(tab[28]){
  if (!(residu -> blk4x4 & 256))
   residu -> blk4x4 += 256;
  if (!(residu -> blk4x4 & 512))
   residu -> blk4x4 += 512;
  if (!(residu -> blk4x4 & 4096))
   residu -> blk4x4 += 4096;
  if (!(residu -> blk4x4 & 8192))
   residu -> blk4x4 += 8192;
 }


 if(tab[30]){
  if (!(residu -> blk4x4 & 1024))
   residu -> blk4x4 += 1024;
  if (!(residu -> blk4x4 & 2048))
   residu -> blk4x4 += 2048;
  if (!(residu -> blk4x4 & 16384))
   residu -> blk4x4 += 16384;
  if (!(residu -> blk4x4 & 32768))
   residu -> blk4x4 += 32768;
 }
}


static __inline void write_back_cbp_C(unsigned char *tab, RESIDU *residu){

 int i, j;
 for(i = 0; i < 4; i++){
  for(j = 0; j < 4; j++){
   if(tab[12 + j + 8 * i] && !(residu -> blk4x4 & (1 << (j + 4 * i)))){
    residu -> blk4x4 += 1 << (j + 4 *i);
   }
  }
 }
}
# 143 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/write_back.h"
static __inline void write_back_non_zero_count_C( const unsigned char ai_tiNon_zero_count_cache [], const short ai_iMb_x, const short ai_iMb_y
             , const int ai_iMb_stride, const int PicHeightInMbs, DATA aio_tstBlock[])
{



 aio_tstBlock -> NonZeroCount[12] = ai_tiNon_zero_count_cache [4 + 8 * 1];
 aio_tstBlock -> NonZeroCount[13] = ai_tiNon_zero_count_cache [5 + 8 * 1];
 aio_tstBlock -> NonZeroCount[14] = ai_tiNon_zero_count_cache [6 + 8 * 1];
 aio_tstBlock -> NonZeroCount[15] = ai_tiNon_zero_count_cache [7 + 8 * 1];

 aio_tstBlock -> NonZeroCount[20] = ai_tiNon_zero_count_cache [4 + 8 * 2];
 aio_tstBlock -> NonZeroCount[21] = ai_tiNon_zero_count_cache [5 + 8 * 2];
 aio_tstBlock -> NonZeroCount[22] = ai_tiNon_zero_count_cache [6 + 8 * 2];
 aio_tstBlock -> NonZeroCount[23] = ai_tiNon_zero_count_cache [7 + 8 * 2];

 aio_tstBlock -> NonZeroCount[28] = ai_tiNon_zero_count_cache [4 + 8 * 3];
 aio_tstBlock -> NonZeroCount[29] = ai_tiNon_zero_count_cache [5 + 8 * 3];
 aio_tstBlock -> NonZeroCount[30] = ai_tiNon_zero_count_cache [6 + 8 * 3];
 aio_tstBlock -> NonZeroCount[31] = ai_tiNon_zero_count_cache [7 + 8 * 3];

 aio_tstBlock -> NonZeroCount[36] = ai_tiNon_zero_count_cache [4 + 8 * 4];
 aio_tstBlock -> NonZeroCount[37] = ai_tiNon_zero_count_cache [5 + 8 * 4];
 aio_tstBlock -> NonZeroCount[38] = ai_tiNon_zero_count_cache [6 + 8 * 4];
 aio_tstBlock -> NonZeroCount[39] = ai_tiNon_zero_count_cache [7 + 8 * 4];



 aio_tstBlock -> NonZeroCount[17] = ai_tiNon_zero_count_cache [1 + 8 * 2];
 aio_tstBlock -> NonZeroCount[18] = ai_tiNon_zero_count_cache [2 + 8 * 2];
 aio_tstBlock -> NonZeroCount[9] = ai_tiNon_zero_count_cache [1 + 8 * 1];
 aio_tstBlock -> NonZeroCount[10] = ai_tiNon_zero_count_cache [2 + 8 * 1];

 aio_tstBlock -> NonZeroCount[41] = ai_tiNon_zero_count_cache [1 + 8 * 5];
 aio_tstBlock -> NonZeroCount[42] = ai_tiNon_zero_count_cache [2 + 8 * 5];
 aio_tstBlock -> NonZeroCount[33] = ai_tiNon_zero_count_cache [1 + 8 * 4];
 aio_tstBlock -> NonZeroCount[34] = ai_tiNon_zero_count_cache [2 + 8 * 4];





 if( ai_iMb_x && aio_tstBlock[- 1] . Transform8x8){
  aio_tstBlock -> NonZeroCount[11] = aio_tstBlock -> NonZeroCount[19] = aio_tstBlock [- 1] . NonZeroCount[14];
  aio_tstBlock -> NonZeroCount[27] = aio_tstBlock -> NonZeroCount[35] = aio_tstBlock [- 1] . NonZeroCount[30];
 }else if( ai_iMb_x){
  aio_tstBlock -> NonZeroCount[11] = aio_tstBlock [-1] . NonZeroCount[15];
  aio_tstBlock -> NonZeroCount[19] = aio_tstBlock [-1] . NonZeroCount[23];
  aio_tstBlock -> NonZeroCount[27] = aio_tstBlock [-1] . NonZeroCount[31];
  aio_tstBlock -> NonZeroCount[35] = aio_tstBlock [-1] . NonZeroCount[39];
 }


 if( ai_iMb_x != ai_iMb_stride - 1){
  aio_tstBlock [1] . NonZeroCount[11] = aio_tstBlock -> NonZeroCount[15];
  aio_tstBlock [1] . NonZeroCount[19] = aio_tstBlock -> NonZeroCount[23];
  aio_tstBlock [1] . NonZeroCount[27] = aio_tstBlock -> NonZeroCount[31];
  aio_tstBlock [1] . NonZeroCount[35] = aio_tstBlock -> NonZeroCount[39];
 }


 if(ai_iMb_y && aio_tstBlock[- ai_iMb_stride] . Transform8x8){
  aio_tstBlock -> NonZeroCount[4] = aio_tstBlock -> NonZeroCount[5] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[28];
  aio_tstBlock -> NonZeroCount[6] = aio_tstBlock -> NonZeroCount[7] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[30];
  aio_tstBlock -> NonZeroCount[1] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[17];
  aio_tstBlock -> NonZeroCount[2] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[18];
  aio_tstBlock -> NonZeroCount[25] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[41];
  aio_tstBlock -> NonZeroCount[26] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[42];
 }else if(ai_iMb_y){
  aio_tstBlock -> NonZeroCount[4] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[36];
  aio_tstBlock -> NonZeroCount[5] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[37];
  aio_tstBlock -> NonZeroCount[6] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[38];
  aio_tstBlock -> NonZeroCount[7] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[39];
  aio_tstBlock -> NonZeroCount[1] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[17];
  aio_tstBlock -> NonZeroCount[2] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[18];
  aio_tstBlock -> NonZeroCount[25] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[41];
  aio_tstBlock -> NonZeroCount[26] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[42];
 }


 if( (ai_iMb_y + 1) < PicHeightInMbs){
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[4] = aio_tstBlock -> NonZeroCount[36];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[5] = aio_tstBlock -> NonZeroCount[37];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[6] = aio_tstBlock -> NonZeroCount[38];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[7] = aio_tstBlock -> NonZeroCount[39];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[1] = aio_tstBlock -> NonZeroCount[17];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[2] = aio_tstBlock -> NonZeroCount[18];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[25] = aio_tstBlock -> NonZeroCount[41];
  aio_tstBlock [ai_iMb_stride] . NonZeroCount[26] = aio_tstBlock -> NonZeroCount[42];
 }
}
# 248 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/write_back.h"
static __inline void init_skip_non_zero_count( const short ai_iMb_x, const short ai_iMb_y, const int ai_iMb_stride,
             const int ai_iPicWidthInMbs, DATA aio_tstBlock[])
{

 const int iMb_xy = ai_iMb_x + ai_iMb_y * (short)(ai_iMb_stride);


 aio_tstBlock[iMb_xy] . NonZeroCount[12] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[13] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[14] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[15] = 0;

 aio_tstBlock[iMb_xy] . NonZeroCount[20] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[21] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[22] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[23] = 0;

 aio_tstBlock[iMb_xy] . NonZeroCount[28] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[29] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[30] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[31] = 0;

 aio_tstBlock[iMb_xy] . NonZeroCount[36] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[37] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[38] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[39] = 0;



 aio_tstBlock[iMb_xy] . NonZeroCount[17] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[18] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[9] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[10] = 0;

 aio_tstBlock[iMb_xy] . NonZeroCount[41] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[42] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[33] = 0;
 aio_tstBlock[iMb_xy] . NonZeroCount[34] = 0;


 if( ai_iMb_x && aio_tstBlock[iMb_xy - 1] . Transform8x8){
  aio_tstBlock[iMb_xy] . NonZeroCount[11] = aio_tstBlock[iMb_xy] . NonZeroCount[19] = aio_tstBlock [iMb_xy - 1] . NonZeroCount[14];
  aio_tstBlock[iMb_xy] . NonZeroCount[27] = aio_tstBlock[iMb_xy] . NonZeroCount[35] = aio_tstBlock [iMb_xy - 1] . NonZeroCount[30];
 }else if( ai_iMb_x){
  aio_tstBlock[iMb_xy] . NonZeroCount[11] = aio_tstBlock [iMb_xy-1] . NonZeroCount[15];
  aio_tstBlock[iMb_xy] . NonZeroCount[19] = aio_tstBlock [iMb_xy-1] . NonZeroCount[23];
  aio_tstBlock[iMb_xy] . NonZeroCount[27] = aio_tstBlock [iMb_xy-1] . NonZeroCount[31];
  aio_tstBlock[iMb_xy] . NonZeroCount[35] = aio_tstBlock [iMb_xy-1] . NonZeroCount[39];
 }


 if( ai_iMb_x != ai_iMb_stride - 1){
  aio_tstBlock [iMb_xy+1] . NonZeroCount[11] = 0;
  aio_tstBlock [iMb_xy+1] . NonZeroCount[19] = 0;
  aio_tstBlock [iMb_xy+1] . NonZeroCount[27] = 0;
  aio_tstBlock [iMb_xy+1] . NonZeroCount[35] = 0;
 }


 if(ai_iMb_y && aio_tstBlock[iMb_xy - ai_iMb_stride] . Transform8x8){
  aio_tstBlock[iMb_xy] . NonZeroCount[4] = aio_tstBlock[iMb_xy] . NonZeroCount[5] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[28];
  aio_tstBlock[iMb_xy] . NonZeroCount[6] = aio_tstBlock[iMb_xy] . NonZeroCount[7] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[30];
  aio_tstBlock[iMb_xy] . NonZeroCount[1] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[17];
  aio_tstBlock[iMb_xy] . NonZeroCount[2] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[18];
  aio_tstBlock[iMb_xy] . NonZeroCount[25] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[41];
  aio_tstBlock[iMb_xy] . NonZeroCount[26] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[42];
 }else if(ai_iMb_y){
  aio_tstBlock[iMb_xy] . NonZeroCount[4] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[36];
  aio_tstBlock[iMb_xy] . NonZeroCount[5] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[37];
  aio_tstBlock[iMb_xy] . NonZeroCount[6] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[38];
  aio_tstBlock[iMb_xy] . NonZeroCount[7] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[39];
  aio_tstBlock[iMb_xy] . NonZeroCount[1] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[17];
  aio_tstBlock[iMb_xy] . NonZeroCount[2] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[18];
  aio_tstBlock[iMb_xy] . NonZeroCount[25] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[41];
  aio_tstBlock[iMb_xy] . NonZeroCount[26] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[42];
 }


 if( (iMb_xy + ai_iMb_stride) < ai_iPicWidthInMbs){
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[4] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[5] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[6] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[7] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[1] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[2] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[25] = 0;
  aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[26] = 0;
 }
}



static __inline void FillMvCacheCurrPosition_C(short MvCache[][2], short *MvdL0){

 MvCache[12 + 0][0] = *(MvdL0++);
 MvCache[12 + 0][1] = *(MvdL0++);
 MvCache[12 + 1][0] = *(MvdL0++);
 MvCache[12 + 1][1] = *(MvdL0++);
 MvCache[12 + 2][0] = *(MvdL0++);
 MvCache[12 + 2][1] = *(MvdL0++);
 MvCache[12 + 3][0] = *(MvdL0++);
 MvCache[12 + 3][1] = *MvdL0;
}
# 442 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/write_back.h"
static __inline void FillMvCurrPosition(short *MvdL0, short MvCache[][2]){

 *(MvdL0++) = MvCache[12 + 0][0];
 *(MvdL0++) = MvCache[12 + 0][1];
 *(MvdL0++) = MvCache[12 + 1][0];
 *(MvdL0++) = MvCache[12 + 1][1];
 *(MvdL0++) = MvCache[12 + 2][0];
 *(MvdL0++) = MvCache[12 + 2][1];
 *(MvdL0++) = MvCache[12 + 3][0];
 *(MvdL0) = MvCache[12 + 3][1];
}


static __inline void FillRefCacheCurrPosition(short *RefCache, short *Ref){

 RefCache[12] = RefCache[12 + 1] = RefCache[12 + 8] = RefCache[12 + 9] = Ref[0];
 RefCache[14] = RefCache[14 + 1] = RefCache[14 + 8] = RefCache[14 + 9] = Ref[1];
}

static __inline void FillRefCurrPosition(short *Ref, short *RefCache){

 Ref[0] = RefCache[12];
 Ref[1] = RefCache[12 + 2];
}
# 39 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/mb_pred_main.h" 1
# 39 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/mb_pred_main.h"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h" 1
# 37 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/symbol.h" 1
# 38 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h" 2
# 51 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
int read_ue_C ( const unsigned char * tab, int *pos );
# 63 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
extern const char golomb_to_intra4x4 [48];
extern const char golomb_to_inter [48];
# 76 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline unsigned int showNbits_9max ( const unsigned char *tab, const int position, const int n )
{
    const unsigned int pos_bit = position & 0x7 ;
    const unsigned int pos_char = position >> 3 ;

    unsigned int c = (unsigned int)(*(tab + pos_char) << 8) | *(tab + pos_char + 1);
    c = (unsigned int)(c << (pos_bit + 16));
    c = (unsigned int)(c >> (32 - n));

    return (c);
}
# 98 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline unsigned int showNbits ( const unsigned char * tab, const int position, const int n )
{
# 115 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
    const unsigned int pos_bit = position & 0x7 ;
    const unsigned int pos_char = position >> 3 ;
    unsigned int c;

    c = (unsigned int)(*(tab + pos_char) << 24) | *(tab + pos_char + 1) << 16 | *(tab + pos_char + 2) << 8 | *(tab + pos_char + 3);
    c = (unsigned int)(c << pos_bit);
    c = (unsigned int)(c >> (32 - n));

    return (c);

}
# 134 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline unsigned int show3Bytes_Nal ( const unsigned char * tab, const int pos_char ){
# 146 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
    unsigned int c = (*(tab + pos_char) << 16 | *(tab + pos_char + 1) << 8 | *(tab + pos_char + 2)) & 0xffffff ;

    return (c);

}
# 162 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline unsigned int getNbits ( const unsigned char *tab, int *position, const int n )
{

    unsigned int tmp = showNbits(tab, *position, n);
    *position += n ;
    return tmp ;
}


static __inline int show32bits_C ( const unsigned char * tab, const int position )
{


    unsigned int tmp_msb = showNbits(tab, position, 16);
    unsigned int tmp_lsb = showNbits(tab, position + 16, 16);
    unsigned int c = (tmp_msb << 16 | tmp_lsb) & 0xffffffff ;

    return c ;
}
# 314 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline unsigned int get32bits ( const unsigned char * tab, int *position )
{

    int tmp = show32bits_C(tab, *position);
    *position += 32 ;
    return ((unsigned int)tmp) ;
}
# 336 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline int read_me ( const unsigned char *tab, int *pos, int mode )
{
    int val = read_ue_C(tab, pos);

    if ( mode < 4)
        return golomb_to_intra4x4 [val];
    else
        return golomb_to_inter [val];

}
# 356 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline int read_se ( const unsigned char * tab, int *pos )
{
    int val = read_ue_C(tab, pos);
    return val & 0x01 ? ((val >> 1) + 1) : -(val >> 1);
}
# 372 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline int log_base2 ( int a )
{



    int n ;

    a-- ;
    n = 0 ;
    while ( a > 0 ) {
        a = a >> 1 ;
        n++ ;
    }
    return (n);

}
# 400 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline int read_te ( const unsigned char * tab, int *pos, int x )
{
    if ( x == 1 ) {
        return 1 - getNbits(tab, pos, 1);
    } else if ( x > 1 ) {
        return read_ue_C(tab, pos);
    }
    return 0 ;
}
# 417 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline int bytes_aligned ( int position )
{
    return position & 0x07 ? 0 : 1 ;
}
# 430 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/bitstream.h"
static __inline void rbsp_trailing_bits (int *position )
{

 int n;
 (*position)++;

 if(!bytes_aligned(*position))
 { n = (8 -((*position) & 0x7));
  (*position) += n;
 }
}
# 40 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/mb_pred_main.h" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/ErrorsDetection.h" 1
# 43 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/ErrorsDetection.h"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/symbol.h" 1
# 44 "/home/masnec/workspace/svc/jni/AVC/h264_baseline_decoder/lib_baseline/ErrorsDetection.h" 2
# 41 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/mb_pred_main.h" 2


extern const int sub_mb_b_name[13];

int sub_mb_pred_B ( int *dct_allowed,const unsigned char *ai_pcData, int *aio_piPosition,
       const SLICE *aio_pstSlice, RESIDU *Current_residu, short mv_cache_l0[][2],
       short ref_cache_l0[], short mv_cache_l1[][2], short ref_cache_l1[],
       short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1,
       int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc);


int mb_pred_B ( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *aio_pstSlice,
    DATA * aio_pstMacroblock, RESIDU *CurrResidu, short mv_cache_l0[][2],
    short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1,
    short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1,
    int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc);


void motion_vector_B_skip(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2],
        short aio_tref_cache_l0 [], short aio_tref_cache_l1 [],
        short aio_tmv_l0[], short aio_tmv_l1[], short ref_l1_l0[], short ref_l1_l1[],
        const SLICE *slice, int direct_8x8_inference_flag,int long_term, int slice_type, int is_svc);


static __inline int GetCavlcBRefLx(const unsigned char *Data, int *Position, int *SubMbType, char *RefIdxLx, int NumRefIdxLx, int PredLx){

 int mbPartIdx;
 for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
  int sub_mb_type = SubMbType[mbPartIdx];
  if ((sub_mb_b_name[sub_mb_type] != 9) && (sub_mb_b_name[sub_mb_type] != PredLx) ) {
   RefIdxLx [mbPartIdx] = read_te(Data, Position, NumRefIdxLx);
  }else {
   RefIdxLx [mbPartIdx] = 0;
  }
 }







 return 0;
}
# 40 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.h" 1
# 40 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.h"
void SkipB(SLICE *Slice, const SPS *Sps, RESIDU *CurrResidu, DATA *Block, unsigned char *SliceTable, short iMb_x, short iMb_y,
     int Last_Qp, short mv_cache_l0[][2], short *ref_cache_l0, short *mvl0_io, short *refl0_io,
     short mv_cache_l1[][2], short *ref_cache_l1, short *mvl1_io, short *refl1_io,
     short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, LIST_MMO *RefListl1);
# 41 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/fill_caches_cabac.h" 1
# 40 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/fill_caches_cabac.h"
# 1 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/main_data.h" 1
# 47 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/main_data.h"
typedef struct CABACContext{
    int low;
    int range;
    int outstanding_count;
    unsigned char lps_range[2*65][4];
    unsigned char lps_state[2*64];
    unsigned char mps_state[2*64];
    unsigned char *bytestream_start;
    unsigned char *bytestream;
    unsigned char *bytestream_end;

}CABACContext;

typedef struct {

    int intra_chroma_pred_mode_left ;
    int intra_chroma_pred_mode_top ;
 int Cbp_a;
 int Cbp_b;
 int last_cbp;
 int last_qp_diff;

}CABAC_NEIGH;




typedef struct {
 STRUCT_PF baseline_vectors[1];
} MAIN_STRUCT_PF;
# 41 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/fill_caches_cabac.h" 2


void fill_caches_cabac ( SLICE *slice, CABAC_NEIGH *neigh, unsigned char slice_table [], int mb_xy, int diff, RESIDU *residu );

void fill_caches_motion_vector_B(SLICE *slice, int b_stride, int b8_stride, short ref_cache_l0 [], short ref_cache_l1 [],
         short mv_cache_l0[][2], short mv_cache_l1[][2], unsigned char slice_table [], RESIDU *CurrResidu,
         short mv_l0[], short mv_l1[], short ref_l0[], short ref_l1[], int iCurrMbAddr, int ai_iX);


void init_mb_skip_mv_ref(short mv[], short ref[], int b_stride, int b8_stride);


void reset_ref_index( int size, int ref_l0[], int ref_l1[]);
void write_back_motion_cache_B(const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy,
          short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short ai_tiMv_cache_l0[ ][2],
          short ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ],
          short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ], int mb_stride);


void write_back_main_interpolation(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ],
           short ai_tiMv_cache_l0[ ][2], short ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ],
           short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]);

void write_back_main_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ],
            short ai_tiMv_cache_l0[ ][2], short ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ],
            short ai_tiRef_l1[ ], short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]);
# 42 "/home/masnec/workspace/svc/jni/AVC/h264_main_decoder/lib_main/BitstreamSkipB.c" 2




void SkipB(SLICE *Slice, const SPS *Sps, RESIDU *CurrResidu, DATA *Block, unsigned char *SliceTable, short iMb_x, short iMb_y,
     int Last_Qp, short mv_cache_l0[][2], short *ref_cache_l0, short *mvl0_io, short *refl0_io,
     short mv_cache_l1[][2], short *ref_cache_l1, short *mvl1_io, short *refl1_io,
     short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, LIST_MMO *RefListl1)
{


 const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;


 init_B_skip_mb(&CurrResidu[iCurrMbAddr], Last_Qp, Slice);
 init_skip_non_zero_count(iMb_x, iMb_y, Slice -> mb_stride, Sps -> PicSizeInMbs, Block);


 init_ref_cache(ref_cache_l0);
 init_ref_cache(ref_cache_l1);
 init_mv_cache(mv_cache_l0);
 init_mv_cache(mv_cache_l1);

 fill_caches_motion_vector_B(Slice, Slice -> b_stride, Slice -> b8_stride, ref_cache_l0, ref_cache_l1,
  mv_cache_l0, mv_cache_l1, SliceTable, &CurrResidu [iCurrMbAddr], mvl0_io, mvl1_io,
  refl0_io, refl1_io, iCurrMbAddr, iMb_x);

 motion_vector_B_skip(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, mvl1_l0, mvl1_l1, refl1_l0, refl1_l1,
  Slice, Sps -> direct_8x8_inference_flag, RefListl1 -> long_term, RefListl1 -> slice_type,0);




 write_back_motion_C( Slice -> b_stride, Slice -> b8_stride, mvl0_io, mv_cache_l0, refl0_io, ref_cache_l0 );
 write_back_motion_C( Slice -> b_stride, Slice -> b8_stride, mvl1_io, mv_cache_l1, refl1_io, ref_cache_l1 );
}
