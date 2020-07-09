// ==================================================
// file: Button.h
// project: 2048
// author: Paulina Kalicka
// ==================================================

#ifndef BUTTON_H
#define BUTTON_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>

struct Button
{
	float sx    = 0.f; // sy will always be 0
	float sw    = 0.f;
	float sh    = 0.f;
	float cx    = 0.f;
	float cy    = 0.f;
	float x1    = 0.f;
	float y1    = 0.f;
	float x2    = 0.f;
	float y2    = 0.f;
	float scale = 0.05f;
	float angle = 0.f;

	std::string text = "empty";

	bool hovered = false;
	bool placed  = false;

	unsigned char a = 0;

	void Set(float sx, float sw, float sh, float cx, float cy, std::string text);
	void Draw(ALLEGRO_BITMAP* bitmap, ALLEGRO_FONT* font);
	void OnTimer();
	void MoveX(float velx);
	bool IsHovered(float mx, float my);
};

#endif // !BUTTON_H