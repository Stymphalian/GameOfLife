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

#include "Model.h"


// ****************************
// Implementation of the Model class
// ****************************
Model::Model(int w,int h){
	population = 0;
	total_steps = 0;
	tick_count = 0;
	speed = 10;

	horizontal_wrapping = true;
	vertical_wrapping = true;

	width = w;
	height = h;
	map = new int[width*height];
	backmap = new int[width*height];
	savedmap = new int[width*height];


	// initialize all the arrays
	memset(map,DEAD, sizeof(int)* width*height);
	memset(backmap, DEAD, sizeof(int)* width*height);	
	load_random_configuration();
	memcpy(savedmap, map, sizeof(int)*width*height);
}


Model::~Model(){
	delete[] map;
	delete[] backmap;
	delete[] savedmap;
}


void Model::apply_condition(int col, int row){
	int new_state = 0;
	int current_state = map[row*width + col];
	int num = number_of_live_neighbours(col,row);

	if( current_state < ALIVE){
		// cell is dead
		if( num == 3){
			new_state = ALIVE;
			population++;
		}else if( current_state != DEAD){
			new_state = current_state - 4;
		}

	}else if( current_state == ALIVE){
		// cell is alive
		if( num < 2){ 
			new_state = DEAD;
		}else if( num  >= 2 && num <= 3){ 
			new_state = ALIVE;
			population++;		
		}else if( num >= 3){
			new_state = DEAD;
		}
	}
		
	if( current_state == ALIVE && new_state == DEAD){
		new_state = current_state -4;
	}

	backmap[row*width + col] = new_state;
}

int Model::number_of_live_neighbours(int col,int row){
	int count= 0;

	for (int c = 0; c < 3 ; ++c){
		for( int r = 0;  r< 3; ++r){
			int c2 = col-1 + c; 
			int r2 = row-1 + r;
			if( c2 == col && r2 == row){continue;}


			if( horizontal_wrapping ){
				if( c2 < 0)	{
					c2 = width + c2; // "+" because c2 is negative and we want to substract
				}else if( c2 >= width ){
					c2 = c2 - width;
				}
			}else if( c2 < 0 || c2 >= width){ continue;}
			
			if( vertical_wrapping) {
				if( r2 < 0){
					r2 = height + r2; // "+" because r2 is negative and we want to substract
				}else if(r2 >= height){
					r2 = r2 - height;
				}
			}else if( r2 < 0 || r2 >= height ){ continue;}

			if( map[r2*width + c2] == ALIVE){count++;}
		}
	}

	return count;
}


void Model::step(){
	total_steps++;
	population = 0;
	// iterate through each position in the map and update the cells
	for( int row = 0 ; row < height; ++row){
		for( int col = 0; col < width; ++col){
			apply_condition(col,row);
		}
	}

	// swap the maps
	int* temp = map;
	map =  backmap;
	backmap = temp;
}


void Model::load_random_configuration(){
	srand(time(0));
	for(int row = 0; row < height; ++row){
		for( int col = 0;  col  < width; ++col){
			
			int num = rand()%20 +1;
			if( num == 1){num = ALIVE; }
			else {num = DEAD; }

			map[row*width + col] = num;
		}
	}
}

void Model::reset(){
	population = 0;
	total_steps = 0;
	tick_count = 0;
	speed = 10;

	horizontal_wrapping = true;
	vertical_wrapping = true;

	// reset the map to the saved map
	memcpy(map, savedmap, sizeof(int)*width*height);
}

void Model::draw(int dispw,int disph){
	int cell_width = dispw/width;
	int cell_height = disph/height;

	for ( int row = 0; row <  height; ++row){
		for( int col = 0; col < width; ++col){
			if( map[row*width + col] == ALIVE){
				int x1 = col*cell_width; 
				int y1 = row*cell_height;
				int x2 = x1 + cell_width;
				int y2 = y1 + cell_height;
				//al_draw_rounded_rectangle(x1,y1,x2,y2,0.5,0.5,al_map_rgb(255,255,255),0);
				al_draw_rectangle(x1,y1,x2,y2,al_map_rgb(180,180,180),1);
			}
		}
	}

}