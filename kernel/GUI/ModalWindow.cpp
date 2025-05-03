#include "../headers.h"

ModalWindow::ModalWindow(const string &msg) : sheet(new Sheet(Size(480, 300))), message(msg) {
	// background and message
	sheet->fillRect(sheet->frame, 0xffffff);
	sheet->drawString(msg, Point((480 - msg.length() * 8) / 2, ((300 - 48 - 10) - 16) / 2), 0);
	
	// button
	Sheet *button = new Sheet(Size(64, 48), true);
	button->fillRect(button->frame, 0xaaaaaa);
	button->drawString("OK", Point((button->frame.size.width - 16) / 2, (button->frame.size.height - 16) / 2), 0);
	button->borderRadius(true, true, true, true);
	button->moveTo(Point((sheet->frame.size.width - button->frame.size.width) / 2, sheet->frame.size.height - button->frame.size.height - 10));
	sheet->appendChild(button, true);
	button->onClick = [](const Point &pos, Sheet &sheet) {
		delete sheet.parent;
	};
	
	// show this window
	sheet->moveTo(Point((SheetCtl::resolution.width - 480) / 2, (SheetCtl::resolution.height - 300) / 2));
	SheetCtl::back->appendChild(sheet);
	sheet->upDown(1);
}
