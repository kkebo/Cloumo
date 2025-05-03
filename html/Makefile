ifeq ($(OS),Windows_NT)
	TOOLPATH = ../../z_tools_win/
	MAKE     = $(TOOLPATH)make.exe -r
	bim2bin  = $(TOOLPATH)bim2bin.exe
else
	TOOLPATH = ../../z_tools/
	MAKE     = make -r
	bim2bin  = $(TOOLPATH)bim2bin
endif

# Default

.PHONY: all
all: index.htm kitai.htm

# 一般生成規則

%.htm: %_.htm Makefile
	$(bim2bin) -osacmp in:$*_.htm out:$*.htm
