#include "player.h"
#include "camera.h"
#include "board.h"
#include "data/defines.h"
#include "misc.h"
#include "text.h"

const uint8_t reuben_sprite_data[] = {
	8, 9,
	// up
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0xCC, 0xCC, 0xC0,
	0x0C, 0xCC, 0xCC, 0xC0,
	0xE0, 0x0C, 0xC0, 0x0E,
	0xE0, 0xD0, 0x0D, 0x0E,
	0x0D, 0xDD, 0xDD, 0xD0,
	0x0D, 0xDD, 0xDD, 0xD0,
	0x00, 0x00, 0xDD, 0x00,
	
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0xCC, 0xCC, 0xC0,
	0x0C, 0xCC, 0xCC, 0xC0,
	0xE0, 0x0C, 0xC0, 0x0E,
	0xE0, 0xD0, 0x0D, 0x0E,
	0x0D, 0xDD, 0xDD, 0xD0,
	0x0D, 0xDD, 0xDD, 0xD0,
	0x00, 0xDD, 0x00, 0x00,
	
	//right
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0xCC, 0x00, 0x00,
	0x0C, 0x00, 0x0F, 0x0E,
	0xE0, 0x0F, 0x0F, 0xF0,
	0x00, 0xD0, 0xDF, 0x0E,
	0x0D, 0xD0, 0xD0, 0xEE,
	0x0D, 0xDD, 0x0E, 0xEE,
	0x00, 0xDD, 0x0E, 0xEE,
	
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0xCC, 0x00, 0x00,
	0x0C, 0x00, 0x0F, 0x0E,
	0xE0, 0x0F, 0x0F, 0xF0,
	0x00, 0xD0, 0xFF, 0x0E,
	0x0D, 0x0D, 0x00, 0xEE,
	0x0D, 0xD0, 0xD0, 0xEE,
	0x0D, 0x00, 0xDD, 0x0E,
	
	// down
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0x00, 0x00, 0xC0,
	0x00, 0x0F, 0xF0, 0x00,
	0x0F, 0x0F, 0xF0, 0xF0,
	0xE0, 0xFF, 0xFF, 0x0E,
	0x0D, 0xDD, 0xDD, 0x00,
	0x0D, 0x0D, 0xDD, 0x00,
	0x00, 0x00, 0xDD, 0x00,
	
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x0C, 0x00, 0x00, 0xC0,
	0x00, 0x0F, 0xF0, 0x00,
	0x0F, 0x0F, 0xF0, 0xF0,
	0xE0, 0xFF, 0xFF, 0x0E,
	0x0D, 0x0D, 0xDD, 0xD0,
	0x00, 0xDD, 0xD0, 0xD0,
	0x00, 0xDD, 0x00, 0x00,
	
	// left
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x00, 0x00, 0xCC, 0xC0,
	0xE0, 0xF0, 0x00, 0xC0,
	0x0F, 0xF0, 0xF0, 0x0E,
	0xE0, 0xFF, 0x0D, 0x00,
	0xEE, 0x00, 0xD0, 0xD0,
	0xEE, 0x0D, 0x0D, 0xD0,
	0xE0, 0xDD, 0x00, 0xD0,
	
	0xEE, 0x00, 0x00, 0xEE,
	0xE0, 0xCC, 0xCC, 0x0E,
	0x00, 0x00, 0xCC, 0xC0,
	0xE0, 0xF0, 0x00, 0xC0,
	0x0F, 0xF0, 0xF0, 0x0E,
	0xE0, 0xFD, 0x0D, 0x00,
	0xEE, 0x0D, 0x0D, 0xD0,
	0xEE, 0xE0, 0xDD, 0xD0,
	0xEE, 0xE0, 0xDD, 0x00,
};

Image reuben_sprite(reuben_sprite_data, 8, 0);

