#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <Arduino.h>
#include "data/defines.h"
#include <Gamebuino-Meta.h>

class Player {
	private:
		float x = 0;
		float y = 0;
		bool isWalkable(float dx, float dy);
		uint8_t events[(TOTAL_EVENTS + 7) / 8];
	public:
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
};

extern Player player;

#endif // _PLAYER_H_
