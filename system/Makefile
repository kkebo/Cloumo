objs_bootpack = bootpack.obj mouse.obj keyboard.obj int.obj descriptor.obj file.obj memory.obj browser.obj sound.obj queue.obj multitask.obj timer.obj datetime.obj graphic.obj tek.obj utf82kt.obj functions.obj alloca.obj

TOOLPATH = ../../z_tools/
INCLUDE = ../../z_tools/include/

bim2hrb  = $(TOOLPATH)bim2hrb
cpp      = $(TOOLPATH)gocc1plus -I$(INCLUDE) -Os -Wall -quiet -fno-exceptions
obj2bim  = $(TOOLPATH)obj2bim
objconv  = $(TOOLPATH)objconv
nask     = $(TOOLPATH)nask
g2n      = $(TOOLPATH)gas2nask -a
rulefile = $(INCLUDE)os.rul

# 

default :
	make -r os.sys
	make -r ipl.bin

# 個別の設定

bootpack.bim : $(objs_bootpack) jpeg.obj bmp.obj Makefile
	$(obj2bim) @$(rulefile) $(objs_bootpack) jpeg.obj bmp.obj out:bootpack.bim map:bootpack.map stack:3136k

bootpack.hrb : bootpack.bim Makefile
	$(bim2hrb) bootpack.bim bootpack.hrb 0

os.sys : asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > os.sys

# 一般の設定

%.obj : %.asm Makefile
	nasm -fwin32 -o $*.obj $*.asm

%.s : %.cpp Makefile
	$(cpp) $*.cpp -o $*.s

%.nas : %.s Makefile
	$(g2n) $*.s $*.nas

%.obj : %.nas Makefile
	$(nask) $*.nas $*.obj

%.bin : %.asm Makefile
	nasm -fbin -o $*.bin $*.asm

# 

clean :
	rm -f *.map
	rm -f *.bin
	rm -f $(objs_bootpack)
	rm -f *.bim
	rm -f *.hrb
	rm -f os.sys
