#ifndef _BOARD_H_
#define _BOARD_H_
#include <Arduino.h>

struct Tilemaps_Data {
	const uint8_t id;
	const uint8_t header;
	const uint8_t chunk;
	const uint8_t* block;
};

struct Actiontiles_LUT {
	const uint8_t offset;
	const uint8_t* script;
};

struct Actiontiles_LUTLUT {
	const uint8_t mapId;
	const uint8_t amount;
	const Actiontiles_LUT* lut;
};

struct World_Data {
	const Tilemaps_Data* maps;
	const uint8_t a;
	const Actiontiles_LUTLUT* actionTiles;
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
		void postloadHouseAnimation();
		void setWorld(uint8_t _world);
		void load(uint8_t _world, uint8_t _map);
		void load(uint8_t _map);
		void postload();
		void render();
		uint16_t getTile(uint8_t x, uint8_t y);
		void runScript(uint8_t x, uint8_t y, uint8_t trigger);
};

extern Board board;

#endif // _BOARD_H_
