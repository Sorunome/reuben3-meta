#include "text.h"
#include "depack.h"
#include <Gamebuino-Meta.h>

struct Strings_LUT {
	const uint16_t offset;
	const uint8_t chunk;
};

#include "data/strings.h"

extern uint8_t decompression_buffer[];

const uint8_t buttonsBuffer[] = {
	4, 4,
	0xFF, 0xFF,
	0x55, 0x5F,
	0xF5, 0xFF,
	0xFF, 0xFF,
	
	0xF5, 0x5F,
	0x55, 0x55,
	0xF5, 0x5F,
	0xFF, 0xFF,
};
Image buttons(buttonsBuffer, 2, 0);

void waitCycles(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
	}
}

bool waitCyclesButton(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
		if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B)) {
			return true;
		}
	}
	return false;
}

void waitRelease() {
	while(1) {
		while(!gb.update());
		if (gb.buttons.released(BUTTON_A) || gb.buttons.released(BUTTON_B)) {
			return;
		}
	}
}

void Text::drawBox(bool up) {
	gb.display.setColor(BROWN);
	gb.display.drawRect(4, up ? 4 : 34, 72, 26);
	gb.display.setColor(BEIGE);
	gb.display.fillRect(5, up ? 5 : 35, 70, 24);
	
}

int8_t Text::box(uint16_t i, bool up) {
	aP_depack(_Text_decompression_LUT[stringsLut[i].chunk], decompression_buffer);
	drawBox(up);
	const uint8_t cursorXStart = (80 - (16*4)) / 2;
	uint8_t cursorYStart = up ? 8 : 38;
	gb.display.setCursor(cursorXStart, cursorYStart);
	uint8_t* textCursor = decompression_buffer + stringsLut[i].offset;
textloop_entry:
	uint8_t c = *textCursor++;
	SerialUSB.println(c, HEX);
	switch(c) {
		case '\n':
			gb.display.write(c);
			gb.display.setCursorX(cursorXStart);
			goto textloop_entry;
		case 0x80: // block wrap
		{
			buttons.setFrame(1);
			bool alt = true;
			do {
				if (alt) {
					gb.display.drawImage(cursorXStart + 16*4 - 1, cursorYStart + 2*6 + 4, buttons);
				} else {
					gb.display.setColor(BEIGE);
					gb.display.fillRect(cursorXStart + 16*4 - 1, cursorYStart + 2*6 + 4, 4, 3);
				}
				alt = !alt;
			} while(!waitCyclesButton(5));
			waitRelease();
			drawBox(up);
			gb.display.setCursor(cursorXStart, cursorYStart);
			goto textloop_entry;
		}
		case 0x81: // page wrap
		{
			buttons.setFrame(0);
			bool alt = true;
			do {
				gb.display.drawImage(cursorXStart + 16*4 - 1, cursorYStart + 2*6 + 1 + alt, buttons);
				alt = !alt;
			} while(!waitCyclesButton(5));
			waitRelease();
			drawBox(up);
			gb.display.setCursor(cursorXStart, cursorYStart);
			goto textloop_entry;
		}
		case 0x82: // question
			break;
		case 0x83: // options
			break;
		case 0:
			break;
		case 0x84:
			i++;
			aP_depack(_Text_decompression_LUT[stringsLut[i].chunk], decompression_buffer);
			textCursor = decompression_buffer;
			goto textloop_entry;
		default:
			gb.display.setColor(BLACK);
			gb.display.write(c);
			if (gb.buttons.repeat(BUTTON_A, 0) || gb.buttons.repeat(BUTTON_B, 0)) {
				waitCycles(1);
			} else {
				waitCycles(5);
			}
			goto textloop_entry;
	}
	if (c == 0) {
		return -1;
	}
	
	// do shop
	return -1;
}


Text text;
