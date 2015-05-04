[org 0x7c00]

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

		jmp		short entry
		db		0x90
		db		"CLOUMO  "		; ブートセクタの名前を自由に書いてよい（8バイト）
		dw		512				; 1セクタの大きさ（512にしなければいけない）
		db		1				; クラスタの大きさ（1セクタにしなければいけない）
		dw		1				; FATがどこから始まるか（普通は1セクタ目からにする）
		db		2				; FATの個数（2にしなければいけない）
		dw		224				; ルートディレクトリ領域の大きさ（普通は224エントリにする）
		dw		2880			; このドライブの大きさ（2880セクタにしなければいけない）
		db		0xf0			; メディアのタイプ（0xf0にしなければいけない）
		dw		9				; FAT領域の長さ（9セクタにしなければいけない）
		dw		18				; 1トラックにいくつのセクタがあるか（18にしなければいけない）
		dw		2				; ヘッドの数（2にしなければいけない）
		DD		0				; パーティションを使ってないのでここは必ず0
		DD		2880			; このドライブ大きさをもう一度書く
		db		0,0,0x29		; よくわからないけどこの値にしておくといいらしい
		DD		0xffffffff		; たぶんボリュームシリアル番号
		db		"CLOUMO      "	; ディスクの名前（11バイト）
		db		"FAT12   "		; フォーマットの名前（8バイト）
		times	18 db 0			; とりあえず18バイトあけておく

; プログラム本体

entry:
		mov		ax,0			; レジスタ初期化
		mov		ss,ax
		mov		sp,0x7c00
		mov		ds,ax
		call	startingmsg

; ディスクを読む

		mov		ax,0x0820
		mov		es,ax
		mov		ch,0			; シリンダ0
		mov		dh,0			; ヘッド0
		mov		cl,2			; セクタ2
		mov		bx,18*2-1		; 読み込みたい合計セクタ数 (2006.12.23 バグ修正)
		call	readfast		; 高速読み込み

		mov		bx,4312+0x8200+2
fatloop:
		add		bx,-2
		cmp		word [bx],0
		JZ		fatloop
		mov		ax,bx
		add		ax,-0x8200-5+54+53
		mov		bl,54
		DIV		bl				; al = ax / bl

		cmp		al,0
		JNZ		fatskip1
		mov		al,1
fatskip1:
		cmp		al,33
		jbe		fatskip2
		mov		al,33
fatskip2:
		mov		[0x0ff0],al		; IPLがどこまで読んだのかをメモ
		add		al,-1
		JZ		fatskip3
		mov		ah,0
		IMUL	bx,ax,18*2

		mov		ax,0x0c80
		mov		es,ax
		mov		ch,1			; シリンダ1
		mov		dh,0			; ヘッド0
		mov		cl,1			; セクタ1
		call	readfast		; 高速読み込み
fatskip3:

; 読み終わったのでos.sysを実行だ！

		jmp		0xc200

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
		mov		si,msg
		call	putloop
fin:
		hlt						; 何かあるまでCPUを停止させる
		jmp		fin				; 無限ループ
msg:
		db		0x0a, 0x0a		; 改行を2つ
		db		"load error"
		db		0x0a			; 改行
		db		0

return:
		ret

msg_starting:
		db		"Starting Cloumo"
		db		0
startingmsg:
		mov		si,msg_starting
		call	putloop
		ret

readfast:	; alを使ってできるだけまとめて読み出す
;	es:読み込み番地, ch:シリンダ, dh:ヘッド, cl:セクタ, bx:読み込みセクタ数

		mov		ax,es			; < esからalの最大値を計算 >
		shl		ax,3			; axを32で割って、その結果をahに入れたことになる （shlは左シフト命令）
		and		ah,0x7f			; ahはahを128で割った余り（512*128=64K）
		mov		al,128			; al = 128 - ah; 一番近い64KB境界まで最大何セクタ入るか
		sub		al,ah

		mov		ah,bl			; < bxからalの最大値をahに計算 >
		cmp		bh,0			; if (bh != 0) { ah = 18; }
		je		.skip1
		mov		ah,18
.skip1:
		cmp		al,ah			; if (al > ah) { al = ah; }
		jbe		.skip2
		mov		al,ah
.skip2:

		mov		ah,19			; < clからalの最大値をahに計算 >
		sub		ah,cl			; ah = 19 - cl;
		cmp		al,ah			; if (al > ah) { al = ah; }
		jbe		.skip3
		mov		al,ah
.skip3:

		push	bx
		mov		si,0			; 失敗回数を数えるレジスタ
retry:
		mov		ah,0x02			; ah=0x02 : ディスク読み込み
		mov		bx,0
		mov		dl,0x00			; Aドライブ
		push	es
		push	dx
		push	cx
		push	ax
		int		0x13			; ディスクBIOS呼び出し
		jnc		next			; エラーがおきなければnextへ
		add		si,1			; siに1を足す
		cmp		si,5			; siと5を比較
		jae		error			; si >= 5 だったらerrorへ
		mov		ah,0x00
		mov		dl,0x00			; Aドライブ
		int		0x13			; ドライブのリセット
		pop		ax
		pop		cx
		pop		dx
		pop		es
		jmp		retry
next:
		pop		ax
		pop		cx
		pop		dx
		pop		bx				; esの内容をbxで受け取る
		shr		bx,5			; bxを16バイト単位から512バイト単位へ
		mov		ah,0
		add		bx,ax			; bx += al;
		shl		bx,5			; bxを512バイト単位から16バイト単位へ
		mov		es,bx			; これで es += al * 0x20; になる
		pop		bx
		sub		bx,ax
		JZ		.ret
		add		cl,al			; clにalを足す
		cmp		cl,18			; clと18を比較
		jbe		readfast		; cl <= 18 だったらreadfastへ
		mov		cl,1
		add		dh,1
		cmp		dh,2
		JB		readfast		; dh < 2 だったらreadfastへ
		mov		dh,0
		add		ch,1
		jmp		readfast
.ret:
		ret

		times	0x1fe-($-$$) db 0	; 0x7dfeまでを0x00で埋める命令

		db		0x55, 0xaa
