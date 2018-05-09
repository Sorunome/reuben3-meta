#include "stats.h"
#include "misc.h"
#include "board.h"
#include "player.h"
#include "language.h"
#include <Gamebuino-Meta.h>

const uint8_t status_icons_data[] = {7, 7, 3, 0, 0, 0xFF, 1, 0x66, 0x6f,0xff, 0xff,0xff, 0x66,0x6f, 0xff,0xff, 0xf0,0x66, 0xff,0xff, 0x09,0x06, 0xff,0xf0, 0x90,0xf6, 0x6f,0x09, 0x0f,0xff, 0x6f,0x00, 0xff,0xff, 0x6f,0xff, 0xfd,0xcf, 0xff,0xff, 0xdc,0xcc, 0xff,0xfd, 0xcd,0xdc, 0xcf,0xfc, 0x55,0xdc, 0xdf,0xc5, 0x55,0xcd, 0xff,0x5c, 0x5c,0xdf, 0xff,0xf5, 0xcf,0xff, 0xff,0xf0, 0x00,0x00, 0xff,0x00, 0x55,0x50, 0x0f,0x05, 0x66,0x65, 0x0f,0x05, 0x60,0x65, 0x0f,0x05, 0x66,0x65, 0x0f,0x00, 0x55,0x50, 0x0f,0xf0, 0x00,0x00, 0xff};

const char* getBottleString(uint8_t i) {
	const MultiLang* strs[] = {
		lang_bottle_bottle,
		lang_bottle_badwater,
		lang_bottle_water,
		lang_bottle_herb,
		lang_bottle_potion,
		lang_bottle_ginseng,
		lang_bottle_elixir,
	};
	return gb.language.get(strs[(uint8_t)player.getBottle(i)]);
}

const MultiLang* areaStrings[] = {
	lang_area_dogglen,        //  0
	lang_area_forest,         //  1
	lang_area_east_mountains, //  2
	lang_area_aerilon,        //  3
	lang_area_ugana_plain,    //  4
	lang_area_aerilon_castle, //  5
	lang_area_eastern_peaks,  //  6
	lang_area_fragile_caves,  //  7
	lang_area_gnah_lake,      //  8
	lang_area_caves,          //  9
	lang_area_cliff_break,    // 10
	lang_area_antique_castle, // 11
	lang_area_desert,         // 12
	lang_area_castle_present, // 13
	lang_area_castle_past,    // 14
};

const uint8_t areaStringsLUT[] = {
	0,
	1,
	1,
	2,
	3,
	4,
	5,
	5,
	6,
	7,
	8,
	9,
	10,
	4,
	11,
	12,
	4,
	13,
	13,
	14,
};

const MultiLang* weaponStrings[] = {
	lang_none,              // 0
	lang_weapon_stick,      // 1
	lang_weapon_axe,        // 2
	lang_weapon_sword,      // 3
	lang_weapon_breadsword, // 4
	lang_weapon_toxicsword, // 5
};

const uint8_t weaponStringsLUT[] = {
	0,
	0,
	1,
	0,
	2,
	3,
	4,
	5,
};

const MultiLang* armorStrings[] = {
	lang_armor_wounds,
	lang_none,
	lang_armor_shield,
};

const uint8_t armorStringsLUT[] = {
	0,
	1,
	0,
	2,
};

const MultiLang* tradequestStrings[] = {
	lang_trade_letter,
	lang_trade_letter_2,
	lang_trade_food,
	lang_trade_fish_rod,
	lang_trade_fish,
	lang_trade_necklace,
	lang_trade_shell,
	lang_trade_rug,
	lang_trade_fox,
};

void drawStatsMenuPage(uint8_t i) {
	gb.display.setColor(BEIGE);
	const uint8_t devider = 9*4 + 6;
	gb.display.fillRect(devider+1, 1, gb.display.width() - devider - 2, 6*6 + 3);
	gb.display.setColor(BLACK);
	switch (i) {
		case 0:
			gb.display.setCursor(devider + 2, 4);
			gb.display.print("HP:");
			gb.display.setCursor(devider + 2, 10);
			gb.display.printf("%d/%d", player.getHp(), player.getHpMax());
			
			gb.display.setCursor(devider + 2, 16);
			gb.display.print("MP:");
			gb.display.setCursor(devider + 2, 22);
			gb.display.printf("%d/%d", player.getMp(), player.getMpMax());
			
			gb.display.setCursor(devider + 2, 28);
			gb.display.print(lang_stats_gold);
			gb.display.print(player.getGold());
			
			if (player.isEvent(EVENT_SEARCH_QUEST)) {
				gb.display.setCursor(devider + 2, 34);
				gb.display.print(lang_stats_scare);
				gb.display.print(player.fright);
			}
			break;
		case 1:
			gb.display.setCursor(devider + 2, 4);
			gb.display.print(lang_stats_level);
			gb.display.print(player.getLvl());
			
			
			gb.display.setCursor(devider + 2, 10);
			gb.display.print(lang_stats_weapon);
			gb.display.setCursor(devider + 2, 16);
			gb.display.print(weaponStrings[weaponStringsLUT[player.sword]]);
			
			gb.display.setCursor(devider + 2, 22);
			gb.display.print(lang_stats_amror);
			gb.display.setCursor(devider + 2, 28);
			gb.display.print(armorStrings[armorStringsLUT[player.armor - ARMOR_WOUNDS]]);
			
			gb.display.setCursor(devider + 2, 34);
			gb.display.print(lang_stats_speed);
			gb.display.print(player.wait);
			break;
		case 2:
			
			gb.display.setCursor(devider + 2, 4);
			gb.display.printf("EXP:%d", player.getExp());
			
			gb.display.setCursor(devider + 2, 10);
			gb.display.print(lang_stats_next_exp);
			gb.display.setCursor(devider + 2, 16);
			if (player.getLvl() >= 40) {
				gb.display.print("---");
			} else {
				gb.display.print(player.getExpNext());
			}
			
			if (player.tradequest && player.tradequest < TRADEQUEST_DONE) {
				gb.display.setCursor(devider + 2, 22);
				gb.display.print(lang_stats_tradeitem);
				gb.display.setCursor(devider + 2, 28);
				gb.display.print(tradequestStrings[player.tradequest-1]);
			}
	}
}

