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

all: b_f.bmp btn_r.bmp copy.bmp source.bmp search.bmp refresh.bmp

# 一般生成規則

%.bmp: %_.bmp Makefile
	$(bim2bin) -osacmp in:$*_.bmp out:$*.bmp

%.jpg: %_.jpg Makefile
	$(bim2bin) -osacmp in:$*_.jpg out:$*.jpg
