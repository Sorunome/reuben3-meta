#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <Arduino.h>
#include "data/defines.h"

class Script {
	private:
		uint8_t vars[SCRIPT_NUM_VARS];
		bool condition();
		uint8_t* getVar();
		uint8_t getNum();
		uint16_t getNum16();
		void jump();
		uint8_t* script;
		uint8_t* script_entry;
		bool isHome;
		int8_t homeX;
		int8_t homeY;
		uint8_t homeMap;
		uint8_t homeWorld;
		uint8_t trigger;
		void checkHome();
	public:
		bool run(const uint8_t* _script, uint8_t _trigger = 0, bool _isHome = true);
		bool run(uint8_t* _script, uint8_t _trigger = 0, bool _isHome = true);
};

extern Script script;

#endif // _SCRIPT_H_
