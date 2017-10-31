#include <Gamebuino-Meta.h>
#include "board.h"
#include "camera.h"
#include "player.h"
#include "data/defines.h"

void setup() {
	gb.begin();
	SerialUSB.begin(115200);
	//while(!SerialUSB);
	board.load(WORLD_OVERWORLD, TILEMAP_37);
	board.postload();
	player.moveTo(7*8, 3*8);
	player.focus();
}

void loop() {
	if (!gb.update()) {
		return;
	}
	player.update();
	
	
	board.render();
	player.render();
//	gb.display.setCursor(0, 0);
//	gb.display.setColor(BLACK, WHITE);
//	gb.display.print(gb.getCpuLoad());
}
