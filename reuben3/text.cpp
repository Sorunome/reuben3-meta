#include "text.h"
#include "depack.h"
#include "misc.h"
#include "player.h"
#include "data/defines.h"
#include <Gamebuino-Meta.h>

struct Strings_MasterLUT {
	const Gamebuino_Meta::LangCode langCode;
	const uint8_t* const* decompressionLut;
	const Strings_LUT* stringLut;
};

#include "data/strings.h"

extern uint8_t decompression_buffer[];

const uint8_t buttonsBuffer[] = {
	4, 4,
	0x03, 0x00,
	0,
	0xFF,
	0x01,
	
	0xFF, 0xFF,
	0x55, 0x5F,
	0xF5, 0xFF,
	0xFF, 0xFF,

	0xF5, 0x5F,
	0x55, 0x55,
	0xF5, 0x5F,
	0xFF, 0xFF,

	0x55, 0x5F,
	0x5F, 0x5F,
	0x55, 0x5F,
	0xFF, 0xFF,
};
Image buttons(buttonsBuffer);

void Text::init() {
	Gamebuino_Meta::LangCode l = gb.language.getCurrentLang();
	bool found = false;
	uint8_t i = 0;
	for (; i < NUMBER_LANGUAGES; i++) {
		if (stringsMasterLut[i].langCode == l) {
			found = true;
			break;
		}
	}
	if (!found) {
		i = 0;
	}
	decompressionLut = stringsMasterLut[i].decompressionLut;
	stringLut = stringsMasterLut[i].stringLut;
}

void Text::drawBox(bool up) {
	gb.display.setColor(BROWN);
	gb.display.drawRect(4, up ? 4 : 34, 72, 26);
	gb.display.setColor(BEIGE);
	gb.display.fillRect(5, up ? 5 : 35, 70, 24);

}

int8_t Text::box(uint16_t i, bool up) {
	aP_depack(decompressionLut[stringLut[i].chunk], decompression_buffer);
	drawBox(up);
	const uint8_t cursorXStart = (80 - (16*4)) / 2;
	uint8_t cursorYStart = up ? 8 : 38;
	gb.display.setCursor(cursorXStart, cursorYStart);
	uint8_t* textCursor = decompression_buffer + stringLut[i].offset;
	bool hasOptions = false;
textloop_entry:
	uint8_t c = *textCursor++;
//	SerialUSB.println(c, HEX);
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
			hasOptions = true;
			gb.display.write(' ');
			goto textloop_entry;
		case 0x83: // options
			break;
		case 0:
			break;
		case 0x84:
			i++;
			aP_depack(decompressionLut[stringLut[i].chunk], decompression_buffer);
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
	if (!hasOptions) {
		return -1;
	}

	// do shop
	// we can assume here that our text isn't spread accross multiple chunks
	// because our creation tool makes sure of that
	uint8_t options = *textCursor++;
	uint8_t* offsets = textCursor;
	uint8_t cursor = 0;
	uint8_t y = gb.display.getCursorY() + 1;
	buttons.setFrame(2);
	while(1) {
		bool alt = true;
		do {
			if (alt) {
				gb.display.drawImage(cursorXStart + offsets[cursor], y, buttons);
			} else {
				gb.display.setColor(BEIGE);
				gb.display.fillRect(cursorXStart + offsets[cursor], y, 4, 3);
			}
			alt = !alt;
		} while(!waitCyclesButton(5));
		waitRelease();
		if (gb.buttons.released(BUTTON_A)) {
			return cursor;
		}
		// make sure theo ld cursor is erased
		gb.display.setColor(BEIGE);
		gb.display.fillRect(cursorXStart + offsets[cursor], y, 4, 3);

		if (gb.buttons.released(BUTTON_LEFT) && cursor > 0) {
			cursor--;
		}
		if (gb.buttons.released(BUTTON_RIGHT) && cursor < options-1) {
			cursor++;
		}
	}
}

int8_t Text::boxPlayer(uint16_t i) {
	box(i, player.getY() > 28);
}


Text text;
