[bits 32]

		GLOBAL	__alloca

[SECTION .text]

__alloca:
		ADD		EAX,-4
		SUB		ESP,EAX
		JMP		DWORD [ESP+EAX]		; RETの代わり
