#include "menu.h"
#include "player.h"
#include "language.h"
#include "misc.h"
#include "area.h"
#include "depack.h"
#include <Gamebuino-Meta.h>

const uint8_t cursor_data[] = {
	3, 3,
	0x01, 0x00,
	0,
	0x05,
	0x01,
	
	0xFF, 0xF5,
	0xF5, 0xF5,
	0xFF, 0xF5,
};

void menuDrawBg(File& f)  {
	if (f.peek() == -1) {
		f.rewind();
	}
	static const uint16_t dispOffset = 80*64/2;
	uint16_t size;
	f.read(&size, 2);
	f.read(&(gb.display._buffer[dispOffset]), size);
	aP_depack(&(gb.display._buffer[dispOffset]), gb.display._buffer);
}

void menuAskDeleteSlot(File& f, uint8_t slot, Image& cursorImg) {
	bool cursor = 0;
	const char* confirmStr = gb.language.get(lang_menu_delete_savegame);
	while(1) {
		while(!gb.update());
		menuDrawBg(f);
		
		cursor = max(0, min(1, cursor + gb.buttons.pressed(BUTTON_DOWN) - gb.buttons.pressed(BUTTON_UP)));
		
		gb.display.setColor(BEIGE);
		gb.display.setCursor(40 - strlen(confirmStr)*2, 34);
		gb.display.print(confirmStr);
		gb.display.setCursor(40, 42);
		gb.display.print(lang_no);
		gb.display.setCursor(40, 48);
		gb.display.print(lang_yes);
		gb.display.drawImage(35, 42+1+6*cursor, cursorImg);
		
		if (gb.buttons.pressed(BUTTON_A)) {
			if (!cursor) {
				return;
			}
			gb.save.del(slot*2);
			return;
		}
		
//		gb.display.setColor(BLACK, WHITE);
//		gb.display.setCursor(0, 0);
//		gb.display.print(gb.getCpuLoad());
	}
}

void menuLoadSlots(bool* haveSlot, SaveData* slot) {
	for (uint8_t i = 0; i < 3; i++) {
		haveSlot[i] = gb.save.get(i*2);
		if (!haveSlot[i]) {
			continue;
		}
		gb.save.get(i*2 + 1, slot[i]);
	}
}

bool menuPickSaveSlot(File& f, Image& cursorImg) {
	bool haveSlot[3];
	SaveData slot[3];
	menuLoadSlots(haveSlot, slot);
	const char* loadGameStr = gb.language.get(lang_menu_load_game);
	int8_t cursor = 0;
	static const int8_t cursorMax = 2;
	
	while(1) {
		while(!gb.update());
		menuDrawBg(f);
		
		cursor = max(0, min(cursorMax, cursor + gb.buttons.pressed(BUTTON_DOWN) - gb.buttons.pressed(BUTTON_UP)));
		
		gb.display.setColor(BEIGE);
		gb.display.setCursor(40 - strlen(loadGameStr)*2, 34);
		gb.display.print(loadGameStr);
		
		for (uint8_t i = 0; i < 3; i++) {
			gb.display.setCursor(17, 42+6*i);
			if (!haveSlot[i]) {
				gb.display.print(lang_menu_empty_slot);
			} else {
				gb.display.print(lang_menu_level);
				gb.display.print(slot[i].lvl);
			}
		}
		gb.display.drawImage(12, 43+6*cursor, cursorImg);
		
		if (gb.buttons.pressed(BUTTON_MENU)) {
			return false;
		}
		
		if (gb.buttons.pressed(BUTTON_A)) {
			player.init(cursor);
			player.load();
			return true;
		}
		
		if (gb.buttons.pressed(BUTTON_B) && haveSlot[cursor]) {
			menuAskDeleteSlot(f, cursor, cursorImg);
			menuLoadSlots(haveSlot, slot);
		}
		
//		gb.display.setColor(BLACK, WHITE);
//		gb.display.setCursor(0, 0);
//		gb.display.print(gb.getCpuLoad());
	}
	return false;
}

