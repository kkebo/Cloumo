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
	CXXFLAGS = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       =
	NASM     =
	DEL      = -del
else
# UNIX
	TOOLPATH = ../../z_tools/
	INCPATH  = ../stdinc/
	MAKE     = make -r
	CC       = gcc
	CFLAGS   = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = g++
	CXXFLAGS = -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++14 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       = ar
	NASM     = nasm
	DEL      = rm -f
endif

all: $(TARGET).a

-include $(DEPS)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET).a: $(OBJS) Makefile
	$(AR) r $@ $^

clean:
	$(DEL) *.o
	$(DEL) $(TARGET).a
