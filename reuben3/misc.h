#ifndef _MISC_H_
#define _MISC_H_
#include <Arduino.h>

void waitCycles(uint8_t num);
bool waitCyclesButton(uint8_t num);
void waitRelease();
void renderAll();

#endif // _MISC_H_
