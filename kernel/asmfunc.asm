[bits 32]

	global Hlt
	global Cli, Sti
	global Input8, Output8
	global Input16, Output16
	global Input32, Output32
	global LoadEflags, StoreEflags
	global LoadGdtr, LoadIdtr
	global LoadCr0, StoreCr0
	global LoadTr
	global AsmIntHandler04
	global AsmIntHandler07
	global AsmIntHandler0d
	global AsmIntHandler20
	global AsmIntHandler21, AsmIntHandler2c
	global AsmIntHandler27
	global MemoryTestSub
	global FarJump
	extern IntHandler04
	extern IntHandler07
	extern IntHandler0d
	extern IntHandler20
	extern IntHandler21, IntHandler2c

[section .text]

Hlt:
	hlt
	ret

Cli:
	cli
	ret

Sti:
	sti
	ret

Input8:
	MOV	EDX,[ESP+4]	; port
	MOV	EAX,0
	IN	AL,DX
	RET

Output8:
	MOV	EDX,[ESP+4]	; port
	MOV	AL,[ESP+8]	; data
	OUT	DX,AL
	RET

Input16:
	MOV	EDX,[ESP+4]	; port
	MOV	EAX,0
	IN	AX,DX
	RET

Output16:
	MOV	EDX,[ESP+4]	; port
	MOV	AX,[ESP+8]	; data
	OUT	DX,AX
	RET

Input32:
	MOV	EDX,[ESP+4]	; port
	MOV	EAX,0
	IN	EAX,DX
	RET

Output32:
	MOV	EDX,[ESP+4]	; port
	MOV	EAX,[ESP+8]	; data
	OUT	DX,EAX
	RET

LoadEflags:
	PUSHFD
	POP		EAX
	RET

StoreEflags:
	MOV		EAX,[ESP+4]
	PUSH	EAX
	POPFD
	RET

LoadGdtr:
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LGDT	[ESP+6]
	RET

LoadIdtr:
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LIDT	[ESP+6]
	RET

LoadCr0:
	MOV		EAX,CR0
	RET

StoreCr0:
	MOV		EAX,[ESP+4]
	MOV		CR0,EAX
	RET

LoadTr:
	LTR		[ESP+4]			; tr
	RET

AsmIntHandler04:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	push	ss
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	IntHandler04
	add		esp,8
	POPAD
	POP		DS
	POP		ES
	add		esp,4
	IRETD

AsmIntHandler07:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV	 EAX,ESP
	PUSH	EAX
	MOV	 AX,SS
	MOV	 DS,AX
	MOV	 ES,AX
	CALL	IntHandler07
	POP	 EAX
	POPAD
	POP	 DS
	POP	 ES
	IRETD

AsmIntHandler0d:
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	push	ss
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	IntHandler0d
	add		esp,8
	POPAD
	POP		DS
	POP		ES
	add		esp,4
	IRETD

AsmIntHandler20:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	IntHandler20
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

AsmIntHandler21:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	IntHandler21
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

AsmIntHandler27:
	MOV		EDX,0x0020
	MOV		AL,0x67
	OUT		DX,AL			; IRQ-07���t������PIC�ɒʒm
	IRETD

AsmIntHandler2c:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	IntHandler2c
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

MemoryTestSub:
	PUSH	EDI						; �iEBX, ESI, EDI ���g�������̂Łj
	PUSH	ESI
	PUSH	EBX
	MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
	MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
	MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
	MOV		EBX,EAX
	ADD		EBX,0xffc				; p = i + 0xffc;
	MOV		EDX,[EBX]				; old = *p;
	MOV		[EBX],ESI				; *p = pat0;
	XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
	CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
	JNE		mts_fin
	XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
	CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
	JNE		mts_fin
	MOV		[EBX],EDX				; *p = old;
	ADD		EAX,0x1000				; i += 0x1000;
	CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
	JBE		mts_loop
	POP		EBX
	POP		ESI
	POP		EDI
	RET
mts_fin:
	MOV		[EBX],EDX				; *p = old;
	POP		EBX
	POP		ESI
	POP		EDI
	RET

FarJump:
	jmp		far [esp+4]
	ret
