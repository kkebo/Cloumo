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
	src/kernel/HTMLTreeBuilder.o \
	src/kernel/File.o \
	src/kernel/ModalWindow.o \
	src/kernel/Tab.o \
	src/kernel/bmp.o \
	src/kernel/jpeg.o \
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
	MFORMAT  =
	MCOPY    =
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
	os.sys   = copy /B src/kernel/asmhead.bin+kernel.bin os.sys
else
# UNIX
	TOOLPATH = ../z_tools/
	MAKE     = make -r
	LD       = ld.lld
	MFORMAT  = mformat
	MCOPY    = mcopy
	QEMU     = qemu-system-x86_64 -vga std
	DEL      = rm -f
	os.sys   = cat src/kernel/asmhead.bin kernel.bin > os.sys
endif

# Default

.PHONY: all
all:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C src/driver
	$(MAKE) -C src/kernel
	$(MAKE) cloumo.img

# 特別生成規則

kernel.bin: $(OBJS) $(LIBS) main.ls
	$(LD) --gc-sections --orphan-handling=warn -nostdlib -m elf_i386 -Map kernel.map -T main.ls -s -o $@ $(OBJS) $(LIBS)

os.sys: src/kernel/asmhead.bin kernel.bin
	$(os.sys)

cloumo.img: src/kernel/ipl.bin os.sys fonts/japanese.fnt html/index.htm html/kitai.htm \
		images/b_f.bmp images/btn_r.bmp images/copy.bmp images/source.bmp \
		images/search.bmp images/refresh.bmp
	$(MFORMAT) -f 1440 -C -B src/kernel/ipl.bin -i cloumo.img ::
	$(MCOPY) -i cloumo.img os.sys ::
	$(MCOPY) -i cloumo.img fonts/japanese.fnt ::
	$(MCOPY) -i cloumo.img html/index.htm ::
	$(MCOPY) -i cloumo.img html/kitai.htm ::
	$(MCOPY) -i cloumo.img images/b_f.bmp ::
	$(MCOPY) -i cloumo.img images/btn_r.bmp ::
	$(MCOPY) -i cloumo.img images/copy.bmp ::
	$(MCOPY) -i cloumo.img images/source.bmp ::
	$(MCOPY) -i cloumo.img images/search.bmp ::
	$(MCOPY) -i cloumo.img images/refresh.bmp ::

# Libraries

$(LIBS):
	$(MAKE) -C libs/golibc
	$(MAKE) -C libs/mylibc++

# Options

.PHONY: run
run:
	$(MAKE) all
	$(QEMU) -m 64 -rtc base=localtime -device intel-hda -drive file=cloumo.img,index=0,if=floppy,format=raw -L .

.PHONY: run-remote
run-remote:
	$(MAKE) all
	$(QEMU) -display vnc=:2 -m 64 -rtc base=localtime -device intel-hda -drive file=cloumo.img,index=0,if=floppy,format=raw -L .

.PHONY: run-virtualbox
run-virtualbox:
	$(MAKE) all
	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage startvm Cloumo

.PHONY: refresh
refresh:
	$(MAKE) all
	$(MAKE) clean

.PHONY: clean
clean:
	$(DEL) kernel.map
	$(DEL) kernel.bin
	$(DEL) os.sys
	$(DEL) cloumo.img
	$(MAKE) -C src/kernel clean
	$(MAKE) -C src/driver clean
	$(MAKE) -C libs/golibc clean
	$(MAKE) -C libs/mylibc++ clean
