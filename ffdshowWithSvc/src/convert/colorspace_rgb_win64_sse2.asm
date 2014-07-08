;/*****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - RGB colorspace conversions -
; *
; *  Copyright(C) 2002-2003 Michael Militzer <isibaar@xvid.org>
; *               2002-2003 Peter Ross <pross@xvid.org>
; *
; *  This program is free software ; you can redistribute it and/or modify
; *  it under the terms of the GNU General Public License as published by
; *  the Free Software Foundation ; either version 2 of the License, or
; *  (at your option) any later version.
; *
; *  This program is distributed in the hope that it will be useful,
; *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
; *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; *  GNU General Public License for more details.
; *
; *  You should have received a copy of the GNU General Public License
; *  along with this program ; if not, write to the Free Software
; *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
; *
; ****************************************************************************/

; sse2 version for WIN64 (ported by h.yamagata)
; only YV12->RGB32 conversion is implemented.

%macro cglobal 1
	%ifdef PREFIX
		%ifdef MARK_FUNCS
			global _%1:function %1.endfunc-%1
			%define %1 _%1:function %1.endfunc-%1
		%else
			global _%1
			%define %1 _%1
		%endif
	%else
		%ifdef MARK_FUNCS
			global %1:function %1.endfunc-%1
		%else
			global %1
		%endif
	%endif
%endmacro

;=============================================================================
; Some constants
;=============================================================================

;-----------------------------------------------------------------------------
; RGB->YV12 yuv constants
;-----------------------------------------------------------------------------

%define Y_R		0.257
%define Y_G		0.504
%define Y_B		0.098
%define Y_ADD	16

%define U_R		0.148
%define U_G		0.291
%define U_B		0.439
%define U_ADD	128

%define V_R		0.439
%define V_G		0.368
%define V_B		0.071
%define V_ADD	128

; Scaling used during conversion
%define SCALEBITS 6

;=============================================================================
; Random access data
;=============================================================================

SECTION .data
ALIGN 16

;-----------------------------------------------------------------------------
; RGB->YV12 multiplication matrices
;-----------------------------------------------------------------------------

cglobal bgr_to_yv12_mmx_data
bgr_to_yv12_mmx_data:

;         FIX(Y_B)	FIX(Y_G)	FIX(Y_R) Ignored

y_mul: dw    25,      129,        66,      0,        25,      129,        66,      0
u_mul: dw   112,      -74,       -38,      0,       112,      -74,       -38,      0
v_mul: dw   -18,      -94,       112,      0,       -18,      -94,       112,      0
y_add: dw    16,        0

ALIGN 16

;-----------------------------------------------------------------------------
; YV12->RGB data
;-----------------------------------------------------------------------------

cglobal yv12_to_bgr_mmx_data
yv12_to_bgr_mmx_data:

Y_SUB: dw  16,  16,  16,  16,  16,  16,  16,  16
U_SUB: dw 128, 128, 128, 128, 128, 128, 128, 128
V_SUB: dw 128, 128, 128, 128, 128, 128, 128, 128

Y_MUL: dw  74,  74,  74,  74,  74,  74,  74,  74

UG_MUL: dw  25,  25,  25,  25, 25,  25,  25,  25
VG_MUL: dw  52,  52,  52,  52, 52,  52,  52,  52

UB_MUL: dw 129, 129, 129, 129, 129, 129, 129, 129
VR_MUL: dw 102, 102, 102, 102, 102, 102, 102, 102

BRIGHT: db 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128

;=============================================================================
; Helper macros used with the colorspace_sse2.inc file
;=============================================================================

;------------------------------------------------------------------------------
; BGR_TO_YV12( BYTES )
;
; BYTES		3=bgr(24bit), 4=bgra(32-bit)
;
; bytes=3/4, pixels = 2, vpixels=2
;------------------------------------------------------------------------------

; TODO optimize.

%macro BGR_TO_YV12_INIT		2
  movdqa xmm7, [y_mul wrt rip]
%endmacro


