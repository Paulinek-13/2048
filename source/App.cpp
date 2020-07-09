// ==================================================
// file: App.cpp
// project: 2048
// author: Paulina Kalicka
// ==================================================

#include "App.h"

bool App::Init()
{
	// todo: add icon
	if(!PK13_App::_Init(960, 720, "2048", "resources//fonts//kenney_blocks.ttf", "resources//icon.png", 1 / 20.0, ALLEGRO_RESIZABLE))
		return false;

	al_register_event_source(_event_queue, al_get_mouse_event_source());
	al_register_event_source(_event_queue, al_get_keyboard_event_source());

	al_reserve_samples(2);
	audio_stream = al_load_audio_stream("resources//music//Arpent.ogg", 2, 1024);
	if(!audio_stream) return false;
	al_set_audio_stream_playmode(audio_stream, ALLEGRO_PLAYMODE_LOOP);
	al_set_audio_stream_gain(audio_stream, 0.5f);
	al_set_audio_stream_playing(audio_stream, 1);
	al_attach_audio_stream_to_mixer(audio_stream, al_get_default_mixer());

	sample_click = al_load_sample("resources//music//click.ogg");
	if(!sample_click) return false;
	sample_click2 = al_load_sample("resources//music//click2.ogg");
	if(!sample_click) return false;

	bitmap_ui = al_load_bitmap("resources//bitmaps//ui.png");
	if(!bitmap_ui) return false;
	bitmap_mouse = al_load_bitmap("resources//bitmaps//mouse.png");
	if(!bitmap_mouse) return false;

	al_hide_mouse_cursor(_display);

	return true;
}
void App::Clean()
{
	PK13_App::_Clean();

	al_destroy_audio_stream(audio_stream);
	al_destroy_sample(sample_click);
	al_destroy_sample(sample_click2);
	al_destroy_bitmap(bitmap_ui);
	al_destroy_bitmap(bitmap_mouse);
}
void App::Draw(int mx, int my)
{
	al_draw_bitmap(bitmap_mouse, mx, my, 0);
}