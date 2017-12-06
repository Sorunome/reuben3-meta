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

uint16_t Battle::calcPlayerDamage(Battle_Attack_Type type) {
	uint16_t upper = (2*p.lvl*p.lvl + random(10*p.lvl) + 2) * p.sword;
	uint16_t lower = 2*e.lvl;
	uint16_t ret = (upper + (lower / 2)) / lower;
	if (type == Battle_Attack_Type::normal) {
		return ret;
	}
	return (e.slots[(uint8_t)type - 1] & 0x0F) * ret;
}

uint16_t Battle::calcEnemyDamage() {
	uint16_t upper = 5 + random(10*e.lvl) + e.lvl*e.lvl*e.lvl;
	uint16_t lower = 2*p.lvl + 4*(random(p.armor) + p.armor) + p.armor*p.armor;
	return ((upper + (lower / 2)) / lower) * 5;
}

void Battle::playerAttack(Battle_Attack_Type type) {
	uint16_t damage = calcPlayerDamage(type);
	render();
	gb.display.setCursor(52, 16);
	gb.display.print(damage);
	waitCycles(45);
	if (damage > e.hp) {
		e.hp = 0;
	} else {
		e.hp -= damage;
	}
	p.state = Battle_Player_State::normal;
	p.curwait = p.wait;
}

bool Battle::useMp(uint8_t amount) {
	if (amount > p.mp) {
		return false;
	}
	p.mp = player.useMp(amount);
	return true;
}


void Battle::enemyAttack() {
	e.curwait = e.wait;
	switch(e.slots[random(5)] & 0xF0) {
		case 0x00:
		default:
			// normal attack
			enemyNormalAttack();
			break;
		case 0x10:
			// fire attack
			enemyFireAttack();
			break;
		case 0x20:
			// bolt attack
			enemyBoltAttack();
			break;
		case 0x30:
			// poison
			if (p.poison) {
				enemyNormalAttack();
				break;
			}
			p.poison = e.wait;
			break;
		case 0x40:
			// stun
			if (p.stuncounter) {
				enemyNormalAttack();
				break;
			}
			p.stuncounter = e.wait;
			enemyAttack();
			break;
	}
}

void Battle::enemyNormalAttack() {
	uint8_t damage = calcEnemyDamage();
	render();
	gb.display.setCursor(60, 20);
	gb.display.print(damage);
	waitCycles(45);
	p.hp = player.damage(damage);
}

void Battle::enemyFireAttack() {
	uint8_t damage = calcEnemyDamage()*2;
	render();
	gb.display.setCursor(60, 20);
	gb.display.print(damage);
	waitCycles(45);
	p.hp = player.damage(damage);
}

void Battle::enemyBoltAttack() {
	uint8_t damage = calcEnemyDamage()*4;
	render();
	gb.display.setCursor(57, 20);
	gb.display.print(damage);
	waitCycles(45);
	p.hp = player.damage(damage);
}

Image enemyImage;

void Battle::render() {
	gb.display.clear(WHITE);
	gb.display.drawImage(enemies[i].xpos, enemies[i].ypos, enemyImage);
	gb.display.drawImage(80 - 16 - 2, 64 - 16 - 8, reuben_image);
}

