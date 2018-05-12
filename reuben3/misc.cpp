#include "misc.h"

#include "board.h"
#include "player.h"
#include "camera.h"
#include "text.h"
#include "data/defines.h"
#include "battle.h"
#include "ambient.h"
#include "script.h"
#include <Gamebuino-Meta.h>
#include <utility/Misc.h> // pixel to rgb converters

void waitCycles(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
		ambient.update();
	}
}

bool waitCyclesButton(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		waitCycles(1);
		if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_LEFT) || gb.buttons.pressed(BUTTON_RIGHT)) {
			return true;
		}
	}
	return false;
}

void waitRelease() {
	while(1) {
		waitCycles(1);
		if (!gb.buttons.repeat(BUTTON_A, 0) && !gb.buttons.repeat(BUTTON_B, 0) && !gb.buttons.repeat(BUTTON_LEFT, 0) && !gb.buttons.repeat(BUTTON_RIGHT, 0)) {
			return;
		}
	}
}

void renderAll() {
	board.render();
	player.render();
//	gb.display.setCursor(0, 0);
//	gb.display.setColor(BLACK, WHITE);
//	gb.display.printf("%d:%d", gb.getCpuLoad(), gb.getFreeRam());
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
	0x02, 0x00,
	2,
	0x01,
	0x01,
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
	Image img(bomb_frames);
	for (uint8_t i = 0; i < 2*2 - 1; i++) {
		waitCycles(1);
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
		waitCycles(1);
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
			waitCycles(1);
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
				waitCycles(1);
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
			waitCycles(1);
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
			waitCycles(1);
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
				waitCycles(1);
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
			waitCycles(1);
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
			waitCycles(1);
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
				waitCycles(1);
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
			waitCycles(1);
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
			waitCycles(1);
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
				waitCycles(1);
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
			waitCycles(1);
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

void pushblock_up(int8_t x, int8_t y) {
	if (y <= 0) {
		return;
	}
	if (board.getTile(x, y-1) >= SPRITE_AFTER_WALK) {
		return;
	}
	board.eraseTile(x, y);
	for (uint8_t i = 0; i < 9; i++) {
		board.render();
		board.drawTile(x*8, y*8 - i, SPRITE_BLOCK);
		player.render();
		waitCycles(1);
	}
	board.setTile(x, y-1, SPRITE_BLOCK);
}

void pushblock_right(int8_t x, int8_t y) {
	if (x >= 11) {
		return;
	}
	if (board.getTile(x+1, y) >= SPRITE_AFTER_WALK) {
		return;
	}
	board.eraseTile(x, y);
	for (uint8_t i = 0; i < 9; i++) {
		board.render();
		board.drawTile(x*8 + i, y*8, SPRITE_BLOCK);
		player.render();
		waitCycles(1);
	}
	board.setTile(x + 1, y, SPRITE_BLOCK);
}

void pushblock_down(int8_t x, int8_t y) {
	if (y >= 7) {
		return;
	}
	if (board.getTile(x, y+1) >= SPRITE_AFTER_WALK) {
		return;
	}
	board.eraseTile(x, y);
	for (uint8_t i = 0; i < 9; i++) {
		board.render();
		board.drawTile(x*8, y*8 + i, SPRITE_BLOCK);
		player.render();
		waitCycles(1);
	}
	board.setTile(x, y+1, SPRITE_BLOCK);
}

void pushblock_left(int8_t x, int8_t y) {
	if (x <= 0) {
		return;
	}
	if (board.getTile(x-1, y) >= SPRITE_AFTER_WALK) {
		return;
	}
	board.eraseTile(x, y);
	for (uint8_t i = 0; i < 9; i++) {
		board.render();
		board.drawTile(x*8 - i, y*8, SPRITE_BLOCK);
		player.render();
		waitCycles(1);
	}
	board.setTile(x-1, y, SPRITE_BLOCK);
}

void pushblock(int8_t x, int8_t y, Direction dir) {
	switch(dir) {
		case Direction::up:
			pushblock_up(x, y);
			break;
		case Direction::right:
			pushblock_right(x, y);
			break;
		case Direction::down:
			pushblock_down(x, y);
			break;
		case Direction::left:
			pushblock_left(x, y);
			break;
	}
	board.runScript(x, y, SCRIPT_PUSHBLOCK);
}



bool shop(uint16_t ask, uint16_t price, bool bottle) {
	if (!text.boxPlayer(ask)) {
		return false;
	}
	
	if (bottle && !player.isCurItemBottle()) {
		text.boxPlayer(STRING_NEED_BOTTLE);
		return false;
	}
	
	if (!player.payGold(price)) {
		text.boxPlayer(STRING_NOT_ENOUGH_GOLD);
		return false;
	}
	
	text.boxPlayer(STRING_SHOP_THANKS);
	return true;
}


void fade_to_white(void (*r)(void)) {
	const uint8_t steps = 20;
	Color palette[16];
	gb.display.setPalette(palette);
	
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)Gamebuino_Meta::defaultColorPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r += (0xFF - rgb.r)*i / steps;
			rgb.g += (0xFF - rgb.g)*i / steps;
			rgb.b += (0xFF - rgb.b)*i / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		(*r)();
		waitCycles(1);
	}
	
	gb.display.setPalette(Gamebuino_Meta::defaultColorPalette);
}

