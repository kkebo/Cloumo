OBJS   = bootpack.o mouse.o keyboard.o int.o descriptor.o file.o memory.o browser.o sound.o queue.o multitask.o timer.o datetime.o graphic.o tek.o utf82kt.o asmfunc.o
GOLIBC = ../golibc/golibc.a

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -Wall -nostdinc -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c11 -fno-stack-protector
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -Wall -nostdinc -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++1y -fpermissive -fno-stack-protector
	LD       =
	NASM     =
	os.sys   = copy /B asmhead.bin+bootpack.bin os.sys
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = make -r
	CC       = i686-elf-gcc
	CFLAGS   = -I$(INCPATH) -Os -Wall -nostdinc -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c11 -fno-stack-protector
	CXX      = i686-elf-g++
	CXXFLAGS = -I$(INCPATH) -Os -Wall -nostdinc -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++1y -fpermissive -fno-stack-protector
	LD       = i686-elf-ld
	NASM     = nasm
	os.sys   = cat asmhead.bin bootpack.bin > os.sys
	DEL      = rm -f
endif

# デフォルトの設定
all: os.sys ipl.bin

# 個別の設定
bootpack.bin: $(OBJS) jpeg.obj bmp.obj $(GOLIBC)
	$(LD) -m elf_i386 -Map bootpack.map -T main.ls -s -o $@ $(OBJS) jpeg.obj bmp.obj $(GOLIBC)

os.sys: asmhead.bin bootpack.bin
	$(os.sys)

# 一般の設定
%.o: %.asm
	$(NASM) -felf32 -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.bin: %.asm
	$(NASM) -fbin -o $@ $<

# Libraries
$(GOLIBC):
	$(MAKE) -C ../golibc

# 生成物を削除
clean:
	$(DEL) *.map
	$(DEL) *.bin
	$(DEL) $(OBJS)
	$(DEL) os.sys
