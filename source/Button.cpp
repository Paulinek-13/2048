// ==================================================
// file: Button.cpp
// project: 2048
// author: Paulina Kalicka
// ==================================================

#include "Button.h"

#include "PK13_Text.h"

void Button::Set(float sx, float sw, float sh, float cx, float cy, std::string text)
{
	this->sx   = sx;
	this->sw   = sw;
	this->sh   = sh;
	this->cx   = cx;
	this->cy   = cy;
	this->x1   = cx - (sw / 2.f);
	this->y1   = cy - (sh / 2.f);
	this->x2   = cx + (sw / 2.f);
	this->y2   = cy + (sh / 2.f);
	this->text = text;
}
void Button::Draw(ALLEGRO_BITMAP* bitmap, ALLEGRO_FONT* font)
{
	if(placed)
	{
		if(hovered)
		{
			al_draw_bitmap_region(bitmap, sx, sh, sw, sh, x1, y1, 0);
			pk13_draw_string(font, sw, sh * 1.2f, cx, cy, al_map_rgb(132, 132, 0), text.c_str());
		}
		else
		{
			al_draw_bitmap_region(bitmap, sx, 0.f, sw, sh, x1, y1, 0);
			pk13_draw_string(font, sw, sh, cx, cy, al_map_rgba(0, 0, 0, a), text.c_str());
		}
	}
	else
	{
		al_draw_tinted_scaled_rotated_bitmap_region(bitmap, sx, 0.f, sw, sh,
		                                            al_map_rgba(132, 132, 132, 132),
		                                            sw / 2.f, sh / 2.f, cx, cy,
		                                            scale, scale, angle, 0);
	}
}
void Button::OnTimer()
{
	if(!placed)
	{
		scale += 0.05f;
		angle += 0.75f;
		if(scale > 1.f)
		{
			scale  = 1.f;
			angle  = 0.f;
			placed = true;
		}
	}
	else
	{
		a += 5;
		if(a == 255) a = 250;
	}
}
void Button::MoveX(float velx)
{
	x1 += velx;
	cx += velx;
	x2 += velx;
}
bool Button::IsHovered(float mx, float my)
{
	if((mx > x1) && (mx < x2) && (my > y1) && (my < y2))
	{
		hovered = true;
		return true;
	}
	else
	{
		hovered = false;
		return false;
	}
}