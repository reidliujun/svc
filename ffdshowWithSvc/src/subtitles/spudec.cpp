/* SPUdec.c
   Skeleton of function spudec_process_controll() is from xine sources.
   Further works:
   LGB,... (yeah, try to improve it and insert your name here! ;-)

   Kim Minh Kaplan
   implement fragments reassembly, RLE decoding.
   read brightness from the IFO.

   For information on SPU format see <URL:http://sam.zoy.org/doc/dvd/subtitles/>
   and <URL:http://members.aol.com/mpucoder/DVD/spu.html>

 */

// to ffdshow imported from mplayer

#include "stdafx.h"
#include "ffImgfmt.h"
#include "spudec.h"
#include "postproc/swscale.h"
#include "Tlibmplayer.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "Tconfig.h"
#include "ffdebug.h"

/* Valid values for spu_aamode:
   0: none (fastest, most ugly)
   1: approximate
   2: full (slowest)
   3: bilinear (similiar to vobsub, fast and not too bad)
   4: uses swscaler gaussian (this is the only one that looks good)
 */
/*
Tspudec::Tspudec(IffdshowBase *Ideci):deci(Ideci)
{
 spu_aamode = 3;
 spu_alignment = -1;
 spu_gaussvar = 1.0;
 sub_pos=0;
 firsttime=1;

}
*/
void Tspudec::spudec_queue_packet( packet_t *packet)
{
  if (queue_head == NULL)
    queue_head = packet;
  else
    queue_tail->next = packet;
  queue_tail = packet;
}

Tspudec::packet_t* Tspudec::spudec_dequeue_packet()
{
  packet_t *retval = queue_head;

  queue_head = retval->next;
  if (queue_head == NULL)
    queue_tail = NULL;

  return retval;
}

void Tspudec::spudec_free_packet(packet_t *packet)
{
  if (packet->packet != NULL)
    free(packet->packet);
  free(packet);
}

unsigned int Tspudec::get_be16(const unsigned char *p)
{
  return (p[0] << 8) + p[1];
}

unsigned int Tspudec::get_be24(const unsigned char *p)
{
  return (get_be16(p) << 8) + p[2];
}

void Tspudec::next_line(packet_t *packet)
{
  if (packet->current_nibble[packet->deinterlace_oddness] % 2)
    packet->current_nibble[packet->deinterlace_oddness]++;
  packet->deinterlace_oddness = (packet->deinterlace_oddness + 1) % 2;
}

unsigned char Tspudec::get_nibble(packet_t *packet)
{
  unsigned char nib;
  unsigned int *nibblep = packet->current_nibble + packet->deinterlace_oddness;
  if (*nibblep / 2 >= packet->control_start) {
    DPRINTF( _l("SPUdec: ERROR: get_nibble past end of packet"));
    return 0;
  }
  nib = packet->packet[*nibblep / 2];
  if (*nibblep % 2)
    nib &= 0xf;
  else
    nib >>= 4;
  ++*nibblep;
  return nib;
}

/* Cut the sub to visible part */
void Tspudec::spudec_cut_image(void)
{
  unsigned int fy, ly;
  unsigned int first_y, last_y;
  unsigned char *image;
  unsigned char *aimage;

  if (this->stride == 0 || this->height == 0) {
    return;
  }

  for (fy = 0; fy < this->image_size && !this->aimage[fy]; fy++);
  for (ly = this->stride * this->height-1; ly && !this->aimage[ly]; ly--);
  first_y = fy / this->stride;
  last_y = ly / this->stride;
  //printf("first_y: %d, last_y: %d", first_y, last_y);
  this->start_row += first_y;

  // Some subtitles trigger this condition
  if (last_y + 1 > first_y ) {
          this->height = last_y - first_y +1;
  } else {
          this->height = 0;
          this->image_size = 0;
          return;
  }

//  printf("new h %d new start %d (sz %d st %d)---\n", this->height, this->start_row, this->image_size, this->stride);

  image = (unsigned char*)malloc(2 * this->stride * this->height);
  if(image){
    this->image_size = this->stride * this->height;
    aimage = image + this->image_size;
    memcpy(image, this->image + this->stride * first_y, this->image_size);
    memcpy(aimage, this->aimage + this->stride * first_y, this->image_size);
    free(this->image);
    this->image = image;
    this->aimage = aimage;
  } else {
    DPRINTF(_l("Fatal: update_spu: malloc requested %d bytes"), 2 * this->stride * this->height);
  }
}

int Tspudec::mkalpha(int i)
{
  /* In mplayer's alpha planes, 0 is transparent, then 1 is nearly
     opaque upto 255 which is transparent */
  switch (i) {
  case 0xf:
    return 1;
  case 0:
    return 0;
  default:
    return (0xf - i) << 4;
  }
}

