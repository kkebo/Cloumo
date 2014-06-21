ifeq ($(OS),Windows_NT)
	TOOLPATH = ../z_tools_win/
	INCLUDE = ../z_tools_win/include/
	MAKE     = $(TOOLPATH)make.exe -r
	EDIMG    = $(TOOLPATH)edimg.exe
	DEL      = -del
	qemu     = $(TOOLPATH)qemu/qemu.exe -std-vga
else
	TOOLPATH = ../z_tools/
	INCLUDE = ../z_tools/include/
	MAKE     = make -r
	EDIMG    = $(TOOLPATH)edimg
        DEL      = rm -f
	qemu     = /usr/local/bin/qemu-system-x86_64 -vga std
endif

# デフォルト動作

default :
	$(MAKE) full

# ファイル生成規則

cloumo.img : system/ipl.bin system/os.sys images/b_f.bmp images/btn_r.bmp \
		images/copy.bmp images/source.bmp images/search.bmp images/refresh.bmp
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:system/ipl.bin len:512 from:0 to:0 \
		copy from:system/os.sys to:@: \
		copy from:fonts/japanese.fnt to:@: \
		copy from:html/index.htm to:@: \
		copy from:images/b_f.bmp to:@: \
		copy from:images/btn_r.bmp to:@: \
		copy from:images/copy.bmp to:@: \
		copy from:images/source.bmp to:@: \
		copy from:images/search.bmp to:@: \
		copy from:images/refresh.bmp to:@: \
		imgout:cloumo.img

# コマンド

run :
	$(MAKE) full
	$(qemu) -m 32 -localtime -soundhw all -fda cloumo.img -L .

full :
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C system
	$(MAKE) cloumo.img

clean :
	$(DEL) cloumo.img
	$(MAKE) -C system clean

refresh :
	$(MAKE) full
	$(MAKE) clean
