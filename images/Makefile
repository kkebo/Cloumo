ifeq ($(OS),Windows_NT)
	TOOLPATH = ../../z_tools_win/
	MAKE     = $(TOOLPATH)make.exe -r
	bim2bin  = $(TOOLPATH)bim2bin.exe
else
	TOOLPATH = ../../z_tools/
	MAKE     = make -r
	bim2bin  = $(TOOLPATH)bim2bin
endif

# デフォルト動作

default :
	$(MAKE) b_f.bmp
	$(MAKE) btn_r.bmp
	$(MAKE) copy.bmp
	$(MAKE) source.bmp
	$(MAKE) search.bmp
	$(MAKE) refresh.bmp

# 一般規則

%.bmp : %_.bmp Makefile
	$(bim2bin) -osacmp in:$*_.bmp out:$*.bmp

%.jpg : %_.jpg Makefile
	$(bim2bin) -osacmp in:$*_.jpg out:$*.jpg
