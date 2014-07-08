;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - Optimized memcpy functions -
; *
; *  Copyright (C) 2001 the xine project <http://xinehq.de/>
; *                2003 Edouard Gomez <ed.gomez@free.fr>
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
; *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
; *
; ***************************************************************************/

;-----------------------------------------------------------------------------
; This code has been taken from the xine project and turned into pure asm
; It is a bit smaller than what gcc generates from inlined volatile asm.
; MMX instructions have been (a bit) paired for better performance, i just
; can guarantee it's not slower :-)
;-----------------------------------------------------------------------------

BITS 32

%macro cglobal 1
	%ifdef PREFIX
		global _%1
		%define %1 _%1
	%else
		global %1
	%endif
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal memcpy_x86
cglobal memcpy_mmx
cglobal memcpy_xmm
cglobal memcpy_sse

;-----------------------------------------------------------------------------
; SMALL_MEMCPY_X86(void *dst, cont void *src, int size)
; edi - dst address holder
; esi - src address holder
; ecx - size in bytes
;
; NB: well adapted for size < 4
;-----------------------------------------------------------------------------
%macro SMALL_MEMCPY_X86 0
    rep movsb
%endmacro

;-----------------------------------------------------------------------------
; BIG_MEMCPY_X86(void *dst, cont void *src, int size)
; edi - dst address holder
; esi - src address holder
; ecx - size in bytes
;
;
; NB: well adapted for size >= 4
;-----------------------------------------------------------------------------

%macro BIG_MEMCPY_X86 0
  mov eax, ecx                  ; store for later use
  shr ecx, 2
  rep movsd
  test al, 2                    ; can we move a word ?
  je .dont_move_word
  movsw
.dont_move_word:
  test al, 1
  je .dont_move_byte
  movsb
.dont_move_byte:
%endmacro

;-----------------------------------------------------------------------------
; void *memcpy_x86(void *dst, cont void *src, int size);
;-----------------------------------------------------------------------------

ALIGN 16
memcpy_x86:
  push esi
  push edi
  mov edi, [esp+8+4]            ; dst
  mov esi, [esp+8+8]            ; src
  mov ecx, [esp+8+12]           ; size
  cmp ecx, 3
  jg .big_memcpy
  SMALL_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret
.big_memcpy
  BIG_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret

;-----------------------------------------------------------------------------
; void *memcpy_sse(void *dst, cont void *src, int size);
;-----------------------------------------------------------------------------

%define SSE_MMREG_SIZE 16
%define SSE_MIN_LEN 0x40        ; 64-byte blocks

ALIGN 16
memcpy_sse:
  push esi
  push edi

  mov edi, [esp+8+4]
  mov esi, [esp+8+8]
  mov ecx, [esp+8+12]

  ; Prefetch some data, this is even good for the 386 code: movs(b|w|d)
  prefetchnta [esi]
  prefetchnta [esi+64]
  prefetchnta [esi+128]
  prefetchnta [esi+192]
  prefetchnta [esi+256]

  ; Is the block > MIN_SIZE in order to benefit from SSE block copying ?
  cmp ecx, SSE_MIN_LEN
  jl near .trailing_bytes       ; Not enough bytes, then just copy them the 386 way

  ; We must align the destination address to a SSE_MMREG_SIZE boundary
  ; to allow max throughput
  mov edx, edi
  and edx, SSE_MMREG_SIZE-1     ; see if it's an aligned address
  je .destination_aligned

  mov eax, ecx                  ; copy the number of bytes to copy
  sub edx, SSE_MMREG_SIZE       ; compute -number of unaligned bytes
  neg edx                       ; edx now holds the number of bytes to copy to the unaligned dest
  sub eax, edx                  ; computes the remaining number of aligned bytes to copy during the sse2 memcpy
  mov ecx, edx                  ; we will only copy the unaligned bytes for now
  SMALL_MEMCPY_X86
  mov ecx, eax                  ; put to ecx the number of bytes we have still to copy

.destination_aligned
  mov eax, ecx
  shr eax, 6                    ; eax will be the loop counter == number of 64bytes blocks
  and ecx, (64-1)               ; ecx will just keep trace of trailing bytes to copy
  test esi, (16-1)
  je .aligned_loop

ALIGN 8
.unaligned_loop                 ; SSE block copying (src address is not 16 byte aligned)
  prefetchnta [esi+320]
  movups xmm0, [esi+ 0]
  movups xmm1, [esi+16]
  movntps [edi+ 0], xmm0
  movntps [edi+ 16], xmm1
  movups xmm2, [esi+32]
  movups xmm3, [esi+48]
  movntps [edi+ 32], xmm2
  movntps [edi+ 48], xmm3
  add esi, 64
  add edi, 64
  dec eax
  jne .unaligned_loop
  sfence                        ; back to temporal data
  jmp .trailing_bytes

ALIGN 8
.aligned_loop                   ; SSE block copying (src address is 16 byte aligned)
  prefetchnta [esi+320]
  movaps xmm0, [esi+ 0]
  movaps xmm1, [esi+16]
  movntps [edi+ 0], xmm0
  movntps [edi+ 16], xmm1
  movaps xmm2, [esi+32]
  movaps xmm3, [esi+48]
  movntps [edi+ 32], xmm2
  movntps [edi+ 48], xmm3
  add esi, 64
  add edi, 64
  dec eax
  jne .aligned_loop
  sfence                        ; back to temporal data

  ; Copy the last bytes left after the sse block copying
  ; ecx must hold the number of remaining bytes to copy
  ; esi must hold the source address
  ; edi must hold the destination address
.trailing_bytes
  cmp ecx, 3
  jg .quite_a_few_trailing_bytes
  SMALL_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret
.quite_a_few_trailing_bytes
  BIG_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret

