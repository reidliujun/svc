;------------------------------------------------------------------------------
;
; MAKE_COLORSPACE(NAME,STACK, BYTES,PIXELS,ROWS, FUNC, ARG1)
;
; This macro provides a assembler width/height scroll loop
; NAME		function name
; STACK		additional stack bytes required by FUNC
; BYTES		bytes-per-pixel for the given colorspace
; PIXELS	pixels (columns) operated on per FUNC call
; VPIXELS	vpixels (rows) operated on per FUNC call
; FUNC		conversion macro name; we expect to find FUNC_INIT and FUNC macros
; ARG1		argument passed to FUNC
; 
; throughout the FUNC the registers mean:
; rax		y_stride
; rbx		u_ptr
; rcx		v_ptr
; rdx		x_stride
; rsi		y_ptr
; rdi		x_ptr
; rbp		height (loop counter)
; r8        width  (loop counter)
; r9        fixedwidth
; r10       x_dif
; r11       y_dif
; r12       uv_dif
; r13,r14   for macro FUNC
;
; void NAME (uint8_t * x_ptr,
;            size_t x_stride,
;            uint8_t * y_ptr,
;            uint8_t * v_ptr,
;            uint8_t * u_ptr,
;            size_t y_stride,
;            size_t uv_stride,
;            size_t width,
;            size_t height,
;            size_t vflip);
;
;------------------------------------------------------------------------------

; sse2 version for WIN64 (ported by h.yamagata)

%macro		MAKE_COLORSPACE			8
%define NAME		%1
%define STACK		%2 ; must be multiple of 16.
%define BYTES		%3
%define PIXELS		%4
%define VPIXELS		%5
%define FUNC		%6
%define ARG1		%7
%define ARG2		%8
	; --- define function global/symbol
ALIGN 16
cglobal NAME
NAME:
	; --- init stack ---

%define pushsize	56
%define localsize	160 + STACK

%define vflip           rsp + localsize + pushsize + 80
%define height          rsp + localsize + pushsize + 72
%define width           rsp + localsize + pushsize + 64
%define uv_stride       rsp + localsize + pushsize + 56
%define y_stride        rsp + localsize + pushsize + 48
%define v_ptr           rsp + localsize + pushsize + 40
%define u_ptr           rsp + localsize + pushsize + 32
%define y_ptr           rsp + localsize + pushsize + 24
%define x_stride        rsp + localsize + pushsize + 16
%define x_ptr           rsp + localsize + pushsize + 8
%define _ip             rsp + localsize + pushsize + 0

%define save_xmm6       rsp + localsize - 160
%define save_xmm7       rsp + localsize - 144
%define save_xmm8       rsp + localsize - 128
%define save_xmm9       rsp + localsize - 112
%define save_xmm10      rsp + localsize - 96
%define save_xmm11      rsp + localsize - 80
%define save_xmm12      rsp + localsize - 64
%define save_xmm13      rsp + localsize - 48
%define save_xmm14      rsp + localsize - 32
%define save_xmm15      rsp + localsize - 16

  push r14    ;   rsp + localsize + 56
  push r13    ;   rsp + localsize + 48
  push r12    ;   rsp + localsize + 40
  push rbx    ;   rsp + localsize + 32
  push rsi    ;   rsp + localsize + 16
  push rdi    ;   rsp + localsize + 8
  push rbp    ;   rsp + localsize + 0

    sub rsp, localsize

    ; --- save non-volatile xmm registers ---

  movdqa [save_xmm6], xmm6
  movdqa [save_xmm7], xmm7
  movdqa [save_xmm8], xmm8
  movdqa [save_xmm9], xmm9
  movdqa [save_xmm10], xmm10
  movdqa [save_xmm11], xmm11
  movdqa [save_xmm12], xmm12
  movdqa [save_xmm13], xmm13
  movdqa [save_xmm14], xmm14
  movdqa [save_xmm15], xmm15

    ; --- save parameters (win64) ---

  mov [x_ptr], rcx
  mov [x_stride], rdx
  mov [y_ptr], r8
  mov [u_ptr], r9

    ; --- init varibles ---
    
  mov rax, [width]          ; fixed width
  add rax, 15               ;
  and rax, ~15              ;
  mov r9,rax                ;

  mov rbx, [x_stride]       ;
