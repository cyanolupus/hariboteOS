; naskfunc
; TAB=4

[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "naskfunc.nas"]

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_memtest_main
		EXTERN	_inthandler21, _inthandler27, _inthandler2c

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI 
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS
		RET

_load_gdtr:			; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET
	
_load_idtr:			; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asm_inthandler21:	; void asm_inthandler21(void);
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:	; void asm_inthandler2c(void);
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:	; void asm_inthandler27(void);
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_load_cr0: ; int load_cr0();
		MOV		EAX,CR0
		RET

_store_cr0: ; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_memtest_main: ; unsigned int memtest_main(unsigned int start, unsigned int end);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0x55aa55aa ; pat0
		MOV		EDI,0xaa55aa55 ; pat1
		MOV		EAX,[ESP+12+4] ; i
memtest_loop:
		MOV		EBX,EAX ; p
		ADD		EBX,0xffc
		MOV		EDX,[EBX] ; old
		MOV		[EBX],ESI
		XOR		DWORD [EBX],0xffffffff ; reverse
		CMP		EDI,[EBX]
		JNE		memtest_fin
		XOR 	DWORD [EBX],0xffffffff ; reverse2
		CMP 	ESI,[EBX]
		JNE		memtest_fin
		MOV		[EBX],EDX
		ADD		EAX,0x1000
		CMP 	EAX,[ESP+12+8]
		JBE		memtest_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
memtest_fin:
		MOV		[EBX],EDX
		POP		EBX
		POP		ESI
		POP		EDI
		RET
