#include "GameOfLife.h"
#include "Model.h"
#include "Textlog.cpp"

// ****************************
// Implementation of the Model class
// ****************************

// Load a default, random map model
Model::Model(){
	init(Defaults::model_speed,
		Defaults::model_horizontal_wrapping,
		Defaults::model_vertical_wrapping,
		Defaults::model_width,
		Defaults::model_height,
		Defaults::model_enable_threads,
		Defaults::model_num_threads);
	load_random_configuration();
}
Model::Model(const char* mapfile){
	ALLEGRO_CONFIG* config = al_load_config_file(mapfile);
	if(!config){return;}

	const char* value = 0;
	int num_value  = 0;

	// read valuds from the  map file, and assign them to the appropriate variables
	#define _read_config(var,key) \
	do{ \
		value = al_get_config_value(config,"",key); \
		if( value) { \
			num_value = atoi(value); \
			var = num_value; \
		} \
	}while(false);
	_read_config(speed,"speed");
	_read_config(horizontal_wrapping,"horizontal_wrapping");
	_read_config(vertical_wrapping,"vertical_wrapping");
	_read_config(width,"width");
	_read_config(height,"height");
	_read_config(num_threads,"num_threads");
	_read_config(thread_enable_flag,"enable_threads");
	#undef _read_config


	// dump the values into the log
	Textlog::get().log("Loading map file: %s\n",mapfile);
	Textlog::get().log("(speed, horizontal_wrapping, vertical_wrapping, width, height,thread_enable_flag, num_threads)\n");
	Textlog::get().log("(%d,",speed);
	Textlog::get().log("%d,",horizontal_wrapping);
	Textlog::get().log("%d,",vertical_wrapping);
	Textlog::get().log("%d,",width);
	Textlog::get().log("%d,",height);
	Textlog::get().log("%d,",thread_enable_flag);
	Textlog::get().log("%d)\n",num_threads);

	// assign the variables.
	init(speed, horizontal_wrapping, vertical_wrapping,
		width, height,thread_enable_flag, num_threads);


	// read in the map coordinates of ALIVE cells
	ALLEGRO_CONFIG_ENTRY* iterator;
	const char* key  = al_get_first_config_entry(config,"map",&iterator);
	int x = -1;
	int y = -1;
	while( key != NULL){
		value = al_get_config_value(config,"map",key);
		key = al_get_next_config_entry(&iterator);
		if (value){
			int rs = sscanf(value,"%d %d",&x,&y);
			if( rs != 2){continue;}
			if( x< 0 || x >= width){continue;}
			if( y< 0 || y >= height){continue;}

			map[y*width + x]= ALIVE;
			savedmap[y*width + x]= ALIVE;
		}	
	}

	al_destroy_config(config);
}


void Model::init(int speed, bool horiz_wrapping, bool vert_wrapping,
								int width,int height, bool enable_thread, int num_threads)
{
	population = 0;
	total_steps = 0;
	this->speed = speed;
	horizontal_wrapping = horiz_wrapping;
	vertical_wrapping = vert_wrapping;
	this->width = width;
	this->height = height;
	map = new int[width*height];
	backmap = new int[width*height];
	savedmap = new int[width*height];

	this->num_threads = num_threads;
	threads = new ALLEGRO_THREAD*[num_threads];
	thread_args = new struct model_thread_step_t[num_threads];
	thread_enable_flag = enable_thread;

	memset(map,DEAD, sizeof(int)* width*height);
	memset(backmap, DEAD, sizeof(int)* width*height);	
	memset(savedmap, DEAD, sizeof(int)*width*height);
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

	// Look around the 8 cells  and count the number of alive cells
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
			savedmap[row*width + col]=num;

			if( num == ALIVE){
				population++;
			}
		}
	}
}

void Model::reset(){
	population = 0;
	total_steps = 0;
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