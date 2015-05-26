TARGET     = golibc
OBJS       = abs.o atof.o atoi.o errno.o frexp.o ldexp.o \
	memchr.o memcmp.o memcpy.o memmove.o memset.o qsort.o rand.o \
	sprintf.o strcat.o strcmp.o strcpy.o strcspn.o strdup.o \
	strlen.o strncat.o strncmp.o strncpy.o strpbrk.o strrchr.o \
	strspn.o strstr.o strtol.o strtoul.o strtoul0.o vsprintf.o

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = .
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       =
	NASM     =
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = .
	MAKE     = make -r
	CC       = ~/opt/cross/bin/i686-elf-gcc
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = ~/opt/cross/bin/i686-elf-g++
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       = ~/opt/cross/bin/i686-elf-ar
	NASM     = /usr/local/bin/nasm
	DEL      = rm -f
endif

all: $(TARGET).a

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $<

%.o: %.asm Makefile
	$(NASM) -felf32 -o $@ $<

$(TARGET).a: $(OBJS) Makefile
	$(AR) r $@ $^

clean:
	$(DEL) *.o
	$(DEL) $(TARGET).a
