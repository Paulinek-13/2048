// ==================================================
// file: Header.h
// project: 2048
// author: Paulina Kalicka
// ==================================================

#ifndef HEADER_H
#define HEADER_H

#include "PK13_AllPaulinek.h"

enum class STATE
{
	EXIT = PK13_EXIT_STATE,
	MENU = 1,
	PLAY
};

#define COLOR1 al_map_rgb(232, 232, 0)
#define COLOR2 al_map_rgb(232, 132, 0)
#define COLOR3 al_map_rgb(200, 200, 200)
#define COLOR4 al_map_rgb(123, 123, 123)

#define MIN_SIZE 4
#define MAX_SIZE 16

#endif // !HEADER_H