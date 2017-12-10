#include "misc.h"

#include "board.h"
#include "player.h"
#include "camera.h"
#include "data/defines.h"
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
	bool have_items[7] = {
		player.isEvent(EVENT_ITEM_BOTTLE1),
		player.isEvent(EVENT_ITEM_BOTTLE2),
		player.isEvent(EVENT_ITEM_BOTTLE3),
		player.isEvent(EVENT_ITEM_BOTTLE4),
		player.isEvent(EVENT_ITEM_HOOKSHOT),
		player.isEvent(EVENT_ITEM_BOMB),
		player.isEvent(EVENT_ITEM_PROTECT),
	};
	uint8_t cur_item = player.getCurItem();
	uint8_t cursor = cur_item;
	if (!cursor) {
		cursor = 1;
	}
	const uint8_t devider = 9*4 + 6;
	gb.display.clear(BEIGE);
	gb.display.setColor(BLACK);
	gb.display.drawFastHLine(1, 0, devider-1);
	gb.display.drawFastHLine(devider+1, 0, gb.display.width()-devider-2);
	gb.display.drawFastHLine(1, gb.display.height()-1, devider-1);
	gb.display.drawFastHLine(devider+1, gb.display.height()-1, gb.display.width()-devider-2);
	gb.display.drawFastVLine(0, 1, gb.display.height()-2);
	gb.display.drawFastVLine(gb.display.width()-1, 1, gb.display.height()-2);
	gb.display.drawFastVLine(devider, 1, gb.display.height()-2);
	
	if (have_items[0]) {
		gb.display.setCursor(6, 4);
		gb.display.print("Bottle");
	}
	if (have_items[1]) {
		gb.display.setCursor(6, 10);
		gb.display.print("Bottle");
	}
	if (have_items[2]) {
		gb.display.setCursor(6, 16);
		gb.display.print("Bottle");
	}
	if (have_items[3]) {
		gb.display.setCursor(6, 22);
		gb.display.print("Bottle");
	}
	if (have_items[4]) {
		gb.display.setCursor(6, 28);
		gb.display.print("Hookshot");
	}
	if (have_items[5]) {
		gb.display.setCursor(6, 34);
		gb.display.print("Bombs:");
		gb.display.print(player.bombs);
	}
	if (have_items[6]) {
		gb.display.setCursor(6, 40);
		gb.display.print("Protect");
	}
	
	gb.display.setCursor(devider + 2, 4);
	gb.display.print("HP:");
	gb.display.setCursor(devider + 2, 10);
	gb.display.print(player.getHp());
	gb.display.print("/");
	gb.display.print(player.getHpMax());
	
	gb.display.setCursor(devider + 2, 16);
	gb.display.print("MP:");
	gb.display.setCursor(devider + 2, 22);
	gb.display.print(player.getMp());
	gb.display.print("/");
	gb.display.print(player.getMpMax());
	
	gb.display.setCursor(devider + 2, 28);
	gb.display.print("Gold:");
	gb.display.setCursor(devider + 2, 34);
	gb.display.print(player.getGold());
	
	while(1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.setColor(BEIGE);
		if (cur_item) {
			gb.display.fillRect(8*4 + 6, 6*(cur_item - 1) + 4, 4, 6);
		}
		gb.display.fillRect(2, 6*(cursor - 1) + 4, 4, 6);
		
		if (gb.buttons.pressed(BUTTON_C)) {
			return;
		}
		
		if (gb.buttons.pressed(BUTTON_UP)) {
			if (!--cursor) {
				cursor = 7;
			}
		}
		
		if (gb.buttons.pressed(BUTTON_DOWN)) {
			if (++cursor > 7) {
				cursor = 1;
			}
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			if (have_items[cursor - 1]) {
				player.setCurItem(cursor);
				cur_item = cursor;
			}
		}
		
		gb.display.setColor(BLACK);
		if (cur_item) {
			gb.display.setCursor(8*4 + 6, 6*(cur_item - 1) + 4);
			gb.display.print('\4');
		}
		gb.display.setCursor(2, 6*(cursor - 1) + 4);
		gb.display.print('\x1A');
		
		
//		gb.display.setCursor(0, 0);
//		gb.display.setColor(BLACK, WHITE);
//		gb.display.print(gb.getCpuLoad());
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

void hookshot_draw_updown(int8_t x, int8_t y1, int8_t y2) {
	if (y1 > y2) {
		return hookshot_draw_updown(x, y2, y1);
	}
	gb.display.setColor(BLACK);
	for(; y1 <= y2; y1++) {
		if (y1 % 2) {
			gb.display.drawPixel(x - camera.x, y1 - camera.y);
		} else {
			gb.display.drawPixel(x - 1 - camera.x, y1 - camera.y);
			gb.display.drawPixel(x + 1 - camera.x, y1 - camera.y);
		}
	}
}

void hookshot_draw_leftright(int8_t x1, int8_t x2, int8_t y) {
	if (x1 > x2) {
		return hookshot_draw_leftright(x2, x1, y);
	}
	gb.display.setColor(BLACK);
	for(; x1 <= x2; x1++) {
		if (x1 % 2) {
			gb.display.drawPixel(x1 - camera.x, y - camera.y);
		} else {
			gb.display.drawPixel(x1 - camera.x, y + 1 - camera.y);
			gb.display.drawPixel(x1 - camera.x, y - 1- camera.y);
		}
	}
}

void hookshot_up(int8_t x, int8_t y) {
	if (y < 0) {
		return;
	}
	int8_t _y = y;
	int8_t _x = x;
	x += 4; // for animation
	bool grab = false;
	bool flying = false;
	int8_t y_prev = -1;
	while(1) {
		y--;
		if (y % 2) {
			camera.centerOn(_x + 4, y + 4); // _x as we don't want random jumping
			renderAll();
			hookshot_draw_updown(x, y, _y);
			while(!gb.update());
		}
		if (y <= 0) {
			break;
		}
		if (y_prev == (y / 8)) {
			continue;
		}
		uint8_t t = board.getTile(x / 8, y / 8);
		y_prev = y / 8;
		// check if we can just fly
		if (t < SPRITE_AFTER_HOOKSHOT_FLY) {
			continue;
		}
		// check if we pass over an up-wall and set the flying flag
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_RIGHT_UP && t < SPRITE_AFTER_HOOKSHOT_WALL_LEFT_UP) {
			flying = true;
			continue;
		}
		if (t < SPRITE_AFTER_HOOKSHOT_WALL_DOWN) {
			if (flying) {
				flying = false;
				continue;
			}
			break;
		}
		
		
		
		// check if we grab onto something
		if (t >= SPRITE_FIRST_HOOKSHOT && t < SPRITE_AFTER_HOOKSHOT) {
			grab = true;
		}
		break;
	}
	// revert the last step as we want to appear in front of the object
	// this also fixes a bug where you can hookshot into things
	y++;
	if (grab) {
		for (; _y >= y; _y--) {
			if (_y % 2) {
				player.moveY(_y);
				player.focus();
				renderAll();
				hookshot_draw_updown(x, y, _y);
				while(!gb.update());
			}
		}
		player.moveTo(_x, y);
		player.focus();
		return;
	}
	for(;_y > y; y++) {
		if (y % 2) {
			camera.centerOn(_x + 4, y + 4);
			renderAll();
			hookshot_draw_updown(x, y, _y);
			while(!gb.update());
		}
	}
}

