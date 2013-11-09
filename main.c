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

struct al_object{
	ALLEGRO_DISPLAY* frame;
	ALLEGRO_FONT* font;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_TIMER* timer;
	ALLEGRO_KEYBOARD_STATE keyboard;
	ALLEGRO_MOUSE_STATE mouse;
	
	ALLEGRO_AUDIO_STREAM* stream;
	ALLEGRO_MIXER* mixer;
	ALLEGRO_VOICE* voice;

	ALLEGRO_SAMPLE* sample1;
	ALLEGRO_SAMPLE* sample2;
	ALLEGRO_SAMPLE* sample3;
	ALLEGRO_SAMPLE_INSTANCE* instance1;
	ALLEGRO_SAMPLE_INSTANCE* instance2;
	ALLEGRO_SAMPLE_INSTANCE* instance3;
};

struct mmouse_t{
	int start_x;
	int start_y;
	int old_x;
	int old_y;
	int pressed;
	ALLEGRO_MOUSE_STATE* mouse;
};


void initAllegro(struct al_object*);
void destroyAllegro(struct al_object*);
int isKeyboardEvent(ALLEGRO_EVENT* ev);
int isMouseEvent(ALLEGRO_EVENT* ev);
void draw_mouse(struct mmouse_t* m);


int main(int argc, char** argv){	
	struct al_object m;
	memset(&m,0,sizeof(struct al_object));
	initAllegro(&m);
	struct mmouse_t mouse = { 0,0,0,0,0,&m.mouse};

	int red = 0;int green = 0; int blue = 0;
	int run = 1;
	int draw = 0;
	ALLEGRO_EVENT ev;
	al_start_timer(m.timer);
	while(run) {
		al_wait_for_event(m.queue,&ev);

		if( ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			run = 0;
		}else if( isKeyboardEvent(&ev) ){
			al_get_keyboard_state(&m.keyboard);
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_Q) ) { red = (red +1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_W) ) { green = (green +1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_E) ) { blue = (blue +1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_A) ) { red = abs(red -1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_S) ) { green = abs(green-1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_D) ) { blue = abs(blue -1)%255; }
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_R) ){ red = 0; green = 0; blue = 0;}
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_ESCAPE)) { run = 0;}
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_1) ) {al_play_sample_instance(m.instance1);}
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_2) ) {al_play_sample_instance(m.instance2);}
			if( al_key_down(&m.keyboard,ALLEGRO_KEY_3) ) {al_play_sample_instance(m.instance3);}

		}else if( isMouseEvent(&ev) ){
			al_get_mouse_state(&m.mouse);
		}else if(ev.type == ALLEGRO_EVENT_TIMER){
			draw = 1;
		}

		if( draw && al_event_queue_is_empty(m.queue)){
			draw = 0;
			al_clear_to_color(al_map_rgb(red,green,blue));
			al_draw_textf(m.font,al_map_rgb(78,78,0),5,5,0,"%3d %3d %3d", red,green,blue);
			al_draw_textf(m.font,al_map_rgb(78,78,0),120,5,0,"Mouse:(%3d, %3d)",m.mouse.x, m.mouse.y);
			al_draw_textf(m.font,al_map_rgb(78,78,0),300,5,0,"%d",m.mouse.buttons);


			al_draw_textf(m.font,al_map_rgb(78,78,0),5,18,0,"audio playing (%d,%d,%d)", \
							al_get_sample_instance_playing(m.instance1), \
							al_get_sample_instance_playing(m.instance2), \
							al_get_sample_instance_playing(m.instance3));
			draw_mouse(&mouse);
			al_flip_display();
		}
	}

	destroyAllegro(&m);
	return 0;
}







int isKeyboardEvent(ALLEGRO_EVENT* ev){
	switch(ev->type){
		case(ALLEGRO_EVENT_KEY_UP):;
		case(ALLEGRO_EVENT_KEY_DOWN):;
		case(ALLEGRO_EVENT_KEY_CHAR):return 1;
		default:return 0;
	}

}

int isMouseEvent(ALLEGRO_EVENT* ev){
	switch(ev->type) {	
		case(ALLEGRO_EVENT_MOUSE_AXES):;
		case(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN):;
		case(ALLEGRO_EVENT_MOUSE_BUTTON_UP):return 1;
		default:return 0;
	}

}



void draw_mouse(struct mmouse_t* m){	
	int b = m->mouse->buttons &1;
	if( b == 0 ){
		m->pressed = 0;
		al_draw_rectangle(m->start_x,m->start_y,m->old_x,m->old_y,al_map_rgb(0,0,0),1);
		m->start_x = 0;
		m->start_y = 0;
		m->old_x = 0;
		m->old_y = 0;
		return ;
	}

	if( m->pressed == 0 ){
		if( b == 1 ){
			m->pressed = 1;
			m->start_x = m->mouse->x;
			m->start_y = m->mouse->y;
		}
	}else {
		//clear the rectangle
		al_draw_rectangle(m->start_x,m->start_y,m->old_x,m->old_y,al_map_rgb(0,0,0),1);

		// draw the new rectangle
		al_draw_rectangle(m->start_x,m->start_y,m->mouse->x,m->mouse->y,al_map_rgb(255,0,0),1);
		m->old_x = m->mouse->x;
		m->old_y = m->mouse->y;
	}
}


void destroyAllegro(struct al_object* object){
	al_stop_timer(object->timer);


	al_destroy_sample(object->sample1);
	al_destroy_sample(object->sample2);
	al_destroy_sample(object->sample3);

	al_destroy_sample_instance(object->instance1);
	al_destroy_sample_instance(object->instance2);
	al_destroy_sample_instance(object->instance3);


	al_destroy_mixer(object->mixer);
	al_destroy_voice(object->voice);
	al_destroy_font(object->font);
	al_destroy_timer(object->timer);
	al_destroy_event_queue(object->queue);
	al_destroy_display(object->frame);
}

void initAllegro(struct al_object* borg){
	int x = 9;
	x -= al_init();
	x -= al_init_image_addon();
 	al_init_font_addon();
	x -= al_init_ttf_addon();
	x -= al_install_mouse();
	x -= al_install_keyboard();
	x -= al_install_audio();
	x -= al_init_acodec_addon();
	x -= al_init_primitives_addon();
	x -= al_reserve_samples(10);
	printf("%d errors during loading.\n",x);


	borg->frame = al_create_display(480,480);
	borg->font = al_load_font("data/carbon.ttf",12,0);
	borg->queue = al_create_event_queue();
	borg->timer = al_create_timer(1.0/60);
	
	borg->sample1 = al_load_sample("data\\fire_small.ogg");
	borg->sample2 = al_load_sample("data\\collision.ogg");
	borg->sample3 = al_load_sample("data\\game_music.ogg");
	borg->instance1 = al_create_sample_instance(borg->sample1);
	borg->instance2 = al_create_sample_instance(borg->sample2);
	borg->instance3 = al_create_sample_instance(borg->sample3);
	al_attach_sample_instance_to_mixer(borg->instance1,al_get_default_mixer());
	al_attach_sample_instance_to_mixer(borg->instance2,al_get_default_mixer());
	al_attach_sample_instance_to_mixer(borg->instance3,al_get_default_mixer());

	
	al_register_event_source(borg->queue,al_get_timer_event_source(borg->timer));
	al_register_event_source(borg->queue,al_get_display_event_source(borg->frame));
	al_register_event_source(borg->queue,al_get_mouse_event_source());
	al_register_event_source(borg->queue,al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
}