;-----------------------------------------------------------------------------
; void *memcpy_mmx(void *dst, cont void *src, int size);
;-----------------------------------------------------------------------------

%define MMX_MMREG_SIZE 8
%define MMX_MIN_LEN 0x40  ; 64 byte blocks

ALIGN 16
memcpy_mmx:
  push esi
  push edi

  mov edi, [esp+8+4]
  mov esi, [esp+8+8]
  mov ecx, [esp+8+12]

  ; Is the block > MIN_SIZE in order to benefit from SSE block copying ?
  cmp ecx, MMX_MIN_LEN
  jl near .trailing_bytes       ; Not enough bytes, then just copy them the 386 way

  ; We must align the destination address to a SSE_MMREG_SIZE boundary
  ; to allow max throughput
  mov edx, edi
  and edx, MMX_MMREG_SIZE-1     ; see if it's an aligned address
  je .destination_aligned

  mov eax, ecx                  ; copy the number of bytes to copy
  sub edx, MMX_MMREG_SIZE       ; compute -number of unaligned bytes
  neg edx                       ; edx now holds the number of bytes to copy to the unaligned dest
  sub eax, edx                  ; computes the remaining number of aligned bytes to copy during the sse2 memcpy
  mov ecx, edx                  ; we will only copy the unaligned bytes for now
  SMALL_MEMCPY_X86
  mov ecx, eax                  ; put to ecx the number of bytes we have still to copy

.destination_aligned
  mov eax, ecx
  shr eax, 6                    ; eax will be the loop counter == number of 64bytes blocks
  and ecx, (64-1)               ; ecx will just keep trace of trailing bytes to copy

ALIGN 8
.block_loop
  movq mm0, [esi+ 0]
  movq mm1, [esi+ 8]
  movq [edi+ 0], mm0
  movq [edi+ 8], mm1
  movq mm2, [esi+16]
  movq mm3, [esi+24]
  movq [edi+16], mm2
  movq [edi+24], mm3
  movq mm4, [esi+32]
  movq mm5, [esi+40]
  movq [edi+32], mm4
  movq [edi+40], mm5
  movq mm6, [esi+48]
  movq mm7, [esi+56]
  movq [edi+48], mm6
  movq [edi+56], mm7
  add esi, 64
  add edi, 64
  dec eax
  jne .block_loop
  emms

  ; Copy the last bytes left after the mmx block copying
  ; ecx must hold the number of remaining bytes to copy
  ; esi must hold the source address
  ; edi must hold the destination address
.trailing_bytes
  cmp ecx, 3
  jg .quite_a_few_trailing_bytes
  SMALL_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret
.quite_a_few_trailing_bytes
  BIG_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret

;-----------------------------------------------------------------------------
; void *memcpy_xmm(void *dst, cont void *src, int size);
;-----------------------------------------------------------------------------

%define XMM_MMREG_SIZE 8
%define XMM_MIN_LEN 0x40  ; 64 byte blocks

ALIGN 16
memcpy_xmm:
  push esi
  push edi

  mov edi, [esp+8+4]
  mov esi, [esp+8+8]
  mov ecx, [esp+8+12]

  ; Prefetch some data, this is even good for the 386 code: movs(b|w|d)
  prefetchnta [esi]
  prefetchnta [esi+64]
  prefetchnta [esi+128]
  prefetchnta [esi+192]
  prefetchnta [esi+256]

  ; Is the block > MIN_SIZE in order to benefit from SSE block copying ?
  cmp ecx, XMM_MIN_LEN
  jl near .trailing_bytes       ; Not enough bytes, then just copy them the 386 way

  ; We must align the destination address to a SSE_MMREG_SIZE boundary
  ; to allow max throughput
  mov edx, edi
  and edx, XMM_MMREG_SIZE-1     ; see if it's an aligned address
  je .destination_aligned

  mov eax, ecx                  ; copy the number of bytes to copy
  sub edx, XMM_MMREG_SIZE       ; compute -number of unaligned bytes
  neg edx                       ; edx now holds the number of bytes to copy to the unaligned dest
  sub eax, edx                  ; computes the remaining number of aligned bytes to copy during the sse2 memcpy
  mov ecx, edx                  ; we will only copy the unaligned bytes for now
  SMALL_MEMCPY_X86
  mov ecx, eax                  ; put to ecx the number of bytes we have still to copy

.destination_aligned
  mov eax, ecx
  shr eax, 6                    ; eax will be the loop counter == number of 64bytes blocks
  and ecx, (64-1)               ; ecx will just keep trace of trailing bytes to copy

ALIGN 8
.block_loop
  prefetchnta [esi+320]
  movq mm0, [esi+ 0]
  movq mm1, [esi+ 8]
  movntq [edi+ 0], mm0
  movntq [edi+ 8], mm1
  movq mm2, [esi+16]
  movq mm3, [esi+24]
  movntq [edi+16], mm2
  movntq [edi+24], mm3
  movq mm4, [esi+32]
  movq mm5, [esi+40]
  movntq [edi+32], mm4
  movntq [edi+40], mm5
  movq mm6, [esi+48]
  movq mm7, [esi+56]
  movntq [edi+48], mm6
  movntq [edi+56], mm7
  add esi, 64
  add edi, 64
  dec eax
  jne .block_loop
  sfence
  emms

  ; Copy the last bytes left after the xmm block copying
  ; ecx must hold the number of remaining bytes to copy
  ; esi must hold the source address
  ; edi must hold the destination address
.trailing_bytes
  cmp ecx, 3
  jg .quite_a_few_trailing_bytes
  SMALL_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret
.quite_a_few_trailing_bytes
  BIG_MEMCPY_X86
  mov eax, [esp+8+12]
  pop edi
  pop esi
  ret