void Tspudec::spudec_process_data( packet_t *packet)
{
  unsigned int cmap[4], alpha[4];
  unsigned int i, x, y;

  this->scaled_frame_width = 0;
  this->scaled_frame_height = 0;
  this->start_col = packet->start_col;
  this->end_col = packet->end_col;
  this->start_row = packet->start_row;
  this->end_row = packet->end_row;
  this->height = packet->height;
  this->width = packet->width;
  this->stride = packet->stride;
  for (i = 0; i < 4; ++i) {
    alpha[i] = mkalpha(packet->alpha[i]);
    if (alpha[i] == 0)
      cmap[i] = 0;
    else if (this->custom){
      cmap[i] = this->cuspal[i].Y;
      if (cmap[i] + alpha[i] > 255)
        cmap[i] = 256 - alpha[i];
    }
    else {
      cmap[i] = this->global_palette[packet->palette[i]].Y;
      if (cmap[i] + alpha[i] > 255)
        cmap[i] = 256 - alpha[i];
    }
  }

  if (this->image_size < this->stride * this->height) {
    if (this->image != NULL) {
      free(this->image);
      this->image_size = 0;
    }
    this->image = (unsigned char*)malloc(2 * this->stride * this->height);
    if (this->image) {
      this->image_size = this->stride * this->height;
      this->aimage = this->image + this->image_size;
    }
  }
  if (this->image == NULL)
    return;

  /* Kludge: draw_alpha needs width multiple of 8. */
  if (this->width < this->stride)
    for (y = 0; y < this->height; ++y) {
      memset(this->aimage + y * this->stride + this->width, 0, this->stride - this->width);
      /* FIXME: Why is this one needed? */
      memset(this->image + y * this->stride + this->width, 0, this->stride - this->width);
    }

  i = packet->current_nibble[1];
  x = 0;
  y = 0;
  while (packet->current_nibble[0] < i
         && packet->current_nibble[1] / 2 < packet->control_start
         && y < this->height) {
    unsigned int len, color;
    unsigned int rle = 0;
    rle = get_nibble(packet);
    if (rle < 0x04) {
      rle = (rle << 4) | get_nibble(packet);
      if (rle < 0x10) {
        rle = (rle << 4) | get_nibble(packet);
        if (rle < 0x040) {
          rle = (rle << 4) | get_nibble(packet);
          if (rle < 0x0004)
            rle |= ((this->width - x) << 2);
        }
      }
    }
    color = 3 - (rle & 0x3);
    len = rle >> 2;
    if (len > this->width - x || len == 0)
      len = this->width - x;
    /* FIXME have to use palette and alpha map*/
    memset(this->image + y * this->stride + x, cmap[color], len);
    memset(this->aimage + y * this->stride + x, alpha[color], len);
    x += len;
    if (x >= this->width) {
      next_line(packet);
      x = 0;
      ++y;
    }
  }
  spudec_cut_image();
}


/*
  This function tries to create a usable palette.
  It determines how many non-transparent colors are used, and assigns different
gray scale values to each color.
  I tested it with four streams and even got something readable. Half of the
times I got black characters with white around and half the reverse.
*/
void Tspudec::compute_palette(packet_t *packet)
{
  int used[16],i,cused,start,step,color;

  memset(used, 0, sizeof(used));
  for (i=0; i<4; i++)
    if (packet->alpha[i]) /* !Transparent? */
       used[packet->palette[i]] = 1;
  for (cused=0, i=0; i<16; i++)
    if (used[i]) cused++;
  if (!cused) return;
  if (cused == 1) {
    start = 0x80;
    step = 0;
  } else {
    start = font_start_level;
    step = (0xF0-font_start_level)/(cused-1);
  }
  memset(used, 0, sizeof(used));
  for (i=0; i<4; i++) {
    color = packet->palette[i];
    if (packet->alpha[i] && !used[color]) { /* not assigned? */
       used[color] = 1;
       global_palette[color].Y = UCHAR(start);
       start += step;
    }
  }
}

