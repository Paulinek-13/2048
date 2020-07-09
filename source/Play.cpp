// ==================================================
// file: Play.cpp
// project: 2048
// author: Paulina Kalicka
// ==================================================

#include "Play.h"

#include "Header.h"
#include "PK13_Text.h"

#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <string>

const float GRID_SIZE = 500.f;

#define SAVED_PATH "data//saved.txt"
#define BEST_PATH  "data//best.txt"

static unsigned ulog2(unsigned number)
{
	unsigned i = 0;
	while(number /= 2) ++i;
	return i;
}

static bool LoadGrid(std::vector<std::vector<Box>>& boxes, unsigned char size, unsigned long& curr_score)
{
	std::ifstream file;
	file.open(SAVED_PATH);
	if(file.is_open())
	{
		for(unsigned char x = 0; x < size; ++x)
			for(unsigned char y = 0; y < size; ++y)
				file >> boxes[x][y].value;
		file >> curr_score;
		return true;
	}
	else
		return false;
}
static bool SaveGrid(std::vector<std::vector<Box>>& boxes, unsigned char size, unsigned long curr_score)
{
	std::ofstream file;
	file.open(SAVED_PATH);
	if(file.is_open())
	{
		for(unsigned char x = 0; x < size; ++x)
			for(unsigned char y = 0; y < size; ++y)
				file << boxes[x][y].value << " ";
		file << curr_score;
		return true;
	}
	else
		return false;
}
static bool CanMove(std::vector<std::vector<Box>>& boxes, unsigned char size)
{
	for(unsigned char x = 1; x < size - 1; ++x)
	{
		for(unsigned char y = 1; y < size - 1; ++y)
		{
			unsigned val = boxes[x][y].value;
			if(boxes[x][y - 1].value == val) return true;
			if(boxes[x][y + 1].value == val) return true;
			if(boxes[x - 1][y].value == val) return true;
			if(boxes[x + 1][y].value == val) return true;
		}
	}
	for(unsigned char x = 0; x < size; x += size - 1) // for only 2 columns
	{
		for(unsigned char y = 1; y < size - 1; ++y)
		{
			unsigned val = boxes[x][y].value;
			if(boxes[x][y - 1].value == val) return true;
			if(boxes[x][y + 1].value == val) return true;
		}
	};
	for(unsigned char y = 0; y < size; y += size - 1) // for only 2 rows
	{
		for(unsigned char x = 1; x < size - 1; ++x)
		{
			unsigned val = boxes[x][y].value;
			if(boxes[x - 1][y].value == val) return true;
			if(boxes[x + 1][y].value == val) return true;
		}
	};
	return false;
}

void Box::Set(float px, float py, float size)
{
	x1    = px;
	y1    = py;
	x2    = px + size;
	y2    = py + size;
	cx    = px + (size / 2.f);
	cy    = py + (size / 2.f);
	rxy   = 0.25f * size;
	value = 0;
}
void Box::Draw(ALLEGRO_FONT* font, ALLEGRO_COLOR color)
{
	al_draw_filled_rounded_rectangle(x1, y1, x2, y2, rxy, rxy, color);
	pk13_draw_string(font, x2 - x1, y2 - y1, cx, cy, al_map_rgb(0, 0, 0),
	                 std::to_string(value).c_str());
}
void Box::Move(float velx, float vely)
{
	x1 += velx;
	x2 += velx;
	cx += velx;
	y1 += vely;
	y2 += vely;
	cy += vely;
}
void Box::Put(float px, float py)
{
	float size = x2 - x1;
	x1         = px;
	y1         = py;
	x2         = px + size;
	y2         = py + size;
	cx         = px + (size / 2.f);
	cy         = py + (size / 2.f);
}

void InfoBar::Draw(ALLEGRO_FONT* font)
{
	al_draw_filled_rectangle(0.f, 0.f, 960.f, y2, al_map_rgb(232, 132, 0));
	if(extending)
	{
		float h  = y2 / 4.f;
		float hh = h / 2.f;
		pk13_draw_string(font, 960.f, h, 480.f, y2 * 0.25 - hh, al_map_rgb(232, 232, 232), "R - reset game");
		pk13_draw_string(font, 960.f, h, 480.f, y2 * 0.5 - hh, al_map_rgb(232, 232, 232), "S - save game");
		pk13_draw_string(font, 960.f, h, 480.f, y2 * 0.75 - hh, al_map_rgb(232, 232, 232), "L - load game");
		pk13_draw_string(font, 960.f, h, 480.f, y2 - hh, al_map_rgb(232, 232, 232), "TAB - go to menu");
	}
}
void InfoBar::OnTimer(float max_y2)
{
	if(extending)
	{
		y2 += speed;
		speed *= 0.99f;
		if(y2 > max_y2)
		{
			y2    = max_y2;
			speed = 20.f;
		}
	}
	else
	{
		y2 -= speed;
		speed *= 1.5f;
		if(y2 < 10.f)
		{
			y2    = 10.f;
			speed = 20.f;
		}
	}
}
void InfoBar::OnMouse(float mx, float my)
{
	extending = ((my < y2) && (mx > 0) && (mx < 960)) ? true : false;
}

