#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gl/glu.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>


// default values, to be set by the configuration file (config.ini)
class Defaults{
public:
	static int step_flag;
	static int display_width;
	static int display_height;

	static int model_speed;
	static bool model_horizontal_wrapping;
	static bool model_vertical_wrapping;
	static int model_width;
	static int model_height;
	static int model_num_threads;
	static int model_enable_threads;
};

#define USER_VIEW_EVENT ALLEGRO_GET_EVENT_TYPE('j','o','r','d')

#endif
