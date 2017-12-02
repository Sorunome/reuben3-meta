#include "battle.h"

#include <Gamebuino-Meta.h>
#include "misc.h"
#include "depack.h"
#include "player.h"

#include "data/enemies.h"
#include "data/largeSprites.h"

extern uint8_t decompression_buffer[];

const uint8_t reuben_buffer[] = {
	16, 16,
	0x77, 0x77, 0x77, 0x00, 0x07, 0x77, 0x77, 0x77,
	0x77, 0x77, 0x00, 0x05, 0x00, 0x00, 0x07, 0x77,
	0x77, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x77,
	0x70, 0x55, 0x65, 0x66, 0x00, 0x50, 0x50, 0x07,
	0x06, 0x00, 0x00, 0x05, 0x60, 0x05, 0x05, 0x07,
	0x70, 0x50, 0x06, 0x60, 0x05, 0x00, 0x00, 0x00,
	0x77, 0x00, 0x66, 0x66, 0x60, 0x60, 0x05, 0x00,
	0x77, 0x70, 0x00, 0x70, 0x06, 0x05, 0x00, 0x07,
	0x77, 0x00, 0x60, 0x76, 0x07, 0x60, 0x07, 0x77,
	0x70, 0x60, 0x77, 0x77, 0x76, 0x05, 0x50, 0x07,
	0x06, 0x65, 0x06, 0x66, 0x60, 0x56, 0x65, 0x07,
	0x70, 0x00, 0x50, 0x00, 0x00, 0x00, 0x65, 0x07,
	0x07, 0x60, 0x55, 0x00, 0x07, 0x60, 0x06, 0x07,
	0x06, 0x60, 0x00, 0x00, 0x06, 0x60, 0x06, 0x50,
	0x70, 0x00, 0x56, 0x65, 0x50, 0x00, 0x05, 0x50,
	0x77, 0x77, 0x00, 0x00, 0x07, 0x77, 0x00, 0x07,
};

Image reuben_image(reuben_buffer);

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
		gb.display.drawImage(80 - 16, 64 - 16 - 8, reuben_image);
		
		bool player_disp_hp = true;
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
			gb.display.setColor(BROWN);
			gb.display.drawFastHLine(54, 64 - 16 - 8 - 8, 24);
			gb.display.drawFastHLine(54, 64 - 16 - 8 - 10, 24);
			gb.display.drawPixel(53, 64 - 16 - 8 - 9);
			gb.display.drawPixel(53 + 25, 64 - 16 - 8 - 9);
			gb.display.setColor(RED);
			gb.display.drawFastHLine(54, 64 - 16 - 8 - 9, 24 * (p.wait - p.curwait) / p.wait);
		} else {
			switch(p.state) {
				case Battle_Player_State::normal:
					// Normal attack menu
					break;
				case Battle_Player_State::magic:
					// Magic attack menu
					player_disp_hp = false;
					break;
			}
		}
		gb.display.setColor(BLACK);
		gb.display.setCursor(52, 57);
		if (player_disp_hp) {
			gb.display.print("HP:");
			gb.display.print(p.hp);
		} else {
			gb.display.print("MP:");
			gb.display.print(p.mp);
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
	p.lvl = player.getLvl();
	p.hp = player.getHp();
	p.mp = player.getMp();
	p.armor = player.armor;
	p.wait = player.wait;
	p.sword = player.sword;
	p.curwait = p.wait;
	p.stuncounter = 0;
	p.poison = 0;
	p.state = Battle_Player_State::normal;
	
	e.lvl = enemies[i].lvl;
	e.hp = enemies[i].hp;
	e.wait = enemies[i].wait;
	e.curwait = e.wait;
	aP_depack(EnemySprites[i], decompression_buffer);
	enemyImage.init(decompression_buffer);
	loop();
}


Battle battle;
