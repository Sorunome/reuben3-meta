#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <Arduino.h>

struct Enemy_Data {
	const uint8_t id;
	const uint8_t lvl;
	const uint16_t hp;
	const uint16_t exp;
	const uint8_t sl1;
	const uint8_t sl2;
	const uint8_t sl3;
	const uint8_t sl4;
	const uint8_t sl5;
	const uint8_t xpos;
	const uint8_t ypos;
	const uint8_t wait;
};

enum class Battle_Player_State : uint8_t {
	normal,
	magic,
};

struct Battle_Player {
	uint8_t lvl;
	uint16_t hp;
	uint8_t mp;
	uint8_t armor;
	uint8_t wait;
	uint8_t sword;
	
	uint8_t curwait;
	uint8_t stuncounter;
	uint8_t poison;
	Battle_Player_State state;
};


struct Battle_Enemy {
	uint8_t lvl;
	uint16_t hp;
	uint8_t wait;
	
	uint8_t curwait;
	uint8_t stuncounter;
};

class Battle {
	private:
		Battle_Player p;
		Battle_Enemy e;
		uint16_t calcPlayerDamage();
		uint16_t calcEnemyDamage();
		void loop();
		uint8_t i;
	public:
		bool fight(uint8_t _i);
};

extern Battle battle;

#endif // _BATTLE_H_
