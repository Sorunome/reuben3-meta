#include "script.h"

#include <Gamebuino-Meta.h>
#include "player.h"
#include "camera.h"
#include "board.h"

#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_TRANSITION_MAP_AND_WORLD 0x03
#define SCRIPT_TRANSITION_MAP 0x04
#define SCRIPT_ADD_ENEMY 0x05
#define SCRIPT_FOCUS_CAM 0x06
#define SCRIPT_UPDATE_SCREEN 0x07
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
	}
	return false;
}

bool Script::run(const uint8_t* _script) {
	return run((uint8_t*) _script);
}

bool Script::run(uint8_t* _script) {
	script_entry = script = _script;
	uint8_t* ptr;
	while(1) {
		switch(*script++) {
			case SCRIPT_FADE_TO_WHITE:
				continue;
			case SCRIPT_FADE_FROM_WHITE:
				continue;
			case SCRIPT_TRANSITION_MAP_AND_WORLD:
				board.setWorld(getNum());
				// no continue as from now on it is the same as just setting the map
			case SCRIPT_TRANSITION_MAP:
				board.load(getNum());
				// TODO: neat transitions
				board.postload();
				player.moveTo(getNum(), getNum());
				player.focus();
				continue;
			case SCRIPT_ADD_ENEMY:
				continue;
			case SCRIPT_FOCUS_CAM:
				player.focus();
				continue;
			case SCRIPT_UPDATE_SCREEN:
				continue;
			case SCRIPT_SET_VAR:
				ptr = getVar();
				*ptr = getNum();
				continue;
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
				ptr = getVar();
				*ptr += getNum();
				continue;
			case SCRIPT_INC:
				(*getVar())++;
				continue;
			case SCRIPT_DEC:
				(*getVar())--;
				continue;
			
			case SCRIPT_RETURN_FALSE:
				return false;
			case SCRIPT_RETURN_TRUE:
				return true;
		}
	}
}

Script script;