void menuInstructions() {
	Image buttonsIcons(Gamebuino_Meta::buttonsIconsData);
	Image arrowsIcons(Gamebuino_Meta::arrowsIconsData);
	gb.display.clear(BEIGE);
	gb.display.setColor(BLACK);
	gb.display.drawFastHLine(1, 0, gb.display.width()-2);
	gb.display.drawFastHLine(1, gb.display.height()-1, gb.display.width()-2);
	gb.display.drawFastVLine(0, 1, gb.display.height()-2);
	gb.display.drawFastVLine(gb.display.width()-1, 1, gb.display.height()-2);
	
	gb.display.setCursor(25, 17);
	gb.display.print(lang_instruction_move);
	
	gb.display.setCursor(25, 32);
	gb.display.print(lang_instructions_interact);
	
	gb.display.setCursor(25, 43);
	gb.display.print(lang_instructions_use_item);
	
	gb.display.setCursor(25, 54);
	gb.display.print(lang_instructions_stats_menu);
	
	const char* instructionsStr = gb.language.get(lang_menu_instructions);
	gb.display.setCursor(40 - strlen(instructionsStr)*2, 3);
	gb.display.print(instructionsStr);
	while(1) {
		while(!gb.update());
		
		// draw arrows
		static const uint8_t arrowX = 5;
		static const uint8_t arrowY = 11;
		uint8_t activeArrow = (gb.frameCount%16)/4;
		static const uint8_t arrowXOffsets[] = { 5,  0, 10,  5};
		static const uint8_t arrowYOffsets[] = {10,  5,  5,  0};
		static const uint8_t arrowMap[] = {1, 3, 2, 0};
		for (uint8_t i = 0; i < 4; i++) {
			uint8_t j = arrowMap[i];
			arrowsIcons.setFrame(j*2 + (activeArrow==i ? 1 : 0));
			gb.display.drawImage(arrowX + arrowXOffsets[j], arrowY + arrowYOffsets[j], arrowsIcons);
		}
		
		uint8_t activeButton = (gb.frameCount%12)/4;
		
		// A icon
		buttonsIcons.setFrame(0 + (activeButton==0 ? 1 : 0));
		gb.display.drawImage(9, 30, buttonsIcons);
		
		// B icon
		buttonsIcons.setFrame(2 + (activeButton==1 ? 1 : 0));
		gb.display.drawImage(9, 41, buttonsIcons);
		
		// menu icon
		buttonsIcons.setFrame(4 + (activeButton==2 ? 1 : 0));
		gb.display.drawImage(9, 52, buttonsIcons);
		
		if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_MENU)) {
			break;
		}
	}
}

void mainMenu() {
	Image cursorImg(cursor_data);
	
	area.go(area_theme);
	gb.display.init(80, 64, ColorMode::index);
	File f = SD.open("assets/fita.raw");
	
	int8_t cursor = 0;
	static const int8_t cursorMax = 2;
	bool inMainMenu = true;
	while(inMainMenu) {
		while(!gb.update());
		menuDrawBg(f);
		
		cursor = max(0, min(cursorMax, cursor + gb.buttons.pressed(BUTTON_DOWN) - gb.buttons.pressed(BUTTON_UP)));
		
		gb.display.setColor(BEIGE);
		gb.display.setCursor(20, 40);
		gb.display.print(lang_menu_start);
		gb.display.setCursor(20, 46);
		gb.display.println(lang_menu_instructions);
		gb.display.setCursor(20, 52);
		gb.display.print(lang_menu_credits);
		gb.display.drawImage(15, 41+6*cursor, cursorImg);
		
		if (gb.buttons.pressed(BUTTON_A)) {
			switch(cursor) {
				case 0:
					if (menuPickSaveSlot(f, cursorImg)) {
						inMainMenu = false;
					}
					break;
				case 1:
					gb.display.init(80, 64, ColorMode::rgb565);
					menuInstructions();
					gb.display.init(80, 64, ColorMode::index);
					menuDrawBg(f);
					break;
				case 2:
					credits();
					break;
			}
		}
		
//		gb.display.setColor(BLACK, WHITE);
//		gb.display.setCursor(0, 0);
//		gb.display.print(gb.getCpuLoad());
	}
	f.close();
	gb.display.init(80, 64, ColorMode::rgb565);
}
