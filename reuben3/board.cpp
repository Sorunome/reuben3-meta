#include "board.h"
#include "camera.h"
#include <Gamebuino-Meta.h>
#include "sprites.h"

#include "depack.h"

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

#include "data/defines.h"
#include "data/maps.h"

uint8_t decompression_buffer[12*8*8*2];

void Board::load() {
	aP_depack(worlds[0].maps[0].block, decompression_buffer);
	memcpy(board, decompression_buffer, width*height*2);
}
void Board::render() {
	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			uint16_t i = board[y*width + x];
			sprites.setFrame(i);
			gb.display.drawImage(x*8 - camera.x, y*8 - camera.y, sprites);
		}
	}
}

Board board;
