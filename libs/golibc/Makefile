TARGET     = golibc
OBJS       = src/abs.o src/atof.o src/atoi.o src/errno.o src/frexp.o src/ldexp.o \
	src/memchr.o src/memcmp.o src/memcpy.o src/memmove.o src/memset.o src/qsort.o src/rand.o \
	src/sprintf.o src/strcat.o src/strcmp.o src/strcpy.o src/strcspn.o src/strdup.o \
	src/strlen.o src/strncat.o src/strncmp.o src/strncpy.o src/strpbrk.o src/strrchr.o \
	src/strspn.o src/strstr.o src/strtol.o src/strtoul.o src/strtoul0.o src/vsprintf.o

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = include
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       =
	NASM     =
	DEL      = -del
else
# UNIX
	TOOLPATH = ../../z_tools/
	INCPATH  = include
	MAKE     = make -r
	CC       = gcc
	CFLAGS   = -I$(INCPATH) -Os -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = g++
	CXXFLAGS = -I$(INCPATH) -Os -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       = ar
	NASM     = nasm
	DEL      = rm -f
endif

all: $(TARGET).a

src/%.o: src/%.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

src/%.o: src/%.asm Makefile
	$(NASM) -felf32 -o $@ $<

$(TARGET).a: $(OBJS) Makefile
	$(AR) r $@ $^

clean:
	$(DEL) src/*.o
	$(DEL) $(TARGET).a
