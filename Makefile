OBJS = \
	src/kernel/main.o \
	src/kernel/int.o \
	src/kernel/descriptor.o \
	src/kernel/memory.o \
	src/kernel/multitask.o \
	src/kernel/timer.o \
	src/kernel/datetime.o \
	src/kernel/graphic.o \
	src/kernel/tek.o \
	src/kernel/utf82kt.o \
	src/kernel/sysinfo.o \
	src/kernel/asmfunc.o \
	src/kernel/HTMLToken.o \
	src/kernel/HTMLTokenizer.o \
	src/kernel/HTMLNode.o \
	src/kernel/HTMLTreeConstructor.o \
	src/kernel/File.o \
	src/kernel/ModalWindow.o \
	src/kernel/Tab.o \
	src/driver/FAT12.o \
	src/driver/keyboard.o \
	src/driver/mouse.o \
	src/driver/sound.o \
	src/driver/EmuVGA.o

LIBS = libs/golibc/golibc.a libs/mylibc++/mylibc++.a

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../z_tools_win/
	MAKE     = $(TOOLPATH)make.exe -r
	LD       =
	EDIMG    = $(TOOLPATH)edimg.exe
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
	os.sys   = copy /B src/kernel/asmhead.bin+kernel.bin os.sys
else
# UNIX
	TOOLPATH = ../z_tools/
	MAKE     = make -r
	LD       = ld
	EDIMG    = $(TOOLPATH)edimg
	QEMU     = qemu-system-x86_64 -vga std
	DEL      = rm -f
	os.sys   = cat src/kernel/asmhead.bin kernel.bin > os.sys
endif

# Default

all:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C src/driver
	$(MAKE) -C src/kernel
	$(MAKE) cloumo.img

# 特別生成規則

kernel.bin: $(OBJS) src/kernel/jpeg.obj src/kernel/bmp.obj $(LIBS) main.ls
	$(LD) --gc-sections -nostdlib -m elf_i386 -Map kernel.map -T main.ls -s -o $@ $(OBJS) src/kernel/jpeg.obj src/kernel/bmp.obj $(LIBS)

os.sys: src/kernel/asmhead.bin kernel.bin
	$(os.sys)

cloumo.img: src/kernel/ipl.bin os.sys images/b_f.bmp images/btn_r.bmp \
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
	$(MAKE) -C libs/golibc
	$(MAKE) -C libs/mylibcpp

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
	$(MAKE) -C src/kernel clean
	$(MAKE) -C src/driver clean
	$(MAKE) -C libs/golibc clean
	$(MAKE) -C libs/mylibcpp clean