void Tspudec::spudec_process_control( unsigned int pts100)
{
  int a,b; /* Temporary vars */
  unsigned int date, type;
  unsigned int off;
  unsigned int start_off = 0;
  unsigned int next_off;
  unsigned int start_pts=0;
  unsigned int end_pts=0;
  unsigned int current_nibble[2];
  unsigned int control_start;
  unsigned int display = 0;
  unsigned int start_col = 0;
  unsigned int end_col = 0;
  unsigned int start_row = 0;
  unsigned int end_row = 0;
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int stride = 0;

  control_start = get_be16(this->packet + 2);
  next_off = control_start;
  while (start_off != next_off) {
    start_off = next_off;
    date = get_be16(this->packet + start_off) * 1024;
    next_off = get_be16(this->packet + start_off + 2);
    DPRINTF( _l("date=%d"), date);
    off = start_off + 4;
    for (type = this->packet[off++]; type != 0xff; type = this->packet[off++]) {
      DPRINTF( _l("cmd=%d  "),type);
      switch(type) {
      case 0x00:
        /* Menu ID, 1 byte */
        DPRINTF(_l("Menu ID"));
        /* shouldn't a Menu ID type force display start? */
        start_pts = pts100 + date;
        end_pts = UINT_MAX;
        display = 1;
        this->is_forced_sub=(unsigned int)~0; // current subtitle is forced
        break;
      case 0x01:
        /* Start display */
        start_pts = pts100 + date;
        end_pts = UINT_MAX;
        display = 1;
        DPRINTF(_l("Start display! %i"),start_pts);
        this->is_forced_sub=0;
        break;
      case 0x02:
        /* Stop display */
        end_pts = pts100 + date;
        DPRINTF(_l("Stop display! %i"),end_pts);
        break;
      case 0x03:
        /* Palette */
        this->palette[0] = this->packet[off] >> 4;
        this->palette[1] = this->packet[off] & 0xf;
        this->palette[2] = this->packet[off + 1] >> 4;
        this->palette[3] = this->packet[off + 1] & 0xf;
        DPRINTF(_l("Palette %d, %d, %d, %d"), this->palette[0], this->palette[1], this->palette[2], this->palette[3]);
        off+=2;
        break;
      case 0x04:
        /* Alpha */
        this->alpha[0] = this->packet[off] >> 4;
        this->alpha[1] = this->packet[off] & 0xf;
        this->alpha[2] = this->packet[off + 1] >> 4;
        this->alpha[3] = this->packet[off + 1] & 0xf;
        DPRINTF(_l("Alpha %d, %d, %d, %d"),  this->alpha[0], this->alpha[1], this->alpha[2], this->alpha[3]);
        off+=2;
        break;
      case 0x05:
        /* Co-ords */
        a = get_be24(this->packet + off);
        b = get_be24(this->packet + off + 3);
        start_col = a >> 12;
        end_col = a & 0xfff;
        width = (end_col < start_col) ? 0 : end_col - start_col + 1;
        stride = (width + 7) & ~7; /* Kludge: draw_alpha needs width multiple of 8 */
        start_row = b >> 12;
        end_row = b & 0xfff;
        height = (end_row < start_row) ? 0 : end_row - start_row /* + 1 */;
        DPRINTF(_l("Coords  col: %d - %d  row: %d - %d  (%dx%d)"), start_col, end_col, start_row, end_row, width, height);
        off+=6;
        break;
      case 0x06:
        /* Graphic lines */
        current_nibble[0] = 2 * get_be16(this->packet + off);
        current_nibble[1] = 2 * get_be16(this->packet + off + 2);
        DPRINTF(_l("Graphic offset 1: %d  offset 2: %d"), current_nibble[0] / 2, current_nibble[1] / 2);
        off+=4;
        break;
      case 0xff:
        /* All done, bye-bye */
        DPRINTF(_l("Done!"));
        return;
//      break;
      default:
        DPRINTF(_l("spudec: Error determining control type 0x%02x.  Skipping %d bytes."), type, next_off - off);
        goto next_control;
      }
    }
  next_control:
    if (display) {
      packet_t *packet = (packet_t*)calloc(1, sizeof(packet_t));
      int i;
      packet->start_pts = start_pts;
      if (end_pts == UINT_MAX && start_off != next_off) {
        start_pts = pts100 + get_be16(this->packet + next_off) * 1024;
        packet->end_pts = start_pts - 1;
      } else packet->end_pts = end_pts;
      packet->current_nibble[0] = current_nibble[0];
      packet->current_nibble[1] = current_nibble[1];
      packet->start_row = start_row;
      packet->end_row = end_row;
      packet->start_col = start_col;
      packet->end_col = end_col;
      packet->width = width;
      packet->height = height;
      packet->stride = stride;
      packet->control_start = control_start;
      for (i=0; i<4; i++) {
        packet->alpha[i] = this->alpha[i];
        packet->palette[i] = this->palette[i];
      }
      packet->packet = (unsigned char*)malloc(this->packet_size);
      memcpy(packet->packet, this->packet, this->packet_size);
      spudec_queue_packet(packet);
    }
  }
}

void Tspudec::spudec_decode(unsigned int pts100)
{
    spudec_process_control( pts100);
}

int Tspudec::spudec_changed(void)
{
    return (spu_changed || now_pts > end_pts);
}

void Tspudec::spudec_assemble(const unsigned char *packet, unsigned int len, unsigned int pts100)
{
  if (len < 2) {
      DPRINTF(_l("SPUasm: packet too short"));
      return;
  }
  if ((this->packet_pts + 10000) < pts100) {
    // [cb] too long since last fragment: force new packet
    this->packet_offset = 0;
  }
  this->packet_pts = pts100;
  if (this->packet_offset == 0) {
    unsigned int len2 = get_be16(packet);
    // Start new fragment
    if (this->packet_reserve < len2) {
      if (this->packet != NULL)
              free(this->packet);
      this->packet = (unsigned char*)malloc(len2);
      this->packet_reserve = this->packet != NULL ? len2 : 0;
    }
    if (this->packet != NULL) {
      this->packet_size = len2;
      if (len > len2) {
               DPRINTF(_l("SPUasm: invalid frag len / len2: %d / %d "), len, len2);
               return;
      }
      memcpy(this->packet, packet, len);
      this->packet_offset = len;
      this->packet_pts = pts100;
    }
  } else {
    // Continue current fragment
    if (this->packet_size < this->packet_offset + len){
      DPRINTF(_l("SPUasm: invalid fragment"));
      this->packet_size = this->packet_offset = 0;
      return;
    } else {
      memcpy(this->packet + this->packet_offset, packet, len);
      this->packet_offset += len;
    }
  }
#if 1
  // check if we have a complete packet (unfortunately packet_size is bad
  // for some disks)
  // [cb] packet_size is padded to be even -> may be one byte too long
  if ((this->packet_offset == this->packet_size) ||
      ((this->packet_offset + 1) == this->packet_size)){
    unsigned int x=0,y;
    while(x+4<=this->packet_offset){
      y=get_be16(this->packet+x+2); // next control pointer
      DPRINTF(_l("SPUtest: x=%d y=%d off=%d size=%d"),x,y,this->packet_offset,this->packet_size);
      if(x>=4 && x==y){         // if it points to self - we're done!
        // we got it!
               DPRINTF(_l("SPUgot: off=%d  size=%d "),this->packet_offset,this->packet_size);
               spudec_decode(pts100);
               this->packet_offset = 0;
               break;
      }
      if(y<=x || y>=this->packet_size){ // invalid?
               DPRINTF(_l("SPUtest: broken packet!!!!! y=%d < x=%d"),y,x);
        this->packet_size = this->packet_offset = 0;
        break;
      }
      x=y;
    }
    // [cb] packet is done; start new packet
    this->packet_offset = 0;
  }
#else
  if (this->packet_offset == this->packet_size) {
    spudec_decode(pts100);
    this->packet_offset = 0;
  }
#endif
/*
 packet_t *p=queue_head;
 while (p)
  {
   DPRINTF("packet: %i %i",p->start_pts,p->end_pts);
   p=p->next;
  }*/
}

