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
	fps = 30;
	display_h = 480;
	display_w = 480;
	run_flag = true;
	draw_flag = false;
	step_flag = false;
	step_once_flag = false;

	int x = 8;
	x -= al_init();
	x -= al_init_image_addon();
	al_init_font_addon();
	x -= al_init_ttf_addon();
	x -= al_install_mouse();
	x -= al_install_keyboard();
	x -= al_install_audio();
	x -= al_init_acodec_addon();
	x -= al_init_primitives_addon();
	//x -= al_reserve_samples(10);
	printf("%d errors during loading.\n",x);

	// initializing the allegro objects
	display = al_create_display(display_h,display_w);
	queue = al_create_event_queue();
	timer = al_create_timer(1.0/fps);

	// al_toggle_display_flag(display,ALLEGRO_FULLSCREEN_WINDOW,false);
	// al_toggle_display_flag(display,ALLEGRO_NOFRAME,false);

	// registering the even sourcees
	al_register_event_source(queue,al_get_timer_event_source(timer));
	al_register_event_source(queue,al_get_display_event_source(display));
	al_register_event_source(queue,al_get_mouse_event_source());
	al_register_event_source(queue,al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();

	// instantiate my objects
	mouse  = new _Mouse();

	model = new Model(size_x,size_y,false);
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
	delete mouse;
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
			model = new Model(size_x,size_y,false);
			view->model = model;
			draw_flag = true;
		}
	}


	if( ev->type == ALLEGRO_EVENT_KEY_CHAR) {
		bool key_touched = false;

			// handle the arrow keys.
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

		// handle expanding the size of the camera
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

		// expand camera evenly
		if( al_key_down(&keyboard, ALLEGRO_KEY_U)){
			key_touched = true;
			view->cam_w += view->cam_move_rate;
			view->cam_h += view->cam_move_rate;
		}
		if( al_key_down(&keyboard, ALLEGRO_KEY_J)){
			key_touched = true;
			view->cam_w -= view->cam_move_rate;
			view->cam_h -= view->cam_move_rate;
		}
		// reset the camera to maximum zoom
		if( al_key_down(&keyboard, ALLEGRO_KEY_I)){
			key_touched =true;
			view->cam_w = model->width;
			view->cam_h = model->height;
			view->cam_x = 0;
			view->cam_y = 0;
		}


		// handle the horizontal and vertical wrapping
		if(al_key_down(&keyboard,ALLEGRO_KEY_3) ) {
			model->horizontal_wrapping = !model->horizontal_wrapping;
			key_touched = true;
		}
		if(al_key_down(&keyboard,ALLEGRO_KEY_4) ) {
			model->vertical_wrapping = !model->vertical_wrapping;
			key_touched = true;
		}


		if(key_touched){
			view->make_cam_within_bounds();
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

void _Mouse::update(ALLEGRO_EVENT* ev){
	old_mouse = mouse;
	al_get_mouse_state(&mouse);
}

void AllegroShell::zoom_in_mouse(){
	_Mouse* m = mouse; // just o make it easier to type shit out.

	// Only zoom-in if the x or y position mouse has moved
	int dx = abs( m->start_x - m->mouse.x);
	int dy = abs( m->start_y - m->mouse.y);
	bool moved_enough_flag = ( dx  > 0 || dy > 0);
	if( moved_enough_flag == false){return;}

	// find the top-left corner of the bounding box.
	int tl_x = ( m->start_x < m->mouse.x) ? m->start_x : m->mouse.x;;
	int tl_y = ( m->start_y < m->mouse.y) ? m->start_y : m->mouse.y;;

	// find the width/hieght of each cell in PIXELS
	float cell_width  = (float)view->disp_w/view->cam_w;
	float cell_height = (float)view->disp_h/view->cam_h;

	// Initially set the cam_w/cam_h to be the size of the
	// the bouding box (measured in PIXELS)
	int saved_cam_w = view->cam_w;
	int saved_cam_h = view->cam_h;
	view->cam_w = abs(m->mouse.x - m->start_x);
	view->cam_h = abs(m->mouse.y - m->start_y);

	if( cell_width >= 1){
		// if cell_width is greater than 0, we know that each pixel on the 
		// screen represents only a fraction of the cell.
		view->cam_x += tl_x/cell_width;
		view->cam_w =view->cam_w/cell_width;
	}else{
		// 1.) cell_width == 0. this means that each pixel represents one cell.
		// 2.) cell_width < 0. This means that each pixel represent more than one cell.
		// to know the number of cells in each pixel we take the inverse of cell_width.
		//cell_width = (float)view->cam_w/view->disp_h;
		cell_width = (float) saved_cam_w/view->disp_w;
		view->cam_x +=  tl_x*cell_width;
		view->cam_w *= cell_width;
	}

	if( cell_height >= 1){
		view->cam_y += tl_y/cell_height;
		view->cam_h = view->cam_h /cell_height;
	}else{				
		//cell_height = (float)view->cam_h/view->disp_h;
		cell_height = (float) saved_cam_h/view->disp_h;
		view->cam_y += tl_y*cell_height;
		view->cam_h *= cell_height;
	}
	view->make_cam_within_bounds();
}

void AllegroShell::zoom_out_mouse(){
	_Mouse* m = mouse; // just to make it easier to type shit out.

	int dx = abs( m->start_x - m->mouse.x);
	int dy = abs( m->start_y - m->mouse.y);
	if( !dx && !dy){return;}

	int tl_x = ( m->start_x < m->mouse.x) ? m->start_x : m->mouse.x;;
	int tl_y = ( m->start_y < m->mouse.y) ? m->start_y : m->mouse.y;;
	float cell_w  = (float)view->disp_w/view->cam_w;
	float cell_h = (float)view->disp_h/view->cam_h;

	view->zoom_camera(tl_x,tl_y,dx,dy,cell_w,cell_h,0);
}

void AllegroShell::handle_mouse(ALLEGRO_EVENT* ev){
		// ON BUTTON DOWN
	if( ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
	{
		if(ev->mouse.button == 1)// on left button press
		{
			// start the zoom-in operation
			mouse->start_x = mouse->mouse.x;
			mouse->start_y = mouse->mouse.y;
		}
		else if( ev->mouse.button ==2 )	// on right button press
		{
			// do nothing, or begin zoom out
			mouse->start_x = mouse->mouse.x;
			mouse->start_y = mouse->mouse.y;
		}
				
	} // ON BUTTON UP
	else if ( ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
	{
		if( ev->mouse.button == 1)// on left button release
		{ 
			// zoom-in the view
			zoom_in_mouse();
			mouse->start_x = 0;
			mouse->start_y = 0;
			mouse->draw_mouse = false;
		}
		else if( ev->mouse.button == 2 ) // on right button release
		{
			// do nothing, or process the zoom out.
			zoom_out_mouse();
			mouse->start_x = 0;
			mouse->start_y = 0;
			mouse->draw_mouse = false;
		}
	} // ON MOUSE MOVEMENT
	else if( ev->type == ALLEGRO_EVENT_MOUSE_AXES)
	{
		// on mouse movement
		if(mouse->mouse.buttons & 1) { // if left button
			mouse->draw_mouse = true;
			draw_flag = true;
		}else if( mouse->mouse.buttons & 2){
			mouse->draw_mouse = true;
			draw_flag = true;
		}
	}
}

void AllegroShell::draw(){
	view->draw();
}


void AllegroShell::run(){
// run the loop
	unsigned num = 0;
	ALLEGRO_EVENT ev;
	al_start_timer(timer);

	unsigned keyboard_count = 0;
	unsigned mouse_count = 0;
	unsigned timer_count = 0;

	while( run_flag ){
		al_wait_for_event(queue,&ev);
		num++;
		printf("\r%d",num);

		if( ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			run_flag = false;
		}else if( isKeyboardEvent(&ev)){
			++keyboard_count;
			al_get_keyboard_state(&(keyboard));
			handle_keyboard(&ev);
		}else if( isMouseEvent(&ev) ){
			++mouse_count;
			mouse->update(&ev);
			handle_mouse(&ev);
		}else if( ev.type  == ALLEGRO_EVENT_TIMER){
			++timer_count;
			
			// Update only if we haven't drawn the scene
			if(draw_flag == false){
				if(step_once_flag){
					// always step once if the user requests it
					model->step();
					step_once_flag = false;
				}else if(step_flag) {
					model->step();
				}
			}
			draw_flag = true;
		}else if (ev.type == USER_VIEW_EVENT){
			// handling user events
			al_unref_user_event(&ev.user);
		}

		if( draw_flag && al_event_queue_is_empty(queue)){
			draw();
			draw_flag = false;
		}
	} // end while(run_flag)

	printf("\nkeyboard_count = %u\n",keyboard_count);
	printf("mouse_count = %u\n",mouse_count);
	printf("timer_count = %u\n",timer_count);
}
