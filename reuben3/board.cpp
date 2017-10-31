#include "board.h"
#include "camera.h"
#include <Gamebuino-Meta.h>
#include "sprites.h"

#include "depack.h"
#include "player.h"


#include "data/defines.h"
#include "data/maps.h"

const uint16_t mapsize_bytes = 12*8*2;

uint8_t decompression_buffer[mapsize_bytes*8];

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
}

void Board::postload() {
	memcpy(board, decompression_buffer + (mapsize_bytes*map->chunk), mapsize_bytes);
}

uint16_t Board::getTile(uint8_t x, uint8_t y) {
	return board[y*width + x];
}

void Board::scrollLeft() {
	mapId--;
	player.moveX(0);
	load();
	// scroll animation
	
	for (uint8_t i = 0; i < 12; i++) {
		while(!gb.update());
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
		player.moveX(i*8);
		player.render();
		camera.moveTo(camera.x + 2, camera.y);
	}
	while(!gb.update());
	
	postload();
	player.moveX(width*8 - 8);
	player.focus();
}

void Board::scrollRight() {
	mapId++;
	player.moveX(width*8 - 8);
	load();
	// scroll animation
	
	for (int8_t i = 11; i >= 0; i--) {
		while(!gb.update());
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
		player.moveX(i*8);
		player.render();
		camera.moveTo(camera.x - 2, camera.y);
	}
	while(!gb.update());
	
	postload();
	player.moveX(0);
}

void Board::scrollUp() {
	mapId -= TILEMAPS_WIDTH;
	player.moveY(0);
	load();
	// scroll animation
	
	for (uint8_t i = 0; i < 8; i++) {
		while(!gb.update());
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
		player.moveY(i*8);
		player.render();
	}
	while(!gb.update());
	
	postload();
	player.moveY(height*8 - 8);
}

void Board::scrollDown() {
	mapId += TILEMAPS_WIDTH;
	load();
	// scroll animation
	
	for (int8_t i = 7; i >= 0; i--) {
		while(!gb.update());
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
		player.moveY(i*8);
		player.render();
	}
	while(!gb.update());
	
	postload();
	player.moveY(0);
}


void Board::render() {
	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			sprites.setFrame(board[y*width + x]);
			gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
		}
	}
}


Board board;
