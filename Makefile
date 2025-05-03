ifeq ($(OS),Windows_NT)
# Windows
	TOOLPATH = ../z_tools_win/
	MAKE     = $(TOOLPATH)make.exe -r
	EDIMG    = $(TOOLPATH)edimg.exe
	QEMU     = $(TOOLPATH)qemu/qemu.exe -std-vga
	DEL      = -del
else
# OS X
	TOOLPATH = ../z_tools/
	MAKE     = make -r
	EDIMG    = $(TOOLPATH)edimg
	QEMU     = qemu-system-x86_64 -vga std
	DEL      = rm -f
endif


default:
	$(MAKE) -C images
	$(MAKE) -C html
	$(MAKE) -C system
	$(MAKE) cloumo.img

# ファイル生成規則

cloumo.img: system/ipl.bin system/os.sys images/b_f.bmp images/btn_r.bmp \
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

# Commands

run:
	$(MAKE) default
	$(QEMU) -m 32 -localtime -soundhw all -fda cloumo.img -L .

clean:
	$(DEL) cloumo.img
	$(MAKE) -C system clean
	$(MAKE) -C golibc clean

refresh:
	$(MAKE) default
	$(MAKE) clean
