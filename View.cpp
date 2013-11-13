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

#include "Textlog.cpp"
#include "View.h"
#include "AllegroShell.h"
#include "Model.h"



View::View(AllegroShell* control, Model* model){
	this->model = model;
	this->control = control;
	disp_w= al_get_display_width(control->display);
	disp_h = al_get_display_height(control->display);

	cam_x = 0;
	cam_y = 0;
	cam_w = model->width;
	cam_h = model->height;
	cam_move_rate = 1;

	_gold = al_map_rgb(220,220,160);
	_font_carbon_12 = al_load_font("data/carbon.ttf",12,0);

	al_init_user_event_source(&event_source);
}

View::~View(){
	model = 0;
	control = 0;
	al_destroy_font(_font_carbon_12);
	al_destroy_user_event_source(&event_source);
	printf("View destroyed\n");
}

ALLEGRO_EVENT_SOURCE* View::get_event_source(){
	return &event_source;
}

bool View::emit_event(){
	ALLEGRO_EVENT user_event;
	user_event.user.type = USER_VIEW_EVENT;
	user_event.user.data1 = 444;
	return al_emit_user_event(&event_source,&user_event,NULL);
}


void View::draw(){
	al_clear_to_color(al_map_rgb(0,0,0));
	draw_model();
	draw_status_bar();
	draw_mouse();
	al_flip_display();		
}

void View::draw_mouse(){
	if( control->mouse->draw_mouse){
		_Mouse* mouse = control->mouse;
		al_draw_rectangle(mouse->mouse.x,mouse->mouse.y,
											mouse->start_x,mouse->start_y,
											al_map_rgb(255,0,0),1);
	}
}

void View::draw_model(){
	//model->draw(width,height);
	float cell_width = (float)disp_w/cam_w;
	float cell_height = (float)disp_h/cam_h;


	int x_count = 0;
	int y_count = 0;
	int row = cam_y;
	int col = cam_x;
	bool horiz_wrapping = model->horizontal_wrapping;
	bool vert_wrapping = model->vertical_wrapping;

	// loop through the cells.
	while(y_count < cam_h){			

		x_count = 0;
		col = cam_x;
		while(x_count < cam_w){
	
			// check to see if we draw the cell
			if( model->map[row*model->width + col] == Model::ALIVE){
				float x1 = x_count*cell_width; 
				float y1 = y_count*cell_height;
				float x2 = x1 + cell_width;
				float y2 = y1 + cell_height;
				// al_draw_rectangle(x1,y1,x2,y2,al_map_rgb(180,180,180),0);
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(180,180,180));
				
			}
			if( row == model->width/2 && col == model->height/2 ){
				float x1 = x_count*cell_width; 
				float y1 = y_count*cell_height;
				float x2 = x1 + cell_width;
				float y2 = y1 + cell_height;
				// al_draw_rectangle(x1,y1,x2,y2,al_map_rgb(255,0,0),0);
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(255,0,0));
			}
			if( row == 0 && col == 0){
				float x1 = x_count*cell_width; 
				float y1 = y_count*cell_height;
				float x2 = x1 + cell_width;
				float y2 = y1 + cell_height;
				// al_draw_rectangle(x1,y1,x2,y2,al_map_rgb(255,0,0),0);
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(255,255,0));
			}


			// increment the col we are looking at.
			++x_count;
			++col;
			if( horiz_wrapping){ // handle any wrapping.
				if(col < 0){ col = model->width + col;}
				else if(col >= model->width){ col %= model->width;}
			}else{
				if(col < 0){ col = 0;}
				else if(col >= model->width){ col = model->width;}
			}
		}

		// increment the row we are looking at.
		++row;
		++y_count;
		if( vert_wrapping){ // handle any wrapping.
			if(row < 0){ row = model->height + row;}
			else if(row >= model->height){ row %= model->height;}
		}else{
			if(row < 0){ row = 0;}
			else if(row >= model->height){ row = model->height;}
		}
	}
}

