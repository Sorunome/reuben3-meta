#include "area.h"
#include "music.h"
#include "ambient.h"

void Area::go(uint8_t a) {
	music.playArea(a);
	ambient.setArea(a);
}

const uint8_t area_battle = 20;
const uint8_t area_theme = 21;
const uint8_t area_portal = 22;

Area area;
