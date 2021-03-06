#include "GameOfLife.h"

#include "Textlog.cpp"
#include "AllegroShell.h"
#include "View.h"
#include "Model.h"

int size_x = 480;
int size_y = 480;

AllegroShell::AllegroShell(){
	init(Defaults::display_height,
			Defaults::display_width,
			Defaults::step_flag);
}
AllegroShell::~AllegroShell(){destroy(); }

void AllegroShell::init(int disp_h, int disp_w, int step_flag){
	fps = 30;
	display_h = disp_h; // 480
	display_w = disp_w; // 480
	run_flag = true;
	draw_flag = false;
	this->step_flag = step_flag; // false
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
	display = al_create_display(display_w,display_h);
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

	model = new Model();
	al_set_timer_speed(timer,1.0/model->speed);

	view = new View(this,model);
	al_register_event_source(queue,view->get_event_source());
	view->emit_event();
}
void AllegroShell::destroy(){
	al_stop_timer(timer);
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_display(display);
	delete mouse;
	delete model;
	delete view;
	Textlog::get().log("AllegroShell destroyed\n");
}

bool AllegroShell::isKeyboardEvent(ALLEGRO_EVENT* ev){
	switch(ev->type){
		case(ALLEGRO_EVENT_KEY_UP):
		case(ALLEGRO_EVENT_KEY_DOWN):
		case(ALLEGRO_EVENT_KEY_CHAR): return true;
		default: return false;
	}
}

bool AllegroShell::prompt_for_file(char* buffer,unsigned bufsize,const char* pattern){
	al_rest(0.2); // Hack that i need for it to work!

	// Create a file chooser dialog
	ALLEGRO_FILECHOOSER* d;
	d = al_create_native_file_dialog(NULL,"",pattern,1); // 1 means file must exist
	if(!d){return false;}
	if(!al_show_native_file_dialog(NULL,d)){
		al_destroy_native_file_dialog(d);
		return false;
	}

	// check to make sure something was seleeted
	if( al_get_native_file_dialog_count(d) != 1){
		al_destroy_native_file_dialog(d);
		return false;
	}

	// Get the file that was selected, and copy into 'buffer'
	const char* file = al_get_native_file_dialog_path(d,0);
	strncpy(buffer,file,sizeof(char)*bufsize);
	buffer[bufsize-1] = 0;
	return true;
}


void AllegroShell::handle_keyboard(ALLEGRO_EVENT* ev){
	if(al_key_down(&keyboard,ALLEGRO_KEY_ESCAPE) ) {run_flag = false;}

	// Load a new map file
	if( ev->type == ALLEGRO_EVENT_KEY_DOWN){
		if(al_key_down(&keyboard,ALLEGRO_KEY_E) ) {
			char buffer[1024];
			if(false == prompt_for_file((char*)buffer,1025,"*.map")){return;}
			Textlog::get().log("Loading map file: %s\n",buffer);

			delete model;
			model = new Model(buffer);
			view->model = model;
			view->make_cam_within_bounds();
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}

	}

	// increase/decrease the steps/second of the model
	if(al_key_down(&keyboard,ALLEGRO_KEY_Q) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_CHAR){
			model->speed++;
			if( model->speed > 100){
				model->speed = 100;
			}
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_A) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_CHAR ){
			model->speed--;
			if( model->speed <= 0){
				model->speed = 1;
			}
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}
	}

	// Start/stop the model
	if(al_key_down(&keyboard,ALLEGRO_KEY_S) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_DOWN){
			step_flag = (step_flag == false)? true: false;
		}
	}


	// Iterate the model once.
	if(al_key_down(&keyboard,ALLEGRO_KEY_D) ) {
		step_once_flag = true;
	}

	// Reset the map to the original configuration
	if(al_key_down(&keyboard,ALLEGRO_KEY_R) ) {
		model->reset();
		al_set_timer_speed(timer,1.0/model->speed);
	}

	// Create a new, random model
	if(al_key_down(&keyboard,ALLEGRO_KEY_N) ) {
		if( ev->type == ALLEGRO_EVENT_KEY_DOWN){
			delete model;
			model = new Model();
			view->model = model;
			view->make_cam_within_bounds();
			al_set_timer_speed(timer,1.0/model->speed);
			draw_flag = true;
		}
	}

	if( ev->type == ALLEGRO_EVENT_KEY_CHAR) {
		bool key_touched = false;

		// move the camera horizontally/vertically 
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

		// Modify the width , or height of the camera
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


	// Modify the move-rate of the camera
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

void AllegroShell::_zoom_mouse(int direction){
	_Mouse* m = mouse; // just to make it easier to type shit out.

	// get the distance between the start position and the current mouse position
	int dx = abs( m->start_x - m->mouse.x);
	int dy = abs( m->start_y - m->mouse.y);
	if( !dx && !dy){return;}

	// get the top-left corner ( pixels)
	int tl_x = ( m->start_x < m->mouse.x) ? m->start_x : m->mouse.x;;
	int tl_y = ( m->start_y < m->mouse.y) ? m->start_y : m->mouse.y;;

	// get the width/height (pixels)
	float cell_w  = (float)view->disp_w/view->cam_w;
	float cell_h = (float)view->disp_h/view->cam_h;

	// zoom the camera with the specified bounding box
	view->zoom_camera(tl_x,tl_y,dx,dy,cell_w,cell_h,direction);
}
void AllegroShell::zoom_in_mouse(){_zoom_mouse(0); }
void AllegroShell::zoom_out_mouse(){_zoom_mouse(1);}

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
			// handle keyboard input
			++keyboard_count;
			al_get_keyboard_state(&(keyboard));
			handle_keyboard(&ev);
		}else if( isMouseEvent(&ev) ){
			// handle mouse input
			++mouse_count;
			mouse->update(&ev);
			handle_mouse(&ev);
		}else if( ev.type  == ALLEGRO_EVENT_TIMER){
			++timer_count;
			// Update the model only if we haven't drawn the scene
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

	Textlog::get().log("\nkeyboard_count = %u\n",keyboard_count);
	Textlog::get().log("mouse_count = %u\n",mouse_count);
	Textlog::get().log("timer_count = %u\n",timer_count);
}
