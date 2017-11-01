#include <Gamebuino-Meta.h>
#include "board.h"
#include "camera.h"
#include "player.h"
#include "data/defines.h"

#include "script.h"

//uint8_t scr[] = {0x08, 0x00, 0x0e, 0x0d, 0x0b, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x02, 0xff};

uint8_t scr[] = {0x08, 0x00, 0x00, 0x0e, 0x00, 0x0d, 0x0b, 0x80, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x03, 0xff};
void setup() {
	gb.begin();
	SerialUSB.begin(115200);
	//while(!SerialUSB);
	board.load(WORLD_OVERWORLD, TILEMAP_37);
	board.postload();
	player.moveTo(7*8, 3*8);
	player.focus();
	while(!gb.update());
	script.run(scr);
}


void loop() {
	if (!gb.update()) {
		return;
	}
	player.update();
	
	
	board.render();
	player.render();
	gb.display.setCursor(0, 0);
	gb.display.setColor(BLACK, WHITE);
	gb.display.print(gb.getCpuLoad());
}
