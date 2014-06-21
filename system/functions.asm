[bits 32]

	global _Hlt
	global _Cli, _Sti
	global _Input8, _Output8
	global _LoadEflags, _StoreEflags
	global _LoadGdtr, _LoadIdtr
	global _LoadCr0, _StoreCr0
	global _LoadTr
	;global _AsmIntHandler07
	global _AsmIntHandler20
	global _AsmIntHandler21, _AsmIntHandler2c
	global _AsmIntHandler27
	global _MemoryTestSub
	global _FarJump
	;extern _IntHandler07
	extern _IntHandler20
	extern _IntHandler21, _IntHandler2c

[section .text]

_Hlt:
	hlt
	ret

_Cli:
	cli
	ret

_Sti:
	sti
	ret

_Input8:
	MOV	EDX,[ESP+4]	; port
	MOV	EAX,0
	IN	AL,DX
	RET

_Output8:
	MOV	EDX,[ESP+4]	; port
	MOV	AL,[ESP+8]	; data
	OUT	DX,AL
	RET

_LoadEflags:
	PUSHFD
	POP		EAX
	RET

_StoreEflags:
	MOV		EAX,[ESP+4]
	PUSH	EAX
	POPFD
	RET

_LoadGdtr:
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LGDT	[ESP+6]
	RET

_LoadIdtr:
	MOV		AX,[ESP+4]		; limit
	MOV		[ESP+6],AX
	LIDT	[ESP+6]
	RET

_LoadCr0:
	MOV		EAX,CR0
	RET

_StoreCr0:
	MOV		EAX,[ESP+4]
	MOV		CR0,EAX
	RET

_LoadTr:
	LTR		[ESP+4]			; tr
	RET

;_AsmIntHandler07:
;	PUSH	ES
;	PUSH	DS
;	PUSHAD
;	MOV	 EAX,ESP
;	PUSH	EAX
;	MOV	 AX,SS
;	MOV	 DS,AX
;	MOV	 ES,AX
;	CALL	_IntHandler07
;	POP	 EAX
;	POPAD
;	POP	 DS
;	POP	 ES
;	IRETD

_AsmIntHandler20:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	_IntHandler20
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

_AsmIntHandler21:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	_IntHandler21
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

_AsmIntHandler27:
	MOV		EDX,0x0020
	MOV		AL,0x67
	OUT		DX,AL			; IRQ-07受付完了をPICに通知
	IRETD

_AsmIntHandler2c:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL	_IntHandler2c
	pop eax
	POPAD
	POP DS
	POP ES
	IRETD

_MemoryTestSub:
	PUSH	EDI						; （EBX, ESI, EDI も使いたいので）
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

_FarJump:
	jmp		far [esp+4]
	ret
