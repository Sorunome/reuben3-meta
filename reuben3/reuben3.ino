#include <Gamebuino-Meta.h>
#include "player.h"
#include "data/defines.h"
#include "stats.h"
#include "misc.h"


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

//	SerialUSB.begin(115200);
}

void gameLoop() {
	while (!player.isDead()) {
		waitCycles(1);
		if (gb.buttons.pressed(BUTTON_C)) {
			statsMenu();
			continue; // make sure we do accidental shadowbutton triggers
		}
		player.update();
		
		renderAll();
	}
	if (player.isDead()) {
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
}

void loop() {
	player.init(0);
	player.load();
	gameLoop();
}
