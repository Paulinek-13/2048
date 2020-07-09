// ==================================================
// file: main.cpp
// project: 2048
// author: Paulina Kalicka
// ==================================================

#include "App.h"
#include "Header.h"
#include "Menu.h"
#include "PK13_AllPaulinek.h"
#include "Play.h"
#include "vld.h"

#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

int main()
{
	PK13_INIT_ALLEGRO;
	PK13_INSTALL_MOUSE;
	PK13_INSTALL_KEYBOARD;
	PK13_INSTALL_AUDIO;
	PK13_INIT_ACODEC_ADDON;
	PK13_INIT_FONT_ADDON;
	PK13_INIT_TTF_ADDON;
	PK13_INIT_IMAGE_ADDON;
	PK13_INIT_PRIMITIVES_ADDON;
	PK13_INIT_NATIVE_DIALOG_ADDON;

	srand(time(0));

	App app;

	if(app.Init())
	{
		bool  end   = false;
		STATE state = STATE::MENU;

		while(!end)
		{
			switch(state)
			{
				case STATE::MENU:
				{
					Menu menu;
					menu.app = &app;
					state    = STATE(menu._Loop(&app));
					break;
				}
				case STATE::PLAY:
				{
					Play play;
					play.app = &app;
					state    = STATE(play._Loop(&app));
					break;
				}

				case STATE::EXIT:
				default: end = true; break;
			}
		}
	}

	app.Clean();

	return 0;
}