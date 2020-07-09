// ==================================================
// file: Menu.h
// project: 2048
// author: Paulina Kalicka
// ==================================================

#ifndef MENU_H
#define MENU_H

#include "App.h"
#include "Button.h"
#include "PK13_AppState.h"

#include <allegro5/allegro_audio.h>

struct Menu : PK13_AppState
{
	App* app = 0;

	ALLEGRO_SAMPLE* sample_name_move = 0;

	float name_y  = 360.f;
	float name_ii = 10000.f; // -1 .. +1
	float velx    = 10.f;    // for leaving buttons

	bool name_move = false;
	bool show_gui  = false;
	bool leaving   = false;

	enum BUTTON
	{
		PLAY,
		GRID,
		GRID_L,
		GRID_R,
		MUSIC,
		SOUNDS,
		_N_
	};
	Button buttons[_N_] = {0};

	bool Init();
	void Clean();
	void Draw();
	void Events(ALLEGRO_EVENT event);
};

#endif // !MENU_H