#ifndef _BATTLE_H_
#define _BATTLE_H_

#include <Arduino.h>

struct Enemy_Background_Look {
	const uint8_t frame_id;
	const uint16_t sprites[4];
};

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
		const Enemy_Background_Look* background_look;
		uint16_t calcPlayerDamage(Battle_Attack_Type type);
		uint16_t calcEnemyDamage();
		Battle_Done loop();
		uint8_t i;
		void playerAttackAnimation();
		void playerJump();
		void enemyBlink();
		void playerBlink();
		
		void fireAnimation(uint8_t x, uint8_t y);
		void boltAnimation(uint8_t x, uint8_t y);
		void iceAnimation();
		void stunAnimation();
		void bombAnimation();
		void runAnimation();
		
		
		void playerAttack(Battle_Attack_Type type);
		bool useMp(uint8_t amount);
		void useBottle();
		void addHp(uint16_t amount);
		void addMp(uint8_t amount);
		
		void enemyAttack();
		void enemyNormalAttack();
		void enemyFireAttack();
		void enemyBoltAttack();
		void enemyPoison();
		
		void render(bool render_reuben = true, bool render_enemy = true, int8_t y_offset = 0);
		void drawText(uint8_t x, uint8_t y, char* text);
	public:
		bool fight(uint8_t _i);
};

extern Battle battle;

#endif // _BATTLE_H_
