// ==================================================
// file: App.h
// project: 2048
// author: Paulina Kalicka
// ==================================================

#ifndef APP_H
#define APP_H

#include "PK13_AllPaulinek.h"

#include <allegro5/allegro_audio.h>

struct App : PK13_App
{
	ALLEGRO_AUDIO_STREAM* audio_stream  = 0;
	ALLEGRO_SAMPLE*       sample_click  = 0;
	ALLEGRO_SAMPLE*       sample_click2 = 0;
	ALLEGRO_BITMAP*       bitmap_ui     = 0;
	ALLEGRO_BITMAP*       bitmap_mouse  = 0;

	unsigned char size       = 4; // number of boxes in a row and in a column
	unsigned long best_score = 0; // (ever)

	bool sounds = true;
	bool intro  = true;

	bool Init();
	void Clean();

	void Draw(int mx, int my);
};

#endif // !APP_H