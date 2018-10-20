#include <Gamebuino-Meta.h>
#include <RTCZero.h>
#include "player.h"
#include "data/defines.h"
#include "stats.h"
#include "misc.h"
#include "text.h"
#include "menu.h"

RTCZero rtc;
uint8_t decompression_buffer[max(2048, ENEMY_SPRITES_MAX_SIZE)]; // 2048 > 12*8*2*8 = 1536

const SaveDefault savefileDefaults[] = {
	{ 0, SAVETYPE_INT, 0, 0 },
	{ 1, SAVETYPE_BLOB, sizeof(SaveData), 0 },
	{ 2, SAVETYPE_INT, 0, 0 },
	{ 3, SAVETYPE_BLOB, sizeof(SaveData), 0 },
	{ 4, SAVETYPE_INT, 0, 0 },
	{ 5, SAVETYPE_BLOB, sizeof(SaveData), 0 },
};

void setup() {
	gb.begin();
	gb.save.config(savefileDefaults);

	text.init();
	
	rtc.begin();
//	SerialUSB.begin(115200);
}

void gameLoop() {
	while (!player.isDead() && !player.hasWon()) {
		waitCycles(1);
		if (gb.buttons.pressed(BUTTON_C)) {
			statsMenu();
			continue; // make sure we do accidental shadowbutton triggers
		}
		player.update();
		
		renderAll();
	}
	if (player.isDead()) {
		player.saveTimeOnly();
		gb.display.clear();
		gb.display.println("Game Over");
		gb.display.println("Press A");
		while(1) {
			while(!gb.update());
			if (gb.buttons.pressed(BUTTON_A)) {
				break;
			}
		}
	}
	if (player.hasWon()) {
		player.saveTimeOnly();
		gb.display.clear();
		gb.display.println("Finished Game!!!");
		gb.display.println("Press A");
		while(1) {
			while(!gb.update());
			if (gb.buttons.pressed(BUTTON_A)) {
				break;
			}
		}
	}
}

void loop() {
	mainMenu();
	gameLoop();
}
