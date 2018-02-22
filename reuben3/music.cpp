#include "music.h"
#include <Gamebuino-Meta.h>

const char* tracks[] = {
	"songs/home.wav",
	"songs/lost_in_the_woods.wav",
};

const uint8_t areaLUT[] = {
	   0, // dogglen
	   1, // forest
	   1, // forest
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
	0xFF,
};

void Music::playSong(uint8_t song) {
	if (song == curSong) {
		return;
	}
	curSong = song;
	gb.sound.stop(trackId);
	if (song == 0xFF) {
		trackId = -1;
		return;
	}
	trackId = gb.sound.play(tracks[song], true);
}

void Music::stop() {
	playSong(0xFF);
}

void Music::playArea(uint8_t area) {
	uint8_t t = areaLUT[area];
	playSong(t);
}

Music music;
