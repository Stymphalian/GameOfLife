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
	cam_w = model->width/2;
	cam_h = model->height/2;
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
	al_flip_display();		
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
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(255,0,0));
			}


			// increment the col we are looking at.
			++x_count;
			++col;
			if( horiz_wrapping){ // handle any wrapping.
				if(col < 0){ col = model->width + col;}
				else if(col >= model->width){ col %= model->width;}
			}
		}

		// increment the row we are looking at.
		++row;
		++y_count;
		if( vert_wrapping){ // handle any wrapping.
			if(row < 0){ row = model->height + row;}
			else if(row >= model->height){ row %= model->height;}
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
	al_draw_textf(_font_carbon_12,_gold,5,95,0,"wrap LR%d NS.%d",
		model->horizontal_wrapping, model->vertical_wrapping);

}