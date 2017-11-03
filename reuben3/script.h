#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <Arduino.h>
#include "data/defines.h"

class Script {
	private:
		int8_t vars[SCRIPT_NUM_VARS];
		bool condition();
		uint8_t* getVar();
		uint8_t getNum();
		void jump();
		uint8_t* script;
		uint8_t* script_entry;
	public:
		bool run(const uint8_t* _script);
		bool run(uint8_t* _script);
};

extern Script script;

#endif // _SCRIPT_H_