void statsMenu() {
	bool have_items[7] = {
		player.isEvent(EVENT_ITEM_BOTTLE1),
		player.isEvent(EVENT_ITEM_BOTTLE2),
		player.isEvent(EVENT_ITEM_BOTTLE3),
		player.isEvent(EVENT_ITEM_BOTTLE4),
		player.isEvent(EVENT_ITEM_HOOKSHOT),
		player.isEvent(EVENT_ITEM_BOMB),
		player.isEvent(EVENT_ITEM_PROTECT),
	};
	uint8_t cur_item = player.getCurItem();
	uint8_t cursor = cur_item;
	if (!cursor) {
		cursor = 1;
	}
	const uint8_t devider = 9*4 + 6;
	gb.display.clear(BEIGE);
	gb.display.setColor(BLACK);
	gb.display.drawFastHLine(1, 0, devider-1);
	gb.display.drawFastHLine(devider+1, 0, gb.display.width()-devider-2);
	gb.display.drawFastHLine(1, gb.display.height()-1-8, devider-1);
	gb.display.drawFastHLine(devider+1, gb.display.height()-1-8, gb.display.width()-devider-2);
	gb.display.drawFastHLine(1, gb.display.height()-1, gb.display.width()-2);
	
	gb.display.drawFastVLine(0, 1, gb.display.height()-2-8);
	gb.display.drawFastVLine(gb.display.width()-1, 1, gb.display.height()-2-8);
	gb.display.drawFastVLine(devider, 1, gb.display.height()-2-7);
	gb.display.drawFastVLine(0, gb.display.height()-1-7, 7);
	gb.display.drawFastVLine(gb.display.width()-1, gb.display.height()-1-7, 7);
	
	if (have_items[0]) {
		gb.display.setCursor(6, 4);
		gb.display.print(getBottleString(0));
	}
	if (have_items[1]) {
		gb.display.setCursor(6, 10);
		gb.display.print(getBottleString(1));
	}
	if (have_items[2]) {
		gb.display.setCursor(6, 16);
		gb.display.print(getBottleString(2));
	}
	if (have_items[3]) {
		gb.display.setCursor(6, 22);
		gb.display.print(getBottleString(3));
	}
	if (have_items[4]) {
		gb.display.setCursor(6, 28);
		gb.display.print(lang_item_hookshot);
	}
	if (have_items[5]) {
		gb.display.setCursor(6, 34);
		gb.display.print(lang_item_bombs);
		gb.display.print(player.getBombs());
	}
	if (have_items[6]) {
		gb.display.setCursor(6, 40);
		gb.display.print(lang_item_protect);
	}
	
	gb.display.setCursor(2, gb.display.height()-7);
	gb.display.print(lang_area);
	gb.display.print(areaStrings[areaStringsLUT[board.getAreaId()]]);
	
	Image status_icons(status_icons_data);
	
	if (player.isEvent(EVENT_PICKAXE)) {
		status_icons.setFrame(0);
		gb.display.drawImage(devider + 3, 6*6 + 7, status_icons);
	}
	
	if (player.isEvent(EVENT_CAN_SWIM)) {
		status_icons.setFrame(1);
		gb.display.drawImage(devider + 3 + 12, 6*6 + 7, status_icons);
	}
	
	if (player.isEvent(EVENT_PUSH_BLOCKS)) {
		status_icons.setFrame(2);
		gb.display.drawImage(devider + 3 + 24, 6*6 + 7, status_icons);
	}
	
	uint8_t page = 0;
	drawStatsMenuPage(page);
	
	while(1) {
		waitCycles(1);
		gb.display.setColor(BEIGE);
		if (cur_item) {
			gb.display.fillRect(8*4 + 6, 6*(cur_item - 1) + 4, 4, 6);
		}
		gb.display.fillRect(2, 6*(cursor - 1) + 4, 4, 6);
		
		if (gb.buttons.pressed(BUTTON_C)) {
			return;
		}
		
		if (gb.buttons.pressed(BUTTON_UP)) {
			if (!--cursor) {
				cursor = 7;
			}
		}
		
		if (gb.buttons.pressed(BUTTON_DOWN)) {
			if (++cursor > 7) {
				cursor = 1;
			}
		}
		
		if (gb.buttons.pressed(BUTTON_LEFT)) {
			if (!page) {
				page = 2;
			} else {
				page--;
			}
			drawStatsMenuPage(page);
		}
		
		if (gb.buttons.pressed(BUTTON_RIGHT)) {
			page++;
			if (page > 2) {
				page = 0;
			}
			drawStatsMenuPage(page);
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			if (have_items[cursor - 1]) {
				player.setCurItem(cursor);
				cur_item = cursor;
			}
		}
		
		gb.display.setColor(BLACK);
		if (cur_item) {
			gb.display.setCursor(8*4 + 6, 6*(cur_item - 1) + 4);
			gb.display.print('\4');
		}
		gb.display.setCursor(2, 6*(cursor - 1) + 4);
		gb.display.print('\x1A');
		
		
//		gb.display.setCursor(0, 0);
//		gb.display.setColor(BLACK, WHITE);
//		gb.display.print(gb.getCpuLoad());
	}
}