void Tspudec::spudec_reset(void)  // called after seek
{
  while (queue_head)
    spudec_free_packet(spudec_dequeue_packet());
  now_pts = 0;
  end_pts = 0;
  packet_size = packet_offset = 0;
}

void Tspudec::spudec_heartbeat(unsigned int pts100)
{
  now_pts = pts100;

  while (queue_head != NULL && pts100 >= queue_head->start_pts) {
    packet_t *packet = spudec_dequeue_packet();
    start_pts = packet->start_pts;
    end_pts = packet->end_pts;
    if (auto_palette)
      compute_palette( packet);
    spudec_process_data( packet);
    spudec_free_packet(packet);
    spu_changed = 1;
  }
}

int Tspudec::spudec_visible(void){
    int ret=(start_pts <= now_pts &&
             now_pts < end_pts &&
             height > 0);
//    printf("spu visible: %d  ",ret);
    return ret;
}

void Tspudec::spudec_set_forced_subs_only( const unsigned int flag)
{
      forced_subs_only=flag;
      DPRINTF(_l("SPU: Display only forced subs now %s"), flag ? _l("enabled"): _l("disabled"));
}
/*
void Tspudec::spudec_draw( void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride,const void *self),const void *self)
{
    if (start_pts <= now_pts && now_pts < end_pts && image)
    {
        draw_alpha(start_col, start_row, width, height,
                   image, aimage, stride,self);
        spu_changed = 0;
    }
}
*/
/* calc the bbox for spudec subs */
void Tspudec::spudec_calc_bbox( unsigned int dxs, unsigned int dys, unsigned int* bbox)
{
  int y1;
  if (orig_frame_width == 0 || orig_frame_height == 0
  || (orig_frame_width == dxs && orig_frame_height == dys)) {
    bbox[0] = start_col;
    bbox[1] = start_col + width;
    bbox[2] = start_row;
    bbox[3] = start_row + height;
  }
  else if (scaled_frame_width != dxs || scaled_frame_height != dys) {
    unsigned int scalex = 0x100 * dxs / orig_frame_width;
    unsigned int scaley = 0x100 * dys / orig_frame_height;
    bbox[0] = start_col * scalex / 0x100;
    bbox[1] = start_col * scalex / 0x100 + width * scalex / 0x100;
    switch (spu_alignment) {
    case 0:
      bbox[3] = dys*sub_pos/100 + height * scaley / 0x100;
      if (bbox[3] > dys) bbox[3] = dys;
      bbox[2] = bbox[3] - height * scaley / 0x100;
      break;
    case 1:
      if (sub_pos < 50) {
	 y1 = dys*sub_pos/100 - height * scaley / 0x200;
	 if (y1 < 0) bbox[2] = 0; else bbox[2] = y1;
        bbox[3] = bbox[2] + height;
      } else {
        bbox[3] = dys*sub_pos/100 + height * scaley / 0x200;
        if (bbox[3] > dys) bbox[3] = dys;
        bbox[2] = bbox[3] - height * scaley / 0x100;
      }
      break;
    case 2:
      y1 = dys*sub_pos/100 - height * scaley / 0x100;
      if (y1 < 0) bbox[2] = 0; else bbox[2] = y1;
      bbox[3] = bbox[2] + height;
      break;
    default: /* -1 */
      bbox[2] = start_row * scaley / 0x100;
      bbox[3] = start_row * scaley / 0x100 + height * scaley / 0x100;
      break;
    }
  }
}
/* transform mplayer's alpha value into an opacity value that is linear */
int Tspudec::canon_alpha(int alpha)
{
 return alpha ? 256 - alpha : 0;
}

void Tspudec::scale_table(unsigned int start_src, unsigned int start_tar, unsigned int end_src, unsigned int end_tar, scale_pixel * table)
{
  unsigned int t;
  unsigned int delta_src = end_src - start_src;
  unsigned int delta_tar = end_tar - start_tar;
  int src = 0;
  int src_step;
  if (delta_src == 0 || delta_tar == 0) {
    return;
  }
  src_step = (delta_src << 16) / delta_tar >>1;
  for (t = 0; t<=delta_tar; src += (src_step << 1), t++){
    table[t].position= std::min((unsigned int)(src >> 16), end_src - 1);
    table[t].right_down = src & 0xffff;
    table[t].left_up = 0x10000 - table[t].right_down;
  }
}

