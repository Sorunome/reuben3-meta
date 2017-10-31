#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <Arduino.h>

class Camera {
	private:
		static const int8_t x_min = 0;
		static const int8_t x_max = 16;
		static const int8_t y_min = 0;
		static const int8_t y_max = 00;
	public:
		int8_t x = 0;
		int8_t y = 0;
		void moveTo(int8_t _x, int8_t _y);
		void centerOn(int8_t _x, int8_t _y);
};

extern Camera camera;

#endif // _CAMERA_H_
