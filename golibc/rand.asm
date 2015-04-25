; rand -  K&Rからほぼ抜粋したものを単にASM化した

[BITS 32]
GLOBAL rand_seed
GLOBAL rand

[SECTION .data]

			ALIGNB	4
rand_seed	dd	1

[SECTION .text]

; int rand(void)

rand:

	push	edx
	mov	eax,dword [ds:rand_seed]
	mov	edx,1103515245
	mul	edx
	add	eax,12345
	mov	dword [ds:rand_seed],eax
	shr	eax,16
	pop	edx
	and	eax,07fffh
	ret