void Player::init() {
	reuben_sprite.setTransparentColor(INDEX_PINK);
	
	render_cycle = 0;
	
	memset(events, 0, sizeof(events));
	memset(bottles, 0, sizeof(bottles));
	x = 8*8;
	y = 3*8;
	direction = Direction::down;
	visible = true;
	cur_item = I_ITEM_NONE;
	lvl = 1;
	hp = 120;
	hp_max = 120;
	mp = 23;
	mp_max = 23;
	exp = 0;
	exp_next = 5;
	gold = 0;
	gold_max = 100;
	armor = ARMOR_WOUNDS;
	wait = 0xFF;
	sword = WEAPON_NONE;
	tradequest = TRADEQUEST_NONE;
	fright = 0;
	bombs = 10;
	bombs_max = 10;
	focus();
}

uint8_t Player::getLvl() {
	return lvl;
}

uint16_t Player::getHp() {
	return hp;
}

uint16_t Player::getHpMax() {
	return hp_max;
}

uint8_t Player::getMp() {
	return mp;
}

uint8_t Player::getMpMax() {
	return mp_max;
}

uint16_t Player::getGold() {
	return gold;
}

void Player::hide() {
	visible = false;
}

void Player::show() {
	visible = true;
}

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

void Player::item() {
	if (!gb.buttons.released(BUTTON_B)) {
		return;
	}
	switch(cur_item) {
		case I_ITEM_BOTTLE1:
		case I_ITEM_BOTTLE2:
		case I_ITEM_BOTTLE3:
		case I_ITEM_BOTTLE4:
		{
			// let's bottle this up
			bool up = player.getY() > 28;
			switch(getCurBottle()) {
				case Bottle::empty:
					break;
				case Bottle::dirty_water:
					if (text.box(STRING_BOTTLE_ASKDIRTYWATER_DRINK, up)) {
						addHp(150);
						emptyCurBottle();
					}
					break;
				case Bottle::water:
					if (text.box(STRING_BOTTLE_ASKWATER_DRINK, up)) {
						addHp(300);
						emptyCurBottle();
					}
					break;
				case Bottle::herb:
					if (text.box(STRING_BOTTLE_ASKHERB_DRINK, up)) {
						addHp(600);
						emptyCurBottle();
					}
					break;
				case Bottle::potion:
					if (text.box(STRING_BOTTLE_ASKPOTION_DRINK, up)) {
						addHp(999);
						emptyCurBottle();
					}
					break;
				case Bottle::ginseng:
					if (text.box(STRING_BOTTLE_ASKGINSENG_DRINK, up)) {
						addMp(50);
						emptyCurBottle();
					}
					break;
				case Bottle::elixir:
					if (text.box(STRING_BOTTLE_ASKELIXIR_DRINK, up)) {
						hp = hp_max;
						mp = mp_max;
						emptyCurBottle();
					}
					break;
			}
			return;
		}
		case I_ITEM_HOOKSHOT:
			hookshot(x, y, direction);
			return;
		case I_ITEM_BOMB:
			bomb(x + 3, y + 5);
			return;
	}
}

void Player::interact() {
	if (!gb.buttons.released(BUTTON_A)) {
		return;
	}
	// we check for the tile 2 pixels away, that should account for movement offsets
	// inside the lower 4x4 hitbox, though
	// more like, at the edges of it
	// so edgy
	
	int8_t _x = x;
	int8_t _y = y;
	if (direction == Direction::up) {
		_x += 3;
		//_y += 0;
	} else if (direction == Direction::right) {
		_x += 7;
		_y += 5;
	} else if (direction == Direction::down) {
		_x += 3;
		_y += 9;
	} else { // left
		//_x += 0;
		_y += 5;
	}
	_x /= 8;
	_y /= 8;
	if (board.getTile(_x, _y) >= SPRITE_AFTER_SWIM) {
		int8_t res = board.interact(_x, _y);
	}
}

