OBJS = \
	system/bootpack.o \
	system/int.o \
	system/descriptor.o \
	system/memory.o \
	system/browser.o \
	system/queue.o \
	system/multitask.o \
	system/timer.o \
	system/datetime.o \
	system/graphic.o \
	system/tek.o \
	system/utf82kt.o \
	system/sysinfo.o \
	system/asmfunc.o \
	driver/file.o \
	driver/keyboard.o \
	driver/mouse.o \
	driver/sound.o

GOLIBC = golibc/golibc.a

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../z_tools_win/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = $(TOOLPATH)make.exe -r
	LD       =
	EDIMG    = $(TOOLPATH)edimg.exe
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
	os.sys   = copy /B system/asmhead.bin+bootpack.bin os.sys
else
# OS X
	TOOLPATH = ../z_tools/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = make -r
	LD       = ~/opt/cross/bin/i686-elf-ld
	EDIMG    = $(TOOLPATH)edimg
	QEMU     = /usr/local/bin/qemu-system-x86_64 -vga std
	DEL      = rm -f
	os.sys   = cat system/asmhead.bin bootpack.bin > os.sys
endif

# Default

all:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C driver
	$(MAKE) -C system
	$(MAKE) cloumo.img

# 特別生成規則

bootpack.bin: $(OBJS) system/jpeg.obj system/bmp.obj $(GOLIBC)
	$(LD) -m elf_i386 -Map bootpack.map -T main.ls -s -o $@ $(OBJS) system/jpeg.obj system/bmp.obj $(GOLIBC)

os.sys: system/asmhead.bin bootpack.bin
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

# Libraries

$(GOLIBC):
	$(MAKE) -C golibc

# Options

run:
	$(MAKE) all
	$(QEMU) -m 64 -localtime -soundhw all -fda cloumo.img -L .

run-virtualbox:
	$(MAKE) all
	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage startvm Cloumo

refresh:
	$(MAKE) all
	$(MAKE) clean

clean:
	$(DEL) bootpack.map
	$(DEL) os.sys
	$(DEL) cloumo.img
	$(MAKE) -C system clean
	$(MAKE) -C driver clean
	$(MAKE) -C golibc clean
