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

enum class Battle_Attack_Type : uint8_t {
	normal = 0,
	fire,
	ice,
	bolt,
	bomb,
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
	uint8_t slots[5];
	
	uint8_t curwait;
	uint8_t stuncounter;
};

enum class Battle_Done : uint8_t {
	win,
	lose,
	run,
};

class Battle {
	private:
		Battle_Player p;
		Battle_Enemy e;
		uint16_t calcPlayerDamage(Battle_Attack_Type type);
		uint16_t calcEnemyDamage();
		Battle_Done loop();
		uint8_t i;
		void playerAttack(Battle_Attack_Type type);
		bool useMp(uint8_t amount);
		
		void enemyAttack();
		void enemyNormalAttack();
		void enemyFireAttack();
		void enemyBoltAttack();
		
		void render();
	public:
		bool fight(uint8_t _i);
};

extern Battle battle;

#endif // _BATTLE_H_