/* bilinear scale, similar to vobsub's code */
void Tspudec::scale_image(int x, int y, scale_pixel* table_x, scale_pixel* table_y)
{
  int alpha[4];
  int color[4];
  unsigned int scale[4];
  int base = table_y[y].position * stride + table_x[x].position;
  int scaled = y * scaled_strideY + x;
  alpha[0] = canon_alpha(aimage[base]);
  alpha[1] = canon_alpha(aimage[base + 1]);
  alpha[2] = canon_alpha(aimage[base + stride]);
  alpha[3] = canon_alpha(aimage[base + stride + 1]);
  color[0] = image[base];
  color[1] = image[base + 1];
  color[2] = image[base + stride];
  color[3] = image[base + stride + 1];
  scale[0] = (table_x[x].left_up * table_y[y].left_up >> 16) * alpha[0];
  scale[1] = (table_x[x].right_down * table_y[y].left_up >>16) * alpha[1];
  scale[2] = (table_x[x].left_up * table_y[y].right_down >> 16) * alpha[2];
  scale[3] = (table_x[x].right_down * table_y[y].right_down >> 16) * alpha[3];
  scaled_imageY[scaled] = (unsigned char)((color[0] * scale[0] + color[1] * scale[1] + color[2] * scale[2] + color[3] * scale[3])>>24);
  scaled_aimageY[scaled] = (unsigned char)((scale[0] + scale[1] + scale[2] + scale[3]) >> 16);
/*
  if (scaled_aimage[scaled]){
    scaled_aimage[scaled] = (unsigned char)(256 - scaled_aimage[scaled]);
    if(scaled_aimage[scaled] + scaled_image[scaled] > 255)
      scaled_image[scaled] = (unsigned char)(256 - scaled_aimage[scaled]);
  }*/
}

void Tspudec::sws_spu_image(unsigned char *d1, unsigned char *d2, int dw, int dh, stride_t ds,
        unsigned char *s1, unsigned char *s2, int sw, int sh, stride_t ss,const TrenderedSubtitleLines::TprintPrefs &prefs)
{
 if (!filter.lumH || oldgauss != prefs.vobaagauss)
  {
   if (filter.lumH) libmplayer->sws_freeVec(filter.lumH);
   filter.lumH = filter.lumV = filter.chrH = filter.chrV = libmplayer->sws_getGaussianVec(prefs.vobaagauss/1000.0, 3.0);
   libmplayer->sws_normalizeVec(filter.lumH, 1.0);
   oldgauss = prefs.vobaagauss;
  }

 SwsParams params;Tlibmplayer::swsInitParams(&params,SWS_GAUSS);
 SwsContext *ctx=libmplayer->sws_getContext(sw, sh, IMGFMT_Y800, dw, dh, IMGFMT_Y800, &params, &filter, NULL,NULL);
 libmplayer->sws_scale_ordered(ctx,(const uint8_t**)&s1,&ss,0,sh,&d1,&ds);
 for (stride_t i=ss*sh-1; i>=0; i--) s2[i]=(unsigned char)canon_alpha(s2[i]);
 libmplayer->sws_scale_ordered(ctx,(const uint8_t**)&s2,&ss,0,sh,&d2,&ds);
 libmplayer->sws_freeContext(ctx);
}

