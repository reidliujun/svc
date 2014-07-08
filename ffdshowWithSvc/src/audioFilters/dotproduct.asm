; Copyright (C) 2001 Jean-Marc Valin

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

cglobal vec_inner_prod_3dnow
;void vec_inner_prod_3dnow(const float *a, const float *b, int len,float *sum)
%define ssize 12
vec_inner_prod_3dnow
  push ecx
  push edi
  push edx

  mov ecx,[esp+ssize+12]
  mov eax,[esp+ssize+4]
  mov edi,[esp+ssize+8]
  mov edx,[esp+ssize+16]

  pxor mm4, mm4
  pxor mm5, mm5
  sub ecx,4
  jb mul4_skip

mul4_loop:
  movq  mm0 ,[eax]
  movq  mm1 ,[edi]
  movq  mm2 ,[8+eax]
  movq  mm3 ,[8+edi]
  add  eax ,16
  add  edi ,16
  pfmul mm1,mm0
  pfmul mm3,mm2
  pfadd mm4,mm1
  pfadd mm5,mm3
  sub ecx ,4
  jae mul4_loop
  pfadd mm4,mm5

mul4_skip:
  add ecx,2
  jae mul2_skip
  movq mm0 ,[eax]
  movq mm1 ,[edi]
  add  eax ,8
  add  edi ,8
  pfmul mm1, mm0
  pfadd mm4, mm1

mul2_skip:
  and ecx ,1
  jz even
  pxor mm0, mm0
  pxor mm1, mm1
  movd  mm0, [eax]
  movd  mm1, [edi]
  pfmul mm1 ,mm0
  pfadd mm4 ,mm1

even:
  pxor mm5, mm5
  pfacc mm4 ,mm5
  movq [edx], mm4

  pop edx
  pop edi
  pop ecx

  femms

  ret
