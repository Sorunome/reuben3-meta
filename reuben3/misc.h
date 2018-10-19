#ifndef _MISC_H_
#define _MISC_H_
#include <Arduino.h>

#include "player.h" // for Direction datatype

void waitCycles(uint8_t num);
bool waitCyclesButton(uint8_t num);
void waitRelease();
void renderAll();
void bomb(int8_t x, int8_t y);
void hookshot(int8_t x, int8_t y, Direction dir);
void pushblock(int8_t x, int8_t y, Direction dir);

bool shop(uint16_t ask, uint16_t price, bool bottle);

void fade_to_white();
void fade_from_white();

void fade_to_black();
void fade_to_black(void (*r)(void));
void fade_from_black();

void battleInstructions();
void increaseSpeedArena();
void searchquestPerson();

void intro();

void outro();
void credits();

void map();

void raft(uint16_t frames, bool rain);

#endif // _MISC_H_