namespace
{
	struct Transition : PK13_Transition
	{
		short a     = 250;
		float scale = 0.05f;

		bool Init()
		{
			return true;
		}
		void Clean() {}
		void OnTimer()
		{
			a -= 10;
			scale += 0.2f;
			if(scale >= 1.f) _end = true;
		}
		void Draw(PK13_App* app)
		{
			al_clear_to_color(COLOR1);
			al_draw_scaled_rotated_bitmap(app->_dest_state, app->_hw, app->_hh, app->_hw, app->_hh,
			                              scale, scale, 0.f, 0);

			al_flip_display();
		}
	};
} // namespace

bool Play::Init()
{
	N = app->size;

	boxes.resize(N);
	pboxes.resize(N);
	for(unsigned char i = 0; i < N; ++i)
	{
		boxes[i].resize(N);
		pboxes[i].resize(N);
	}

	// init boxes
	float size = GRID_SIZE / (N);
	float px   = (app->_w - GRID_SIZE) / 2.f;
	float py   = (app->_h - GRID_SIZE) / 2.f;
	for(unsigned char x = 0; x < N; ++x)
	{
		for(unsigned char y = 0; y < N; ++y)
		{
			boxes[x][y].Set(px, py, size);
			py += size;
			pboxes[x][y] = boxes[x][y];
		}
		px += size;
		py = (app->_h - GRID_SIZE) / 2.f;
	}
	boxes[rand() % N][rand() % N].value = rand() % 2 ? 2 : 4;
	boxes[rand() % N][rand() % N].value = rand() % 2 ? 2 : 4;

	sample_key = al_load_sample("resources//music//key.ogg");
	if(!sample_key) return false;

	move_data.resize(N);
	for(unsigned char i = 0; i < N; ++i)
		move_data[i].resize(N);

	if(load_best)
	{
		load_best = false;
		std::ifstream file;
		file.open(BEST_PATH);
		if(file.is_open())
			file >> app->best_score;
	}

	_transition = std::make_unique<Transition>();

	return true;
}
void Play::Clean()
{
	// save best score to file
	std::ofstream file;
	file.open(BEST_PATH);
	if(file.is_open())
		file << app->best_score;

	al_destroy_sample(sample_key);
}
void Play::Draw()
{
	al_clear_to_color(COLOR1);

	al_draw_filled_rounded_rectangle(210.f, 90.f, 750.f, 630.f, 13.f, 13.f,
	                                 al_map_rgb(0, 0, 0));

	// draw grid (boxes)
	for(unsigned char x = 0; x < N; ++x)
		for(unsigned char y = 0; y < N; ++y)
			if(boxes[x][y].value)
				boxes[x][y].Draw(app->_font, colors[ulog2(boxes[x][y].value) % COLORS_N]);

	if(new_box)
		al_draw_filled_rounded_rectangle(new_box->x1, new_box->y1, new_box->x2, new_box->y2,
		                                 new_box->rxy, new_box->rxy,
		                                 al_map_rgba(a, a, a, a));

	al_draw_textf(app->_font, al_map_rgb(0, 0, 0), app->_hw, 30.f, ALLEGRO_ALIGN_CENTER, "%s %d", "score: ", curr_score);
	al_draw_textf(app->_font, al_map_rgb(0, 0, 0), app->_hw, 650.f, ALLEGRO_ALIGN_CENTER, "%s %d", "best score ever: ", app->best_score);

	info_bar.Draw(app->_font);

	if(game_over)
	{
		al_draw_filled_rectangle(0.f, 0.f, app->_w, app->_h, al_map_rgba(232, 0, 0, 32));
		pk13_draw_string(app->_font, app->_w, app->_h, app->_hw, app->_hh, al_map_rgb(0, 0, 0), "GAME OVER");
	}

	app->Draw(_mx_gui, _my_gui);

	al_flip_display();
}
void Play::Events(ALLEGRO_EVENT event)
{
	switch(event.type)
	{
		case ALLEGRO_EVENT_TIMER:
		{
			if(!game_over)
			{
				if((new_box) && (!(--a)))
				{
					new_box = 0;
					a       = 50;
				}
				if(animating)
				{
					--A;
					for(unsigned char x = 0; x < N; ++x)
						for(unsigned char y = 0; y < N; ++y)
							boxes[x][y].Move(move_data[x][y].velx, move_data[x][y].vely);

					if(!A) // movement animation should stop now
					{
						for(unsigned char x = 0; x < N; ++x)
						{
							for(unsigned char y = 0; y < N; ++y)
							{
								boxes[x][y].value = move_data[x][y].dval;
								boxes[x][y].Put(move_data[x][y].dx, move_data[x][y].dy);
							}
						}
						animating = false;
						A         = DEFAULT_A;
					}
				}

				info_bar.OnTimer(350.f);
			}

			break;
		}
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			if(game_over)
			{
				game_over   = false;
				_next_state = int(STATE::PLAY);
				_end        = true;
			}

			break;
		}
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			if(game_over)
			{
				game_over   = false;
				_next_state = int(STATE::PLAY);
				_end        = true;
			}

			if(!animating && !game_over)
			{
				bool moved   = true;
				bool changed = false;

				for(unsigned char x = 0; x < N; ++x)
					for(unsigned char y = 0; y < N; ++y)
						pboxes[x][y] = boxes[x][y];

				switch(event.keyboard.keycode)
				{
					case ALLEGRO_KEY_RIGHT:
					{
						for(short x = N - 1; x >= 0; --x)
						{
							for(short y = 0; y < N; ++y)
							{
								boxes[x][y].p[0] = boxes[x][y].x1;
								boxes[x][y].p[1] = boxes[x][y].y1;
								if(boxes[x][y].value != 0)
								{
									unsigned char xx   = x + 1;
									unsigned char dest = N; // destination position x
									while(xx < N)
									{
										if(boxes[xx][y].value == boxes[x][y].value)
										{
											if(!boxes[xx][y].blocked)
											{
												boxes[xx][y].value += boxes[x][y].value;
												curr_score += boxes[xx][y].value;
												boxes[x][y].value    = 0;
												boxes[xx][y].blocked = true;
												changed              = true;
												boxes[x][y].p[0]     = boxes[xx][y].x1;
												boxes[x][y].p[1]     = boxes[xx][y].y1;
											}
											break;
										}
										else if(!boxes[xx][y].value)
											dest = xx;
										else
											break;
										++xx;
									}
									if(dest != N)
									{
										boxes[dest][y].value = boxes[x][y].value;
										boxes[x][y].value    = 0;
										changed              = true;
										boxes[x][y].p[0]     = boxes[dest][y].x1;
										boxes[x][y].p[1]     = boxes[dest][y].y1;
									}
								}
							}
						}

						break;
					}
					case ALLEGRO_KEY_LEFT:
					{
						for(short x = 0; x < N; ++x)
						{
							for(short y = 0; y < N; ++y)
							{
								boxes[x][y].p[0] = boxes[x][y].x1;
								boxes[x][y].p[1] = boxes[x][y].y1;
								if(boxes[x][y].value != 0)
								{
									short         xx   = x - 1;
									unsigned char dest = N; // destination position x
									while(xx >= 0)
									{
										if(boxes[xx][y].value == boxes[x][y].value)
										{
											if(!boxes[xx][y].blocked)
											{
												boxes[xx][y].value += boxes[x][y].value;
												curr_score += boxes[xx][y].value;
												boxes[x][y].value    = 0;
												boxes[xx][y].blocked = true;
												changed              = true;
												boxes[x][y].p[0]     = boxes[xx][y].x1;
												boxes[x][y].p[1]     = boxes[xx][y].y1;
											}
											break;
										}
										else if(!boxes[xx][y].value)
											dest = xx;
										else
											break;
										--xx;
									}
									if(dest != N)
									{
										boxes[dest][y].value = boxes[x][y].value;
										boxes[x][y].value    = 0;
										changed              = true;
										boxes[x][y].p[0]     = boxes[dest][y].x1;
										boxes[x][y].p[1]     = boxes[dest][y].y1;
									}
								}
							}
						}

						break;
					}
					case ALLEGRO_KEY_UP:
					{
						for(short y = 0; y < N; ++y)
						{
							for(short x = 0; x < N; ++x)
							{
								boxes[x][y].p[0] = boxes[x][y].x1;
								boxes[x][y].p[1] = boxes[x][y].y1;
								if(boxes[x][y].value != 0)
								{
									short         yy   = y - 1;
									unsigned char dest = N; // destination position y
									while(yy >= 0)
									{
										if(boxes[x][yy].value == boxes[x][y].value)
										{
											if(!boxes[x][yy].blocked)
											{
												boxes[x][yy].value += boxes[x][y].value;
												curr_score += boxes[x][yy].value;
												boxes[x][y].value    = 0;
												boxes[x][yy].blocked = true;
												changed              = true;
												boxes[x][y].p[0]     = boxes[x][yy].x1;
												boxes[x][y].p[1]     = boxes[x][yy].y1;
											}
											break;
										}
										else if(!boxes[x][yy].value)
											dest = yy;
										else
											break;
										--yy;
									}
									if(dest != N)
									{
										boxes[x][dest].value = boxes[x][y].value;
										boxes[x][y].value    = 0;
										changed              = true;
										boxes[x][y].p[0]     = boxes[x][dest].x1;
										boxes[x][y].p[1]     = boxes[x][dest].y1;
									}
								}
							}
						}

						break;
					}
					case ALLEGRO_KEY_DOWN:
					{
						for(short y = N - 1; y >= 0; --y)
						{
							for(short x = 0; x < N; ++x)
							{
								boxes[x][y].p[0] = boxes[x][y].x1;
								boxes[x][y].p[1] = boxes[x][y].y1;
								if(boxes[x][y].value != 0)
								{
									short         yy   = y + 1;
									unsigned char dest = N; // destination position y
									while(yy < N)
									{
										if(boxes[x][yy].value == boxes[x][y].value)
										{
											if(!boxes[x][yy].blocked)
											{
												boxes[x][yy].value += boxes[x][y].value;
												curr_score += boxes[x][yy].value;
												boxes[x][y].value    = 0;
												boxes[x][yy].blocked = true;
												changed              = true;
												boxes[x][y].p[0]     = boxes[x][yy].x1;
												boxes[x][y].p[1]     = boxes[x][yy].y1;
											}
											break;
										}
										else if(!boxes[x][yy].value)
											dest = yy;
										else
											break;
										++yy;
									}
									if(dest != N)
									{
										boxes[x][dest].value = boxes[x][y].value;
										boxes[x][y].value    = 0;
										changed              = true;
										boxes[x][y].p[0]     = boxes[x][dest].x1;
										boxes[x][y].p[1]     = boxes[x][dest].y1;
									}
								}
							}
						}

						break;
					}
					case ALLEGRO_KEY_R:
					{
						_next_state = int(STATE::PLAY);
						_end        = true;
						break;
					}
					case ALLEGRO_KEY_S:
					{
						if(!SaveGrid(boxes, N, curr_score))
							al_show_native_message_box(0, "ERROR", "Grid not saved!",
							                           "",
							                           0, ALLEGRO_MESSAGEBOX_ERROR);
						else
							al_show_native_message_box(0, "SUCCESS", "Grid saved!",
							                           "",
							                           0, ALLEGRO_MESSAGEBOX_OK_CANCEL);
						break;
					}
					case ALLEGRO_KEY_L:
					{
						if(!LoadGrid(boxes, N, curr_score))
							al_show_native_message_box(0, "ERROR", "Grid not loaded!",
							                           "",
							                           0, ALLEGRO_MESSAGEBOX_ERROR);
						else
							al_show_native_message_box(0, "SUCCESS", "Grid loaded!",
							                           "",
							                           0, ALLEGRO_MESSAGEBOX_OK_CANCEL);
						break;
					}
					case ALLEGRO_KEY_TAB:
					{
						_next_state = int(STATE::MENU);
						_end        = true;
						break;
					}

					default: moved = false; break;
				}

				if(moved)
				{
					al_play_sample(sample_key, app->sounds, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

					std::vector<std::vector<unsigned char>> empty; // indexes of boxes with 0 value

					bool full = true;
					for(unsigned char x = 0; x < N; ++x)
					{
						for(unsigned char y = 0; y < N; ++y)
						{
							boxes[x][y].blocked = false;
							if(!boxes[x][y].value)
							{
								full = false;
								empty.push_back({x, y});
							}
						}
					}

					if(full)
					{
						if(!CanMove(boxes, N))
							game_over = true;
					}
					if(changed)
					{
						unsigned      rnd = rand() % empty.size();
						unsigned char x   = empty[rnd][0];
						unsigned char y   = empty[rnd][1];
						boxes[x][y].value = rand() % 5 ? 4 : 2;
						new_box           = &boxes[x][y];
						a                 = 50;
						if(curr_score > app->best_score)
							app->best_score = curr_score;

						for(unsigned char x = 0; x < N; ++x)
						{
							for(unsigned char y = 0; y < N; ++y)
							{
								MoveData data;
								data.velx       = (boxes[x][y].p[0] - boxes[x][y].x1) / float(A);
								data.vely       = (boxes[x][y].p[1] - boxes[x][y].y1) / float(A);
								data.dx         = boxes[x][y].x1;
								data.dy         = boxes[x][y].y1;
								data.dval       = boxes[x][y].value;
								move_data[x][y] = data;
								boxes[x][y]     = pboxes[x][y];
								animating       = true; // starts movement
							}
						}
					}
				}
			}

			break;
		}
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			if(!game_over) info_bar.OnMouse(_mx, _my);

			break;
		}

		default: break;
	}
}