void Player::update() {
	/*
	 *  The player has a 4x4 hitbox on the bottom
	 */
	int8_t _dx = gb.buttons.repeat(BUTTON_RIGHT, 0) - gb.buttons.repeat(BUTTON_LEFT, 0);
	int8_t _dy = gb.buttons.repeat(BUTTON_DOWN, 0) - gb.buttons.repeat(BUTTON_UP, 0);
	if (!_dx && !_dy) {
		// nothing to do
		interact();
		item();
		return;
	}
	
	render_cycle++;
	if (render_cycle >= 8) {
		render_cycle = 0;
	}
	
	float v = 1.5f;
	if (_dx && _dy) {
		v *= 0.707f; // 1/sqrt(2)
	}
	float dx = _dx*v;
	float dy = _dy*v;
	
	int8_t oldtile_x = (x + 2) / 8;
	int8_t oldtile_y = (y + 4) / 8;
	if (dx) {
		if (dx > 0) {
			direction = Direction::right;
		} else {
			direction = Direction::left;
		}
		if (isWalkable(2 + dx, 4) && isWalkable(2 + dx, 7) && isWalkable(5 + dx, 4) && isWalkable(5 + dx, 7)) {
			x += dx;
		}
	}
	if (dy) {
		if (dy > 0) {
			direction = Direction::down;
		} else {
			direction = Direction::up;
		}
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
	
	int8_t newtile_x = (x + 2 + 2) / 8;
	int8_t newtile_y = (y + 4 + 2) / 8;
	
	// check for scripts!
	board.runScript(newtile_x, newtile_y, SCRIPT_WALK);
	
	focus();
	
	// now let's check for actions
	interact();
	item();
}

void Player::setAnimationFrame() {
	reuben_sprite.setFrame(((uint8_t)direction)*2 + (render_cycle < 4));
}

void Player::render() {
	if (!visible) {
		return;
	}
	setAnimationFrame();
	gb.display.drawImage(x - camera.x, y - camera.y - 1, reuben_sprite);
}

void Player::render(Image& img, int8_t dx, int8_t dy) {
	if (!visible) {
		return;
	}
	setAnimationFrame();
	img.drawImage(x - camera.x + dx, y - camera.y + dy - 1, reuben_sprite);
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

void Player::addHp(uint16_t num) {
	// we temporary put hp to a 32-bit var to prevent overflows
	uint32_t _hp = hp;
	_hp += num;
	if (_hp > hp_max) {
		_hp = hp_max;
	}
	hp = _hp;
}

void Player::addMp(uint8_t num) {
	// we temporary put mp to a 16-bit var to prevent overflows
	uint16_t _mp = mp;
	_mp += num;
	if (_mp > mp_max) {
		_mp = mp_max;
	}
	mp = _mp;
}

void Player::addGold(uint16_t num) {
	// we temporary put gold to a 32-bit var to prevent overflows
	uint32_t _gold = gold;
	_gold += num;
	if (_gold > gold_max) {
		_gold = gold_max;
	}
	gold = _gold;
}

uint16_t Player::damage(uint16_t dmg) {
	if (dmg > hp) {
		return hp = 0;
	}
	return hp -= dmg;
}

uint8_t Player::useMp(uint8_t amount) {
	if (amount > mp) {
		return mp;
	}
	return mp -= amount;
}

uint8_t Player::getCurItem() {
	return cur_item;
}

bool Player::isCurItemBottle() {
	return cur_item >= I_ITEM_BOTTLE1 && cur_item <= I_ITEM_BOTTLE4;
}

void Player::setCurItem(uint8_t i) {
	cur_item = i;
}

void Player::setCurBottle(Bottle b) {
	if (!isCurItemBottle()) {
		return;
	}
	bottles[cur_item - 1] = b;
}

Bottle Player::getCurBottle() {
	if (!isCurItemBottle()) {
		return Bottle::empty;
	}
	return bottles[cur_item - 1];
}

Bottle Player::getBottle(uint8_t i) {
	if (i >= 4) {
		return Bottle::empty;
	}
	return bottles[i];
}

void Player::emptyCurBottle() {
	setCurBottle(Bottle::empty);
}

Player player;
