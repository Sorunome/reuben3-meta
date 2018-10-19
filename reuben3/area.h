#ifndef _AREA_H_
#define _AREA_H_
#include <Arduino.h>

class Area {
	public:
		void go(uint8_t a);
};

extern Area area;
extern const uint8_t area_battle;
extern const uint8_t area_theme;
extern const uint8_t area_portal;
extern const uint8_t area_rain;

#endif // _AREA_H_
