#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <Arduino.h>

class Player {
	private:
		float x = 0;
		float y = 0;
		bool isWalkable(float dx, float dy);
	public:
		void moveTo(int8_t _x, int8_t _y);
		void moveX(int8_t _x);
		void moveY(int8_t _y);
		void focus();
		void update();
		void render();
};

extern Player player;

#endif // _PLAYER_H_
