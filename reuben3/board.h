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

struct Dyn_Data {
	const uint8_t mapId;
	const uint8_t eventId;
	const uint8_t offset;
	const uint8_t tileLower;
	const uint8_t tileUpper;
};

struct World_Data {
	const Tilemaps_Data* maps;
	const Dyn_Data* dyn;
	const Actiontiles_LUTLUT* actionTiles;
};

class Board {
	private:
		static const uint8_t width = 12;
		static const uint8_t height = 8;
		uint16_t board[width*height];
		uint16_t board_backup[width*height];
		uint8_t worldId;
		uint8_t mapId;
		uint8_t areaId;
		const Tilemaps_Data* map;
		bool haveEnemies;
		uint8_t mapTilemap;
		uint16_t getDefaultTile();
	public:
		bool getHaveEnemies();
		uint8_t getWorldId();
		uint8_t getMapId();
		uint8_t getAreaId();
		void scrollLeft();
		void scrollRight();
		void scrollUp();
		void scrollDown();
		void postloadHouseAnimation();
		void setWorld(uint8_t _world);
		void load(uint8_t _world, uint8_t _map);
		void load(uint8_t _map);
		void load();
		void postload();
		void transitionPortal(uint8_t portal_x, uint8_t portal_y, uint8_t _world, uint8_t _map, uint8_t player_x, uint8_t player_y);
		
		void saveBackup();
		void loadBackup(uint8_t _world, uint8_t _map);
		
		void render();
		uint16_t getTile(uint8_t x, uint8_t y);
		void eraseTile(uint8_t x, uint8_t y);
		void setTile(uint8_t x, uint8_t y, uint16_t tile);
		void drawTile(int8_t x, int8_t y, uint16_t tile);
		int8_t interact(uint8_t x, uint8_t y);
		int8_t runScript(uint8_t x, uint8_t y, uint8_t trigger);
		
		void setMapTilemap(uint8_t m);
		uint8_t getMapTilemap();
};

extern Board board;

#endif // _BOARD_H_