%macro BGR_TO_YV12			2
    ; y_out
  pxor xmm4, xmm4
  pxor xmm5, xmm5
  movd xmm0, [rdi]                ; x_ptr[0...]
  movd xmm2, [rdi+rdx]            ; x_ptr[x_stride...]
  punpcklbw xmm0, xmm4            ; [  |b |g |r ]
  punpcklbw xmm2, xmm5            ; [  |b |g |r ]
  movdqa xmm6, xmm0               ; = [  |b4|g4|r4]
  paddw xmm6, xmm2                ; +[  |b4|g4|r4]
  pmaddwd xmm0, xmm7              ; *= Y_MUL
  pmaddwd xmm2, xmm7              ; *= Y_MUL
  movdqa xmm4, xmm0               ; [r]
  movdqa xmm5, xmm2               ; [r]
  psrldq xmm4, 4                  ; +[g]
  psrldq xmm5, 4                  ; +[g]
  paddd xmm0, xmm4                ; +[b]
  paddd xmm2, xmm5                ; +[b]

  pxor xmm4, xmm4
  pxor xmm5, xmm5
  movd xmm1, [rdi+%1]             ; src[%1...]
  movd xmm3, [rdi+rdx+%1]         ; src[x_stride+%1...]
  punpcklbw xmm1, xmm4            ; [  |b |g |r ]
  punpcklbw xmm3, xmm5            ; [  |b |g |r ]
  paddw xmm6, xmm1                ; +[  |b4|g4|r4]
  paddw xmm6, xmm3                ; +[  |b4|g4|r4]
  pmaddwd xmm1, xmm7              ; *= Y_MUL
  pmaddwd xmm3, xmm7              ; *= Y_MUL
  movdqa xmm4, xmm1               ; [r]
  movdqa xmm5, xmm3               ; [r]
  psrlq xmm4, 32                  ; +[g]
  psrlq xmm5, 32                  ; +[g]
  paddd xmm1, xmm4                ; +[b]
  paddd xmm3, xmm5                ; +[b]

  movq r13, xmm0
  shr r13, 8
  add r13, [y_add wrt rip]
  mov [rsi], r13b                   ; y_ptr[0]

  movq r13, xmm1
  shr r13, 8
  add r13, [y_add wrt rip]
  mov [rsi + 1], r13b               ; y_ptr[1]

  movq r13, xmm2
  shr r13, 8
  add r13, [y_add wrt rip]
  mov [rsi + rax + 0], r13b         ; y_ptr[y_stride + 0]

  movq r13, xmm3
  shr r13, 8
  add r13, [y_add wrt rip]
  mov [rsi + rax + 1], r13b         ; y_ptr[y_stride + 1]

  ; u_ptr, v_ptr
  movdqa xmm0, xmm6               ; = [  |b4|g4|r4]
  pmaddwd xmm6, [v_mul wrt rip]   ; *= V_MUL
  pmaddwd xmm0, [u_mul wrt rip]   ; *= U_MUL
  movdqa xmm1, xmm0
  movdqa xmm2, xmm6
  psrldq xmm1, 4
  psrldq xmm2, 4
  paddd xmm0, xmm1
  paddd xmm2, xmm6

  movq r13, xmm0
  shr r13, 10
  add r13, U_ADD
  mov [rbx], r13b

  movq r13, xmm2
  shr r13, 10
  add r13, V_ADD
  mov [rcx], r13b
%endmacro

;------------------------------------------------------------------------------
; YV12_TO_BGR( BYTES )
;
; BYTES		3=bgr(24-bit), 4=bgra(32-bit)
;
; bytes=3/4, pixels = 16, vpixels=2
;------------------------------------------------------------------------------

%macro YV12_TO_BGR_INIT		2
  pxor xmm7, xmm7             ; clear xmm7
%endmacro

