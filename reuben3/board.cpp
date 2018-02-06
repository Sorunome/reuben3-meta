#include "board.h"
#include "camera.h"
#include <Gamebuino-Meta.h>
#include <utility/Misc.h> // pixel to rgb converters
#include "sprites.h"

#include "depack.h"
#include "player.h"
#include "script.h"
#include "text.h"
#include "misc.h"

#include "data/defines.h"
#include "data/maps.h"

const uint16_t mapsize_bytes = 12*8*2;

Image singleLine(gb.display.width(), 1, ColorMode::rgb565);

extern uint8_t decompression_buffer[];

uint16_t Board::getDefaultTile() {
	switch(worldId) {
		case WORLD_DUNGEON_MOVE_BLOCKS:
		case WORLD_DUNGEON_BOMBS:
		case WORLD_DUNGEON_SWIMSUIT:
		case WORLD_CAVES:
			return SPRITE_203;
		case WORLD_AERILON_CASTLE:
		case WORLD_SAND_CASTLE_PRESENT:
		case WORLD_SAND_CASTLE_PAST:
		case WORLD_DUNGEON_HOOKSHOT:
			return SPRITE_308;
		case WORLD_HOUSES:
			return SPRITE_193;
		default:
			return SPRITE_1;
	}
}

uint8_t Board::getWorldId() {
	return worldId;
}

uint8_t Board::getMapId() {
	return mapId;
}

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

void line(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
	gb.display.drawLine(x1 - camera.x, y1 - camera.y, x2 - camera.x, y2 - camera.y);
}

void Board::transitionPortal(uint8_t portal_x, uint8_t portal_y, uint8_t _world, uint8_t _map, uint8_t player_x, uint8_t player_y) {
	portal_x *= 8;
	portal_x += 4;
	portal_y *= 8;
	portal_y += 4;
	int16_t line_right = portal_x - 5*8;
	int16_t line_left = portal_x + 5*8;
	int16_t line_up = portal_y - 5*8;
	int16_t line_down = portal_y + 5*8;
	
	const uint8_t steps = 5*4;
	Color* origPalette = gb.display.colorIndex;
	Color palette[16];
	gb.display.colorIndex = palette;
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)origPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r = rgb.r*(steps - i) / steps;
			rgb.g = rgb.g*(steps - i) / steps;
			rgb.b = rgb.b*(steps - i) / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		renderAll();
		gb.display.setColor(BLACK);
		line(line_right, portal_y, -1, portal_y);
		line(line_right, portal_y - 1, 0, portal_y - 1);
		
		line(line_left, portal_y, 12*8, portal_y);
		line(line_left, portal_y - 1, 12*8, portal_y - 1);
		
		line(portal_x, line_up, portal_x, -1);
		line(portal_x - 1, line_up, portal_x - 1, 0);
		
		line(portal_x, line_down, portal_x, 8*8);
		line(portal_x - 1, line_down, portal_x - 1, 8*8);
		
		line_right += 2;
		line_left -= 2;
		line_up += 2;
		line_down -= 2;
		
		waitCycles(1);
	}
	load(_world, _map);
	player.moveTo(player_x, player_y);
	player.setDirection(Direction::down);
	player.focus();
	postload();
	for (uint8_t i = 0; i <= steps; i++) {
		for (uint8_t j = 0; j < 16; j++) {
			uint16_t c = (uint16_t)origPalette[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r = rgb.r*i / steps;
			rgb.g = rgb.g*i / steps;
			rgb.b = rgb.b*i / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			palette[j] = (Color)c;
		}
		renderAll();
		waitCycles(1);
	}
	gb.display.colorIndex = origPalette;
}


void Board::saveBackup() {
	memcpy(board_backup, board, mapsize_bytes);
}

void Board::loadBackup(uint8_t _world, uint8_t _map) {
	worldId = _world;
	mapId = _map;
	uint8_t i = 0;
	while(1) {
		if (worlds[worldId].maps[i].id == mapId) {
			map = &worlds[worldId].maps[i];
			break;
		}
		i++;
	}
	memcpy(board, board_backup, mapsize_bytes);
}

uint16_t Board::getTile(uint8_t x, uint8_t y) {
	return board[y*width + x];
}

void Board::eraseTile(uint8_t x, uint8_t y) {
	setTile(x, y, getDefaultTile());
}

void Board::setTile(uint8_t x, uint8_t y, uint16_t tile) {
	board[y*width + x] = tile;
}

void Board::drawTile(int8_t x, int8_t y, uint16_t tile) {
	sprites.setFrame(tile);
	gb.display.drawImage(x - camera.x, y - camera.y, sprites);
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
		waitCycles(1);
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
		waitCycles(1);
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
		waitCycles(1);
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
		waitCycles(1);
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
		waitCycles(1);
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

int8_t Board::runScript(uint8_t x, uint8_t y, uint8_t trigger) {
	uint8_t amount = 0;
	uint8_t i = 0;
	uint8_t offset = y*12 + x;
	
	while(1) {
		uint8_t id = worlds[worldId].actionTiles[i].mapId;
		if (id == mapId) {
			amount = worlds[worldId].actionTiles[i].amount;
			if (!amount) {
				return -1;
			}
			const Actiontiles_LUT* lut = worlds[worldId].actionTiles[i].lut;
			for (i = 0; i < amount; i++) {
				if (lut[i].offset == offset) {
					return script.run(lut[i].script, trigger);
				}
			}
			return -1;
		}
		if (id == 0xFF) {
			return -1;
		}
		i++;
	}
	return -1;
}

int8_t Board::interact(uint8_t x, uint8_t y) {
	uint16_t t = getTile(x, y);
	if (t >= SPRITE_FIRST_WATER && t < SPRITE_AFTER_WATER) {
		if (player.isCurItemBottle() && text.boxPlayer(STRING_BOTTLE_ASKWATER)) {
			player.setCurBottle(Bottle::dirty_water);
		}
		return 1;
	}
	switch(t) {
		case SPRITE_105: // shrub on grass
			setTile(x, y, SPRITE_1);
			return 1;
		case SPRITE_113: // rock on sand
			if (player.isEvent(EVENT_PICKAXE)) {
				setTile(x, y, SPRITE_308);
			}
			return 1;
		case SPRITE_376: // rock on darkgray
			if (player.isEvent(EVENT_PICKAXE)) {
				if (worldId == WORLD_OVERWORLD && mapId == TILEMAP_43) {
					// we place the path sprite
					setTile(x, y, SPRITE_348);
				} else {
					setTile(x, y, SPRITE_309);
				}
			}
			return 1;
		case SPRITE_456: // rock on grass
			if (player.isEvent(EVENT_PICKAXE)) {
				setTile(x, y, SPRITE_1);
			}
			return 1;
		case SPRITE_BLOCK:
			if (player.isEvent(EVENT_PUSH_BLOCKS)) {
				pushblock(x, y, player.getDirection());
			}
			return 1;
		default:
			return runScript(x, y, SCRIPT_ACTION);
	}
}


Board board;
