
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              klib.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                       Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


%include "sconst.inc"

; 导入全局变量
extern	disp_pos


[SECTION .text]

; 导出函数
global	disp_str
global	disp_color_str
global	out_byte
global	in_byte
global 	disp_mode
global	enable_irq
global	disable_irq
global	enable_int
global	disable_int
global	asm_writepix

;========================================================================
;	void disp_mode(char mode)
;========================================================================

disp_mode:
	push	eax
;	push	ebx
	mov 	ah,0fh
	int	10h
;	pop 	ebx
	pop	eax
	ret

; ========================================================================
;                  void disp_str(char * info);
; ========================================================================
disp_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2

	cmp	edi, 80 * 25 * 2
	jb	.1
	
	xor	edi, edi
	jmp	.1

.2:
	mov	[disp_pos], edi

	pop	ebp
	ret

; ========================================================================
;                  void disp_color_str(char * info, int color);
; ========================================================================
disp_color_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [disp_pos]
	mov	ah, [ebp + 12]	; color
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[disp_pos], edi

	pop	ebp
	ret

; ========================================================================
;                  void out_byte(t_port port, t_8 value);
; ========================================================================
out_byte:
	mov	edx, [esp + 4]		; port
	mov	al, [esp + 4 + 4]	; value
	out	dx, al
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  t_8 in_byte(t_port port);
; ========================================================================
in_byte:
	mov	edx, [esp + 4]		; port
	xor	eax, eax
	in	al, dx
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  void disable_irq(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code for irq < 8:
;       out_byte(INT_CTLMASK, in_byte(INT_CTLMASK) | (1 << irq));
; Returns true iff the interrupt was not already disabled.
;
disable_irq:
	mov	ecx, [esp + 4]		; irq
	pushf
	cli
	mov	ah, 1
	rol	ah, cl			; ah = (1 << (irq % 8))
	cmp	cl, 8
	jae	disable_8		; disable irq >= 8 at the slave 8259
disable_0:
	in	al, INT_M_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_M_CTLMASK, al	; set bit at master 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
disable_8:
	in	al, INT_S_CTLMASK
	test	al, ah
	jnz	dis_already		; already disabled?
	or	al, ah
	out	INT_S_CTLMASK, al	; set bit at slave 8259
	popf
	mov	eax, 1			; disabled by this function
	ret
dis_already:
	popf
	xor	eax, eax		; already disabled
	ret

; ========================================================================
;                  void enable_irq(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;	if(irq < 8){
;		out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1 << irq));
;	}
;	else{
;		out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1 << irq));
;	}
;
enable_irq:
        mov	ecx, [esp + 4]		; irq
        pushf
        cli
        mov	ah, ~1
        rol	ah, cl			; ah = ~(1 << (irq % 8))
        cmp	cl, 8
        jae	enable_8		; enable irq >= 8 at the slave 8259
enable_0:
        in	al, INT_M_CTLMASK
        and	al, ah
        out	INT_M_CTLMASK, al	; clear bit at master 8259
        popf
        ret
enable_8:
        in	al, INT_S_CTLMASK
        and	al, ah
        out	INT_S_CTLMASK, al	; clear bit at slave 8259
        popf
        ret

; ========================================================================
;                  void disable_int();
; ========================================================================
disable_int:
	cli
	ret

; ========================================================================
;                  void enable_int();
; ========================================================================
enable_int:
	sti
	ret

; ========================================================================
;                  void asm_writepix(int x, int y, int color);
; ========================================================================
asm_writepix:
	mov bx,[esp+4]
	mov cx,[esp+8]
	mov dx,[esp+12]
	pushf
;=============
	push dx;color压栈
	push bx;x压栈
	
	xor eax,eax
	shr bx,3
	mov ax,50h
	mul cx
	add ax,bx
	mov edi,eax
	pop cx
	and cl,7
	mov ax,80h
	shr al,cl
	xor cx,cx
	mov cl,al
	;bx=80*y+x>>3
	
	mov dx, 03c4h
	mov al,2
	out dx,al
	inc dx
	mov al,0fh
	out dx,al
	;位面全开
		
	mov dx,03eh
	mov al,05h
	out dx,al
	inc dx
	mov al,02h
	out dx,al
	;写模式2
	
	mov dx,3ceh
	mov al,08h
	out dx,al
	mov al,cl
	inc dx
	out dx,al
	
	mov ah,[gs:edi]
	pop ax
	mov [gs:edi],al
;=============
	popf
	ret