%rep BYTES
  sub rbx, rax              ;
%endrep
  mov r10, rbx              ; x_dif = x_stride - BYTES*fixed_width

  mov rbx, [y_stride]       ;
  sub rbx, rax              ;
  mov r11, rbx              ; y_dif = y_stride - fixed_width

  mov rbx, [uv_stride]      ;
  mov rcx, rax              ;
  shr rcx, 1                ;
  sub rbx, rcx              ;
  mov r12, rbx              ; uv_dif = uv_stride - fixed_width/2

  mov rsi, [y_ptr]          ; $rsi$ = y_ptr
  mov rdi, [x_ptr]          ; $rdi$ = x_ptr
  mov rdx, [x_stride]       ; $rdx$ = x_stride
  mov rbp, [height]         ; $rbp$ = height


  mov rbx, [vflip]
  or rbx, rbx
  jz .dont_flip

    ; --- do flipping ---

  xor rbx,rbx
%rep BYTES
  sub rbx, rax
%endrep
  sub rbx, rdx
  mov r10, rbx              ; x_dif = -BYTES*fixed_width - x_stride

  mov rax, rbp
  sub rax, 1
  push rdx                
  mul rdx
  pop rdx
  add rdi, rax              ; $rdi$ += (height-1) * x_stride

  neg rdx                   ; x_stride = -x_stride

.dont_flip

    ; --- begin loop ---

  mov rax, [y_stride]       ; $rax$ = y_stride
  mov rbx, [u_ptr]          ; $rbx$ = u_ptr
  mov rcx, [v_ptr]          ; $rcx$ = v_ptr

  FUNC %+ _INIT ARG1, ARG2  ; call FUNC_INIT

.y_loop
  mov r8, r9

.x_loop
  FUNC ARG1, ARG2           ; call FUNC

  add rdi, BYTES*PIXELS     ; x_ptr += BYTES*PIXELS
  add rsi, PIXELS           ; y_ptr += PIXELS
  add rbx, PIXELS/2         ; u_ptr += PIXELS/2
  add rcx, PIXELS/2         ; v_ptr += PIXELS/2
        
  sub r8, PIXELS            ; $r8$ -= PIXELS
  jg .x_loop                ; if ($r8$ > 0) goto .x_loop

  add rdi, r10              ; x_ptr += x_dif + (VPIXELS-1)*x_stride
  add rsi, r11              ; y_ptr += y_dif + (VPIXELS-1)*y_stride
%rep VPIXELS-1
  add rdi, rdx            
  add rsi, rax            
%endrep

  add rbx, r12              ; u_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride
  add rcx, r12              ; v_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride
%rep (VPIXELS/2)-1
  add rbx, [uv_stride]
  add rcx, [uv_stride]
%endrep

  sub rbp, VPIXELS          ; $rbp$ -= VPIXELS
  jg .y_loop                ; if ($rbp$ > 0) goto .y_loop

  ; restore xmm registers

  movdqa xmm6, [save_xmm6]
  movdqa xmm7, [save_xmm7]
  movdqa xmm8, [save_xmm8]
  movdqa xmm9, [save_xmm9]
  movdqa xmm10, [save_xmm10]
  movdqa xmm11, [save_xmm11]
  movdqa xmm12, [save_xmm12]
  movdqa xmm13, [save_xmm13]
  movdqa xmm14, [save_xmm14]
  movdqa xmm15, [save_xmm15]

  ; cleanup stack & undef everything

  add rsp, localsize
  pop rbp
  pop rdi
  pop rsi
  pop rbx
  pop r12
  pop r13
  pop r14

%undef vflip
%undef height
%undef width
%undef uv_stride
%undef y_stride
%undef v_ptr
%undef u_ptr
%undef y_ptr
%undef x_stride
%undef x_ptr
%undef _ip
        ret
.endfunc
%undef NAME
%undef STACK
%undef BYTES
%undef PIXELS
%undef VPIXELS
%undef FUNC
%undef ARG1
%endmacro
;------------------------------------------------------------------------------