void Tspudec::spudec_draw_scaled(unsigned int dxs, unsigned int dys, TdrawAlpha draw_alpha,const TrenderedSubtitleLines::TprintPrefs &prefs)
{
  scale_pixel *table_x;
  scale_pixel *table_y;
  int y1;

  if (start_pts <= now_pts && now_pts < end_pts) {

    // check if only forced subtitles are requested
    if( (forced_subs_only) && !(is_forced_sub) ){
        return;
    }
/*
    if (!(spu_aamode&16) && (orig_frame_width == 0 || orig_frame_height == 0
        || (orig_frame_width == dxs && orig_frame_height == dys))) {
      if (image)
      {
        draw_alpha(start_col, start_row, width, height,
                   imageY, aimageY, strideY, imageUV, aimageUV,strideUV self);
        spu_changed = 0;
      }
    }
    else*/ {
      if (/*oldscale!=prefs.vobscale || */scaled_frame_width != dxs || scaled_frame_height != dys || spu_aamode!=prefs.vobaamode) {  /* Resizing is needed */
        spu_aamode=prefs.vobaamode;
        oldscale=prefs.vobscale;
        /* scaled_x = scalex * x / 0x100
           scaled_y = scaley * y / 0x100
           order of operations is important because of rounding. */
        unsigned int scalex = prefs.vobscale * dxs / orig_frame_width;
        unsigned int scaley = prefs.vobscale * dys / orig_frame_height;
        scaled_start_col = start_col * scalex / 0x100;
        scaled_start_row = start_row * scaley / 0x100;
         scaled_width = width * scalex / 0x100;
        scaled_height = height * scaley / 0x100;
        /* Kludge: draw_alpha needs width multiple of 8 */
        scaled_strideY = (scaled_width + 7) & ~7;
        scaled_strideUV = ((scaled_width/2) + 7) & ~7;
        if (scaled_image_size < scaled_strideY * scaled_height) {
          if (scaled_imageY) {
            free(scaled_imageY);
            scaled_image_size = 0;
          }
          if (scaled_imageUV)
            free(scaled_imageUV);
          scaled_imageY = (unsigned char*) malloc(2 * scaled_strideY * scaled_height);
          scaled_imageUV= (unsigned char*) malloc(2 * scaled_strideUV* ((scaled_height+1)/2));
          if (scaled_imageY) {
            scaled_image_size = scaled_strideY * scaled_height;
            scaled_aimageY = scaled_imageY + scaled_image_size;
            scaled_aimageUV= scaled_imageUV+ scaled_strideUV * ((scaled_height+1)/2);
          }
        }
        if (scaled_imageY) {
          //unsigned int x, y;
          /* Kludge: draw_alpha needs width multiple of 8. */
          if (scaled_width < scaled_strideY)
           for (unsigned int y = 0; y < scaled_height; ++y)
            {
             memset(scaled_aimageY + y * scaled_strideY + scaled_width, 0, scaled_strideY - scaled_width);
             /* FIXME: Why is self one needed? */
             memset(scaled_imageY + y * scaled_strideY + scaled_width, 0, scaled_strideY - scaled_width);
            }
          /*
          if (scaled_width/2 < scaled_strideUV)
           for (unsigned int y = 0; y < scaled_height/2; ++y)
            {
             memset(scaled_aimageUV + y * scaled_strideUV + scaled_width/2, 128, scaled_strideUV - scaled_width/2);
             memset(scaled_imageUV + y * scaled_strideUV + scaled_width/2, 128, scaled_strideUV - scaled_width/2);
            }*/
            memset(scaled_imageUV ,0,scaled_strideUV*(scaled_height/2));
            memset(scaled_aimageUV,128,scaled_strideUV*((scaled_height+1)/2));
          if (scaled_width <= 1 || scaled_height <= 1) {
            goto nothing_to_do;
          }
          switch(spu_aamode&15) {
          case 4:
          sws_spu_image(scaled_imageY, scaled_aimageY,
                  scaled_width, scaled_height, scaled_strideY,
                  image, aimage, width, height, stride,prefs);
          break;
          case 3:
          table_x = (scale_pixel*)calloc(scaled_width, sizeof(scale_pixel));
          table_y = (scale_pixel*)calloc(scaled_height, sizeof(scale_pixel));
          if (!table_x || !table_y) {
            DPRINTF(_l("Fatal: spudec_draw_scaled: calloc failed"));
          }
          scale_table(0, 0, width - 1, scaled_width - 1, table_x);
          scale_table(0, 0, height - 1, scaled_height - 1, table_y);
          for (unsigned int y = 0; y < scaled_height; y++)
            for (unsigned int x = 0; x < scaled_width; x++)
              scale_image(x, y, table_x, table_y);
          free(table_x);
          free(table_y);
          break;
          case 0:
          /* no antialiasing */
          for (unsigned int y = 0; y < scaled_height; ++y) {
            int unscaled_y = y * 0x100 / scaley;
            int strides = stride * unscaled_y;
            int scaled_strides = scaled_strideY * y;
            for (unsigned int x = 0; x < scaled_width; ++x) {
              int unscaled_x = x * 0x100 / scalex;
              scaled_imageY[scaled_strides + x] = image[strides + unscaled_x];
              scaled_aimageY[scaled_strides + x] = (unsigned char)canon_alpha(aimage[strides + unscaled_x]);
            }
          }
          break;
          case 1:
          {
            /* Intermediate antialiasing. */
            for (unsigned int y = 0; y < scaled_height; ++y) {
              const unsigned int unscaled_top = y * 0x100 / scaley;
              unsigned int unscaled_bottom = (y + 1) * 0x100 / scaley;
              if (unscaled_bottom >= height)
                unscaled_bottom = height - 1;
              for (unsigned int x = 0; x < scaled_width; ++x) {
                const unsigned int unscaled_left = x * 0x100 / scalex;
                unsigned int unscaled_right = (x + 1) * 0x100 / scalex;
                unsigned int color = 0;
                unsigned int alpha = 0;
                unsigned int walkx, walky;
                unsigned int base, tmp;
                if (unscaled_right >= width)
                  unscaled_right = width - 1;
                for (walky = unscaled_top; walky <= unscaled_bottom; ++walky)
                  for (walkx = unscaled_left; walkx <= unscaled_right; ++walkx) {
                    base = walky * stride + walkx;
                    tmp = canon_alpha(aimage[base]);
                    alpha += tmp;
                    color += tmp * image[base];
                  }
                base = y * scaled_strideY + x;
                scaled_imageY[base] = (unsigned char)(alpha ? color / alpha : 0);
                scaled_aimageY[base] = (unsigned char)(alpha * (1 + unscaled_bottom - unscaled_top) * (1 + unscaled_right - unscaled_left));
              }
            }
          }
          break;
          case 2:
          {
            /* Best antialiasing.  Very slow. */
            /* Any pixel (x, y) represents pixels from the original
               rectangular region comprised between the columns
               unscaled_y and unscaled_y + 0x100 / scaley and the rows
               unscaled_x and unscaled_x + 0x100 / scalex

               The original rectangular region that the scaled pixel
               represents is cut in 9 rectangular areas like this:

               +---+-----------------+---+
               | 1 |        2        | 3 |
               +---+-----------------+---+
               |   |                 |   |
               | 4 |        5        | 6 |
               |   |                 |   |
               +---+-----------------+---+
               | 7 |        8        | 9 |
               +---+-----------------+---+

               The width of the left column is at most one pixel and
               it is never null and its right column is at a pixel
               boundary.  The height of the top row is at most one
               pixel it is never null and its bottom row is at a
               pixel boundary. The width and height of region 5 are
               integral values.  The width of the right column is
               what remains and is less than one pixel.  The height
               of the bottom row is what remains and is less than
               one pixel.

               The row above 1, 2, 3 is unscaled_y.  The row between
               1, 2, 3 and 4, 5, 6 is top_low_row.  The row between 4,
               5, 6 and 7, 8, 9 is (unsigned int)unscaled_y_bottom.
               The row beneath 7, 8, 9 is unscaled_y_bottom.

               The column left of 1, 4, 7 is unscaled_x.  The column
               between 1, 4, 7 and 2, 5, 8 is left_right_column.  The
               column between 2, 5, 8 and 3, 6, 9 is (unsigned
               int)unscaled_x_right.  The column right of 3, 6, 9 is
               unscaled_x_right. */
            const double inv_scalex = (double) 0x100 / scalex;
            const double inv_scaley = (double) 0x100 / scaley;
            for (unsigned int y = 0; y < scaled_height; ++y) {
              const double unscaled_y = y * inv_scaley;
              const double unscaled_y_bottom = unscaled_y + inv_scaley;
              const unsigned int top_low_row = (unsigned int)std::min(unscaled_y_bottom, unscaled_y + 1.0);
              const double top = top_low_row - unscaled_y;
              const unsigned int height = unscaled_y_bottom > top_low_row
                ? (unsigned int) unscaled_y_bottom - top_low_row
                : 0;
              const double bottom = unscaled_y_bottom > top_low_row
                ? unscaled_y_bottom - floor(unscaled_y_bottom)
                : 0.0;
              for (unsigned int x = 0; x < scaled_width; ++x) {
                const double unscaled_x = x * inv_scalex;
                const double unscaled_x_right = unscaled_x + inv_scalex;
                const unsigned int left_right_column = (unsigned int)std::min(unscaled_x_right, unscaled_x + 1.0);
                const double left = left_right_column - unscaled_x;
                const unsigned int width = unscaled_x_right > left_right_column
                  ? (unsigned int) unscaled_x_right - left_right_column
                  : 0;
                const double right = unscaled_x_right > left_right_column
                  ? unscaled_x_right - floor(unscaled_x_right)
                  : 0.0;
                double color = 0.0;
                double alpha = 0.0;
                double tmp;
                unsigned int base;
                /* Now use these informations to compute a good alpha,
                   and lightness.  The sum is on each of the 9
                   region's surface and alpha and lightness.

                  transformed alpha = sum(surface * alpha) / sum(surface)
                  transformed color = sum(surface * alpha * color) / sum(surface * alpha)
                */
                /* 1: top left part */
                base = stride * (unsigned int) unscaled_y;
                tmp = left * top * canon_alpha(aimage[base + (unsigned int) unscaled_x]);
                alpha += tmp;
                color += tmp * image[base + (unsigned int) unscaled_x];
                /* 2: top center part */
                if (width > 0) {
                  unsigned int walkx;
                  for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx) {
                    base = stride * (unsigned int) unscaled_y + walkx;
                    tmp = /* 1.0 * */ top * canon_alpha(aimage[base]);
                    alpha += tmp;
                    color += tmp * image[base];
                  }
                }
                /* 3: top right part */
                if (right > 0.0) {
                  base = stride * (unsigned int) unscaled_y + (unsigned int) unscaled_x_right;
                  tmp = right * top * canon_alpha(aimage[base]);
                  alpha += tmp;
                  color += tmp * image[base];
                }
                /* 4: center left part */
                if (height > 0) {
                  unsigned int walky;
                  for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky) {
                    base = stride * walky + (unsigned int) unscaled_x;
                    tmp = left /* * 1.0 */ * canon_alpha(aimage[base]);
                    alpha += tmp;
                    color += tmp * image[base];
                  }
                }
                /* 5: center part */
                if (width > 0 && height > 0) {
                  unsigned int walky;
                  for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky) {
                    unsigned int walkx;
                    base = stride * walky;
                    for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx) {
                      tmp = /* 1.0 * 1.0 * */ canon_alpha(aimage[base + walkx]);
                      alpha += tmp;
                      color += tmp * image[base + walkx];
                    }
                  }
                }
                /* 6: center right part */
                if (right > 0.0 && height > 0) {
                  unsigned int walky;
                  for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky) {
                    base = stride * walky + (unsigned int) unscaled_x_right;
                    tmp = right /* * 1.0 */ * canon_alpha(aimage[base]);
                    alpha += tmp;
                    color += tmp * image[base];
                  }
                }
                /* 7: bottom left part */
                if (bottom > 0.0) {
                  base = stride * (unsigned int) unscaled_y_bottom + (unsigned int) unscaled_x;
                  tmp = left * bottom * canon_alpha(aimage[base]);
                  alpha += tmp;
                  color += tmp * image[base];
                }
                /* 8: bottom center part */
                if (width > 0 && bottom > 0.0) {
                  unsigned int walkx;
                  base = stride * (unsigned int) unscaled_y_bottom;
                  for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx) {
                    tmp = /* 1.0 * */ bottom * canon_alpha(aimage[base + walkx]);
                    alpha += tmp;
                    color += tmp * image[base + walkx];
                  }
                }
                /* 9: bottom right part */
                if (right > 0.0 && bottom > 0.0) {
                  base = stride * (unsigned int) unscaled_y_bottom + (unsigned int) unscaled_x_right;
                  tmp = right * bottom * canon_alpha(aimage[base]);
                  alpha += tmp;
                  color += tmp * image[base];
                }
                /* Finally mix these transparency and brightness information suitably */
                base = scaled_strideY * y + x;
                scaled_imageY[base] = (unsigned char)(alpha > 0 ? color / alpha : 0);
                scaled_aimageY[base] = (unsigned char)(alpha * scalex * scaley / 0x10000);
                /*
                if (scaled_aimage[base]) {
                  scaled_aimage[base] = (unsigned char)(256 - scaled_aimage[base]);
                  if (scaled_aimage[base] + scaled_image[base] > 255)
                    scaled_image[base] = (unsigned char)(256 - scaled_aimage[base]);
                }*/
              }
            }
          }
          }
