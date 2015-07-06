OBJS = \
	kernel/main.o \
	kernel/int.o \
	kernel/descriptor.o \
	kernel/memory.o \
	kernel/multitask.o \
	kernel/timer.o \
	kernel/datetime.o \
	kernel/graphic.o \
	kernel/tek.o \
	kernel/utf82kt.o \
	kernel/sysinfo.o \
	kernel/asmfunc.o \
	kernel/HTMLToken.o \
	kernel/HTMLTokenizer.o \
	kernel/HTMLNode.o \
	kernel/HTMLTreeConstructor.o \
	kernel/File.o \
	kernel/ModalWindow.o \
	kernel/Tab.o \
	driver/FAT12.o \
	driver/keyboard.o \
	driver/mouse.o \
	driver/sound.o \
	driver/EmuVGA.o

LIBS = golibc/golibc.a mylibcpp/mylibcpp.a

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../z_tools_win/
	INCPATH  = stdinc/
	MAKE     = $(TOOLPATH)make.exe -r
	LD       =
	EDIMG    = $(TOOLPATH)edimg.exe
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
	os.sys   = copy /B kernel/asmhead.bin+kernel.bin os.sys
else
# OS X
	TOOLPATH = ../z_tools/
	INCPATH  = stdinc/
	MAKE     = make -r
	LD       = ~/opt/cross/bin/i686-elf-ld
	EDIMG    = $(TOOLPATH)edimg
	QEMU     = /usr/local/bin/qemu-system-x86_64 -vga std
	DEL      = rm -f
	os.sys   = cat kernel/asmhead.bin kernel.bin > os.sys
endif

# Default

all:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C driver
	$(MAKE) -C kernel
	$(MAKE) cloumo.img

# 特別生成規則

kernel.bin: $(OBJS) kernel/jpeg.obj kernel/bmp.obj $(LIBS) main.ls
	$(LD) --gc-sections -nostdlib -m elf_i386 -Map kernel.map -T main.ls -s -o $@ $(OBJS) kernel/jpeg.obj kernel/bmp.obj $(LIBS)

os.sys: kernel/asmhead.bin kernel.bin
	$(os.sys)

cloumo.img: kernel/ipl.bin os.sys images/b_f.bmp images/btn_r.bmp \
		images/copy.bmp images/source.bmp images/search.bmp images/refresh.bmp \
		html/index.htm
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:kernel/ipl.bin len:512 from:0 to:0 \
		copy from:os.sys to:@: \
		copy from:fonts/japanese.fnt to:@: \
		copy from:html/index.htm to:@: \
		copy from:html/kitai.htm to:@: \
		copy from:images/b_f.bmp to:@: \
		copy from:images/btn_r.bmp to:@: \
		copy from:images/copy.bmp to:@: \
		copy from:images/source.bmp to:@: \
		copy from:images/search.bmp to:@: \
		copy from:images/refresh.bmp to:@: \
		imgout:cloumo.img

# Libraries

$(LIBS):
	$(MAKE) -C golibc
	$(MAKE) -C mylibcpp

# Options

run:
	$(MAKE) all
	$(QEMU) -m 64 -localtime -soundhw all -fda cloumo.img -L .

run-remote:
	$(MAKE) all
	$(QEMU) -vnc :2 -m 64 -localtime -soundhw all -fda cloumo.img -L .

run-virtualbox:
	$(MAKE) all
	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage startvm Cloumo

refresh:
	$(MAKE) all
	$(MAKE) clean

clean:
	$(DEL) kernel.map
	$(DEL) kernel.bin
	$(DEL) os.sys
	$(DEL) cloumo.img
	$(MAKE) -C kernel clean
	$(MAKE) -C driver clean
	$(MAKE) -C golibc clean
	$(MAKE) -C mylibcpp clean
