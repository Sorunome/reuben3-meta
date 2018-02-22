#ifndef _AMBIENT_H_
#define _AMBIENT_H_
#include <Gamebuino-Meta.h>

class Ambient {
	private:
		uint8_t base_c[3];
		uint8_t cur_c[3];
		uint8_t goto_c[3];
		uint8_t back_c[3];
		uint8_t base_r, base_g, base_b;
		uint8_t cur_r, cur_g, cur_b;
		uint8_t goto_r, goto_g, goto_b;
		Color c[8];
		uint8_t curAmbient = 0xFF;
		uint8_t maxRandom;
		void setGotoColor();
		void flowColor();
	public:
		void on();
		void off();
		void update();
		void setArea(uint8_t a);
};

extern Ambient ambient;

#endif // _AMBIENT_H_
