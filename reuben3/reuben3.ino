#include <Gamebuino-Meta.h>
#include "board.h"
#include "camera.h"
#include "player.h"
#include "data/defines.h"

#include "misc.h"



#include "battle.h"

uint8_t decompression_buffer[max(2048, ENEMY_SPRITES_MAX_SIZE)]; // 2048 > 12*8*2*8 = 1536

void setup() {
	gb.begin();
	SerialUSB.begin(115200);
	//while(!SerialUSB);
	board.load(WORLD_OVERWORLD, TILEMAP_37);
	board.postload();
	player.init();
	
	battle.fight(2);
	gb.sound.play("songs/home.wav", true);
}


void loop() {
	if (!gb.update()) {
		return;
	}
	if (gb.buttons.pressed(BUTTON_C)) {
		statsMenu();
		return; // make sure we do accidental shadowbutton triggers
	}
	player.update();
	
	
	renderAll();
}
