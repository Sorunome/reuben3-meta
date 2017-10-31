#ifndef _BOARD_H_
#define _BOARD_H_
#include <Arduino.h>

struct Tilemaps_Data {
	const uint8_t id;
	const uint8_t header;
	const uint8_t chunk;
	const uint8_t* block;
};

struct World_Data {
	const Tilemaps_Data* maps;
	const uint8_t a;
	const uint8_t b;
};

class Board {
	private:
		static const uint8_t width = 12;
		static const uint8_t height = 8;
		uint16_t board[width*height];
		uint8_t worldId;
		uint8_t mapId;
		const Tilemaps_Data* map;
		void load();
	public:
		void scrollLeft();
		void scrollRight();
		void scrollUp();
		void scrollDown();
		void load(uint8_t _world, uint8_t _map);
		void load(uint8_t _map);
		void postload();
		void render();
		uint16_t getTile(uint8_t x, uint8_t y);
};

extern Board board;

#endif // _BOARD_H_