void hookshot_right(int8_t x, int8_t y) {
	if (x >= 11*8) {
		return;
	}
	int8_t _y = y;
	int8_t _x = x;
	x += 7; // for checking
	y += 4; // for animation
	
	bool grab = false;
	bool flying = false;
	int x_prev = -1;
	while(1) {
		x++;
		if (x % 2) {
			camera.centerOn(x + 4, _y + 4); // _y as we don't want random jumping
			renderAll();
			hookshot_draw_leftright(x, _x + 7, y);
			while(!gb.update());
		}
		if (x >= 12*8) {
			break;
		}
		if (x_prev == (x / 8)) {
			continue;
		}
		uint8_t t = board.getTile(x / 8, y / 8);
		x_prev = x / 8;
		// check if we can just fly
		if (t < SPRITE_AFTER_HOOKSHOT_FLY) {
			continue;
		}
		// check if we pass over an up-wall and set the flying flag
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_RIGHT && t < SPRITE_AFTER_HOOKSHOT_WALL_RIGHT) {
			flying = true;
			continue;
		}
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_LEFT_UP && t < SPRITE_AFTER_HOOKSHOT_WALL_LEFT) {
			if (flying) {
				flying = false;
				continue;
			}
			break;
		}
		
		
		
		// check if we grab onto something
		if (t >= SPRITE_FIRST_HOOKSHOT && t < SPRITE_AFTER_HOOKSHOT) {
			grab = true;
		}
		break;
	}
	// revert the last step as we want to appear in front of the object
	// this also fixes a bug where you can hookshot into things
	x--;
	if (grab) {
		for (; _x <= (x - 7); _x++) {
			if (_x % 2) {
				player.moveX(_x);
				player.focus();
				renderAll();
				hookshot_draw_leftright(x, _x + 7, y);
				while(!gb.update());
			}
		}
		player.moveTo(x - 7, _y);
		player.focus();
		return;
	}
	for(;_x + 7 < x; x--) {
		if (x % 2) {
			camera.centerOn(x + 4, _y + 4);
			renderAll();
			hookshot_draw_leftright(x, _x + 7, y);
			while(!gb.update());
		}
	}
	player.focus();
}

