#ifndef _CC_DECODER_H_
#define _CC_DECODER_H_

#include "interfaces.h"

class TccDecoder
{
private:
 int64_t pts; /* The PTS and SCR at which the captioning chunk started */
 uint32_t f_offset; /* holds the NTSC frame offset to last known pts/scr */
 uint32_t lastcode;  /* the last captioning code seen (control codes are often sent twice in a row, but should be processed only once) */
 uint32_t capid;

 enum { WHITE, GREEN, BLUE, CYAN, RED, YELLOW, MAGENTA, BLACK };

 static int good_parity(uint16_t data);

 static const wchar_t TRANSP_SPACE=0x19;   /* code for transparent space, essentially arbitrary */
 static const int CC_ROWS=15,CC_COLUMNS=32,CC_CHANNELS=2;
 static wchar_t chartbl[128];

 struct cc_attribute_t
  {
   uint8_t italic;
   uint8_t underline;
   uint8_t foreground;
   uint8_t background;
  };

 struct cc_char_cell_t
  {
   wchar_t c;                   /* character code, not the same as ASCII */
   cc_attribute_t attributes;   /* attributes of this character, if changed here */
   int midrow_attr;             /* true if this cell changes an attribute */
  };

 struct cc_renderer_t;
 struct cc_row_t
  {
   cc_char_cell_t cells[CC_COLUMNS];
   int pos;                   /* position of the cursor */
   int num_chars;             /* how many characters in the row are data */
   int attr_chg;              /* true if midrow attr. change at cursor pos */
   int pac_attr_chg;          /* true if attribute has changed via PAC */
   cc_attribute_t pac_attr;   /* PAC attr. that hasn't been applied yet */
   void ccrow_fill_transp(void);
   bool ccrow_render(cc_renderer_t *renderer, int rownum);
   int ccrow_find_next_text_part(int pos);
   int ccrow_find_end_of_text_part(int pos);
   int ccrow_find_current_attr(int pos);
   int ccrow_find_next_attr_change(int pos, int lastpos);
  };

 struct cc_buffer_t
  {
   cc_row_t rows[CC_ROWS];
   int rowpos;              /* row cursor position */
   void ccbuf_add_char(wchar_t c);
   void ccbuf_set_cursor(int row, int column, int underline, int italics, int color);
   void ccbuf_apply_attribute(cc_attribute_t *attr);
   void ccbuf_tab(int tabsize);
   int cc_buf_has_displayable(void);
   void ccbuf_render(cc_renderer_t *renderer);
  };

 struct cc_memory_t
  {
   cc_memory_t(void) {ccmem_clear();}
   cc_buffer_t channel[CC_CHANNELS];
   int channel_no;          /* currently active channel */
   void ccmem_clear(void);
  } **active,*on_buf,*off_buf,buffer[2];

 struct cc_renderer_t
  {
   cc_renderer_t(IffdshowDecVideo *IdeciV):deciV(IdeciV) {}
   IffdshowDecVideo *deciV;
   int displayed;              /* true when caption currently is displayed */
   void cc_renderer_show_caption(cc_buffer_t *buf, int64_t vpts);
   void cc_renderer_hide_caption(int64_t vpts);
  } cc_renderer;

 struct cc_state_t
  {
   cc_state_t(cc_renderer_t *Irenderer):renderer(Irenderer) {}
   cc_renderer_t *renderer;
  } cc_state;

 void cc_set_channel(int channel);
 void cc_decode_PAC(int channel,  uint8_t c1, uint8_t c2);
 void cc_decode_standard_char(uint8_t c1, uint8_t c2);
 void cc_decode_ext_attribute(int channel, uint8_t c1, uint8_t c2);
 void cc_decode_special_char(int channel,  uint8_t c1, uint8_t c2);
 void cc_decode_extended_special_char(int channel,  uint8_t c1, uint8_t c2);
 void cc_decode_more_extended_special_char(int channel,  uint8_t c1, uint8_t c2);
 void cc_decode_midrow_attr(int channel,  uint8_t c1, uint8_t c2);
 void cc_decode_misc_control_code(int channel,uint8_t c1, uint8_t c2);
 void cc_decode_tab(int channel, uint8_t c1, uint8_t c2);
 void cc_decode_EIA608(uint16_t data);
 cc_buffer_t* active_ccbuffer(void);
 void cc_swap_buffers(void);
 void cc_hide_displayed(void),cc_show_displayed(void);
 int cc_onscreen_displayable(void);
public:
 TccDecoder(IffdshowDecVideo *deciV);
 void decode(const uint8_t *buffer,size_t buf_len);
 void onSeek(void);
};

#endif
