#include "board.h"
#include "camera.h"
#include <Gamebuino-Meta.h>
#include "sprites.h"

#include "depack.h"
#include "player.h"
#include "script.h"


#include "data/defines.h"
#include "data/maps.h"

const uint16_t mapsize_bytes = 12*8*2;

Image singleLine(gb.display.width(), 1, ColorMode::rgb565);

extern uint8_t decompression_buffer[];

void Board::setWorld(uint8_t _world) {
	worldId = _world;
}

void Board::load(uint8_t _world, uint8_t _map) {
	worldId = _world;
	load(_map);
}

void Board::load(uint8_t _map) {
	mapId = _map;
	load();
}

void Board::load() {
	uint8_t i = 0;
	while(1) {
		if (worlds[worldId].maps[i].id == mapId) {
			map = &worlds[worldId].maps[i];
			break;
		}
		i++;
	}
	aP_depack(map->block, decompression_buffer);
	const Dyn_Data* d = worlds[worldId].dyn;
	uint8_t* mapbuf = decompression_buffer + (mapsize_bytes*map->chunk);
	for(;;d++) {
		if (d->mapId == 0xFF) {
			return;
		}
		if (d->mapId != mapId) {
			continue;
		}
		if (!player.isEvent(d->eventId)) {
			continue;
		}
		mapbuf[d->offset] = d->tileLower;
		mapbuf[d->offset + 1] = d->tileUpper;
	}
}

void Board::postload() {
	memcpy(board, decompression_buffer + (mapsize_bytes*map->chunk), mapsize_bytes);
}

uint16_t Board::getTile(uint8_t x, uint8_t y) {
	return board[y*width + x];
}

void Board::setTile(uint8_t x, uint8_t y, uint16_t tile) {
	board[y*width + x] = tile;
}

void Board::scrollLeft() {
	mapId--;
	load();
	// scroll animation
	
	for (uint8_t i = 0; i < 12; i++) {
		camera.moveTo(camera.x + 2, camera.y);
		for (uint8_t y = 0; y < height; y++) {
			for (uint8_t x = 0; x < width; x++) {
				if (i <= x) {
					// old map
					sprites.setFrame(board[y*width + x - i]);
				} else {
					// new map
					uint16_t j = mapsize_bytes*map->chunk + (y*width + x + 12 - i)*2;
					sprites.setFrame(decompression_buffer[j] | (decompression_buffer[j + 1] << 8));
				}
				gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
			}
		}
		player.moveX(i*8 - max(0, min(i, 8)));
		player.render();
		while(!gb.update());
	}
	
	postload();
	player.moveX(width*8 - 8);
	player.focus();
}

void Board::scrollRight() {
	mapId++;
	load();
	// scroll animation
	
	for (int8_t i = 11; i >= 0; i--) {
		camera.moveTo(camera.x - 2, camera.y);
		for (uint8_t y = 0; y < height; y++) {
			for (uint8_t x = 0; x < width; x++) {
				if (i > x) {
					// old map
					sprites.setFrame(board[y*width + x + 12 - i]);
				} else {
					// new map
					uint16_t j = mapsize_bytes*map->chunk + (y*width + x - i)*2;
					sprites.setFrame(decompression_buffer[j] | (decompression_buffer[j + 1] << 8));
				}
				gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
			}
		}
		player.moveX(i*8 - max(0, min(i, 8)));
		player.render();
		while(!gb.update());
	}
	
	postload();
	player.moveX(0);
}

void Board::scrollUp() {
	mapId -= TILEMAPS_WIDTH;
	load();
	// scroll animation
	
	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t y = 0; y < height; y++) {
			for (uint8_t x = 0; x < width; x++) {
				if (i <= y) {
					// old map
					sprites.setFrame(board[(y - i)*width + x]);
				} else {
					// new map
					uint16_t j = mapsize_bytes*(map->chunk + 1) + ((y - i)*width + x)*2;
					sprites.setFrame(decompression_buffer[j] | (decompression_buffer[j + 1] << 8));
				}
				gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
			}
		}
		player.moveY(i*8 - max(0, min(i, 8)));
		player.render();
		while(!gb.update());
	}
	
	postload();
	player.moveY(height*8 - 8);
}

void Board::scrollDown() {
	mapId += TILEMAPS_WIDTH;
	load();
	// scroll animation
	
	for (int8_t i = 7; i >= 0; i--) {
		for (uint8_t y = 0; y < height; y++) {
			for (uint8_t x = 0; x < width; x++) {
				if (i > y) {
					// old map
					sprites.setFrame(board[(y + 8 - i)*width + x]);
				} else {
					// new map
					uint16_t j = mapsize_bytes*map->chunk + ((y - i)*width + x)*2;
					sprites.setFrame(decompression_buffer[j] | (decompression_buffer[j + 1] << 8));
				}
				gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
			}
		}
		player.moveY(i*8 - max(0, min(i, 8)));
		player.render();
		while(!gb.update());
	}
	
	postload();
	player.moveY(0);
}

void Board::postloadHouseAnimation() {
	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t y = 0; y < height; y++) {
			for (uint8_t x = 0; x < width; x++) {
				uint16_t j = mapsize_bytes*map->chunk + (y*width + x)*2;
				sprites.setFrame(decompression_buffer[j] | (decompression_buffer[j + 1] << 8));
				singleLine.drawImage(x*8 - camera.x, camera.y - i, sprites);
			}
			player.render(singleLine, 0, -y*8 - i);
			gb.display.drawImage(0, y*8 + i, singleLine);
		}
		while(!gb.update());
	}
	postload();
}


void Board::render() {
	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			sprites.setFrame(board[y*width + x]);
			gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
		}
	}
}

bool Board::runScript(uint8_t x, uint8_t y, uint8_t trigger) {
	uint8_t amount = 0;
	uint8_t i = 0;
	uint8_t offset = y*12 + x;
	
	while(1) {
		uint8_t id = worlds[worldId].actionTiles[i].mapId;
		if (id == mapId) {
			amount = worlds[worldId].actionTiles[i].amount;
			if (!amount) {
				return false;
			}
			const Actiontiles_LUT* lut = worlds[worldId].actionTiles[i].lut;
			for (i = 0; i < amount; i++) {
				if (lut[i].offset == offset) {
					script.run(lut[i].script);
					return true;
				}
			}
			return false;
		}
		if (id == 0xFF) {
			return false;
		}
		i++;
	}
	return false;
}

void Board::interact(uint8_t x, uint8_t y) {
	switch(getTile(x, y)) {
		case SPRITE_105: // shrub on grass
			setTile(x, y, SPRITE_1);
			return;
		case SPRITE_113: // rock on sand
			if (player.isEvent(EVENT_PICKAXE)) {
				setTile(x, y, SPRITE_308);
			}
			return;
		case SPRITE_376: // rock on darkgray
			if (player.isEvent(EVENT_PICKAXE)) {
				if (worldId == WORLD_OVERWORLD && mapId == TILEMAP_43) {
					// we place the path sprite
					setTile(x, y, SPRITE_348);
				} else {
					setTile(x, y, SPRITE_309);
				}
			}
			return;
		default:
			runScript(x, y, SCRIPT_ACTION);
	}
}


Board board;
