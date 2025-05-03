#pragma once

class Screen {
private:
	static union VRAM {
		unsigned short *p16;
		unsigned char (*p24)[3];
		unsigned int *p32;
	} vram;
	Container container;

	Screen() : container() {
		// BootInfo から
		
		// 描画
	}
	void refresh() {
		// container の　buf を vram に書き写し
	}

public:
	Screen &getInstance() {
		static Screen instance = Screen();
		return instance;
	}
};
