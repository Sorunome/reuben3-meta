#ifndef _MISC_H_
#define _MISC_H_
#include <Arduino.h>

void waitCycles(uint8_t num);
bool waitCyclesButton(uint8_t num);
void waitRelease();
void renderAll();
void statsMenu();
void bomb(int8_t x, int8_t y);

#endif // _MISC_H_
