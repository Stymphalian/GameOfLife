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
#include "Textlog.cpp"

// ****************************
// Implementation of the Model class
// ****************************
Model::Model(int w,int h,bool enable_thread){
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

	num_threads = 10;
	threads = new ALLEGRO_THREAD*[num_threads];
	thread_args = new struct model_thread_step_t[num_threads];
	thread_enable_flag = enable_thread;

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

	delete[] threads;
	delete[] thread_args;
	printf("Model destroyed\n");
}


void Model::apply_condition(int col, int row){
	static int decrease_rate = 1;
	int new_state = 0;
	int current_state = map[row*width + col];
	int num = number_of_live_neighbours(col,row);

	if( current_state < ALIVE){
		// cell is dead
		if( num == 3){
			new_state = ALIVE;
		}else if( current_state != DEAD){
			new_state = current_state - decrease_rate;
		}

	}else if( current_state == ALIVE){
		// cell is alive
		if( num < 2){ 
			new_state = DEAD;
		}else if( num  >= 2 && num <= 3){ 
			new_state = ALIVE;
		}else if( num >= 3){
			new_state = DEAD;
		}
	}
		
	if( current_state == ALIVE && new_state == DEAD){
		new_state = current_state - decrease_rate;
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

			if( horizontal_wrapping ){ // handle horizontal wrapping
				if( c2 < 0)	{c2 = width + c2; }
				else if( c2 >= width ){c2 = c2 %width; }
			}else if( c2 < 0 || c2 >= width){ continue;}
			
			if( vertical_wrapping) { // handle vertical wrapping
				if( r2 < 0){r2 = height + r2;}
				else if(r2 >= height){r2 = r2%height; }
			}else if( r2 < 0 || r2 >= height ){ continue;}

			// count the alive cells
			if( map[r2*width + c2] == ALIVE){count++;}
		}
	}

	return count;
}


void Model::load_random_configuration(){
	srand(time(0));
	population = 0;

	for(int row = 0; row < height; ++row){
		for( int col = 0;  col  < width; ++col){
			
			int num = rand()%20 +1;
			if( num == 1){num = ALIVE; }
			else {num = DEAD; }

			map[row*width + col] = num;

			if( num == ALIVE){
				population++;
			}
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

	// count the current population
	for(int row = 0; row < height; ++row){
		for(int col = 0; col < width; ++col){
			if(map[row*width +col] == ALIVE) {
				population++;
			}
		}
	}

}

static void* model_thread_step(ALLEGRO_THREAD* thread, void* args){
	struct model_thread_step_t* arg = (struct model_thread_step_t*) args;

	int width = arg->model->width;
	int c= 0;
	for( int row = arg->start_row; row < arg->end_row; ++row){
		for( c= 0; c < width; ++c ){
			arg->model->apply_condition(c,row);
			if( arg->model->backmap[row*width + c] == Model::ALIVE){
				arg->population++;
			}
		}
		
	}

	return 0;
}

void Model::step(){
	total_steps++;
	population = 0;
	if(thread_enable_flag){		
		int row_segment = height/num_threads;
		int start_row= 0; 
		int end_row = start_row + row_segment;
		int tpos = 0;

		// create the threads to run the step.
		for(int i = 0; i < num_threads; ++i){
			// setup the arguments
			thread_args[tpos].model = this;
			thread_args[tpos].start_row = start_row;
			thread_args[tpos].end_row = end_row;
			thread_args[tpos].population = 0;

			// create and start the thread
			threads[tpos] = al_create_thread(model_thread_step,(void*) &thread_args[tpos]);
			al_start_thread(threads[tpos]);

			// set the start and end-rows for the next thread
			start_row = end_row;
			end_row = start_row + row_segment;
			if( start_row > height){break;}
			if( end_row >= height ) { end_row = height;}
			if( tpos == num_threads - 1){ end_row = height;}
			++tpos;
		}

		// wait until all the threads are done.
		for( int i = 0; i< tpos; ++i){
			al_join_thread(threads[i],NULL);			
			population += thread_args[i].population;
		}

	}else{	
		// iterate through each position in the map and update the cells
		for( int row = 0 ; row < height; ++row){
			for( int col = 0; col < width; ++col){
				apply_condition(col,row);			
				if( backmap[row*width  + col] == ALIVE){
					population++;
				}
			}
		}
		
	}

	// swap the maps
	int* temp = map;
	map =  backmap;
	backmap = temp;		
}