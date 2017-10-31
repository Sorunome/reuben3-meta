#ifndef _BOARD_H_
#define _BOARD_H_
#include <Arduino.h>

class Board {
	private:
		static const uint8_t width = 12;
		static const uint8_t height = 8;
		uint16_t board[width*height];
	public:
		void load();
		void render();
};

extern Board board;

#endif // _BOARD_H_
