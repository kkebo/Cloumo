TARGET     = golibc
OBJS       = abs.o atof.o atoi.o errno.o frexp.o ldexp.o \
	memchr.o memcmp.o memmove.o memset.o qsort.o rand.o \
	sprintf.o strcat.o strcmp.o strcpy.o strcspn.o strdup.o \
	strlen.o strncat.o strncmp.o strncpy.o strpbrk.o strrchr.o \
	strspn.o strstr.o strtol.o strtoul.o strtoul0.o vsprintf.o

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = .
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -Wall -m32 -nostdlib -fno-builtin -fno-exceptions -fno-rtti -fpermissive -B$(TOOLPATH) -mno-stack-arg-probe
	CXX      = $(TOOLPATH)gcc.exe
	CXXFLAGS = -I$(INCPATH) -Os -Wall -m32 -nostdlib -fno-builtin -fno-exceptions -fno-rtti -fpermissive -B$(TOOLPATH) -mno-stack-arg-probe
	LD       =
	AR       =
	NASM     =
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = .
	MAKE     = make -r
	CC       = clang
	CFLAGS   = -target i686-pc-linux-gnu -I$(INCPATH) -Os -Wall -nostdlib -nostdinc -fno-builtin -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c11
	CXX      = clang++
	CXXFLAGS = -target i686-pc-linux-gnu -I$(INCPATH) -Os -Wall -nostdlib -nostdinc -fno-builtin -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++1z
	LD       = i686-elf-ld
	AR       = i686-elf-ar
	NASM     = nasm
	DEL      = rm -f
endif

all: $(TARGET).a

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.asm
	$(NASM) -felf32 -o $@ $<

$(TARGET).a: $(OBJS)
	$(AR) r $@ $^

clean:
	$(DEL) *.o
	$(DEL) $(TARGET).a