%macro YV12_TO_BGR			2
%define TEMP_Y1  xmm8
%define TEMP_Y2  xmm9
%define TEMP_G1  xmm10
%define TEMP_G2  xmm11
%define TEMP_B1  xmm12
%define TEMP_B2  xmm13

  movq xmm2, [rbx]                    ; u_ptr[0]
  movq xmm3, [rcx]                    ; v_ptr[0]
  punpcklbw xmm2, xmm7                ; u3u2u1u0 -> xmm2
  punpcklbw xmm3, xmm7                ; v3v2v1v0 -> xmm3
  psubsw xmm2, [U_SUB wrt rip]        ; U - 128
  psubsw xmm3, [V_SUB wrt rip]        ; V - 128
  movdqa xmm4, xmm2
  movdqa xmm5, xmm3
  pmullw xmm2, [UG_MUL wrt rip]
  pmullw xmm3, [VG_MUL wrt rip]
  movdqa xmm6, xmm2                   ; u3u2u1u0 -> xmm6
  punpckhwd xmm2, xmm2                ; u3u3u2u2 -> xmm2
  punpcklwd xmm6, xmm6                ; u1u1u0u0 -> xmm6
  pmullw xmm4, [UB_MUL wrt rip]       ; B_ADD -> xmm4
  movdqa xmm0, xmm3
  punpckhwd xmm3, xmm3                ; v3v3v2v2 -> xmm2
  punpcklwd xmm0, xmm0                ; v1v1v0v0 -> xmm6
  paddsw xmm2, xmm3
  paddsw xmm6, xmm0
  pmullw xmm5, [VR_MUL wrt rip]       ; R_ADD -> xmm5
  %2     xmm0, [rsi]                  ; y7y6y5y4y3y2y1y0 -> xmm0
  movdqa xmm1, xmm0
  punpckhbw xmm1, xmm7                ; y7y6y5y4 -> xmm1
  punpcklbw xmm0, xmm7                ; y3y2y1y0 -> xmm0
  psubsw xmm0, [Y_SUB wrt rip]        ; Y - Y_SUB
  psubsw xmm1, [Y_SUB wrt rip]        ; Y - Y_SUB
  pmullw xmm1, [Y_MUL wrt rip]
  pmullw xmm0, [Y_MUL wrt rip]
  movdqa TEMP_Y2, xmm1                ; y7y6y5y4 -> xmm3
  movdqa TEMP_Y1, xmm0                ; y3y2y1y0 -> xmm7
  psubsw xmm1, xmm2                   ; g7g6g5g4 -> xmm1
  psubsw xmm0, xmm6                   ; g3g2g1g0 -> xmm0
  psraw xmm1, SCALEBITS
  psraw xmm0, SCALEBITS
  packuswb xmm0, xmm1                 ;g7g6g5g4g3g2g1g0 -> xmm0
  movdqa TEMP_G1, xmm0
  %2     xmm0, [rsi+rax]              ; y7y6y5y4y3y2y1y0 -> xmm0
  movdqa xmm1, xmm0
  punpckhbw xmm1, xmm7                ; y7y6y5y4 -> xmm1
  punpcklbw xmm0, xmm7                ; y3y2y1y0 -> xmm0
  psubsw xmm0, [Y_SUB wrt rip]        ; Y - Y_SUB
  psubsw xmm1, [Y_SUB wrt rip]        ; Y - Y_SUB
  pmullw xmm1, [Y_MUL wrt rip]
  pmullw xmm0, [Y_MUL wrt rip]
  movdqa xmm3, xmm1
  psubsw xmm1, xmm2                   ; g7g6g5g4 -> xmm1
  movdqa xmm2, xmm0
  psubsw xmm0, xmm6                   ; g3g2g1g0 -> xmm0
  psraw xmm1, SCALEBITS
  psraw xmm0, SCALEBITS
  packuswb xmm0, xmm1                 ; g7g6g5g4g3g2g1g0 -> xmm0
  movdqa TEMP_G2, xmm0
  movdqa xmm0, xmm4
  punpckhwd xmm4, xmm4                ; u3u3u2u2 -> xmm2
  punpcklwd xmm0, xmm0                ; u1u1u0u0 -> xmm6
  movdqa xmm1, xmm3                   ; y7y6y5y4 -> xmm1
  paddsw xmm3, xmm4                   ; b7b6b5b4 -> xmm3
  movdqa xmm7, xmm2                   ; y3y2y1y0 -> xmm7
  paddsw xmm2, xmm0                   ; b3b2b1b0 -> xmm2
  psraw xmm3, SCALEBITS
  psraw xmm2, SCALEBITS
  packuswb xmm2, xmm3                 ; b7b6b5b4b3b2b1b0 -> xmm2
  movdqa TEMP_B2, xmm2
  movdqa xmm3, TEMP_Y2
  movdqa xmm2, TEMP_Y1
  movdqa xmm6, xmm3                   ; TEMP_Y2 -> xmm6
  paddsw xmm3, xmm4                   ; b7b6b5b4 -> xmm3
  movdqa xmm4, xmm2                   ; TEMP_Y1 -> xmm4
  paddsw xmm2, xmm0                   ; b3b2b1b0 -> xmm2
  psraw xmm3, SCALEBITS
  psraw xmm2, SCALEBITS
  packuswb xmm2, xmm3                 ; b7b6b5b4b3b2b1b0 -> xmm2
  movdqa TEMP_B1, xmm2
  movdqa xmm0, xmm5
  punpckhwd xmm5, xmm5                ; v3v3v2v2 -> xmm5
  punpcklwd xmm0, xmm0                ; v1v1v0v0 -> xmm0
  paddsw xmm1, xmm5                   ; r7r6r5r4 -> xmm1
  paddsw xmm7, xmm0                   ; r3r2r1r0 -> xmm7
  psraw xmm1, SCALEBITS
  psraw xmm7, SCALEBITS
  packuswb xmm7, xmm1                 ; r7r6r5r4r3r2r1r0 -> xmm7 (TEMP_R2)
  paddsw xmm6, xmm5                   ; r7r6r5r4 -> xmm6
  paddsw xmm4, xmm0                   ; r3r2r1r0 -> xmm4
  psraw xmm6, SCALEBITS
  psraw xmm4, SCALEBITS
  packuswb xmm4, xmm6                 ; r7r6r5r4r3r2r1r0 -> xmm4 (TEMP_R1)
  movdqa xmm0, TEMP_B1
  movdqa xmm1, TEMP_G1
  movdqa xmm6, xmm7
  movdqa xmm2, xmm0
  punpcklbw xmm2, xmm4                ; r3b3r2b2r1b1r0b0 -> xmm2
  punpckhbw xmm0, xmm4                ; r7b7r6b6r5b5r4b4 -> xmm0
  pxor xmm7, xmm7
  movdqa xmm3, xmm1
  punpcklbw xmm1, xmm7                ; 0g30g20g10g0 -> xmm1
  punpckhbw xmm3, xmm7                ; 0g70g60g50g4 -> xmm3
  movdqa xmm4, xmm2
  punpcklbw xmm2, xmm1                ; 0r1g1b10r0g0b0 -> xmm2
  punpckhbw xmm4, xmm1                ; 0r3g3b30r2g2b2 -> xmm4
  movdqa xmm5, xmm0
  punpcklbw xmm0, xmm3                ; 0r5g5b50r4g4b4 -> xmm0
  punpckhbw xmm5, xmm3                ; 0r7g7b70r6g6b6 -> xmm5