void hookshot_down(int8_t x, int8_t y) {
	if (y >= 7*8) {
		return;
	}
	int8_t _y = y;
	int8_t _x = x;
	x += 4; // for animation
	y += 7; // for checking
	bool grab = false;
	bool flying = false;
	int8_t y_prev = -1;
	while(1) {
		y++;
		if (y % 2) {
			camera.centerOn(_x + 4, y + 4); // _x as we don't want random jumping
			renderAll();
			hookshot_draw_updown(x, y, _y + 7);
			while(!gb.update());
		}
		if (y >= 8*8) {
			break;
		}
		if (y_prev == (y / 8)) {
			continue;
		}
		uint8_t t = board.getTile(x / 8, y / 8);
		y_prev = y / 8;
		// check if we can just fly
		if (t < SPRITE_AFTER_HOOKSHOT_FLY) {
			continue;
		}
		// check if we pass over an up-wall and set the flying flag
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_DOWN && t < SPRITE_AFTER_HOOKSHOT_WALL_DOWN) {
			flying = true;
			continue;
		}
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_RIGHT_UP && t < SPRITE_AFTER_HOOKSHOT_WALL_LEFT_UP) {
			if (flying) {
				flying = false;
				continue;
			}
			break;
		}
		
		
		
		// check if we grab onto something
		if (t >= SPRITE_FIRST_HOOKSHOT && t < SPRITE_AFTER_HOOKSHOT) {
			grab = true;
		}
		break;
	}
	// revert the last step as we want to appear in front of the object
	// this also fixes a bug where you can hookshot into things
	y--;
	if (grab) {
		for (; _y <= (y - 7); _y++) {
			if (_y % 2) {
				player.moveY(_y);
				player.focus();
				renderAll();
				hookshot_draw_updown(x, y, _y + 7);
				while(!gb.update());
			}
		}
		player.moveTo(_x, y - 7);
		player.focus();
		return;
	}
	for(;_y + 7 < y; y--) {
		if (y % 2) {
			camera.centerOn(_x + 4, y + 4);
			renderAll();
			hookshot_draw_updown(x, y, _y + 7);
			while(!gb.update());
		}
	}
}

void hookshot_left(int8_t x, int8_t y) {
	if (x < 0) {
		return;
	}
	int8_t _y = y;
	int8_t _x = x;
	y += 4; // for animation
	
	bool grab = false;
	bool flying = false;
	int x_prev = -1;
	while(1) {
		x--;
		if (x % 2) {
			camera.centerOn(x + 4, _y + 4); // _y as we don't want random jumping
			renderAll();
			hookshot_draw_leftright(x, _x, y);
			while(!gb.update());
		}
		if (x <= 0) {
			break;
		}
		if (x_prev == (x / 8)) {
			continue;
		}
		uint8_t t = board.getTile(x / 8, y / 8);
		x_prev = x / 8;
		// check if we can just fly
		if (t < SPRITE_AFTER_HOOKSHOT_FLY) {
			continue;
		}
		// check if we pass over an up-wall and set the flying flag
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_LEFT_UP && t < SPRITE_AFTER_HOOKSHOT_WALL_LEFT) {
			flying = true;
			continue;
		}
		
		if (t >= SPRITE_FIRST_HOOKSHOT_WALL_RIGHT && t < SPRITE_AFTER_HOOKSHOT_WALL_RIGHT) {
			if (flying) {
				flying = false;
				continue;
			}
			break;
		}
		
		
		
		// check if we grab onto something
		if (t >= SPRITE_FIRST_HOOKSHOT && t < SPRITE_AFTER_HOOKSHOT) {
			grab = true;
		}
		break;
	}
	// revert the last step as we want to appear in front of the object
	// this also fixes a bug where you can hookshot into things
	x++;
	if (grab) {
		for (; _x >= x; _x--) {
			if (_x % 2) {
				player.moveX(_x);
				player.focus();
				renderAll();
				hookshot_draw_leftright(x, _x, y);
				while(!gb.update());
			}
		}
		player.moveTo(x, _y);
		player.focus();
		return;
	}
	for(;_x > x; x++) {
		if (x % 2) {
			camera.centerOn(x + 4, _y + 4);
			renderAll();
			hookshot_draw_leftright(x, _x, y);
			while(!gb.update());
		}
	}
	player.focus();
}

void hookshot(int8_t x, int8_t y, Direction dir) {
	switch(dir) {
		case Direction::up:
			hookshot_up(x, y);
			break;
		case Direction::right:
			hookshot_right(x, y);
			break;
		case Direction::down:
			hookshot_down(x, y);
			break;
		case Direction::left:
			hookshot_left(x, y);
			break;
	}
}
