#include <Gamebuino-Meta.h>
#include "board.h"
#include "camera.h"
#include "player.h"
#include "data/defines.h"

#include "misc.h"


uint8_t decompression_buffer[2048]; // 2048 > 12*8*2*8 = 1536

void setup() {
	gb.begin();
	SerialUSB.begin(115200);
	//while(!SerialUSB);
	board.load(WORLD_OVERWORLD, TILEMAP_37);
	board.postload();
	player.init();
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
