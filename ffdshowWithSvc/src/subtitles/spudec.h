#ifndef _SPUDEC_H_
#define _SPUDEC_H_

#include "postproc/swscale.h"
#include "Tfont.h"
#include "interfaces.h"

struct Tlibmplayer;
class Tspudec
{
private:
 Tlibmplayer *libmplayer;
 int spu_aamode;
 int spu_alignment;
 IffdshowBase *deci;
 int sub_pos;
 SwsFilter filter;
 int oldgauss;

 struct packet_t
  {
   unsigned char *packet;
   unsigned int palette[4];
   unsigned int alpha[4];
   unsigned int control_start;  /* index of start of control data */
   unsigned int current_nibble[2]; /* next data nibble (4 bits) to be
                                      processed (for RLE decoding) for
                                      even and odd lines */
   int deinterlace_oddness;     /* 0 or 1, index into current_nibble */
   unsigned int start_col, end_col;
   unsigned int start_row, end_row;
   unsigned int width, height, stride;
   unsigned int start_pts, end_pts;
   packet_t *next;
 };

 packet_t *queue_head;
 packet_t *queue_tail;
 AM_DVD_YUV global_palette[16];
 unsigned int orig_frame_width, orig_frame_height;
 unsigned char* packet;
 size_t packet_reserve;       /* size of the memory pointed to by packet */
 unsigned int packet_offset;  /* end of the currently assembled fragment */
 unsigned int packet_size;    /* size of the packet once all fragments are assembled */
 unsigned int packet_pts;     /* PTS for self packet */
 unsigned int palette[4];
 unsigned int alpha[4];
 AM_DVD_YUV cuspal[4];
 unsigned int custom;
 unsigned int now_pts;
 unsigned int start_pts, end_pts;
 unsigned int start_col, end_col;
 unsigned int start_row, end_row;
 unsigned int width, height, stride;
 size_t image_size;           /* Size of the image buffer */
 unsigned char *image;                /* Grayscale value */
 unsigned char *aimage;       /* Alpha value */
 unsigned int scaled_frame_width, scaled_frame_height;
 unsigned int scaled_start_col, scaled_start_row;
 unsigned int scaled_width, scaled_height, scaled_strideY,scaled_strideUV;
 size_t scaled_image_size;
 unsigned char *scaled_imageY,*scaled_imageUV;
 unsigned char *scaled_aimageY,*scaled_aimageUV;
 int auto_palette; /* 1 if we lack a palette and must use an heuristic. */
 int font_start_level;  /* Darkest value used for the computed font */
 int spu_changed;
 unsigned int forced_subs_only;     /* flag: 0=display all subtitle, !0 display only forced subtitles */
 unsigned int is_forced_sub;         /* true if current subtitle is a forced subtitle */

 struct scale_pixel
  {
   unsigned position;
   unsigned left_up;
   unsigned right_down;
  };

 void spudec_queue_packet(packet_t *packet);
 packet_t *spudec_dequeue_packet(void);
 static void spudec_free_packet(packet_t *packet);
 static void next_line(packet_t *packet);
 static unsigned char get_nibble(packet_t *packet);
 void spudec_cut_image(void);
 void spudec_process_data(packet_t *packet);
 void compute_palette( packet_t *packet);
 void spudec_process_control( unsigned int pts100);
 void spudec_decode(unsigned int pts100);
 void scale_image(int x, int y, scale_pixel* table_x, scale_pixel* table_y);
 static void scale_table(unsigned int start_src, unsigned int start_tar, unsigned int end_src, unsigned int end_tar, scale_pixel * table);
 void sws_spu_image(unsigned char *d1, unsigned char *d2, int dw, int dh, stride_t ds, unsigned char *s1, unsigned char *s2, int sw, int sh, stride_t ss,const TrenderedSubtitleLines::TprintPrefs &prefs);
 static int mkalpha(int i);
 static int canon_alpha(int alpha);
 static unsigned int get_be16(const unsigned char *p);
 static unsigned int get_be24(const unsigned char *p);
 int oldscale;
public:
 Tspudec(IffdshowBase *Ideci,const AM_DVD_YUV *palette, const AM_DVD_YUV *cuspal, unsigned int custom, unsigned int frame_width, unsigned int frame_height);
 ~Tspudec();
 void spudec_heartbeat(unsigned int pts100);
 void spudec_assemble(const unsigned char *packet, unsigned int len, unsigned int pts100);
 //void spudec_draw(void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride,const void *self),const void *self);
 typedef void (*TdrawAlpha)(int x0,int y0, unsigned int w,unsigned int h, const unsigned char* srcY, const unsigned char *srcaY, int strideY,const unsigned char* srcUV, const unsigned char *srcaUV, int strideUV,const TrenderedSubtitleLines::TprintPrefs &prefs);
 void spudec_draw_scaled(unsigned int dxs, unsigned int dys, TdrawAlpha draw_alpha,const TrenderedSubtitleLines::TprintPrefs &prefs);
 void spudec_update_palette(unsigned int *palette);
 void spudec_free(void);
 void spudec_reset(void);       // called after seek
 int spudec_visible(void); // check if spu is visible
 void spudec_set_font_factor(double factor); // sets the equivalent to ffactor
 int spudec_changed(void);
 void spudec_calc_bbox(unsigned int dxs, unsigned int dys, unsigned int* bbox);
 void spudec_set_forced_subs_only(const unsigned int flag);
};

#endif