nothing_to_do:
          scaled_frame_width = dxs;
          scaled_frame_height = dys;
          for (unsigned int y=0;y<scaled_height/2;y++)
           for (unsigned int x=0;x<scaled_width/2;x++)
            {
             scaled_imageUV[x+scaled_strideUV*y]=0;
             scaled_aimageUV[x+scaled_strideUV*y]=scaled_aimageY[x*2+scaled_strideY*(y*2)];
            }
        }
      }
      if (scaled_imageY){
        switch (spu_alignment) {
        case 0:
          scaled_start_row = dys*sub_pos/100;
          if (scaled_start_row + scaled_height > dys)
            scaled_start_row = dys - scaled_height;
          break;
        case 1:
	   y1 = dys*sub_pos/100 - scaled_height/2;
          if (sub_pos < 50) {
	    if (y1 < 0) scaled_start_row = 0; else scaled_start_row = y1;
          } else {
	    if (y1 + scaled_height > dys)
	      scaled_start_row = dys - scaled_height;
	    else
	      scaled_start_row = y1;
          }
          break;
        case 2:
          y1 = dys*sub_pos/100 - scaled_height;
	  if (y1 < 0) scaled_start_row = 0; else scaled_start_row = 0;
          break;
        }
        draw_alpha(scaled_start_col, scaled_start_row, scaled_width, scaled_height,
                   scaled_imageY, scaled_aimageY, scaled_strideY,scaled_imageUV, scaled_aimageUV, scaled_strideUV,prefs);
        spu_changed = 0;
      }
    }
  }
  else
  {
    DPRINTF(_l("SPU not displayed: start_pts=%d  end_pts=%d  now_pts=%d"),start_pts, end_pts, now_pts);
  }
}

