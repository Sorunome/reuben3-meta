#include "camera.h"
#include <Gamebuino-Meta.h>

void Camera::moveTo(int8_t _x, int8_t _y) {
	if (_x < x_min) {
		_x = x_min;
	}
	if (_x > x_max) {
		_x = x_max;
	}
	if (_y < y_min) {
		_y = y_min;
	}
	if (_y > y_max) {
		_y = y_max;
	}
	x = _x;
	y = _y;
}

void Camera::setY(int8_t _y) {
	y = _y;
}

void Camera::centerOn(int8_t _x, int8_t _y) {
	moveTo(_x - (gb.display.width()/2), _y - (gb.display.height()/2));
}


Camera camera;
