#include "stats.h"
#include "misc.h"
#include "board.h"
#include "player.h"
#include <Gamebuino-Meta.h>

const char* getBottleString(uint8_t i) {
	const char* strs[] = {
		"Bottle",
		"badWater",
		"Water",
		"Herb",
		"Potion",
		"Ginseng",
		"Elixir",
	};
	return strs[(uint8_t)player.getBottle(i)];
}

const char* areaStrings[] = {
	"Dogglen",        //  0
	"Forest",         //  1
	"East Mountains", //  2
	"Aerilon",        //  3
	"Ugana Plain",    //  4
	"Aerilon Castle", //  5
	"Eastern Peaks",  //  6
	"Fragile Caves",  //  7
	"Gnah Lake",      //  8
	"Caves",          //  9
	"Cliff-Break",    // 10
	"Antique Castle", // 11
	"Desert",         // 12
	"Castle Present", // 13
	"Castle Past",    // 14
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

const char* weaponStrings[] = {
	"none",      // 0
	"Stick",     // 1
	"Axe",       // 2
	"Sword",     // 3
	"BreadSwrd", // 4
	"ToxicSwrd", // 5
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

const char* armorStrings[] = {
	"Wounds",
	"none",
	"Shield",
};

const uint8_t armorStringsLUT[] = {
	0,
	1,
	0,
	2,
};

const char* tradequestStrings[] = {
	"Letter",
	"Letter 2",
	"Food",
	"Fish Rod",
	"Fish",
	"Necklace",
	"Shell",
	"Rug",
	"Fox",
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
			gb.display.printf("Gold:%d", player.getGold());
			
			if (player.isEvent(EVENT_SEARCH_QUEST)) {
				gb.display.setCursor(devider + 2, 34);
				gb.display.printf("Scare:%d", player.fright);
			}
			break;
		case 1:
			gb.display.setCursor(devider + 2, 4);
			gb.display.printf("Level:%d", player.getLvl());
			
			
			gb.display.setCursor(devider + 2, 10);
			gb.display.print("Weapon:");
			gb.display.setCursor(devider + 2, 16);
			gb.display.print(weaponStrings[weaponStringsLUT[player.sword]]);
			
			gb.display.setCursor(devider + 2, 22);
			gb.display.print("Armor:");
			gb.display.setCursor(devider + 2, 28);
			gb.display.print(armorStrings[armorStringsLUT[player.armor - ARMOR_WOUNDS]]);
			
			gb.display.setCursor(devider + 2, 34);
			gb.display.printf("Speed:%d", player.wait);
			break;
		case 2:
			
			gb.display.setCursor(devider + 2, 4);
			gb.display.printf("EXP:%d", player.getExp());
			
			gb.display.setCursor(devider + 2, 10);
			gb.display.print("Next EXP:");
			gb.display.setCursor(devider + 2, 16);
			if (player.getLvl() >= 40) {
				gb.display.print("---");
			} else {
				gb.display.print(player.getExpNext());
			}
			
			if (player.tradequest && player.tradequest < TRADEQUEST_DONE) {
				gb.display.setCursor(devider + 2, 22);
				gb.display.print("TradeItm:");
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
		gb.display.print("Hookshot");
	}
	if (have_items[5]) {
		gb.display.setCursor(6, 34);
		gb.display.print("Bombs:");
		gb.display.print(player.getBombs());
	}
	if (have_items[6]) {
		gb.display.setCursor(6, 40);
		gb.display.print("Protect");
	}
	
	gb.display.setCursor(2, gb.display.height()-7);
	gb.display.print("Area:");
	gb.display.print(areaStrings[areaStringsLUT[board.getAreaId()]]);
	
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
