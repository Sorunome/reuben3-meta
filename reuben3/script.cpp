#include "script.h"

#include <Gamebuino-Meta.h>
#include "player.h"
#include "camera.h"
#include "board.h"
#include "text.h"
#include "misc.h"
#include "battle.h"

#include "data/scripts.h"

#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_TRANSITION_MAP_AND_WORLD 0x03
#define SCRIPT_TRANSITION_MAP 0x04
#define SCRIPT_BATTLE 0x05
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
#define SCRIPT_MOVE_PLAYER 0x11
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
#define SCRIPT_GET_PLAYER_X 0x1D
#define SCRIPT_GET_PLAYER_Y 0x1E
#define SCRIPT_RENDER 0x1F
#define SCRIPT_DRAW_TILE 0x20
#define SCRIPT_RELOAD_MAP 0x21
#define SCRIPT_FADE_TO_MAP_AND_WORLD_POS 0x22
#define SCRIPT_ADD_GOLD 0x23
#define SCRIPT_SHOP 0x24
#define SCRIPT_SHOP_BOTTLE 0x25
#define SCRIPT_SET_BOTTLE 0x26
#define SCRIPT_ADD_BOMBS 0x27
#define SCRIPT_GET_SWIMSUIT 0x28
#define SCRIPT_TRANSITION_PORTAL 0x29
#define SCRIPT_MOVE_LEFT_AND_SHAKE_SCREEN 0x2A
#define SCRIPT_BATTLE_INSTRUCTIONS 0x2B
#define SCRIPT_INCREASE_SPEED_ARENA 0x2C
#define SCRIPT_SEARCHQUEST_PERSON 0x2D
#define SCRIPT_SFX 0x2E

#define SCRIPT_RETURN_FALSE 0xFE
#define SCRIPT_RETURN_TRUE 0xFF

const Gamebuino_Meta::Sound_FX sfx_chest[] = {
	{Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,147,0,0,67,5},
	{Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,147,0,0,63,5},
	{Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,147,0,0,56,13},
};

const Gamebuino_Meta::Sound_FX sfx_badchest[] = {
	{Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,147,0,0,567,4},
	{Gamebuino_Meta::Sound_FX_Wave::NOISE,1,0,0,0,0,3},
	{Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,147,0,0,567,10},
};

struct Script_Sfx {
	const Gamebuino_Meta::Sound_FX* sfx;
	const uint16_t delay;
};

const Script_Sfx script_sfx[] = {
	{ sfx_chest, 360 },
	{ sfx_badchest, 340 },
};

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
	const uint8_t* ptrs[] = {(uint8_t*)&(camera.x), (uint8_t*)&(camera.y), &trigger, &(player.armor), &(player.wait), &(player.sword), &(player.tradequest), &(player.fright), &(player.tmp_map)};
	const uint8_t len = sizeof(ptrs) / sizeof(const uint8_t*);
	if (0xFF - i <= len) {
		return (uint8_t*)(ptrs[0xFF - i]);
	}
	return &(vars[i]);
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

uint16_t Script::getNum16() {
	uint8_t l = getNum();
	uint8_t h = getNum();
	return (h << 8) | l;
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
		case SCRIPT_SHOP:
			return shop(getNum16(), getNum16(), false);
		case SCRIPT_SHOP_BOTTLE:
			return shop(getNum16(), getNum16(), true);
	}
	return false;
}

bool Script::run(const uint8_t* _script, uint8_t _trigger, bool _isHome) {
	return run((uint8_t*) _script, _trigger, _isHome);
}

void Script::checkHome() {
	if (isHome) {
		isHome = false;
		homeMap = board.getMapId();
		homeWorld = board.getWorldId();
		homeX = player.getX();
		homeY = player.getY();
		board.saveBackup();
	}
}

