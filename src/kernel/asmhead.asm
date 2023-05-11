BOTPAK	equ		0x00280000		; bootpackのロード先
DSKCAC	equ		0x00100000		; ディスクキャッシュの場所
DSKCAC0	equ		0x00008000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	equ		0x0ff0			; ブートセクタが設定する
LEDS	equ		0x0ff1
VMODE	equ		0x0ff2			; 色数に関する情報。何ビットカラーか？
SCRNX	equ		0x0ff4			; 解像度のX
SCRNY	equ		0x0ff6			; 解像度のY
VRAM	equ		0x0ff8			; グラフィックバッファの開始番地

[org 0xc200]					; このプログラムがどこに読み込まれるのか

; VBE存在確認
		mov		ax,0x9000
		mov		es,ax
		mov		di,0
		mov		ax,0x4f00
		int		0x10
		cmp		ax,0x004f
		jne		_loop

; VBEのバージョンチェック
		mov		ax,[es:di+4]
		cmp		ax,0x0200
		jb		_loop

; 1600x1200 32/24bit
		mov		cx,0x11F
		mov		dl,32
		call	vbe_video
		cmp		ax,1
		je		keystatus
		mov		dl,24
		call	vbe_video
		cmp		ax,1
		je		keystatus

; 1440x900 32/24bit
		mov		cx,0x163
		mov		dl,32
		call	vbe_video
		cmp		ax,1
		je		keystatus
		mov		dl,24
		call	vbe_video
		cmp		ax,1
		je		keystatus

; 1280x1024, 1024x768, 800x600, 640x480 32/24bit
		mov		cx,0x11B
try_vbe3224:
		mov		dl,32
		call	vbe_video
		cmp		ax,1
		je		keystatus
		mov		dl,24
		call	vbe_video
		cmp		ax,1
		je		keystatus
		sub		cx,3
		cmp		cx,0x112
		jae		try_vbe3224

; 1600x1200 16bit
		mov		cx,0x11E
		mov		dl,16
		call	vbe_video
		cmp		ax,1
		je		keystatus

; 1440x900 16bit
		mov		cx,0x162
		mov		dl,16
		call	vbe_video
		cmp		ax,1
		je		keystatus

; 1280x1024, 1024x768, 800x600, 640x480 16bit
		mov		cx,0x11A
try_vbe16:
		mov		dl,16
		call	vbe_video
		cmp		ax,1
		je		keystatus
		sub		cx,3
		cmp		cx,0x111
		jae		try_vbe16

_loop:
		mov		si,msg_dame
		call	putloop
		hlt
		jmp short $-1

keystatus:
; 画面モード情報
		mov		al,[es:di+0x19]
		mov		[VMODE],al
		mov		ax,[es:di+0x12]
		mov		[SCRNX],ax
		mov		ax,[es:di+0x14]
		mov		[SCRNY],ax
		push	dword [es:di+0x28]
		pop		dword [VRAM]

; キーボードのLED状態をBIOSに教えてもらう

		mov		ah,0x02
		int		0x16 			; keyboard BIOS
		mov		[LEDS],al

; PICが一切の割り込みを受け付けないようにする

		cli
		mov		al,0xff
		out		0x21,al
		nop						; out命令を連続させるとうまくいかない機種があるらしいので
		out		0xa1,al
		sti
		nop
		cli						; さらにCPUレベルでも割り込み禁止

; CPUから1MB以上のメモリにアクセスできるように、A20GATEを設定

		call	waitkbdout
		mov		al,0xd1
		out		0x64,al
		call	waitkbdout
		mov		al,0xdf			; enable A20
		out		0x60,al
		call	waitkbdout

; プロテクトモード移行
		lgdt	[GDTR0]			; 暫定GDTを設定
		mov		eax,cr0
		and		eax,0x7fffffff	; bit31を0にする（ページング禁止のため）
		or		eax,0x00000001	; bit0を1にする（プロテクトモード移行のため）
		mov		cr0,eax
		jmp		pipelineflush
pipelineflush:
		mov		ax,1*8			;  Data Segment Selector
		mov		ds,ax
		mov		es,ax
		mov		fs,ax
		mov		gs,ax
		mov		ss,ax