void Tspudec::spudec_update_palette( unsigned int *palette)
{
  if (palette) {
    memcpy(global_palette, palette, sizeof(global_palette));
  }
}

void Tspudec::spudec_set_font_factor( double factor)
{
  font_start_level = (int)(0xF0-(0xE0*factor));
}
/*
Tspudec::Tspudec(unsigned int *palette, unsigned int frame_width, unsigned int frame_height)
{
 Tspudec(palette, NULL, 0, frame_width, frame_height);
}
*/
/* get palette custom color, width, height from .idx file */
Tspudec::Tspudec(IffdshowBase *Ideci,const AM_DVD_YUV *palette, const AM_DVD_YUV *cuspal, unsigned int custom, unsigned int frame_width, unsigned int frame_height)
{
 memset(this,0,sizeof(*this));
 deci=Ideci;
 spu_aamode = 4;
 spu_alignment = -1;
 sub_pos=0;
 oldgauss=-1;
 deci->getPostproc(&libmplayer);
    this->packet = NULL;
    this->image = NULL;
    this->scaled_imageY=this->scaled_imageUV = NULL;
    /* XXX Although the video frame is some size, the SPU frame is
       always maximum size i.e. 720 wide and 576 or 480 high */
    this->orig_frame_width = 720;
    this->orig_frame_height = (frame_height == 480 || frame_height == 240) ? 480 : 576;
    this->custom = custom;
    // set up palette:
    this->auto_palette = 1;
    if (palette){
      memcpy(this->global_palette, palette, sizeof(this->global_palette));
      this->auto_palette = 0;
    }
    this->custom = custom;
    if (custom && cuspal) {
      memcpy(this->cuspal, cuspal, sizeof(this->cuspal));
      this->auto_palette = 0;
    }
    // forced subtitles default: show all subtitles
    this->forced_subs_only=0;
    this->is_forced_sub=0;
    packet_pts=0;
}
/*
Tspudec::Tspudec(unsigned int *palette)
{
    Tspudec(palette, 0, 0);
}
*/
Tspudec::~Tspudec()
{
    if (filter.lumH) libmplayer->sws_freeVec(filter.lumH);
    while (queue_head)
      spudec_free_packet(spudec_dequeue_packet());
    if (packet)
      free(packet);
    if (scaled_imageY)
        free(scaled_imageY);
    if (scaled_imageUV)
        free(scaled_imageUV);
    if (image)
      free(image);
    libmplayer->Release();
}
