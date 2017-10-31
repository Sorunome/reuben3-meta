#include "player.h"
#include <Gamebuino-Meta.h>
#include "camera.h"
#include "board.h"
#include "data/defines.h"

const uint8_t player_sprite_data[] = {
	8, 8,
	0xF0, 0xF0, 0xF0, 0xF0,
	0x0F, 0x0F, 0x0F, 0x0F,
	0xF0, 0xF0, 0xF0, 0xF0,
	0x0F, 0x0F, 0x0F, 0x0F,
	0xF0, 0xF0, 0xF0, 0xF0,
	0x0F, 0x0F, 0x0F, 0x0F,
	0xF0, 0xF0, 0xF0, 0xF0,
	0x0F, 0x0F, 0x0F, 0x0F,
};

Image player_sprite(player_sprite_data);

void Player::moveTo(int8_t _x, int8_t _y) {
	x = _x;
	y = _y;
}

void Player::moveX(int8_t _x) {
	x = _x;
}

void Player::moveY(int8_t _y) {
	y = _y;
}

void Player::focus() {
	camera.centerOn(x + 4, y + 4);
}

bool Player::isWalkable(int8_t dx, int8_t dy) {
	int8_t _x = (x + dx) / 8;
	int8_t _y = (y + dy) / 8;
	if (_x < 0 || _y < 0 || _x > 11 || _y > 7) {
		return true;
	}
	uint16_t tile = board.getTile(_x, _y);
	return tile < SPRITE_AFTER_WALK;
}

void Player::update() {
	/*
	 *  The player has a 4x4 hitbox on the bottom
	 */
	
	if (gb.buttons.repeat(BUTTON_LEFT, 0)) {
		if (isWalkable(1, 4) && isWalkable(1, 7)) {
			x--;
			if (x < -2) {
				board.scrollLeft();
			}
		}
	}
	if (gb.buttons.repeat(BUTTON_RIGHT, 0)) {
		if (isWalkable(6, 4) && isWalkable(6, 7)) {
			x++;
			if (x >= 11*8 + 2) {
				board.scrollRight();
			}
		}
	}
	if (gb.buttons.repeat(BUTTON_UP, 0)) {
		if (isWalkable(2, 3) && isWalkable(5, 3)) {
			y--;
			if (y < -2) {
				board.scrollUp();
			}
		}
	}
	if (gb.buttons.repeat(BUTTON_DOWN, 0)) {
		if (isWalkable(2, 8) && isWalkable(5, 8)) {
			y++;
			if (y >= 7*8 + 2) {
				board.scrollDown();
			}
		}
	}
	focus();
}

void Player::render() {
	gb.display.drawImage(x - camera.x, y - camera.y, player_sprite);
}

Player player;
