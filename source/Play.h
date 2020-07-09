// ==================================================
// file: Play.h
// project: 2048
// author: Paulina Kalicka
// ==================================================

#ifndef PLAY_H
#define PLAY_H

#include "App.h"
#include "PK13_AppState.h"

#include <vector>

struct Box
{
	float x1   = 0.f;
	float y1   = 0.f;
	float x2   = 0.f;
	float y2   = 0.f;
	float cx   = 0.f;
	float cy   = 0.f;
	float rxy  = 0.f;
	float p[2] = {0.f, 0.f};

	unsigned value = 2; // 2, 4, 8, 16, ...

	bool blocked = false; // if can be merged

	void Set(float px, float py, float size);
	void Draw(ALLEGRO_FONT* font, ALLEGRO_COLOR color);
	void Move(float velx, float vely);
	void Put(float dx, float dy);
};

struct MoveData
{
	float    velx = 0.f;
	float    vely = 0.f;
	float    dx   = 0.f;
	float    dy   = 0.f;
	unsigned dval = 0; // destination value of a box
};

struct InfoBar
{
	float y2        = 10.f;
	float speed     = 20.f;
	bool  extending = false;

	void Draw(ALLEGRO_FONT* font);
	void OnTimer(float max_y2);
	void OnMouse(float mx, float my);
};

struct Play : PK13_AppState
{
	App*    app     = 0;
	Box*    new_box = 0; // pointer to a newest box
	InfoBar info_bar;

	std::vector<std::vector<Box>> boxes;  // size x size
	std::vector<std::vector<Box>> pboxes; // previous boxes

	ALLEGRO_SAMPLE* sample_key = 0;
#define COLORS_N 6
	ALLEGRO_COLOR colors[COLORS_N] =
	    {
	        al_map_rgb(232, 0, 0),
	        al_map_rgb(0, 232, 0),
	        al_map_rgb(0, 0, 232),
	        al_map_rgb(232, 232, 0),
	        al_map_rgb(232, 0, 232),
	        al_map_rgb(0, 232, 232),
	    };

	std::vector<std::vector<MoveData>> move_data; // information needed for movement animation

	bool animating = false; // if there is a movement animation now
	bool load_best = true;
	bool game_over = false;

	unsigned char N = 0;  // number of boxes in a row and in a column
	unsigned char a = 50; // alpha for the newest box
#define DEFAULT_A 3
	unsigned char A          = DEFAULT_A; // how many timer ticks should animation consists of
	unsigned long curr_score = 0;

	bool Init();
	void Clean();
	void Draw();
	void Events(ALLEGRO_EVENT event);
};

#endif // !PLAY_H
