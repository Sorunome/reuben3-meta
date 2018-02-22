#include "ambient.h"

const uint8_t lights[][3] = {
	{0, 80, 0},
	{3, 35, 3},
	{79, 65, 13},
};

const uint8_t areaLUT[] = {
	   0, // dogglen
	   1, // forest
	   1, // forest
	   2, // mountains
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
};

void Ambient::update() {
	flowColor();
	for (uint8_t y = 0; y < 4; y++) {
		for (uint8_t x = 0; x < 2; x++) {
			gb.lights.drawPixel(x, y, c[y*2 + x]);
		}
	}
}

void Ambient::setGotoColor() {
	int8_t offset = random(maxRandom);
	for (uint8_t i = 0; i < 3; i++) {
		int8_t o = (offset * base_c[i] / 0xFF);
		if (o > base_c[i]) {
			o = base_c[i];
		}
		goto_c[i] = base_c[i] - o;
	}
}

void Ambient::flowColor() {
	bool change = false;
	for (uint8_t i = 0; i < 3; i++) {
		if (goto_c[i] != cur_c[i]) {
			change = true;
			if (goto_c[i] > cur_c[i]) {
				if (cur_c[i] > (0xFF - 4)) {
					cur_c[i] = 0xFF - 4;
				} else {
					cur_c[i] += 4;
				}
				if (goto_c[i] < cur_c[i]) {
					cur_c[i] = goto_c[i];
				}
			} else {
				if (cur_c[i] < 4) {
					cur_c[i] = 0;
				} else {
					cur_c[i] -= 4;
				}
				if (goto_c[i] > cur_c[i]) {
					cur_c[i] = goto_c[i];
				}
			}
		}
	}
	if (!change) {
		setGotoColor();
	}
	
	for (uint8_t i = 7; i > 0; i--) {
		c[i] = c[i-1];
	}
	c[0] = gb.createColor(cur_c[0], cur_c[1], cur_c[2]);
}

void Ambient::on() {
	memcpy(base_c, back_c, sizeof(base_c));
}

void Ambient::off() {
	memset(base_c, 0, sizeof(base_c));
}

void Ambient::setArea(uint8_t a) {
	a = areaLUT[a];
	if (a == curAmbient) {
		return;
	}
	curAmbient = a;
	if (a == 0xFF) {
		memset(back_c, 0, sizeof(back_c));
		off();
		return;
	}
	memcpy(base_c, lights[a], sizeof(base_c));
	memcpy(back_c, base_c, sizeof(base_c));
	
	maxRandom = max(base_c[0], max(base_c[1], base_c[2]));
	
	
	setGotoColor();
}

Ambient ambient;