; bootpackの転送

		mov		esi,bootpack	; 転送元
		mov		edi,BOTPAK		; 転送先
		mov		ecx,512*1024/4
		call	memcpy

; ついでにディスクデータも本来の位置へ転送

; まずはブートセクタから

		mov		esi,0x7c00		; 転送元
		mov		edi,DSKCAC		; 転送先
		mov		ecx,512/4
		call	memcpy

; 残り全部

		mov		esi,DSKCAC0+512	; 転送元
		mov		edi,DSKCAC+512	; 転送先
		mov		ecx,0
		mov		cl,byte [CYLS]
		imul	ecx,512*18*2/4	; シリンダ数からバイト数/4に変換
		sub		ecx,512/4		; IPLの分だけ差し引く
		call	memcpy

; asmheadでしなければいけないことは全部し終わったので、
;    あとはbootpackに任せる

; bootpackの起動

		mov		ebx,BOTPAK
		mov		ecx,[ebx+16]
		add		ecx,3			; ecx += 3;
		shr		ecx,2			; ecx /= 4;
		jz		skip			; 転送するべきものがない
		mov		esi,[ebx+20]	; 転送元
		add		esi,ebx
		mov		edi,[ebx+12]	; 転送先
		call	memcpy
skip:
		mov		esp,[ebx+12]	; スタック初期値
		jmp		dword 2*8:0x0000001b	; Code Segment Selector

vbe_video:
		mov		ax,0x4f01
		int		0x10
		cmp		ax,0x004f
		jne		vbe_failure

		cmp		byte [es:di+0x19],dl
		jne		vbe_failure
		cmp		byte [es:di+0x1b],6
		jne		vbe_failure
		mov		ax,[es:di+0x00]
		and		ax,0x0080
		jz		vbe_failure

		mov		ax,0x4f02
		mov		bx,0x4000
		add		bx,cx
		int		0x10
		mov		ax,1
		ret
vbe_failure:
		mov		ax,0
		ret

waitkbdout:
		in		al,0x64
		and		al,0x02
		jnz		waitkbdout		; ANDの結果が0でなければwaitkbdoutへ
		ret

return:
		ret

ptlp_err_msg:
		db		0x0a, 0x0a		; 改行を2つ
		db		"putloop error"
		db		0x0a			; 改行
		db		0

putloop:
		mov		al,[si]
		add		si,1			; siに1を足す
		cmp		al,0
		je		return
		mov		ah,0x0e			; 一文字表示ファンクション
		mov		bx,15			; カラーコード
		int		0x10			; ビデオBIOS呼び出し
		jmp		putloop
error:
		mov		ax,0
		mov		es,ax
		mov		si,ptlp_err_msg
		call	putloop
fin:
		hlt						; 何かあるまでCPUを停止させる
		jmp		fin				; 無限ループ

msg_dame:
		db		"dame dame dame!!!"
		db		0

memcpy:
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		sub		ecx,1
		jnz		memcpy			; 引き算した結果が0でなければmemcpyへ
		ret
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		align 8, db 0
GDT0:
; Null Descriptor
		times 8 db 0
; Data Segment Descriptor
		dw		0xffff			; Limit 0~15bit (4GB)
		dw		0x0000			; Base Address 0~15bit (0x00000000)
		db		0x00			; Base Address 16~23bit
		db		0x92			; P:1, DPL:0, data, non-conforming, read/write
		db		0xcf			; G:1, D:1, Limit 16~19bit
		db		0x00			; Base Address 24~31bit
; Code Segment Descriptor
		dw		0xffff			; Limit 0~15bit (0x7ffff byte)
		dw		0x0000			; Base Address 0~15bit (0x00280000)
		db		0x28			; Base Address 16~23bit
		db		0x9a			; P:1, DPL:0, code, expand-up, read/exec
		db		0x47			; G:0, D:1, Limit 16~19bit
		db		0x00			; Base Address 24~31bit

		dw		0
GDTR0:
		dw		8*3-1			; GDTのLimit
		dd		GDT0			; GDTのベースアドレス

		align 8, db 0
bootpack:
