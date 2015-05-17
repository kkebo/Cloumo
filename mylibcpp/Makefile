TARGET     = mylibcpp
OBJS       = SmartPointer.o pistring.o
DEPS       = $(OBJS:%.o=%.d)

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = ../stdinc/
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fpermissive -fno-stack-protector -ffunction-sections -fdata-sections
	AR       =
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
	AR       = ~/opt/cross/bin/i686-elf-ar
	NASM     = /usr/local/bin/nasm
	DEL      = rm -f
endif

all: $(TARGET).a

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET).a: $(OBJS)
	$(AR) r $@ $^

clean:
	$(DEL) *.o
	$(DEL) $(TARGET).a
