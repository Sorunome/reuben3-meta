#include <Gamebuino-Meta.h>
#include "board.h"
#include "camera.h"

void setup() {
	gb.begin();
	SerialUSB.begin(115200);
	//while(!SerialUSB);
	board.load();
	camera.centerOn(0, 0);
}

void loop() {
	if (!gb.update()) {
		return;
	}
	board.render();
}
