OBJS = \
	mouse.o \
	keyboard.o \
	sound.o \
	FAT12.o
DEPS = $(OBJS:%.o=%.d)

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = ../stdinc/
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fpermissive -fno-stack-protector -ffunction-sections -fdata-sections
	NASM     =
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = ../stdinc/
	MAKE     = make -r
	CC       = ~/opt/cross/bin/i686-elf-gcc
	CFLAGS   = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = ~/opt/cross/bin/i686-elf-g++
	CXXFLAGS = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fpermissive -fno-stack-protector -ffunction-sections -fdata-sections
	NASM     = nasm
	DEL      = rm -f
endif

# Default
all: $(OBJS)

# 一般生成規則
-include $(DEPS)

%.o: %.asm Makefile
	$(NASM) -felf32 -o $@ $<

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $<

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $<

# Options
clean:
	$(DEL) $(OBJS)
