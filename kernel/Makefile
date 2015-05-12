OBJS = \
	bootpack.o \
	int.o \
	descriptor.o \
	memory.o \
	browser.o \
	multitask.o \
	timer.o \
	datetime.o \
	graphic.o \
	tek.o \
	utf82kt.o \
	sysinfo.o \
	asmfunc.o \
	SmartPointer.o

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = ../stdinc/
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fpermissive -fno-stack-protector -ffunction-sections -fdata-sections
	NASM     =
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = ../stdinc/
	MAKE     = make -r
	CC       = ~/opt/cross/bin/i686-elf-gcc
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = ~/opt/cross/bin/i686-elf-g++
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fpermissive -fno-stack-protector -ffunction-sections -fdata-sections
	NASM     = /usr/local/bin/nasm
	DEL      = rm -f
endif

# Default
all: $(OBJS) ipl.bin asmhead.bin

# 一般生成規則
%.o: %.asm
	$(NASM) -felf32 -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.bin: %.asm
	$(NASM) -fbin -o $@ $<

# Options
clean:
	$(DEL) *.bin
	$(DEL) $(OBJS)