%if %1 == 3     ; BGR (24-bit)
; TODO optimize
  movd [rdi], xmm2
  psrldq xmm2, 4
  movd [rdi + 3], xmm2
  psrldq xmm2, 4
  movd [rdi + 6], xmm2
  psrldq xmm2, 4
  movd [rdi + 9], xmm2

  movd [rdi + 12], xmm4
  psrldq xmm4, 4
  movd [rdi + 15], xmm4
  psrldq xmm4, 4
  movd [rdi + 18], xmm4
  psrldq xmm4, 4
  movd [rdi + 21], xmm4

  movd [rdi + 24], xmm0
  psrldq xmm0, 4
  movd [rdi + 27], xmm0
  psrldq xmm0, 4
  movd [rdi + 30], xmm0
  psrldq xmm0, 4
  movd [rdi + 33], xmm0

  movd [rdi + 36], xmm5
  psrldq xmm5, 4
  movd [rdi + 39], xmm5
  psrldq xmm5, 4
  movd [rdi + 42], xmm5
  movdqa xmm2, xmm5
  pslldq xmm2, 13
  psrldq xmm2, 15
  psrldq xmm5, 3
  por    xmm5, xmm2
  movd  [rdi + 44], xmm5

  movdqa xmm0, TEMP_B2
  movdqa xmm1, TEMP_G2
  movdqa xmm2, xmm0
  punpcklbw xmm2, xmm6                ; r3b3r2b2r1b1r0b0 -> xmm2
  punpckhbw xmm0, xmm6                ; r7b7r6b6r5b5r4b4 -> xmm0
  movdqa xmm3, xmm1
  punpcklbw xmm1, xmm7                ; 0g30g20g10g0 -> xmm1
  punpckhbw xmm3, xmm7                ; 0g70g60g50g4 -> xmm3
  movdqa xmm4, xmm2
  punpcklbw xmm2, xmm1                ; 0r1g1b10r0g0b0 -> xmm2
  punpckhbw xmm4, xmm1                ; 0r3g3b30r2g2b2 -> xmm4
  movdqa xmm5, xmm0
  punpcklbw xmm0, xmm3                ; 0r5g5b50r4g4b4 -> xmm0
  punpckhbw xmm5, xmm3                ; 0r7g7b70r6g6b6 -> xmm5

  movd [rdi+rdx], xmm2
  psrldq xmm2, 4
  movd [rdi+rdx + 3], xmm2
  psrldq xmm2, 4
  movd [rdi+rdx + 6], xmm2
  psrldq xmm2, 4
  movd [rdi+rdx + 9], xmm2

  movd [rdi+rdx + 12], xmm4
  psrldq xmm4, 4
  movd [rdi+rdx + 15], xmm4
  psrldq xmm4, 4
  movd [rdi+rdx + 18], xmm4
  psrldq xmm4, 4
  movd [rdi+rdx + 21], xmm4

  movd [rdi+rdx + 24], xmm0
  psrldq xmm0, 4
  movd [rdi+rdx + 27], xmm0
  psrldq xmm0, 4
  movd [rdi+rdx + 30], xmm0
  psrldq xmm0, 4
  movd [rdi+rdx + 33], xmm0

  movd [rdi +rdx + 36], xmm5
  psrldq xmm5, 4
  movd [rdi +rdx + 39], xmm5
  psrldq xmm5, 4
  movd [rdi +rdx + 42], xmm5
  movdqa xmm2, xmm5
  pslldq xmm2, 13
  psrldq xmm2, 15
  psrldq xmm5, 3
  por    xmm5, xmm2
  movd  [rdi +rdx + 44], xmm5

