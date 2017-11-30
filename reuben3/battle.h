#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <Arduino.h>

enum class Battle_Player_States : uint8_t {
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
	Battle_Player_States state;
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
	public:
		
};

#endif // _BATTLE_H_
