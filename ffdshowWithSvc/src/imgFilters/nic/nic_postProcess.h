#ifndef _NICPOSTPROCESS_H_
#define _NICPOSTPROCESS_H_

#include "ffImgfmt.h"

void nic_postprocess(unsigned char * dst[],    stride_t dst_stride[4],
                     unsigned int chromaShiftX,unsigned int chromaShiftY,
                     int horizontal_size,      int vertical_size,
                     const int8_t *QP_store,   int QP_stride,
                     int mode,
                     int DEBLOCK_HORIZ_USEDC_THR, int DEBLOCK_VERT_USEDC_THR);
typedef void (*Tnic_deringFc)(uint8_t *image, int width, int height, stride_t stride, const int8_t *QP_store, int QP_stride, unsigned int chromaShiftX, unsigned int chromaShiftY);

Tnic_deringFc getNic_dering(void);

#endif
