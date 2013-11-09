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

#include "AllegroShell.h"
#include "Model.h"

int size_x = 480;
int size_y = 480;

AllegroShell::AllegroShell(){
	fps = 100;
	display_h = 480;
	display_w = 480;
	run_flag = true;
	draw_flag = false;
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

	// initializing the allegro objects
	display = al_create_display(display_h,display_w);
	font_carbon_12 = al_load_font("data/carbon.ttf",12,0);
	queue = al_create_event_queue();
	timer = al_create_timer(1.0/fps);

	// registering the even sourcees
	al_register_event_source(queue,al_get_timer_event_source(timer));
	al_register_event_source(queue,al_get_display_event_source(display));
	al_register_event_source(queue,al_get_mouse_event_source());
	al_register_event_source(queue,al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();

	model = new Model(size_x,size_y);
	step_flag = false;
	step_once_flag = false;

}

AllegroShell::~AllegroShell(){
	al_stop_timer(timer);
	al_destroy_timer(timer);
	al_destroy_font(font_carbon_12);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	delete model;
	printf("AllegroShell destroyed\n");
}

bool AllegroShell::isKeyboardEvent(ALLEGRO_EVENT* ev){
	switch(ev->type){
		case(ALLEGRO_EVENT_KEY_UP):
		case(ALLEGRO_EVENT_KEY_DOWN):
		case(ALLEGRO_EVENT_KEY_CHAR): return true;
		default: return false;
	}
}
bool AllegroShell::isMouseEvent(ALLEGRO_EVENT* ev){
	switch(ev->type){
		case(ALLEGRO_EVENT_MOUSE_AXES):
		case(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN):
		case(ALLEGRO_EVENT_MOUSE_BUTTON_UP): return true;
		default: return false;
	}
}

void AllegroShell::handle_keyboard(){
	static int gaurd = 0;
	gaurd--;
	if( gaurd < 0 ){ gaurd = 0;}



	if(al_key_down(&keyboard,ALLEGRO_KEY_ESCAPE) ) {run_flag = false;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_Q) ) {
		model->speed++;
		if( model->speed >100){
			model->speed = 100;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_W) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_E) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_A) ) {
		model->speed--;
		if( model->speed <= 0){
			model->speed = 1;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_S) ) {
		if( gaurd == 0){
			step_flag = (step_flag == false)? true: false;
			gaurd = 2;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_D) ) {
		step_once_flag = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_R) ) {
		model->reset();
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_N) ) {
		delete model;
		model = new Model(size_x,size_y);
	}

	if(al_key_down(&keyboard,ALLEGRO_KEY_1) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_2) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_3) ) {;}

}
void AllegroShell::handle_mouse(){

}

void AllegroShell::draw(){
	al_clear_to_color(al_map_rgb(0,0,0));
	model->draw(display_w,display_h);
	ALLEGRO_COLOR c = al_map_rgb(120,120,80);
	al_draw_textf(font_carbon_12,c,5,5,0,"steps: %d",model->total_steps);
	al_draw_textf(font_carbon_12,c,5,20,0,"population: %d",model->population);
	al_draw_textf(font_carbon_12,c,5,35,0,"speed: %d",model->speed);
			
	al_flip_display();
}

void AllegroShell::run(){
// run the loop
	ALLEGRO_EVENT ev;
	al_start_timer(timer);
	while( run_flag ){
		al_wait_for_event(queue,&ev);

		if( ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			run_flag = false;
		}else if( isKeyboardEvent(&ev)){
			al_get_keyboard_state(&(keyboard));
			handle_keyboard();
		}else if( isMouseEvent(&ev) ){
			al_get_mouse_state(&(mouse));
			handle_mouse();
		}else if( ev.type  == ALLEGRO_EVENT_TIMER){
			
			// Update only if we haven't drawn the scene
			if(draw_flag == false){

				if( step_once_flag){
					// always step once if the user requests it
					model->step();
					model->tick_count = 0;
					step_once_flag = false;
				}else if(step_flag) {
					// step only if we have matched speeds
					if(model->tick_count < model->speed) {
						model->tick_count++;
					}else{
						model->step();
						model->tick_count =0;
					}
				}
			}
			draw_flag = true;
		}

		if( draw_flag && al_event_queue_is_empty(queue)){
			draw();
			draw_flag = false;
		}
	}
}