bool Script::run(uint8_t* _script, uint8_t _trigger, bool _isHome) {
	script_entry = script = _script;
	trigger = _trigger;
	isHome = _isHome;
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
			case SCRIPT_BATTLE:
			{
				bool ret = battle.fight(getNum());
				if (!ret) {
					return false;
				}
				continue;
			}
			case SCRIPT_FOCUS_CAM:
				player.focus();
				continue;
			case SCRIPT_FADE_HOME:
				fade_to_white();
				board.loadBackup(homeWorld, homeMap);
				isHome = true;
				player.moveTo(homeX, homeY);
				player.show();
				player.focus();
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
			case SCRIPT_CALL:
			{
				uint8_t a = getNum();
				uint8_t* _script = script;
				uint8_t* _script_entry = script_entry;
				run(scripts[a], trigger, isHome);
				script = _script;
				script_entry = _script_entry;
				continue;
			}
			case SCRIPT_MOVE_PLAYER:
				player.moveX(getNum());
				player.moveY(getNum());
				player.show();
				player.focus();
				continue;
			case SCRIPT_SETEVENT:
				player.setEvent(getNum());
				continue;
			case SCRIPT_CLEAREVENT:
				player.clearEvent(getNum());
				continue;
			case SCRIPT_TEXT:
			{
				uint16_t i = getNum16();
				text.boxPlayer(i);
				continue;
			}
			case SCRIPT_TEXT_ANSWER:
			{
				uint16_t i = getNum16();
				uint8_t* ptr = getVar();
				*ptr = (uint8_t)text.boxPlayer(i);
				continue;
			}
			case SCRIPT_SHAKE_SCREEN:
				for (uint8_t i = 0; i < 2; i++) {
					for (uint8_t j = 0; j < 4; j++) {
						waitCycles(1);
						camera.setY(j*2);
						renderAll();
					}
					for (int8_t j = 4; j > 0; j--) {
						waitCycles(1);
						camera.setY(j*2);
						renderAll();
					}
				}
				waitCycles(1);
				camera.setY(0);
				renderAll();
				gb.update(); // send the screen out
				continue;
			case SCRIPT_UPDATE_SCREEN:
				waitCycles(1);
				gb.update(); // send the screen out
				continue;
			case SCRIPT_SET_TILE:
			{
				uint8_t x = getNum();
				uint8_t y = getNum();
				uint16_t s = getNum16();
				board.setTile(x, y, s);
				continue;
			}
			case SCRIPT_FADE_TO_MAP_AND_WORLD:
				checkHome();
				fade_to_white();
				player.hide();
				board.load(getNum(), getNum());
				board.postload();
				fade_from_white();
				continue;
			case SCRIPT_FADE_TO_MAP:
				checkHome();
				fade_to_white();
				player.hide();
				board.load(getNum());
				board.postload();
				fade_from_white();
				continue;
			case SCRIPT_GET_PLAYER_X:
				(*getVar()) = player.getX();
				continue;
			case SCRIPT_GET_PLAYER_Y:
				(*getVar()) = player.getY();
				continue;
			case SCRIPT_RENDER:
				renderAll();
				continue;
			case SCRIPT_DRAW_TILE:
			{
				uint8_t x = getNum();
				uint8_t y = getNum();
				uint16_t s = getNum16();
				board.drawTile(x, y, s);
				continue;
			}
			case SCRIPT_RELOAD_MAP:
				board.load();
				board.postload();
				renderAll();
				continue;
			case SCRIPT_FADE_TO_MAP_AND_WORLD_POS:
				checkHome();
				fade_to_white();
				player.hide();
				board.load(getNum(), getNum());
				player.moveTo(getNum(), getNum());
				player.focus();
				board.postload();
				fade_from_white();
				continue;
			case SCRIPT_ADD_GOLD:
				player.addGold(getNum16());
				continue;
			case SCRIPT_SET_BOTTLE:
				player.setCurBottle((Bottle)getNum());
				continue;
			case SCRIPT_ADD_BOMBS:
				player.addBombs(getNum());
				continue;
			case SCRIPT_GET_SWIMSUIT:
				player.getSwimsuit();
				continue;
			case SCRIPT_TRANSITION_PORTAL:
				board.transitionPortal(getNum(), getNum(), getNum(), getNum(), getNum(), getNum());
				continue;
			case SCRIPT_MOVE_LEFT_AND_SHAKE_SCREEN:
			{
				uint8_t px = player.getX();
				bool up = true;
				int8_t j = 0;
				while (px) {
					camera.setY(j*2);
					if (up) {
						if (j < 4) {
							j++;
						} else {
							up = false;
						}
					} else {
						if (j > 0) {
							j--;
						} else {
							up = true;
						}
					}
					player.moveX(px);
					renderAll();
					px--;
					waitCycles(1);
				}
				camera.setY(0);
				renderAll();
				gb.update();
				continue;
			}
			case SCRIPT_BATTLE_INSTRUCTIONS:
				battleInstructions();
				continue;
			case SCRIPT_INCREASE_SPEED_ARENA:
				increaseSpeedArena();
				continue;
			case SCRIPT_SEARCHQUEST_PERSON:
				searchquestPerson();
				continue;
			case SCRIPT_SFX:
			{
				Script_Sfx fx = script_sfx[getNum()];
				gb.sound.fx(fx.sfx);
				delay(fx.delay);
				continue;
			}
			
			case SCRIPT_RETURN_FALSE:
				return false;
			case SCRIPT_RETURN_TRUE:
				return true;
		}
	}
}

Script script;
