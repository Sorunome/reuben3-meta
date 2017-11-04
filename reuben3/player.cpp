#include "player.h"
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

float Player::getX() {
	return x;
}

float Player::getY() {
	return y;
}

void Player::focus() {
	camera.centerOn(x + 4, y + 4);
}

bool Player::isWalkable(float dx, float dy) {
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
	int8_t _dx = gb.buttons.repeat(BUTTON_RIGHT, 0) - gb.buttons.repeat(BUTTON_LEFT, 0);
	int8_t _dy = gb.buttons.repeat(BUTTON_DOWN, 0) - gb.buttons.repeat(BUTTON_UP, 0);
	if (!_dx && !_dy) {
		// nothing to do
		return;
	}
	float v = 1.5f;
	if (_dx && _dy) {
		v *= 0.707f; // 1/sqrt(2)
	}
	float dx = _dx*v;
	float dy = _dy*v;
	float _x = x;
	float _y = y;
	
	int8_t oldtile_x = (x + 2) / 8;
	int8_t oldtile_y = (y + 4) / 8;
	if (dx) {
		if (isWalkable(2 + dx, 4) && isWalkable(2 + dx, 7) && isWalkable(5 + dx, 4) && isWalkable(5 + dx, 7)) {
			x += dx;
		}
	}
	if (dy) {
		if (isWalkable(2, 4 + dy) && isWalkable(5, 4 + dy) && isWalkable(2, 7 + dy) && isWalkable(5, 7 + dy)) {
			y += dy;
		}
	}
	if (x < -2) {
		board.scrollLeft();
		return;
	}
	if (x >= 11*8 + 2) {
		board.scrollRight();
		return;
	}
	if (y < -2) {
		board.scrollUp();
		return;
	}
	
	if (y >= 7*8 + 2) {
		board.scrollDown();
		return;
	}
	
	int8_t newtile_x = (x + 2) / 8;
	int8_t newtile_y = (y + 4) / 8;
	
	if (oldtile_x != newtile_x || oldtile_y != newtile_y) {
		// check for scripts!
		board.runScript(newtile_x, newtile_y, SCRIPT_WALK);
	}
	
	focus();
}

void Player::render() {
	gb.display.drawImage(x - camera.x, y - camera.y, player_sprite);
}

void Player::render(Image& img, int8_t dx, int8_t dy) {
	img.drawImage(x - camera.x + dx, y - camera.y + dy, player_sprite);
}


bool Player::isEvent(uint8_t e) {
	uint8_t offset = e / 8;
	uint8_t index = e % 8;
	return (events[offset] >> index) & 1;
}

void Player::setEvent(uint8_t e) {
	uint8_t offset = e / 8;
	uint8_t index = e % 8;
	uint8_t mask = 1 << index;
	events[offset] |= mask;
}

void Player::clearEvent(uint8_t e) {
	uint8_t offset = e / 8;
	uint8_t index = e % 8;
	uint8_t mask = 1 << index;
	events[offset] &= ~mask;
}

Player player;
