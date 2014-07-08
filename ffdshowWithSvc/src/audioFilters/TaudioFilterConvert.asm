; optimized floating point conversion routines from Avisynth (c) Klaus Post 2001 - 2004

BITS 32

%macro cglobal 1
 %ifdef PREFIX
  global _%1
  %define %1 _%1
 %else
  global %1
 %endif
%endmacro

section .text

align 16

cglobal convert_16_32_mmx
;[esp+8+ 4] - inbuf
;[esp+8+ 8] - outbuf
;[esp+8+12] - c_loop
convert_16_32_mmx
 push esi
 push edi
 push ebx

 xor eax,eax
 mov ebx,[esp+12+12]
 shl ebx,1
 mov esi,[esp+12+ 4]
 mov edi,[esp+12+ 8]
 pxor mm0,mm0
 pxor mm1,mm1
 align 16
sl1_mmx:
 punpcklwd mm0,[esi+eax]
 punpcklwd mm1,[esi+eax+4]
 movq [edi+2*eax],mm0
 movq [edi+2*eax+8],mm1
 add eax,8
 cmp eax,ebx
 jne sl1_mmx
 emms

 pop ebx
 pop edi
 pop esi
 ret

align 16

multiplier_float_16 dd 32768.0,32768.0,32768.0,32768.0

cglobal convert_float_16_3dnow
;[esp+12+ 4] - inbuf
;[esp+12+ 8] - samples
;[esp+12+12] - c_loop
convert_float_16_3dnow
 push esi
 push ebx
 push edi

 xor eax,eax                   ; count
  mov ebx, [esp+12+12]
  shl ebx, 1                     ; in output bytes (*2)
  mov esi, [esp+12+4];
 mov edi, [esp+12+8];
 movq mm7,[multiplier_float_16]
  ;pshufw mm7,mm7, 01000100b
  align 16
c16f_loop:
 movq mm1, [esi+eax*2]            ;  b b | a a
  movq mm2, [esi+eax*2+8]          ;  d d | c c
  pfmul mm1,mm7                  ; x * 32 bit
  pfmul mm2,mm7                  ; x * 32 bit
  pf2id mm1, mm1                 ;  xb=int(b) | xa=int(a)
  pf2id mm2, mm2                 ;  xb=int(d) | xa=int(c)
  packssdw mm1,mm2
  movq [edi+eax], mm1            ;  store xb | xa
  add eax,8
  cmp eax, ebx
  jne c16f_loop
  emms

 pop edi
 pop ebx
 pop esi
 ret

align 16

multiplier_float_32 dd 2147483647.0,2147483647.0,2147483647.0,2147483647.0
limit_float_32      dd 2147483520.0,2147483520.0,2147483520.0,2147483520.0

cglobal convert_float_32_3dnow
;[esp+12+ 4] - inbuf
;[esp+12+ 8] - samples
;[esp+12+12] - c_loop
convert_float_32_3dnow
 push esi
 push ebx
 push edi

 xor eax,eax                   ; count
   mov ebx, [esp+12+12]
   shl ebx, 2                     ; in output bytes (*4)
   mov esi, [esp+12+4];
 mov edi, [esp+12+8];
 movq mm7,[multiplier_float_32]
   ;pshufw mm7,mm7, 01000100b
   align 16
c32f_loop:
 movq mm1, [esi+eax]            ;  b b | a a
   movq mm2, [esi+eax+8]          ;  d d | c c
   pfmul mm1,mm7                  ; x * 32 bit
   pfmul mm2,mm7                  ; x * 32 bit
   pf2id mm1, mm1                 ;  xb=int(b) | xa=int(a)
   pf2id mm2, mm2                 ;  xb=int(d) | xa=int(c)
   movq [edi+eax], mm1            ;  store xb | xa
   movq [edi+eax+8], mm2          ;  store xd | xc
   add eax,16
   cmp eax, ebx
   jne c32f_loop
   emms

 pop edi
 pop ebx
 pop esi
 ret

align 16

divisor_float_16 dd 0.000030517578125,0.000030517578125

cglobal convert_16_float_3dnow
;[esp+12+ 4] - inbuf
;[esp+12+ 8] - samples
;[esp+12+12] - c_loop
convert_16_float_3dnow
 push esi
 push ebx
 push edi

 xor eax,eax                   ; count
 mov ebx, [esp+12+12]
 shl ebx, 1  ; Number of input bytes.
 mov esi, [esp+12+4];
 mov edi, [esp+12+8];
 movq mm7,[divisor_float_16]
 ;pshufw mm7,mm7, 01000100b
 pxor mm6,mm6
 align 16
c16_loop_3dnow
 movq mm0, [esi+eax]          ;  d c | b a
 movq mm1, mm0
 punpcklwd mm0, mm6             ;  b b | a a
 punpckhwd mm1, mm6             ;  d d | c c
 pi2fw mm0, mm0                 ;  xb=float(b) | xa=float(a)
 pi2fw mm1, mm1                 ;  xb=float(d) | xa=float(c)
 pfmul mm0,mm7                  ; x / 32768.0
 pfmul mm1,mm7                  ; x / 32768.0
 movq [edi+eax*2], mm0          ;  store xb | xa
 movq [edi+eax*2+8], mm1        ;  store xd | xc
 add eax,8
 cmp eax, ebx
 jne c16_loop_3dnow
 emms

 pop edi
 pop ebx
 pop esi
 ret

align 16

divisor_float_32 dd 4.656612875245796924105750827168e-10,4.656612875245796924105750827168e-10

cglobal convert_32_float_3dnow
;[esp+12+ 4] - inbuf
;[esp+12+ 8] - samples
;[esp+12+12] - c_loop
convert_32_float_3dnow
 push esi
 push ebx
 push edi

 xor eax,eax                   ;  count
 mov ebx, [esp+12+12]
 shl ebx, 2                     ;  in input bytes (*4)
 mov esi, [esp+12+4];
 mov edi, [esp+12+8];
 movq mm7,[divisor_float_32]
 ;pshufw mm7,mm7, 01000100b
 align 16
c32_loop_3dnow:
 movq mm1, [esi+eax]            ;  b b | a a
 movq mm2, [esi+eax+8]          ;  d d | c c
 pi2fd mm1, mm1                 ;  xb=float(b) | xa=float(a)
 pi2fd mm2, mm2                 ;  xb=float(d) | xa=float(c)
 pfmul mm1,mm7                  ; x / 32768.0
 pfmul mm2,mm7                  ; x / 32768.0
 movq [edi+eax], mm1            ;  store xb | xa
 movq [edi+eax+8], mm2          ;  store xd | xc
 add eax,16
 cmp eax, ebx
 jne c32_loop_3dnow
 emms

 pop edi
 pop ebx
 pop esi
 ret
