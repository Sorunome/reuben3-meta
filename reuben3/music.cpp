#include "music.h"
#include <Gamebuino-Meta.h>

const char* tracks[] = {
	"songs/home.wav",              // 0
	"songs/lost_in_the_woods.wav", // 1
	"songs/home2.wav",             // 2
	"songs/dungeon_master.wav",    // 3
	"songs/plains.wav",            // 4
	"songs/desert.wav",            // 5
};

const uint8_t areaLUT[] = {
	   0, // dogglen
	   1, // forest
	   1, // forest
	0xFF, // mountains
	   2, // aerilon
	   4, // ugana plain
	   3, // aerilon castle
	   3, // aerilon castle
	   3, // eastern peaks
	   3, // fragile caves
	   3, // gnah lake
	0xFF, // caves
	0xFF, // cliff break
	   4, // ugana plain
	   3, // antique castle
	   5, // desert
	   4, // ugana plain
	   3, // castle present
	   3, // castle present
	   3, // castle past
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
