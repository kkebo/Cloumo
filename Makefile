OBJS = \
	system/bootpack.obj \
	system/int.obj \
	system/descriptor.obj \
	system/memory.obj \
	system/browser.obj \
	system/queue.obj \
	system/multitask.obj \
	system/timer.obj \
	system/datetime.obj \
	system/graphic.obj \
	system/tek.obj \
	system/utf82kt.obj \
	system/sysinfo.obj \
	system/functions.obj \
	system/alloca.obj \
	driver/file.obj \
	driver/keyboard.obj \
	driver/mouse.obj \
	driver/sound.obj

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../z_tools_win/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = $(TOOLPATH)make.exe -r
	BIM2HRB  = $(TOOLPATH)bim2hrb.exe
	CXX      =
	CXXFLAGS =
	OBJ2BIM  = $(TOOLPATH)obj2bim.exe
	NASK     = $(TOOLPATH)nask.exe
	NASM     =
	G2N      =
	EDIMG    = $(TOOLPATH)edimg.exe
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
	os.sys   = copy /B system/asmhead.bin+bootpack.hrb os.sys
else
# OS X
	TOOLPATH = ../z_tools/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = make -r
	BIM2HRB  = $(TOOLPATH)bim2hrb
	CXX      = $(TOOLPATH)gocc1plus
	CXXFLAGS = -I$(INCPATH) -Os -Wall -quiet -fno-exceptions
	OBJ2BIM  = $(TOOLPATH)obj2bim
	NASK     = $(TOOLPATH)nask
	NASM     = nasm
	G2N      = $(TOOLPATH)gas2nask -a
	EDIMG    = $(TOOLPATH)edimg
	QEMU     = qemu-system-x86_64 -vga std
	DEL      = rm -f
	os.sys   = cat system/asmhead.bin bootpack.hrb > os.sys
endif

RULEFILE = $(INCPATH)os.rul

# Default

all:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C driver
	$(MAKE) -C system
	$(MAKE) cloumo.img

# 特別生成規則

bootpack.bim: $(OBJS) system/jpeg.obj system/bmp.obj
	$(OBJ2BIM) @$(RULEFILE) $(OBJS) system/jpeg.obj system/bmp.obj out:bootpack.bim map:bootpack.map stack:3136k

bootpack.hrb: bootpack.bim
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

os.sys: system/asmhead.bin bootpack.hrb
	$(os.sys)

cloumo.img: system/ipl.bin os.sys images/b_f.bmp images/btn_r.bmp \
		images/copy.bmp images/source.bmp images/search.bmp images/refresh.bmp
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:system/ipl.bin len:512 from:0 to:0 \
		copy from:os.sys to:@: \
		copy from:fonts/japanese.fnt to:@: \
		copy from:html/index.htm to:@: \
		copy from:images/b_f.bmp to:@: \
		copy from:images/btn_r.bmp to:@: \
		copy from:images/copy.bmp to:@: \
		copy from:images/source.bmp to:@: \
		copy from:images/search.bmp to:@: \
		copy from:images/refresh.bmp to:@: \
		imgout:cloumo.img

# Options

run:
	$(MAKE) all
	$(QEMU) -m 32 -localtime -soundhw all -fda cloumo.img -L .

refresh:
	$(MAKE) all
	$(MAKE) clean

clean:
	$(DEL) bootpack.map
	$(DEL) bootpack.bim
	$(DEL) bootpack.hrb
	$(DEL) os.sys
	$(DEL) cloumo.img
	$(MAKE) -C system clean
	$(MAKE) -C driver clean