void fade_to_white() {
	fade_to_white(&renderAll);
}

void fade_from_white(void (*r)(void)) {
	const uint8_t steps = 20;
	Color palette[16];
	gb.display.setPalette(palette);
	
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)Gamebuino_Meta::defaultColorPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r += (0xFF - rgb.r)*(steps - i) / steps;
			rgb.g += (0xFF - rgb.g)*(steps - i) / steps;
			rgb.b += (0xFF - rgb.b)*(steps - i) / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		(*r)();
		waitCycles(1);
	}
	gb.display.setPalette(Gamebuino_Meta::defaultColorPalette);
}

void fade_from_white() {
	fade_from_white(&renderAll);
}


void fade_to_black(void (*r)(void)) {
	const uint8_t steps = 20;
	Color palette[16];
	gb.display.setPalette(palette);
	
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)Gamebuino_Meta::defaultColorPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r = rgb.r*(steps - i) / steps;
			rgb.g = rgb.g*(steps - i) / steps;
			rgb.b = rgb.b*(steps - i) / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		(*r)();
		waitCycles(1);
	}
	
	gb.display.setPalette(Gamebuino_Meta::defaultColorPalette);
}

void fade_to_black() {
	fade_to_black(&renderAll);
}

void fade_from_black(void (*r)(void)) {
	const uint8_t steps = 20;
	Color palette[16];
	gb.display.setPalette(palette);
	
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)Gamebuino_Meta::defaultColorPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r = rgb.r*i / steps;
			rgb.g = rgb.g*i / steps;
			rgb.b = rgb.b*i / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		(*r)();
		waitCycles(1);
	}
	gb.display.setPalette(Gamebuino_Meta::defaultColorPalette);
}

void fade_from_black() {
	fade_from_black(&renderAll);
}


void renderBattleBar() {
	battle.render();
	battle.renderBar();
	battle.renderHpMp();
}

void renderBattle() {
	battle.render();
}


void battleInstructions() {
	if (text.boxPlayer(STRING_PERSON_DOGGLEN_BATTLE_INSTRUCTIONS_ASK)) {
		return;
	}
	fade_to_white();
	battle.load(ENEMY_MOUSE);
	battle.p.wait = battle.p.curwait = 24;
	fade_from_white(&renderBattleBar);
	renderBattleBar();
	
	waitCycles(20);
	text.box(STRING_PERSON_DOGGLEN_BATTLE_INSTRUCTIONS_BAR, false);
	battle.load(ENEMY_MOUSE);
	battle.p.wait = battle.p.curwait = 24;
	renderBattleBar();
	while(battle.p.curwait--) {
		battle.renderBar();
		waitCycles(1);
	}
	
	battle.render();
	battle.renderMenu();
	battle.renderHpMp();
	waitCycles(20);
	text.box(STRING_PERSON_DOGGLEN_BATTLE_INSTRUCTIONS_ATTACK_MENU, false);
	battle.load(ENEMY_MOUSE);
	battle.render();
	battle.renderMenu();
	battle.renderHpMp();
	waitCycles(20);
	battle.render();
	battle.playerAttackAnimation();
	battle.drawText(52, 16, "42");
	
	waitCycles(45);
	fade_to_white(&renderBattle);
	fade_from_white();
	text.boxPlayer(STRING_PERSON_DOGGLEN_BATTLE_INSTRUCTIONS_DONE);
}


const uint8_t maruEnemies1[] = {
	ENEMY_MOUSE,
	ENEMY_GECKO,
	ENEMY_FROG,
	ENEMY_SMALL_SPIDERS,
	ENEMY_MOUNTAIN_BEE,
	ENEMY_MOUNTAIN_SNAKE,
	ENEMY_MOUNTAIN_BEE,
	0xFF,
};
const uint8_t maruEnemies2[] = {
	ENEMY_WEIRD_CATERPILLAR_THING,
	ENEMY_AIR_SNAKE_THING,
	ENEMY_SLIME_BLOB,
	ENEMY_EYEBALL_OCTOPUS,
	0xFF,
};
const uint8_t maruEnemies3[] = {
	ENEMY_EYEBALLS_1_2,
	ENEMY_EYEBALLS_2_2,
	ENEMY_PAST_CASTLE_1,
	ENEMY_PAST_CASTLE_2,
	0xFF,
};
const uint8_t maruEnemies4[] = {
	ENEMY_CACTUS,
	ENEMY_CAMEL,
	ENEMY_DESERTBUG,
	ENEMY_BIRDEYE_THING_2,
	0xFF,
};
const uint8_t* maruEnemyLUT[] = {
	maruEnemies1,
	maruEnemies2,
	maruEnemies3,
	maruEnemies4,
};

