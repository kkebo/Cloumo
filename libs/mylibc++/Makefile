TARGET     = mylibc++
OBJS       = src/SmartPointer.o src/pistring.o
DEPS       = $(OBJS:%.o=%.d)

ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../../z_tools_win/
	INCPATH  = include
	MAKE     = $(TOOLPATH)make.exe -r
	CC       = $(TOOLPATH)gcc.exe
	CFLAGS   = -I../../include -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = $(TOOLPATH)g++.exe
	CXXFLAGS = -I../../include -I$(INCPATH) -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++11 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       =
	NASM     =
	DEL      = -del
else
# UNIX
	TOOLPATH = ../../z_tools/
	INCPATH  = include
	MAKE     = make -r
	CC       = clang
	CFLAGS   = -I../../include -I$(INCPATH) --target=i686-unknown-unknown-elf -Os -MD -MP -Wall -fno-exceptions -B$(TOOLPATH) -std=c11 -fno-stack-protector -ffunction-sections -fdata-sections
	CXX      = clang++
	CXXFLAGS = -I../../include -I$(INCPATH) --target=i686-unknown-unknown-elf -Os -MD -MP -Wall -fno-exceptions -fno-rtti -B$(TOOLPATH) -std=c++11 -fno-stack-protector -ffunction-sections -fdata-sections
	AR       = ar
	NASM     = nasm
	DEL      = rm -f
endif

.PHONY: all
all: $(TARGET).a

-include $(DEPS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET).a: $(OBJS)
	$(AR) r $@ $^

.PHONY: clean
clean:
	$(DEL) src/*.o
	$(DEL) $(TARGET).a
