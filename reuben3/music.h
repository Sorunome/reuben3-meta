#ifndef _MUSIC_H_
#define _MUSIC_H_
#include <Arduino.h>


class Music {
	private:
		uint8_t curSong = 0xFF;
		int8_t trackId = -1;
		void playSong(uint8_t song);
	public:
		void stop();
		void playArea(uint8_t area);
		void play(const char* s);
};

extern Music music;

#endif // _MUSIC_H_