void increaseSpeedArena() {
	text.boxPlayer(STRING_MARU_INTRO);
	uint8_t reply;
	uint16_t price = 0;
	uint8_t new_wait;
	uint8_t i;
	if (player.wait > 200) {
		reply = text.boxPlayer(STRING_MARU_50GOLD);
		price = 50;
		new_wait = 200;
		i = 0;
	} else if (player.wait > 150) {
		reply = text.boxPlayer(STRING_MARU_100GOLD);
		price = 100;
		new_wait = 150;
		i = 1;
	} else if (player.wait > 100) {
		reply = text.boxPlayer(STRING_MARU_300GOLD);
		price = 300;
		new_wait = 100;
		i = 2;
	} else if (player.wait > 50) {
		reply = text.boxPlayer(STRING_MARU_500GOLD);
		price = 500;
		new_wait = 50;
		i = 3;
	} else {
		text.boxPlayer(STRING_MARU_NOTEACH);
		return;
	}
	if (!reply) {
		return;
	}
	if (!player.payGold(price)) {
		text.boxPlayer(STRING_NOT_ENOUGH_GOLD);
		return;
	}
	
	if (text.boxPlayer(STRING_MARU_THANKS_BUYING)) {
		while(text.boxPlayer(STRING_MARU_RULES));
	}
	
	uint16_t player_hp_backup = player.getHp();
	player.addHp(player.getHpMax());
	
	uint8_t j = 0;
	bool dead = false;
	while(maruEnemyLUT[i][j] != 0xFF) {
		text.boxPlayer(STRING_MARU_ENEMY);
		dead = !battle.fight(maruEnemyLUT[i][j], true);
		renderAll();
		waitCycles(1);
		if (dead) {
			break;
		}
		j++;
	}
	
	player.setHp(player_hp_backup);
	if (dead) {
		text.boxPlayer(STRING_MARU_DEAD);
		return;
	}
	player.wait = new_wait;
	text.boxPlayer(STRING_MARU_SUCCESS);
}


void searchquestPerson() {
	if (!player.isEvent(EVENT_SEARCH_QUEST)) {
		if (!text.boxPlayer(STRING_PERSON_AERILON_MOVELESS)) {
			player.setEvent(EVENT_SEARCH_QUEST);
		}
		return;
	}
	
	if (player.fright >= 1 && player.getGoldMax() < 200) {
		text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_WALLET_200);
		player.setGoldMax(200);
		return;
	}
	
	if (player.fright >= 3 && player.getGoldMax() < 500) {
		text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_WALLET_500);
		player.setGoldMax(500);
		return;
	}
	
	if (player.fright >= 5 && player.getGoldMax() < 999) {
		text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_WALLET_999);
		player.setGoldMax(999);
		return;
	}
	
	if (player.fright >= 7 && player.getBombsMax() < 30) {
		text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_BOMBS_30);
		player.setBombsMax(30);
		return;
	}
	
	if (player.fright >= 11) {
		if (player.getBombsMax() < 99) {
			text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_BOMBS_99);
			player.setBombsMax(99);
		} else {
			text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_DONE);
		}
		return;
	}
	
	text.boxPlayer(STRING_PERSON_AERILON_MOVELESS_THANKS);
}


void raft() {
	
}


void outro() {
	static const uint8_t cursorXStart = (80 - (16*4)) / 2;
	fade_to_black();
	waitRelease();
	gb.display.setColor(WHITE);
	gb.display.setCursor(cursorXStart, (64 - (5*6)) / 2);
	text.load(STRING_OUTRO);
	while (int8_t res = text.progress(false)) {
		if (res == 1) {
			gb.display.write('\n');
			gb.display.setCursorX(cursorXStart);
		} else {
			break;
		}
	}
	waitCycles(30);
	
	player.win();
}

void credits() {
	gb.display.fill(BLACK);
	waitCycles(1);
	waitRelease();
	static const uint8_t cursorXStart = 4;
	gb.display.setColor(WHITE);
	gb.display.setCursor(cursorXStart, 8);
	text.load(STRING_CREDITS);
	while (int8_t res = text.progress(false)) {
		if (res == 1) {
			gb.display.write('\n');
			gb.display.setCursorX(cursorXStart);
		} else if (res == 2) {
			waitCycles(30);
			gb.display.fill(BLACK);
			gb.display.setColor(WHITE);
			gb.display.setCursor(cursorXStart, 8);
		}
	}
}