Battle_Done Battle::loop() {
	uint8_t cursor_menu = 0;
	uint8_t cursor_magic = 0;
	while(1) {
		if (!gb.update()) {
			continue;
		}
		render();
		
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
			cursor_menu = 0;
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
				{
					// Normal attack menu
					if (gb.buttons.pressed(BUTTON_UP)) {
						if (cursor_menu) {
							cursor_menu--;
						} else {
							cursor_menu = 3;
						}
					}
					
					if (gb.buttons.pressed(BUTTON_DOWN)) {
						if (cursor_menu < 3) {
							cursor_menu++;
						} else {
							cursor_menu = 0;
						}
					}
					
					gb.display.setColor(BROWN);
					gb.display.drawFastHLine(54, 6, 6*4 + 1);
					gb.display.drawFastHLine(54, 8 + 6*4, 6*4 + 1);
					gb.display.drawFastVLine(53, 7, 6*4 + 1);
					gb.display.drawFastVLine(55 + 6*4, 7, 6*4 + 1);
					
					gb.display.setColor(BEIGE);
					gb.display.fillRect(54, 7 + 6*cursor_menu, 6*4 + 1, 7);
					gb.display.setColor(BLACK);
					gb.display.setCursor(55, 8);
					gb.display.println("Attack");
					gb.display.setCursorX(55);
					gb.display.println("Magic");
					gb.display.setCursorX(55);
					gb.display.println("Item");
					gb.display.setCursorX(55);
					gb.display.println("Run");
					
					
					if (gb.buttons.pressed(BUTTON_A)) {
						switch(cursor_menu) {
							case 0:
								playerAttack(Battle_Attack_Type::normal);
								break;
							case 1:
								p.state = Battle_Player_State::magic;
								cursor_magic = 0;
								break;
							case 2:
								// TODO: items
								break;
							case 3:
								if (!(e.slots[4] & 0x0F)) {
									break;
								}
								// TODO: no running away from maru
								if (!random(2)) {
									return Battle_Done::run;
								}
								p.state = Battle_Player_State::normal;
								p.curwait = p.wait;
								break;
						}
					}
					break;
				}
				case Battle_Player_State::magic:
					// Magic attack menu
					player_disp_hp = false;
					
					if (gb.buttons.pressed(BUTTON_UP)) {
						if (cursor_magic) {
							cursor_magic--;
						} else {
							cursor_magic = 3;
						}
					}
					
					if (gb.buttons.pressed(BUTTON_DOWN)) {
						if (cursor_magic < 3) {
							cursor_magic++;
						} else {
							cursor_magic = 0;
						}
					}
					
					gb.display.setColor(BROWN);
					gb.display.drawFastHLine(54, 6, 6*4 + 1);
					gb.display.drawFastHLine(54, 8 + 6*4, 6*4 + 1);
					gb.display.drawFastVLine(53, 7, 6*4 + 1);
					gb.display.drawFastVLine(55 + 6*4, 7, 6*4 + 1);
					
					gb.display.setColor(BEIGE);
					gb.display.fillRect(54, 7 + 6*cursor_magic, 6*4 + 1, 7);
					gb.display.setColor(BLACK);
					gb.display.setCursor(55, 8);
					gb.display.println("Fire");
					gb.display.setCursorX(55);
					gb.display.println("Ice");
					gb.display.setCursorX(55);
					gb.display.println("Bolt");
					gb.display.setCursorX(55);
					gb.display.println("Stun");
					
					if (gb.buttons.pressed(BUTTON_A)) {
						switch(cursor_magic) {
							case 0:
								if (useMp(5)) {
									playerAttack(Battle_Attack_Type::fire);
								}
								break;
							case 1:
								if (useMp(5)) {
									playerAttack(Battle_Attack_Type::ice);
								}
								break;
							case 2:
								if (useMp(5)) {
									playerAttack(Battle_Attack_Type::bolt);
								}
								break;
							case 3:
								if (useMp(10)) {
									e.stuncounter = 0xFF;
									p.state = Battle_Player_State::normal;
									cursor_menu = 0;
								}
								break;
						}
					}
					if (gb.buttons.pressed(BUTTON_B)) {
						p.state = Battle_Player_State::normal;
					}
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
			enemyAttack();
		}
		
		if (p.poison) {
			if (!(--p.poison)) {
				// POISOOOOOON!
				uint8_t damage = e.lvl;
				render();
				gb.display.setCursor(57, 20);
				gb.display.print(damage);
				waitCycles(45);
				p.hp = player.damage(damage);
				
				p.poison = e.wait;
			}
		}
		if (p.hp <= 0) {
			return Battle_Done::lose;
		}
		if (e.hp <= 0) {
			return Battle_Done::win;
		}
		
		gb.display.setCursor(0, 0);
		gb.display.setColor(BLACK, WHITE);
		gb.display.print(gb.getCpuLoad());
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
	p.stuncounter = 0;
	p.poison = 0;
	p.state = Battle_Player_State::normal;
	
	p.lvl = 4;
	p.wait = 50;
	p.sword = WEAPON_STICK;
	
	e.lvl = enemies[i].lvl;
	e.hp = enemies[i].hp;
	e.wait = enemies[i].wait;
	e.slots[0] = enemies[i].sl1;
	e.slots[1] = enemies[i].sl2;
	e.slots[2] = enemies[i].sl3;
	e.slots[3] = enemies[i].sl4;
	e.slots[4] = enemies[i].sl5;
	
	// time to check for dynamic lvl / exp / wait
	if (e.lvl > 200)  {
		int16_t lvl = p.lvl - random(e.lvl & 0x0F) - 2;
		e.lvl = lvl <= 0 ? 1 : lvl;
	}
	if (e.hp >= 0xFF00) {
		e.hp &= 0xFF;
		e.hp = (player.getHpMax() + (e.hp/2)) / e.hp;
	}
	if (p.lvl - e.lvl > 6) {
		e.lvl = random(p.lvl - e.lvl - 1);
		if ((e.wait - p.wait) > 30) {
			e.wait = e.wait - random(e.wait - p.wait);
		}
	}
	
	
	p.curwait = p.wait;
	e.curwait = e.wait;
	
	aP_depack(EnemySprites[i], decompression_buffer);
	enemyImage.init(decompression_buffer);
	return loop() != Battle_Done::lose;
}


Battle battle;
