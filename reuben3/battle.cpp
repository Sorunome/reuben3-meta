#include "battle.h"

#include <Gamebuino-Meta.h>
#include "misc.h"
#include "depack.h"

#include "data/enemies.h"
#include "data/largeSprites.h"

extern uint8_t decompression_buffer[];

uint16_t Battle::calcPlayerDamage() {
	uint16_t upper = (2*p.lvl*p.lvl + random(10*p.lvl) + 2) * p.sword;
	uint16_t lower = 2*e.lvl;
	return (upper + (lower - 1)) / lower;
}

uint16_t Battle::calcEnemyDamage() {
	uint16_t upper = 5 + random(10*e.lvl) + e.lvl*e.lvl*e.lvl;
	uint16_t lower = 2*p.lvl + 4*(random(p.armor) + p.armor) + p.armor*p.armor;
	return ((upper + (lower - 1)) / lower) * 5;
}

Image enemyImage;

void Battle::loop() {
	while(1) {
		if (!gb.update()) {
			continue;
		}
		gb.display.clear(WHITE);
		gb.display.drawImage(enemies[i].xpos, enemies[i].ypos, enemyImage);
		if (gb.buttons.pressed(BUTTON_C)) {
			statsMenu();
			continue; // make sure we do accidental shadowbutton triggers
		}
		if (p.stuncounter) {
			// we are stunned
			p.stuncounter--;
		} else if (p.curwait) {
			// we still have to wait
			p.curwait--;
		} else {
			switch(p.state) {
				case Battle_Player_States::normal:
					// Normal attack menu
					break;
				case Battle_Player_States::magic:
					// Magic attack menu
					break;
			}
		}
		
		if (e.stuncounter) {
			// enemy is stunned
			e.stuncounter--;
		} else if (e.curwait) {
			// enemy has to wait
			e.curwait--;
		} else if (!random(25)) {
			// ATTACK!!!
		}
		
		if (p.poison) {
			if (!(--p.poison)) {
				// POISOOOOOON!
				p.poison = e.wait;
			}
		}
	}
}

bool Battle::fight(uint8_t _i) {
	i = _i;
	e.lvl = enemies[i].lvl;
	e.hp = enemies[i].hp;
	e.wait = enemies[i].wait;
	e.curwait = e.wait;
	aP_depack(EnemySprites[i], decompression_buffer);
	enemyImage.init(decompression_buffer);
	loop();
}


Battle battle;
