// ==================================================
// file: Menu.cpp
// project: 2048
// author: Paulina Kalicka
// ==================================================

#include "Menu.h"

#include "Header.h"
#include "PK13_Text.h"

#include <math.h>

namespace
{
	struct Transition : PK13_Transition
	{
		short a = 250;

		bool Init()
		{
			return true;
		}
		void Clean() {}
		void OnTimer()
		{
			a -= 20;
			if(a < 20) _end = true;
		}
		void Draw(PK13_App* app)
		{
			al_clear_to_color(COLOR1);
			al_draw_tinted_bitmap(app->_dest_state, al_map_rgba(0, 0, 0, a),
			                      0, 0, 0);
			al_flip_display();
		}
	};
} // namespace

bool Menu::Init()
{
	sample_name_move = al_load_sample("resources//music//move.ogg");
	if(!sample_name_move) return false;

	buttons[BUTTON::PLAY].Set(0.f, 190.f, 45.f, app->_hw, app->_hh, "play");
	buttons[BUTTON::GRID].Set(0.f, 190.f, 45.f, app->_hw, app->_hh + 50.f,
	                          std::to_string(app->size));
	buttons[BUTTON::GRID_L].Set(190.f, 39.f, 28.f, app->_hw * 0.5f, app->_hh + 50.f, "");
	buttons[BUTTON::GRID_R].Set(229.f, 39.f, 28.f, app->_w * 0.75f, app->_hh + 50.f, "");
	buttons[BUTTON::MUSIC].Set(0.f, 190.f, 45.f, app->_hw, app->_hh + 100.f,
	                           al_get_audio_stream_playing(app->audio_stream) ? "music off" : "music on");
	buttons[BUTTON::SOUNDS].Set(0.f, 190.f, 45.f, app->_hw, app->_hh + 150.f,
	                            app->sounds ? "sounds off" : "sounds on");

	if(!app->intro)
	{
		show_gui  = true;
		name_move = true;
	}
	else
		app->intro = false;

	_transition = std::make_unique<Transition>();

	return true;
}
void Menu::Clean()
{
	al_destroy_sample(sample_name_move);
}
void Menu::Draw()
{
	al_clear_to_color(COLOR1);

	pk13_draw_string(app->_font, 500, 100, app->_hw, name_y, al_map_rgb(0, 0, 0), "2048");

	if(show_gui)
		for(unsigned char i = 0; i < BUTTON::_N_; ++i)
			buttons[i].Draw(app->bitmap_ui, app->_font);

	app->Draw(_mx_gui, _my_gui);

	al_flip_display();
}
void Menu::Events(ALLEGRO_EVENT event)
{
	switch(event.type)
	{
		case ALLEGRO_EVENT_TIMER:
		{
			if(name_move)
			{
				name_y -= 0.05f * name_y;
				if(name_y < 100.f)
				{
					name_move = false;
					show_gui  = true;
				}
			}
			else
			{
				name_y += sin(name_ii);
				name_ii -= 0.1;
			}

			if(show_gui)
				for(unsigned char i = 0; i < BUTTON::_N_; ++i)
					buttons[i].OnTimer();

			if(leaving)
			{
				for(unsigned char i = 0; i < BUTTON::_N_; ++i)
				{
					buttons[i].MoveX(velx);
				}
				if(velx > 1.f)
				{
					velx *= 0.75;
				}
				else
				{
					velx = -abs(1.75f * velx);
					if(buttons[0].x2 < -20.f)
					{
						_next_state = int(STATE::PLAY);
						_end        = true;
						break;
					}
				}
			}

			break;
		}
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			if(!show_gui)
			{
				name_move = true;
				al_play_sample(sample_name_move, app->sounds, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			}
			else
			{
				bool clicked = true;

				if(buttons[BUTTON::PLAY].hovered)
				{
					leaving = true;
				}
				else if(buttons[BUTTON::GRID].hovered)
				{
					buttons[BUTTON::GRID].text = std::to_string(app->size = 4);
				}
				else if(buttons[BUTTON::GRID_L].hovered && (app->size > MIN_SIZE))
				{
					buttons[BUTTON::GRID].text = std::to_string(--(app->size));
				}
				else if(buttons[BUTTON::GRID_R].hovered && (app->size < MAX_SIZE))
				{
					buttons[BUTTON::GRID].text = std::to_string(++(app->size));
				}
				else if(buttons[BUTTON::MUSIC].hovered)
				{
					bool on = al_get_audio_stream_playing(app->audio_stream);
					al_set_audio_stream_playing(app->audio_stream, !on);
					buttons[BUTTON::MUSIC].text = (on ? "music on" : "music off");
				}
				else if(buttons[BUTTON::SOUNDS].hovered)
				{
					buttons[BUTTON::SOUNDS].text = ((app->sounds = !app->sounds)
					                                    ? "music off"
					                                    : "music on");
				}
				else
				{
					clicked = false;
					al_play_sample(app->sample_click2, app->sounds, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
					break;
				}

				if(clicked)
					al_play_sample(app->sample_click, app->sounds, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			}

			break;
		}
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			if(show_gui)
				for(unsigned char i = 0; i < BUTTON::_N_; ++i)
					buttons[i].IsHovered(_mx, _my);

			break;
		}

		default: break;
	}
}