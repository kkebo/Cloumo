OBJS = \
	main.o \
	int.o \
	descriptor.o \
	memory.o \
	multitask.o \
	timer.o \
	datetime.o \
	GUI/graphic.o \
	tek.o \
	utf82kt.o \
	sysinfo.o \
	asmfunc.o \
	HTML/HTMLToken.o \
	HTML/HTMLTokenizer.o \
	HTML/HTMLNode.o \
	HTML/HTMLTreeBuilder.o \
	File.o \
	GUI/ModalWindow.o \
	GUI/Tab.o \
	bmp.o \
	jpeg.o
DEPS = $(OBJS:%.o=%.d)

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = ../../include
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I../../libs/golibc/include -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I../../libs/mylibc++/include -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++11 -fno-stack-protector -ffunction-sections -fdata-sections -fno-use-cxa-atexit
	NASM     =
	DEL      = -del
else
# UNIX
	TOOLPATH = ../../z_tools/
	INCPATH  = ../../include
	MAKE     = make -r
	CC       = clang
	CFLAGS   = -I../../libs/golibc/include -I$(INCPATH) --target=i686-unknown-unknown-elf -Os -MD -MP -Wall -fno-builtin -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = clang++
	CXXFLAGS = -I../../libs/mylibc++/include -I$(INCPATH) --target=i686-unknown-unknown-elf -Os -MD -MP -Wall -fno-builtin -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++11 -fno-stack-protector -ffunction-sections -fdata-sections -fno-use-cxa-atexit
	NASM     = nasm
	DEL      = rm -f
endif

# Default
.PHONY: all
all: $(OBJS) ipl.bin asmhead.bin

# 一般生成規則
-include $(DEPS)

%.o: %.asm Makefile
	$(NASM) -felf32 -o $@ $<

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $<

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $<

%.bin: %.asm Makefile
	$(NASM) -fbin -o $@ $<

# Options
.PHONY: clean
clean:
	$(DEL) *.bin
	$(DEL) $(OBJS)
