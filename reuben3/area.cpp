#include "area.h"
#include "music.h"
#include "ambient.h"

void Area::go(uint8_t a) {
	music.playArea(a);
	ambient.setArea(a);
}

Area area;
