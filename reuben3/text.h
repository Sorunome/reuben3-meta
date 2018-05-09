#ifndef _TEXT_H_
#define _TEXT_H_
#include <Arduino.h>

struct Strings_LUT {
	const uint16_t offset;
	const uint8_t chunk;
};

class Text {
	private:
		void drawBox(bool up);
		const uint8_t* const* decompressionLut;
		const Strings_LUT* stringLut;
		uint16_t i;
		bool hasOptions;
		uint8_t* textCursor;
	public:
		void init();
		void load(uint16_t i);
		int8_t box(uint16_t i, bool up = false);
		int8_t boxPlayer(uint16_t i);
		int8_t progress();
};

extern Text text;

#endif // _TEXT_H_
