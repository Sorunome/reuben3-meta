#ifndef _TEXT_H_
#define _TEXT_H_
#include <Arduino.h>

class Text {
	private:
		void drawBox(bool up);
	public:
		int8_t box(uint16_t i, bool up = false);
		int8_t boxPlayer(uint16_t i);
};

extern Text text;

#endif // _TEXT_H_
