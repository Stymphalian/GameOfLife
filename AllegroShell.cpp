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
#include "View.h"
#include "Model.h"

int size_x = 960;
int size_y = 960;

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
	queue = al_create_event_queue();
	timer = al_create_timer(1.0/fps);

	al_toggle_display_flag(display,ALLEGRO_FULLSCREEN_WINDOW,false);
	al_toggle_display_flag(display,ALLEGRO_NOFRAME,false);

	// registering the even sourcees
	al_register_event_source(queue,al_get_timer_event_source(timer));
	al_register_event_source(queue,al_get_display_event_source(display));
	al_register_event_source(queue,al_get_mouse_event_source());
	al_register_event_source(queue,al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();


	step_flag = false;
	step_once_flag = false;

	model = new Model(size_x,size_y);
	al_set_timer_speed(timer,1.0/model->speed);


	view = new View(this,model);
	al_register_event_source(queue,view->get_event_source());
	view->emit_event();
}

AllegroShell::~AllegroShell(){
	al_stop_timer(timer);
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	delete model;
	delete view;
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

void AllegroShell::handle_keyboard(ALLEGRO_EVENT* ev){
	if(al_key_down(&keyboard,ALLEGRO_KEY_ESCAPE) ) {run_flag = false;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_Q) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_CHAR|| 
				ev->type == ALLEGRO_EVENT_KEY_CHAR ){
			model->speed++;
			if( model->speed > 100){
				model->speed = 100;
			}
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_W) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_E) ) {;}
	if(al_key_down(&keyboard,ALLEGRO_KEY_A) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_CHAR || 
				ev->type == ALLEGRO_EVENT_KEY_CHAR ){
			model->speed--;
			if( model->speed <= 0){
				model->speed = 1;
			}
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_S) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_DOWN){
			step_flag = (step_flag == false)? true: false;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_D) ) {
		step_once_flag = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_R) ) {
		model->reset();
		al_set_timer_speed(timer,1.0/model->speed);
	}



	if(al_key_down(&keyboard,ALLEGRO_KEY_N) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_DOWN){
			delete model;
			model = new Model(size_x,size_y);
			view->model = model;
			draw_flag = true;
		}
	}

	// handle the arrow keys.
	if( ev->type == ALLEGRO_EVENT_KEY_CHAR) {
		bool key_touched = false;
		if(al_key_down(&keyboard,ALLEGRO_KEY_UP)){
			key_touched = true;
			view->cam_y -= view->cam_move_rate;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_DOWN)){
			key_touched = true;
			view->cam_y += view->cam_move_rate;	
		}			
		if(al_key_down(&keyboard,ALLEGRO_KEY_RIGHT)){
			key_touched = true;
			view->cam_x += view->cam_move_rate;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_LEFT)){
			key_touched = true;
			view->cam_x -= view->cam_move_rate;
		}

		if(key_touched){
			if( model->horizontal_wrapping){
				if(view->cam_x < 0){ view->cam_x = model->width + view->cam_x;}
				else if(view->cam_x >= model->width){ view->cam_x %= model->width;}
			}else{				
				if(view->cam_x >= model->width - view->cam_w){ view->cam_x = model->width - view->cam_w;}
				else if(view->cam_x < 0){ view->cam_x = 0;}
			}
			
			if( model->vertical_wrapping){
				if(view->cam_y < 0){ view->cam_y = model->height + view->cam_y;}
				else if(view->cam_y >= model->height){ view->cam_y %= model->height;}
			}else{
				if(view->cam_y >= model->height - view->cam_h){view->cam_y = model->height - view->cam_h ;}
				else if(view->cam_y < 0){view->cam_y = 0;}
			}
			draw_flag = true;
		}
	}
	// end handling of arrow keys

	// handle adjusting the camera size
	if( ev->type == ALLEGRO_EVENT_KEY_CHAR){
		bool key_touched = false;
		if(al_key_down(&keyboard,ALLEGRO_KEY_T)){
			key_touched = true;
			view->cam_w -= view->cam_move_rate;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_Y)){
			key_touched = true;
			view->cam_w += view->cam_move_rate;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_G)){
			key_touched = true;
			view->cam_h -= view->cam_move_rate;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_H)){
			key_touched = true;
			view->cam_h += view->cam_move_rate;
		}
		if( key_touched){
			if( view->cam_w < 1){view->cam_w = 1;}
			if( view->cam_h < 1){view->cam_h = 1;}
			if( view->cam_w > model->width ) { view->cam_w = model->width;}
			if( view->cam_h > model->height ){ view->cam_h = model->height;}
			draw_flag = true;
		}
	}


	if( ev->type == ALLEGRO_EVENT_KEY_CHAR) {
		if(al_key_down(&keyboard,ALLEGRO_KEY_1) ) {
			if(ev->type == ALLEGRO_EVENT_KEY_CHAR){
				view->cam_move_rate++;
				if( view->cam_move_rate > 15){view->cam_move_rate = 15;}
			}
			draw_flag = true;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_2) ) {
			if(ev->type == ALLEGRO_EVENT_KEY_CHAR){
				view->cam_move_rate--;
				if( view->cam_move_rate < 1){view->cam_move_rate = 1;}
			}
			draw_flag = true;
		}

		if(al_key_down(&keyboard,ALLEGRO_KEY_3) ) {
			model->horizontal_wrapping = !model->horizontal_wrapping;
			draw_flag = true;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_4) ) {
			model->vertical_wrapping = !model->vertical_wrapping;
			draw_flag = true;
		}
	
	}

}
void AllegroShell::handle_mouse(ALLEGRO_EVENT* ev){

}

void AllegroShell::draw(){
	view->draw();
}

void AllegroShell::run(){
// run the loop
	unsigned num = 0;
	ALLEGRO_EVENT ev;
	al_start_timer(timer);
	while( run_flag ){
		al_wait_for_event(queue,&ev);
		num++;
		printf("\r%d",num);

		if( ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			run_flag = false;
		}else if( isKeyboardEvent(&ev)){
			al_get_keyboard_state(&(keyboard));
			handle_keyboard(&ev);
		}else if( isMouseEvent(&ev) ){
			al_get_mouse_state(&(mouse));
			handle_mouse(&ev);
		}else if( ev.type  == ALLEGRO_EVENT_TIMER){
			
			// Update only if we haven't drawn the scene
			if(draw_flag == false){

				if( step_once_flag){
					// always step once if the user requests it
					model->step();
					step_once_flag = false;
				}else if(step_flag) {
					model->step();
				}
			}
			draw_flag = true;
		}else if (ev.type == USER_VIEW_EVENT ){
			// handling user events
			al_unref_user_event(&ev.user);
		}

		if( draw_flag && al_event_queue_is_empty(queue)){
			draw();
			draw_flag = false;
		}
	}
}