%else       ; BGRA (32-bit)
  %2     [rdi], xmm2
  %2     [rdi + 16], xmm4
  %2     [rdi + 32], xmm0
  %2     [rdi + 48], xmm5
  movdqa xmm0, TEMP_B2
  movdqa xmm1, TEMP_G2
  movdqa xmm2, xmm0
  punpcklbw xmm2, xmm6                ; r3b3r2b2r1b1r0b0 -> xmm2
  punpckhbw xmm0, xmm6                ; r7b7r6b6r5b5r4b4 -> xmm0
  movdqa xmm3, xmm1
  punpcklbw xmm1, xmm7                ; 0g30g20g10g0 -> xmm1
  punpckhbw xmm3, xmm7                ; 0g70g60g50g4 -> xmm3
  movdqa xmm4, xmm2
  punpcklbw xmm2, xmm1                ; 0r1g1b10r0g0b0 -> xmm2
  punpckhbw xmm4, xmm1                ; 0r3g3b30r2g2b2 -> xmm4
  movdqa xmm5, xmm0
  punpcklbw xmm0, xmm3                ; 0r5g5b50r4g4b4 -> xmm0
  punpckhbw xmm5, xmm3                ; 0r7g7b70r6g6b6 -> xmm5
  %2     [rdi + rdx], xmm2
  %2     [rdi + rdx + 16], xmm4
  %2     [rdi + rdx + 32], xmm0
  %2     [rdi + rdx + 48], xmm5
%endif

%undef TEMP_Y1
%undef TEMP_Y2
%undef TEMP_G1
%undef TEMP_G2
%undef TEMP_B1
%undef TEMP_B2
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

%include "colorspace_win64_sse2.inc"

; input
MAKE_COLORSPACE  bgr_to_yv12_win64_sse2,0,    3,2,2,  BGR_TO_YV12,  3, -1
MAKE_COLORSPACE  bgra_to_yv12_win64_sse2,0,   4,2,2,  BGR_TO_YV12,  4, -1

; output
; aligned version : x_ptr, y_src, x_stride, y_stride must be multiple of 16
MAKE_COLORSPACE  yv12_to_bgr_win64_sse2a,0,   3,16,2,  YV12_TO_BGR,  3, movdqa
MAKE_COLORSPACE  yv12_to_bgra_win64_sse2a,0,  4,16,2,  YV12_TO_BGR,  4, movdqa
; unaligned version for fail over
MAKE_COLORSPACE  yv12_to_bgr_win64_sse2u,0,   3,16,2,  YV12_TO_BGR,  3, movdqu
MAKE_COLORSPACE  yv12_to_bgra_win64_sse2u,0,  4,16,2,  YV12_TO_BGR,  4, movdqu

;
; Sample code
;
;#ifndef WIN64
;  // WIN32
;#else
;  // WIN64
;  if (((size_t)x_ptr&15) || ((size_t)x_stride&15) || ((size_t)y_src&15) || ((size_t)y_stride&15))
;   yv12_to_bgra_win64_sse2u(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // fail over (unaligned)
;  else
;   yv12_to_bgra_win64_sse2a(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // faster
;#endif
