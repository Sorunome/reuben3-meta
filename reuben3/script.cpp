#include "script.h"

#include <Gamebuino-Meta.h>
#include <utility/Misc.h> // pixel to rgb converters
#include "player.h"
#include "camera.h"
#include "board.h"
#include "text.h"
#include "misc.h"

#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_TRANSITION_MAP_AND_WORLD 0x03
#define SCRIPT_TRANSITION_MAP 0x04
#define SCRIPT_ADD_ENEMY 0x05
#define SCRIPT_FOCUS_CAM 0x06
#define SCRIPT_FADE_HOME 0x07
#define SCRIPT_SET_VAR 0x08
#define SCRIPT_JUMP 0x09
#define SCRIPT_JUMP_IFNOT 0x0A
#define SCRIPT_LT 0x0B
#define SCRIPT_ADD 0x0C
#define SCRIPT_JUMP_IF 0x0D
#define SCRIPT_INC 0x0E
#define SCRIPT_DEC 0x0F
#define SCRIPT_CALL 0x10
#define SCRIPT_RET 0x11
#define SCRIPT_ISEVENT 0x12
#define SCRIPT_SETEVENT 0x13
#define SCRIPT_CLEAREVENT 0x14
#define SCRIPT_EQ 0x15
#define SCRIPT_TEXT 0x16
#define SCRIPT_TEXT_ANSWER 0x17
#define SCRIPT_SHAKE_SCREEN 0x18
#define SCRIPT_UPDATE_SCREEN 0x19
#define SCRIPT_SET_TILE 0x1A
#define SCRIPT_FADE_TO_MAP_AND_WORLD 0x1B
#define SCRIPT_FADE_TO_MAP 0x1C

#define SCRIPT_RETURN_FALSE 0xFE
#define SCRIPT_RETURN_TRUE 0xFF


#define SCRIPT_VAR_CAMERA_X 0xFF
#define SCRIPT_VAR_CAMERA_y 0xFE

union View32 {
	uint32_t addr;
	struct {
		uint8_t b1;
		uint8_t b2;
		uint8_t b3;
		uint8_t b4;
	};
};

uint8_t* Script::getVar() {
	uint8_t i = *script++;
	switch(i) {
		case SCRIPT_VAR_CAMERA_X:
			return (uint8_t*)&(camera.x);
		case SCRIPT_VAR_CAMERA_y:
			return (uint8_t*)&(camera.y);
		default:
			return (uint8_t*)&(vars[i]);
	}
}

uint8_t Script::getNum() {
	uint8_t i = *script++;
	if (i != 0x80) {
		return i;
	}
	i = *script;
	if (i == 0x80) {
		script++;
		return i;
	}
	return *getVar();
}

void Script::jump() {
	View32 a;
	a.b4 = *script++;
	a.b3 = *script++;
	a.b2 = *script++;
	a.b1 = *script++;
	script = script_entry + a.addr;
}

bool Script::condition() {
	switch(*script++) {
		case SCRIPT_LT:
			return getNum() < getNum();
		case SCRIPT_ISEVENT:
			return player.isEvent(getNum());
		case SCRIPT_EQ:
			return getNum() == getNum();
	}
	return false;
}

bool Script::run(const uint8_t* _script) {
	return run((uint8_t*) _script);
}

