OBJS = \
	mouse.obj \
	keyboard.obj \
	sound.obj \
	file.obj

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = $(TOOLPATH)make.exe -r
	CXX      =
	CXXFLAGS =
	NASK     = $(TOOLPATH)nask.exe
	NASM     =
	G2N      =
	DEL      = -del
else
# OS X
	TOOLPATH = ../../z_tools/
	INCPATH  = $(TOOLPATH)include/
	MAKE     = make -r
	CXX      = $(TOOLPATH)gocc1plus
	CXXFLAGS = -I$(INCPATH) -Os -Wall -quiet -fno-exceptions
	NASK     = $(TOOLPATH)nask
	NASM     = nasm
	G2N      = $(TOOLPATH)gas2nask -a
	DEL      = rm -f
endif

# Default
all: $(OBJS)

# 一般生成規則
%.obj: %.asm
	$(NASM) -fwin32 -o $@ $<

%.s: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

%.nas: %.s
	$(G2N) $< $@

%.obj: %.nas
	$(NASK) $< $@

# Options
clean:
	$(DEL) $(OBJS)
