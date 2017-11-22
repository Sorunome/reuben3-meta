#include "misc.h"

#include "board.h"
#include "player.h"
#include "camera.h"
#include <Gamebuino-Meta.h>

void waitCycles(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
	}
}

bool waitCyclesButton(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
		if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_LEFT) || gb.buttons.pressed(BUTTON_RIGHT)) {
			return true;
		}
	}
	return false;
}

void waitRelease() {
	while(1) {
		while(!gb.update());
		if (gb.buttons.released(BUTTON_A) || gb.buttons.released(BUTTON_B) || gb.buttons.released(BUTTON_LEFT) || gb.buttons.released(BUTTON_RIGHT)) {
			return;
		}
	}
}

void renderAll() {
	board.render();
	player.render();
	gb.display.setCursor(0, 0);
	gb.display.setColor(BLACK, WHITE);
	gb.display.print(gb.getCpuLoad());
}

void statsMenu() {
	while(1) {
		if (!gb.update()) {
			continue;
		}
		if (gb.buttons.pressed(BUTTON_C)) {
			return;
		}
		gb.display.clear(BEIGE);
		gb.display.drawFastHLine(1, 0, gb.display.width()-2);
		gb.display.drawFastHLine(1, gb.display.height()-1, gb.display.width()-2);
	}
}

void bombErase(int8_t x, int8_t y) {
	if (x < 0 || y < 0 || x >= (12*8) || y >= (8*8)) {
		return;
	}
	x /= 8;
	y /= 8;
	uint16_t tile = board.getTile(x, y);
	if (tile < SPRITE_FIRST_BOMB || tile >= SPRITE_AFTER_BOMB) {
		return;
	}
	board.eraseTile(x, y);
	board.runScript(x, y, SCRIPT_BOMB);
}

const uint8_t bomb_frames[] = {
	24, 24,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x10, 0x11, 0x01, 0x99, 0x81, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x10, 0x00, 0x11, 0x11, 0x11, 0x11, 0x10, 0x01, 0x11, 0x11,
	0x11, 0x11, 0x01, 0x01, 0x11, 0x81, 0x81, 0x11, 0x11, 0x01, 0x11, 0x11,
	0x11, 0x11, 0x10, 0x01, 0x11, 0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x19, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x18, 0x81, 0x11, 0x11, 0x11, 0x11, 0x18, 0x18, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x18, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x19, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x19, 0x10, 0x11, 0x11,
	0x11, 0x11, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x91, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x10, 0x11, 0x81, 0x11, 0x11, 0x01, 0x10, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x81, 0x11, 0x11, 0x01, 0x11, 0x11,
	0x11, 0x11, 0x10, 0x01, 0x99, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x10, 0x11, 0x91, 0x18, 0x11, 0x10, 0x11, 0x19, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
	
	
	0x10, 0x11, 0x11, 0x11, 0x11, 0x10, 0x91, 0x10, 0x11, 0x11, 0x91, 0x01,
	0x11, 0x11, 0x01, 0x18, 0x81, 0x10, 0x18, 0x11, 0x01, 0x99, 0x11, 0x11,
	0x11, 0x81, 0x01, 0x11, 0x11, 0x01, 0x11, 0x81, 0x10, 0x11, 0x18, 0x11,
	0x10, 0x10, 0x11, 0x18, 0x19, 0x01, 0x11, 0x81, 0x11, 0x01, 0x11, 0x11,
	0x11, 0x10, 0x10, 0x11, 0x01, 0x99, 0x81, 0x10, 0x11, 0x11, 0x11, 0x01,
	0x11, 0x11, 0x10, 0x00, 0x11, 0x11, 0x11, 0x11, 0x10, 0x01, 0x11, 0x11,
	0x10, 0x11, 0x01, 0x01, 0x11, 0x81, 0x81, 0x11, 0x11, 0x01, 0x91, 0x91,
	0x11, 0x11, 0x10, 0x01, 0x11, 0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x11,
	0x19, 0x11, 0x19, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01,
	0x11, 0x00, 0x18, 0x81, 0x11, 0x11, 0x11, 0x11, 0x18, 0x18, 0x88, 0x11,
	0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x81, 0x11,
	0x18, 0x81, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x18, 0x11, 0x11, 0x10,
	0x11, 0x10, 0x11, 0x19, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x11,
	0x00, 0x01, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x19, 0x10, 0x11, 0x01,
	0x11, 0x11, 0x18, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x91, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x10, 0x11, 0x81, 0x11, 0x11, 0x01, 0x10, 0x19, 0x91,
	0x10, 0x00, 0x11, 0x10, 0x11, 0x11, 0x81, 0x11, 0x11, 0x01, 0x01, 0x01,
	0x11, 0x11, 0x10, 0x01, 0x99, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x10,
	0x91, 0x99, 0x10, 0x11, 0x91, 0x18, 0x11, 0x10, 0x11, 0x19, 0x11, 0x11,
	0x11, 0x11, 0x01, 0x11, 0x10, 0x11, 0x10, 0x11, 0x11, 0x11, 0x01, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x01, 0x11, 0x01, 0x11, 0x19, 0x11, 0x11, 0x91,
	0x01, 0x10, 0x01, 0x11, 0x01, 0x11, 0x00, 0x11, 0x11, 0x91, 0x10, 0x11,
	0x01, 0x11, 0x11, 0x11, 0x10, 0x11, 0x11, 0x10, 0x11, 0x91, 0x11, 0x10,
};

void bomb(int8_t x, int8_t y) {
	Image img(bomb_frames, 2, 2);
	img.setTransparentColor(INDEX_DARKBLUE);
	for (uint8_t i = 0; i < 2*2 - 1; i++) {
		while(!gb.update());
		renderAll();
		gb.display.drawImage(x - 3 - 8 - camera.x, y - 5 - 8 - camera.y, img);
	}
	for (int8_t i = -1; i < 2; i++) {
		for (int8_t j = -1; j < 2; j++) {
			bombErase(x + i*6, y + j*6);
		}
	}
	img.setFrame(1);
	for (uint8_t i = 0; i < 2*2; i++) {
		while(!gb.update());
		renderAll();
		gb.display.drawImage(x - 3 - 8 - camera.x, y - 5 - 8 - camera.y, img);
	}
}
