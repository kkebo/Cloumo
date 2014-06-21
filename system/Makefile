objs_bootpack = bootpack.obj mouse.obj keyboard.obj int.obj descriptor.obj file.obj memory.obj browser.obj sound.obj queue.obj multitask.obj timer.obj datetime.obj graphic.obj tek.obj utf82kt.obj functions.obj alloca.obj

ifeq ($(OS),Windows_NT)
	TOOLPATH = ../../z_tools_win/
	INCLUDE = ../../z_tools_win/include/
	make     = $(TOOLPATH)make.exe -r
	bim2hrb  = $(TOOLPATH)bim2hrb.exe
	cpp      = $(TOOLPATH)gcc.exe -I$(INCPATH) -Os -Wall -nostdlib -fno-builtin -fno-exceptions -fno-rtti -B$(TOOLPATH) -mno-stack-arg-probe -c
	obj2bim  = $(TOOLPATH)obj2bim.exe
	nask     = $(TOOLPATH)nask.exe
	os.sys   = copy /B asmhead.bin+bootpack.hrb os.sys
	del      = -del
else
	TOOLPATH = ../../z_tools/
	INCLUDE = ../../z_tools/include/
	make     = make -r
	bim2hrb  = $(TOOLPATH)bim2hrb
	cpp      = $(TOOLPATH)gocc1plus -I$(INCLUDE) -Os -Wall -quiet -fno-exceptions
	obj2bim  = $(TOOLPATH)obj2bim
	objconv  = $(TOOLPATH)objconv
	nask     = $(TOOLPATH)nask
	g2n      = $(TOOLPATH)gas2nask -a
	os.sys   = cat asmhead.bin bootpack.hrb > os.sys
	del      = rm -f
endif
rulefile = $(INCLUDE)os.rul

# デフォルトの設定
default :
	$(make) os.sys
	$(make) ipl.bin

# 個別の設定
bootpack.bim : $(objs_bootpack) jpeg.obj bmp.obj
	$(obj2bim) @$(rulefile) $(objs_bootpack) jpeg.obj bmp.obj out:bootpack.bim map:bootpack.map stack:3136k

bootpack.hrb : bootpack.bim
	$(bim2hrb) bootpack.bim bootpack.hrb 0

os.sys : asmhead.bin bootpack.hrb
	$(os.sys)

# 一般の設定
%.obj : %.asm
	nasm -fwin32 -o $*.obj $*.asm

%.s : %.cpp
	$(cpp) $*.cpp -o $*.s

%.nas : %.s
	$(g2n) $*.s $*.nas

%.obj : %.nas
	$(nask) $*.nas $*.obj

%.bin : %.asm
	nasm -fbin -o $*.bin $*.asm

# 生成物を削除
clean :
	$(del) *.map
	$(del) *.bin
	$(del) $(objs_bootpack)
	$(del) *.bim
	$(del) *.hrb
	$(del) os.sys
