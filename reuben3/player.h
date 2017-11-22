#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <Arduino.h>
#include "data/defines.h"
#include <Gamebuino-Meta.h>

enum class Direction : uint8_t {
	up = 0,
	right,
	down,
	left,
};

class Player {
	private:
		float x;
		float y;
		bool isWalkable(float dx, float dy);
		uint8_t events[(TOTAL_EVENTS + 7) / 8];
		Direction direction;
		void interact();
		bool visible = true;
		uint8_t lvl;
		uint16_t hp;
		uint16_t hp_max;
		uint8_t mp;
		uint8_t mp_max;
		uint16_t exp;
		uint16_t exp_next;
		uint16_t gold;
		uint16_t gold_max;
	public:
		uint8_t armor;
		uint8_t wait;
		uint8_t sword;
		uint8_t tradequest;
		uint8_t fright;
		uint8_t bombs;
		uint8_t bombs_max;
		void init();
		void hide();
		void show();
		void moveTo(int8_t _x, int8_t _y);
		void moveX(int8_t _x);
		void moveY(int8_t _y);
		float getX();
		float getY();
		void focus();
		void update();
		void render();
		void render(Image& img, int8_t dx, int8_t dy);
		
		bool isEvent(uint8_t e);
		void setEvent(uint8_t e);
		void clearEvent(uint8_t e);
		void addGold(uint16_t num);
};

extern Player player;

#endif // _PLAYER_H_