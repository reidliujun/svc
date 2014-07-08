;      TITLE("Sample App")
;++


        AREA sample, CODE, READONLY  ; name this block of code
              
  
  EXPORT ict_Calcul_1erebcle_arm
  EXPORT ict_Calcul_2emebcle_arm 
  
  
ict_Calcul_1erebcle_arm proc

	stmfd r13!,{r14}
	stmfd r13!,{r0-r12}
	ldmia r0,{r3,r4,r5,r6}
	
	;verifie
	mov r7,r3,lsl #16	  
	mov r7,r7,asr #16		;r7=block[0]
	mov r8,r4,lsl #16	
	mov r8,r8,asr #16		;r8=block[2]
	add r10,r7,r8			;r10=block[0]+block[2]=z0
	
	mov r3,r3,asr #16		;r3=block[1]
	mov r4,r4,asr #16		;r4=block[3]
	add r12,r3,r4,asr #1	;r12=block[1]+block[3]>>1=z3
	
	;non verifie
	add r14,r10,r12
	mov r14,r14,lsl #16		;(r14)PF=Temp[0]
	sub r11,r10,r12			;(r11)Pf=Temp[3]
	mov r11,r11,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r11 est négatif
	mov r11,r11,lsr #16
	
	;verifie
	sub r10,r7,r8			;r10=block[0]-block[2]=z1
	rsb r12,r4,r3,asr #1	;r12=block[1]>>1-block[3]=z2

	;non verifie
	add r3,r10,r12
	
	mov r3,r3,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r3 est négatif
	mov r3,r3,lsr #16
	
	orr r14,r14,r3			;(r14)Pf=Temp[1]		
	sub r4,r10,r12
	orr r11,r11,r4,lsl #16	;(r11)PF=Temp[2]
			
	;r14=Temp[0]:Temp[1]	r11=Temp[2]:Temp[3]
	;r3(free),r4(free),r7(free),r9(free),r10(free),r11(free)
	
	;non verifie
	mov r14,r14,ror #16
	mov r11,r11,ror #16		;OK!!
	
	;verifie
	mov r7,r5,lsl #16
	mov r7,r7,asr #16		;r7=block[4]
	mov r8,r6,lsl #16
	mov r8,r8,asr #16		;r8=block[6]
	add r10,r7,r8			;r10=block[4]+block[6]=z0
	
	
	mov r5,r5,asr #16		;r7=block[5]
	mov r6,r6,asr #16		;r6=block[7]
	add r12,r5,r6,asr #1	;r12=block[5]+block[7]>>1=z3
	
	
	;non verifie
	add r9,r10,r12	
	mov r9,r9,lsl #16		;(r9)PF=Temp[4]
	sub r4,r10,r12			;(r4)Pf=Temp[7]
	mov r4,r4,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r4 est négatif
	mov r4,r4,lsr #16
	
	;verifie
	sub r10,r7,r8			;r10=block[4]-block[6]=z1
	rsb r12,r6,r5,asr #1	;r12=block[5]>>1-block[7]=z2
	
	;non verifie
	add r5,r10,r12			
	mov r5,r5,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r5 est négatif
	mov r5,r5,lsr #16
	orr r9,r5,r9			;(r9)Pf=Temp[5]
			
	sub r5,r10,r12			;(r4)PF=Temp[6]
	orr r4,r4,r5,lsl #16	

	;r9=Temp[4]:Temp[5]		r4=Temp[6]:Temp[7]
	
	;non verifie
	mov r9,r9,ror #16
	mov r4,r4,ror #16
	
	
	;non verifie
	mov r3,r14
	mov r14,r4
	mov r4,r3
	
	mov r3,r11
	mov r11,r9
	mov r9,r3
	
	stmia r1,{r4,r9,r11,r14}	;OK!!!
	add r1,r1,#16
	
	;stmfd r13!,{r4,r9,r11,r14}	;do this later
	
	;verifie
	add r0,r0,#16
	ldmia r0,{r3,r4,r5,r6}
	
	;verifie
	mov r8,r3,lsl #16
	mov r8,r8,asr #16		;r8=block[8]
	mov r9,r4,lsl #16
	mov r9,r9,asr #16		;r9=block[10]
	add	r14,r8,r9			;r14=block[8]+block[10]=z0
	
	;verifie
	mov r3,r3,asr #16		;r3=block[9]
	mov r4,r4,asr #16		;r4=block[11]
	add r12,r3,r4,asr #1	;r12=block[9]+block[11]>>1=z3

	;non verifie
	add r7,r14,r12			
	mov r7,r7,lsl #16		;(r7)PF=Temp[8] 
	sub r11,r14,r12			;(r11)Pf=Temp[11]
	mov r11,r11,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r11 est négatif
	mov r11,r11,lsr #16
	
	;verifie
	sub r14,r8,r9			;r14=block[8]-block[10]=z1
	rsb r12,r4,r3,asr #1	;r12=block[9]>>1-block[11]=z2
	
	
	;non verifie
	add r3,r14,r12
	mov r3,r3,lsl #16		;on force les bits de PF à zero dans le cas où le resultat dans r3 est négatif
	mov r3,r3,lsr #16
	orr r7,r3,r7			;(r7)Pf=Temp[9]
	sub r3,r14,r12
	orr r11,r11,r3,lsl #16	;(r11)PF=Temp[10]
	
	;r7=Temp[8]:Temp[9]		r11=Temp[10]:Temp[11]
	
	;non verifie
	mov r7,r7,ror #16
	mov r11,r11,ror #16
	
	
	;verifie
	mov r4,r5,lsl #16
	mov r4,r4,asr #16		;r4=block[12]
	mov r9,r6,lsl #16
	mov r9,r9,asr #16		;r9=block[14]
	add r14,r4,r9			;r14=block[12]+block[14]=z0
	
	mov r5,r5,asr #16		;r5=block[13]
	mov r6,r6,asr #16		;r6=block[15]
	add r12,r5,r6,asr #1	;r12=block[13]+block[15]>>1=z3
	
	; verifie
	add r3,r14,r12
	mov r3,r3,lsl #16		;(r3)PF=Temp[12]
	sub r10,r14,r12			;(r10)Pf=Temp[15]
	
	mov r10,r10,lsl #16
	mov r10,r10,lsr #16
	
	;verifie
	sub r14,r4,r9			;r14=block[12]-block[14]=z1
	rsb r12,r6,r5,asr #1	;r12=block[13]>>1-block[15]=z2

	; verifie
	add r5,r14,r12			;r5=Temp[13]
	mov r5,r5,lsl #16
	mov r5,r5,lsr #16
	orr r3,r5,r3				
	sub r5,r14,r12			;r5=Temp[14]	
	orr r10,r10,r5,lsl #16

	
	;r3=Temp[12]:Temp[13]	r10=Temp[14]:Temp[15]
	
	; verifie
	mov r3,r3,ror #16
	mov r10,r10,ror #16
	
	mov r4,r10
	mov r10,r11
	mov r11,r4
	
	mov r4,r3
	mov r3,r7
	mov r7,r4
	
	mov r4,r10
	mov r10,r7
	mov r7,r4
	
	stmia r1,{r3,r7,r10,r11}  ;OK until here
	
	ldmfd r13!,{r0-r12}
	ldmfd r13!,{r14}

	mov pc,lr
	endp
	
	
	
	
