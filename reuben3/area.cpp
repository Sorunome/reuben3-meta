#include "area.h"
#include "music.h"
#include "ambient.h"

void Area::go(uint8_t a) {
	music.playArea(a);
	ambient.setArea(a);
}

const uint8_t area_battle = 22;
const uint8_t area_theme = 23;
const uint8_t area_portal = 24;
const uint8_t area_rain = 25;

Area area;