void fade_to_white() {
	const uint8_t steps = 17;
	uint16_t limit = gb.display.width() * gb.display.height();
	for (uint8_t i = 0; i <= steps; i++) {
		renderAll();
		for (uint16_t j = 0; j < limit; j++) {
			uint16_t c = gb.display._buffer[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r += (0xFF - rgb.r)*i / steps;
			rgb.g += (0xFF - rgb.g)*i / steps;
			rgb.b += (0xFF - rgb.b)*i / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			gb.display._buffer[j] = c;
		}
		while(!gb.update());
	}
}

void fade_from_white() {
	const uint8_t steps = 17;
	uint16_t limit = gb.display.width() * gb.display.height();
	for (uint8_t i = 0; i <= steps; i++) {
		renderAll();
		for (uint16_t j = 0; j < limit; j++) {
			uint16_t c = gb.display._buffer[j];
			Gamebuino_Meta::RGB888 rgb = Gamebuino_Meta::rgb565Torgb888(c);
			rgb.r += (0xFF - rgb.r)*(steps - i) / steps;
			rgb.g += (0xFF - rgb.g)*(steps - i) / steps;
			rgb.b += (0xFF - rgb.b)*(steps - i) / steps;
			c = Gamebuino_Meta::rgb888Torgb565(rgb);
			gb.display._buffer[j] = c;
		}
		while(!gb.update());
	}
}

bool Script::run(uint8_t* _script) {
	script_entry = script = _script;
	isHome = true;
	while(1) {
		switch(*script++) {
			case SCRIPT_FADE_TO_WHITE:
				fade_to_white();
				continue;
			case SCRIPT_FADE_FROM_WHITE:
				fade_from_white();
				continue;
			case SCRIPT_TRANSITION_MAP_AND_WORLD:
				board.setWorld(getNum());
				// no continue as from now on it is the same as just setting the map
			case SCRIPT_TRANSITION_MAP:
				board.load(getNum());
				player.moveTo(getNum(), getNum());
				player.focus();
				board.postloadHouseAnimation();
				continue;
			case SCRIPT_ADD_ENEMY:
				continue;
			case SCRIPT_FOCUS_CAM:
				player.focus();
				continue;
			case SCRIPT_FADE_HOME:
				fade_to_white();
				board.setWorld(homeWorld);
				board.load(homeMap);
				board.postload();
				player.show();
				fade_from_white();
				continue;
			case SCRIPT_SET_VAR:
			{
				uint8_t* ptr = getVar();
				*ptr = getNum();
				continue;
			}
			case SCRIPT_JUMP:
				jump();
				continue;
			case SCRIPT_JUMP_IFNOT:
				if (condition()) {
					script += 4;
					continue;
				}
				jump();
				continue;
			case SCRIPT_JUMP_IF:
				if (condition()) {
					jump();
					continue;
				}
				script += 4;
				continue;
			case SCRIPT_ADD:
			{
				uint8_t* ptr = getVar();
				*ptr += getNum();
				continue;
			}
			case SCRIPT_INC:
				(*getVar())++;
				continue;
			case SCRIPT_DEC:
				(*getVar())--;
				continue;
			case SCRIPT_SETEVENT:
				player.setEvent(getNum());
				continue;
			case SCRIPT_CLEAREVENT:
				player.clearEvent(getNum());
				continue;
			case SCRIPT_TEXT:
			{
				uint16_t i = *script++;
				i += (*script++) << 8;
				text.box(i, player.getY() > 28);
				continue;
			}
			case SCRIPT_TEXT_ANSWER:
			{
				uint16_t i = *script++;
				i += (*script++) << 8;
				uint8_t* ptr = getVar();
				*ptr = (uint8_t)text.box(i, player.getY() > 28);
				continue;
			}
			case SCRIPT_SHAKE_SCREEN:
				for (uint8_t i = 0; i < 2; i++) {
					for (uint8_t j = 0; j < 4; j++) {
						while(!gb.update());
						camera.setY(j*2);
						renderAll();
					}
					for (int8_t j = 4; j > 0; j--) {
						while(!gb.update());
						camera.setY(j*2);
						renderAll();
					}
				}
				// no need to update, we just roll over to update_screen and all is good to go
			case SCRIPT_UPDATE_SCREEN:
				while(!gb.update());
				camera.setY(0);
				renderAll();
				gb.update(); // send the screen out
				continue;
			case SCRIPT_SET_TILE:
			{
				uint8_t x = getNum();
				uint8_t y = getNum();
				uint8_t s1 = *script++;
				uint8_t s2 = *script++;
				board.setTile(x, y, s1 + (s2 << 8));
				continue;
			}
			case SCRIPT_FADE_TO_MAP_AND_WORLD:
				if (isHome) {
					isHome = false;
					homeMap = board.getMapId();
					homeWorld = board.getWorldId();
				}
				fade_to_white();
				player.hide();
				board.setWorld(getNum());
				board.load(getNum());
				board.postload();
				fade_from_white();
				continue;
			case SCRIPT_FADE_TO_MAP:
				if (isHome) {
					isHome = false;
					homeMap = board.getMapId();
					homeWorld = board.getWorldId();
				}
				fade_to_white();
				player.hide();
				board.load(getNum());
				board.postload();
				fade_from_white();
				continue;
			
			
			case SCRIPT_RETURN_FALSE:
				return false;
			case SCRIPT_RETURN_TRUE:
				return true;
		}
	}
}

Script script;
