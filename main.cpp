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

#include "GameOfLife.h"
#include "AllegroShell.h"


int Defaults::step_flag= 0;
int Defaults::display_width = 480;
int Defaults::display_height= 480;
int Defaults::model_speed=10;
bool Defaults:: model_horizontal_wrapping=true;
bool Defaults:: model_vertical_wrapping=true;
int Defaults::model_width=480;
int Defaults::model_height=480;
int Defaults::model_num_threads=10;
int Defaults::model_enable_threads=false;


void load_configuration(const char* filename){
	ALLEGRO_CONFIG* config = al_load_config_file(filename);
	if(!config){return;}

	const char* value = 0;
	int num_value  = 0;


	value = al_get_config_value(config,"","step_flag");
	if( value){
		num_value = atoi(value);
		Defaults::step_flag = num_value;
	}

	value = al_get_config_value(config,"","display_width");
	if( value){
		num_value = atoi(value);
		Defaults::display_width = num_value;
	}


	value = al_get_config_value(config,"","display_height");
	if( value){
		num_value = atoi(value);
		Defaults::display_height = num_value;
	}


	value = al_get_config_value(config,"","model_speed");
	if( value){
		num_value = atoi(value);
		Defaults::model_speed = num_value;
	}


	value = al_get_config_value(config,"","model_horizontal_wrapping");
	if( value){
		num_value = atoi(value);
		Defaults::model_horizontal_wrapping = num_value;
	}


	value = al_get_config_value(config,"","model_vertical_wrapping");
	if( value){
		num_value = atoi(value);
		Defaults::model_vertical_wrapping = num_value;
	}


	value = al_get_config_value(config,"","model_width");
	if( value){
		num_value = atoi(value);
		Defaults::model_width = num_value;
	}


	value = al_get_config_value(config,"","model_height");
	if( value){
		num_value = atoi(value);
		Defaults::model_height = num_value;
	}


	value = al_get_config_value(config,"","model_num_threads");
	if( value){
		num_value = atoi(value);
		Defaults::model_num_threads= num_value;
	}

	value = al_get_config_value(config,"","model_enable_threads");
	if( value){
		num_value = atoi(value);
		Defaults::model_enable_threads = num_value;
	}

	al_destroy_config(config);
}

int main(int argc, char** argv){
	load_configuration("config.ini");
	AllegroShell s;
	s.run();
}