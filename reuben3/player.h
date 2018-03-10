#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <Arduino.h>
#include "data/defines.h"
#include <Gamebuino-Meta.h>

enum class Direction : uint8_t {
	up = 0,
	right,
	down,
	left,
};

enum class Bottle : uint8_t {
	empty = 0,
	dirty_water,
	water,
	herb,
	potion,
	ginseng,
	elixir,
};

struct SaveData {
	int8_t x;
	int8_t y;
	
	uint8_t events[(TOTAL_EVENTS + 7) / 8];
	Bottle bottles[4];
	
	uint8_t cur_item;
	uint8_t lvl;
	uint16_t hp;
	uint16_t hp_max;
	uint8_t mp;
	uint8_t mp_max;
	uint16_t exp;
	uint16_t exp_next;
	uint16_t gold;
	uint16_t gold_max;
	uint8_t bombs;
	uint8_t bombs_max;
	
	uint8_t armor;
	uint8_t wait;
	uint8_t sword;
	uint8_t tradequest;
	uint8_t fright;
	
	uint8_t world;
	uint8_t tilemap;
};

class Player {
	private:
		uint8_t render_cycle;
		uint8_t fx_cycle;
		
		float x;
		float y;
		float battle_counter;
		bool isWalkable(float dx, float dy);
		uint8_t events[(TOTAL_EVENTS + 7) / 8];
		Bottle bottles[4];
		Direction direction;
		void item();
		void interact();
		bool visible;
		uint16_t after_walkable;
		uint8_t cur_item;
		uint8_t lvl;
		uint16_t hp;
		uint16_t hp_max;
		uint8_t mp;
		uint8_t mp_max;
		uint16_t exp;
		uint16_t exp_next;
		uint16_t gold;
		uint16_t gold_max;
		uint8_t bombs;
		uint8_t bombs_max;
		void setAnimationFrame();
		
		uint8_t slot;
		bool dead;
		bool swimming;
	public:
		uint8_t armor;
		uint8_t wait;
		uint8_t sword;
		uint8_t tradequest;
		uint8_t fright;
		uint8_t tmp_map;
		
		void setBattleCounter();
		void getSwimsuit();
		
		uint8_t getLvl();
		uint16_t getHp();
		uint16_t getHpMax();
		uint8_t getMp();
		uint8_t getMpMax();
		uint16_t getExp();
		uint16_t getExpNext();
		uint16_t getGold();
		uint16_t getGoldMax();
		void setGoldMax(uint16_t max);
		uint8_t getBombs();
		uint8_t getBombsMax();
		void setBombsMax(uint8_t max);
		Direction getDirection();
		void setDirection(Direction d);
		bool isDead();
		
		void init(uint8_t _slot);
		void save();
		void load();
		void hide();
		void show();
		void moveTo(int8_t _x, int8_t _y);
		void moveX(int8_t _x);
		void moveY(int8_t _y);
		float getX();
		float getY();
		void focus();
		void update();
		void render();
		void render(Image& img, int8_t dx, int8_t dy);
		
		bool isEvent(uint8_t e);
		void setEvent(uint8_t e);
		void clearEvent(uint8_t e);
		void addHp(uint16_t num);
		void setHp(uint16_t num);
		void addMp(uint8_t num);
		void addGold(uint16_t num);
		bool payGold(uint16_t num);
		void addBombs(uint8_t num);
		bool useBomb();
		bool addExp(uint16_t num);
		
		uint16_t damage(uint16_t dmg);
		uint8_t useMp(uint8_t amount);
		
		uint8_t getCurItem();
		bool isCurItemBottle();
		void setCurItem(uint8_t i);
		void setCurBottle(Bottle b);
		Bottle getCurBottle();
		Bottle getBottle(uint8_t i);
		void emptyCurBottle();
};

extern Player player;

#endif // _PLAYER_H_