void View::draw_status_bar(){
	al_draw_textf(_font_carbon_12,_gold,5,5,0,"steps: %d",model->total_steps);
	al_draw_textf(_font_carbon_12,_gold,5,20,0,"population: %d",model->population);
	al_draw_textf(_font_carbon_12,_gold,5,35,0,"speed: %d",model->speed);		

	al_draw_textf(_font_carbon_12,_gold,5,50,0,"cam_x=%d,cam_y=%d",cam_x,cam_y);
	al_draw_textf(_font_carbon_12,_gold,5,65,0,"cam_w=%d,cam_h=%d",cam_w,cam_h);
	al_draw_textf(_font_carbon_12,_gold,5,80,0,"cam_move_rate=%d",cam_move_rate);
	al_draw_textf(_font_carbon_12,_gold,5,95,0,"wrap LR.%d NS.%d",
		model->horizontal_wrapping, model->vertical_wrapping);

	al_draw_textf(_font_carbon_12,_gold,5,110,0,"mx:%d.my:%d",
							control->mouse->mouse.x,control->mouse->mouse.y);

}




// tl_x, tl_y, width,height, cell_w, cell_h are measured in pixels
// direction = 0, means to zoom out
// direction = 1, measn to zoom in
void View::zoom_camera(int tl_x, int tl_y, int width, int height,
											 float cell_w, float cell_h,int direction){
	assert(cell_w != 0);
	assert(cell_h != 0);

	if( direction == 0) // Zooming-In
	{
		// if cell_w > 1 then each pixel is a fractio of a cell
		// if cell_w == 1 then each pixel is exactly one cell
		// if cell_w < 1  then each pixel represent more thatn one cell
		Textlog::get().log("zoom-in: cam(x,y,w,h) |");
		Textlog::get().log("(%d,%d,%d,%d)",cam_x,cam_y,cam_w,cam_h);

		cam_x += (tl_x/cell_w);
		cam_y += (tl_y/cell_h);
		cam_w = width/cell_w;
		cam_h = height/cell_h;
		make_cam_within_bounds();

		Textlog::get().log("--> (%d,%d,%d,%d)\n",cam_x,cam_y,cam_w,cam_h);
	}
	else if( direction == 1) // Zooming-Out
	{
	
		float new_cell_w = ((float)width/disp_w)*cell_w;
		float new_cell_h = ((float)height/disp_h)*cell_h;
		if(new_cell_w < disp_w/model->width) { new_cell_w = disp_w/model->width;}
		if(new_cell_h < disp_h/model->height) { new_cell_h = disp_h/model->height;}

		Textlog::get().log("zoom-out:cell_w,cell_h,cam(x,y,w,h)|");
		Textlog::get().log("%0.2f,%0.2f,",cell_w,new_cell_w);
		Textlog::get().log("%0.2f,%0.2f,",cell_h,new_cell_h);
		Textlog::get().log("(%d,%d,%d,%d)",cam_x,cam_y,cam_w,cam_h);
		
		cam_x = cam_x - tl_x/new_cell_w;
		cam_y = cam_y - tl_y/new_cell_h;
		cam_w = disp_w/(new_cell_w);
		cam_h = disp_h/(new_cell_h);
		make_cam_within_bounds();

		Textlog::get().log("->(%d,%d,%d,%d)\n",cam_x,cam_y,cam_w,cam_h);
	}
}

void View::make_cam_within_bounds(){
	// handle camera width
	if(cam_w < 1){ cam_w = 1;}
	else if( cam_w > model->width){ cam_w = model->width;}

	// handle camera height
	if( cam_h < 1) {cam_h = 1;}
	else if( cam_h > model->height){cam_h = model->height;}

	// Handle the horizontal camer position
	if( model->horizontal_wrapping){
		// make sure cam_x is good, with wrapping
		if( cam_x < 0 ){ cam_x = model->width - abs(cam_x)%model->width;}
		else if ( cam_x >= model->width) { cam_x %= model->width;}
	}else{
		// 0 <= cam_x < model->width - cam_w
		if( cam_x >= model->width -cam_w ) { cam_x = model->width - cam_w;}
		else if( cam_x < 0 ){ cam_x = 0;} 
	}

	// handle the vertical camera position
	if( model->vertical_wrapping){
		// make sure that cam_y is good, with wrapping
		if( cam_y < 0 ) { cam_y = model->height  - abs(cam_y)%model->height;}
		else if( cam_y >= model->height) { cam_y %= model->height;}
	}else{
		//  0 <= cam_y < model->height - cam_h
		if( cam_y >= model->height - cam_h){ cam_y = model->height - cam_h;}
		else if( cam_y < 0){ cam_y = 0;}
	}
}