ict_Calcul_2emebcle_arm proc

	stmfd r13!,{r14}
	stmfd r13!,{r0-r12}
	
	mov r3,r0
	stmfd r13!,{r0}
	
	ldr r4,[r3],#8			;r4 = Temp[1]:Temp[0]
	ldr r5,[r3],#8			;r5 = Temp[5]:Temp[4]
	ldr r6,[r3],#8			;r6 = Temp[9]:Temp[8]
	ldr r7,[r3]				;r7 = Temp[13]:Temp[12]
	
	mov r3,r4,lsl #16		
	mov r3,r3,asr #16		;r3 = Temp[0]
	mov r8,r6,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[8]
	sub r9,r3,r8			;r9 = Temp[0]-Temp[8] = z1
	
	mov r3,r5,lsl #16
	mov r3,r3,asr #16		;r3 = Temp[4]
	mov r8,r7,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[12]
	rsb r8,r8,r3,asr #1		;r8 = Temp[4]>>1-Temp[12] = z2

	add r11,r8,r9			;r11 = z2+z1
	mov r11,r11,asr #6
	
	ldr r3,[r1,r2]			;#4(PicWidthInPix) is replaced by r2
	mov r3,r3,lsl #24
	mov r3,r3,lsr #24
	add r11,r3,r11
	
	cmp r11,#0xff
	movpl r11,#0xff
	cmp r11,#0x00
	movmi r11,#0x00			;(r11)7::0 = image[4]'
	
	sub r12,r9,r8
	mov r12,r12,asr #6
	
	ldr r3,[r1,r2,lsl #1]	;#8 is replaced by 'r2,lsl #1'
	mov r3,r3,lsl #24
	mov r3,r3,lsr #24
	add r12,r3,r12
	
	cmp r12,#0xff
	movpl r12,#0xff
	cmp r12,#0x00
	movmi r12,#0x00			;(r12)7::0 = image[8]'		
	
	mov r3,r4,asr #16		;r3 = Temp[1]
	mov r8,r6,asr #16		;r8 = Temp[9]
	sub r9,r3,r8			;r9 = Temp[1]-Temp[9] = z1
	
	mov r3,r5,asr #16		;r3 = Temp[5]
	mov r8,r7,asr #16		;r8 = Temp[13]
	rsb r8,r8,r3,asr #1		;r8 = Temp[5]>>1-Temp[13] = z2
	
	add r0,r8,r9			;r0 = z2+z1
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2]
	mov r3,r3,lsl #16
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[5]'
	
	
	mov r0,r0,lsl #8
	orr r11,r0,r11			;(r11)15::0 = image[5]':image[4]'
	
	sub r0,r9,r8			;r0 = z1-z2
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2,lsl #1]
	mov r3,r3,lsl #16
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00
	
	mov r0,r0,lsl #8
	orr r12,r0,r12			;(r12)15::0 =image[9]':image[8]'
	
	mov r3,r4,lsl #16		
	mov r3,r3,asr #16		;r3 = Temp[0]
	mov r8,r6,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[8]
	add r9,r3,r8			;r9 = Temp[0]+Temp[8] = z0
	
	mov r3,r5,lsl #16
	mov r3,r3,asr #16		;r3 = Temp[4]
	mov r8,r7,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[12]
	add r8,r3,r8,asr #1		;r8 = Temp[4]+Temp[12]>>1 = z3
	
	add r10,r8,r9			;r10 = z3+z0
	mov r10,r10,asr #6
	
	ldr r3,[r1]
	mov r3,r3,lsl #24
	mov r3,r3,lsr #24
	add r10,r3,r10
	
	cmp r10,#0xff
	movpl r10,#0xff
	cmp r10,#0x00
	movmi r10,#0x00			;(r10)7::0 = image[0]'

	sub r14,r9,r8			;r14 = z0-z3
	mov r14,r14,asr #6
	
	add r8,r2,r2,lsl #1		;r8=3*PicWidthInPix
	ldr r3,[r1,r8]
	mov r3,r3,lsl #24
	mov r3,r3,lsr #24
	add r14,r3,r14
	
	cmp r14,#0xff
	movpl r14,#0xff
	cmp r14,#0x00
	movmi r14,#0x00			;(r14)7::0 = image[12]'	
	
	mov r3,r4,asr #16		;r3 = Temp[1]
	mov r8,r6,asr #16		;r8 = Temp[9]
	add r9,r3,r8			;r9 = Temp[1]+Temp[9] = z0
	
	mov r3,r5,asr #16		;r3 = Temp[5]
	mov r8,r7,asr #16		;r8 = Temp[13]
	add r8,r3,r8,asr #1		;r8 = Temp[5]+Temp[13>>1] = z3
	
	add r0,r8,r9			;r0 = z3+z0
	mov r0,r0,asr #6
	
	ldr r3,[r1]
	mov r3,r3,lsl #16
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[1]'
	
	mov r0,r0,lsl #8
	orr r10,r0,r10			;(r10)15::0 = image[1]':image[0]'
	
	sub r0,r9,r8			;r0 = z0-z3
	mov r0,r0,asr #6
	
	add r8,r2,r2,lsl #1		;r8=3*PicWidthInPix
	ldr r3,[r1,r8]
	mov r3,r3,lsl #16
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00
	
	mov r0,r0,lsl #8
	orr r14,r0,r14			;(r14)15::0 =image[13]':image[12]'
	
	ldmfd r13!,{r0}
	
	ldr r4,[r0,#4]			;r4 = Temp[3]:Temp[2]
	add r0,r0,#12
	ldr r5,[r0],#8			;r5 = Temp[7]:Temp[6]
	ldr r6,[r0],#8			;r6 = Temp[11]:Temp[10]
	ldr r7,[r0]				;r7 = Temp[15]:Temp[14]
	
	mov r3,r4,lsl #16		
	mov r3,r3,asr #16		;r3 = Temp[2]
	mov r8,r6,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[10]
	sub r9,r3,r8			;r9 = Temp[2]-Temp[10] = z1
	
	mov r3,r5,lsl #16
	mov r3,r3,asr #16		;r3 = Temp[6]
	mov r8,r7,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[14]
	rsb r8,r8,r3,asr #1		;r8 = Temp[6]>>1-Temp[14] = z2

	add r0,r8,r9			;r0 = z2+z1
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2]
	mov r3,r3,lsl #8
	mov r3,r3,lsr #24
	add r0,r3,r0
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0) = image[6]'
	
	mov r0,r0,lsl #16
	orr r11,r0,r11			;(r11)23::0 = image[6]':image[5]':image[4]'
	
	sub r0,r9,r8
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2,lsl #1]
	mov r3,r3,lsl #8
	mov r3,r3,lsr #24
	add r0,r3,r0
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[10]'	
	
	mov r0,r0,lsl #16
	orr r12,r0,r12			;(r12)23::0 = image[10]':image[9]':image[8]'	
	
	mov r3,r4,asr #16		;r3 = Temp[3]
	mov r8,r6,asr #16		;r8 = Temp[11]
	sub r9,r3,r8			;r9 = Temp[3]-Temp[11] = z1
	
	mov r3,r5,asr #16		;r3 = Temp[7]
	mov r8,r7,asr #16		;r8 = Temp[15]
	rsb r8,r8,r3,asr #1		;r8 = Temp[7]>>1-Temp[15] = z2
	
	add r0,r8,r9			;r0 = z2+z1
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2]
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[7]'
	
	mov r0,r0,lsl #24
	orr r11,r0,r11			;(r11)31::0 = image[7]:image[6]:image[5]':image[4]'
	
	
	sub r0,r9,r8			;r0 = z1-z2
	mov r0,r0,asr #6
	
	ldr r3,[r1,r2,lsl #1]
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00
	
	mov r0,r0,lsl #24
	orr r12,r0,r12			;(r12)15::0 =image[11]:image[10]:image[9]':image[8]'
	
	mov r3,r4,lsl #16		
	mov r3,r3,asr #16		;r3 = Temp[2]
	mov r8,r6,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[10]
	add r9,r3,r8			;r9 = Temp[2]+Temp[10] = z0
	
	mov r3,r5,lsl #16
	mov r3,r3,asr #16		;r3 = Temp[6]
	mov r8,r7,lsl #16
	mov r8,r8,asr #16		;r8 = Temp[14]
	add r8,r3,r8,asr #1		;r8 = Temp[6]+Temp[14]>>1 = z3
	
	add r0,r8,r9			;r0 = z3+z0
	mov r0,r0,asr #6
	
	ldr r3,[r1]
	mov r3,r3,lsl #8
	mov r3,r3,lsr #24
	add r0,r3,r0
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[2]'
	
	
	mov r0,r0,lsl #16
	orr r10,r0,r10			;(r10)23::0 = image[2]':image[1]':image[0]'
	
	sub r0,r9,r8			;r0 = z0-z3
	mov r0,r0,asr #6
	
	add r8,r2,r2,lsl #1		;r8=3*PicWidthInPix
	ldr r3,[r1,r8]
	mov r3,r3,lsl #8
	mov r3,r3,lsr #24
	add r0,r3,r0
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[14]'
	mov r0,r0,lsl #16
	orr r14,r0,r14			;(r14)23::0 = image[14]':image[13]':image[12]'
	
	mov r3,r4,asr #16		;r3 = Temp[3]
	mov r8,r6,asr #16		;r8 = Temp[11]
	add r9,r3,r8			;r9 = Temp[3]+Temp[11] = z0
	
	mov r3,r5,asr #16		;r3 = Temp[7]
	mov r8,r7,asr #16		;r8 = Temp[15]
	add r8,r3,r8,asr #1		;r8 = Temp[7]+Temp[15>>1] = z3
	
	add r0,r8,r9			;r0 = z3+z0
	mov r0,r0,asr #6
	
	
	ldr r3,[r1]
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00			;r0 = image[3]'
	
	mov r0,r0,lsl #24
	orr r10,r0,r10			;(r10)31::0 = image[3]':image[2]':image[1]':image[0]'
	
	sub r0,r9,r8			;r0 = z0-z3
	mov r0,r0,asr #6
	
	add r8,r2,r2,lsl #1		;r8=3*PicWidthInPix
	ldr r3,[r1,r8]
	mov r3,r3,lsr #24
	add r0,r0,r3
	
	cmp r0,#0xff
	movpl r0,#0xff
	cmp r0,#0x00
	movmi r0,#0x00
	
	mov r0,r0,lsl #24
	orr r14,r0,r14			;(r14)31::0 =image[15]':image[14]':image[13]':image[12]'
	
	
	str r10,[r1],r2
	str r11,[r1],r2
	str r12,[r1],r2
	str r14,[r1]
	;stmia r1,{r10,r11,r12,r14}
	
	ldmfd r13!,{r0-r12}
	ldmfd r13!,{r14}
	mov pc,lr
	endp
	
	LTORG
	END
	
