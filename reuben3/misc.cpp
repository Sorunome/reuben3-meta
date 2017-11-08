#include "misc.h"

#include <Gamebuino-Meta.h>

void waitCycles(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
	}
}

bool waitCyclesButton(uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		while(!gb.update());
		if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_LEFT) || gb.buttons.pressed(BUTTON_RIGHT)) {
			return true;
		}
	}
	return false;
}

void waitRelease() {
	while(1) {
		while(!gb.update());
		if (gb.buttons.released(BUTTON_A) || gb.buttons.released(BUTTON_B) || gb.buttons.released(BUTTON_LEFT) || gb.buttons.released(BUTTON_RIGHT)) {
			return;
		}
	}
}
