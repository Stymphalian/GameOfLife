#ifndef MODEL_H
#define MODEL_H

class Model{	
public:
	int population;
	int total_steps;
	int tick_count; // allows you to change the speed of the simuation
	int speed;

	bool horizontal_wrapping;
	bool vertical_wrapping;

	int width;
	int height;
	int* map; // the map
	int* backmap; // the map acting as a backmap buffer
	int* savedmap;

	int* vertices;
	int vert_pos;
	
	Model(int w,int h);
	~Model();
	void step();
	void reset();
private:
	// private methods
	void apply_condition(int col, int row);
	int number_of_live_neighbours(int col, int row);
	void load_random_configuration();	

public:
	static const int ALIVE = 100;
	static const int DEAD = 0;
};

#endif

/*
class Model{
public:
	int width;
	int height;
	int cell_width;
	int* map;

	Model(int w,int h,int cell_width);
	~Model();
	void step();
	void draw(int width,int height);


private:
	// nested class
	class Backbuffer{
	public:
		static const int num_rows = 3;
		int width;
		int height;
		int row_start;
		int* array[num_rows];

		Backbuffer(int w,int h);
		~Backbuffer();
		int get(int col, int row);
		int number_of_live_neighbours(int col, int row);
		
	};
	Backbuffer* backbuffer; // the back buffer

	// private methods
	void apply_condition(int col, int row);
	int number_of_live_neighbours(int col, int row);


// ****************************
// Implementation of the Model class
// ****************************
Model::Model(int w,int h,int cw){
	srand(time(0));
	width = w;
	height = h;
	cell_width = cw;

	map = new int[width*height];
	backbuffer = new Backbuffer(width, height);

	// initialize a configuration
	for(int row = 0; row < height; ++row){
		for( int col = 0;  col  < width; ++col){
			
			int num = rand()%20 +1;
			if( num == 1){num = 1; }
			else {num = 0; }

			map[row*width + col]  = num;
		}
	}
}

Model::~Model(){
	delete[] map;
	delete backbuffer;
}


void Model::apply_condition(int col, int row){
	int new_state = 0;
	int current_state = backbuffer->get(col,row);
	if( current_state == -1){printf("-1\n");}
	//	current_state = backbuffer->array[1][col];
	int num = backbuffer->number_of_live_neighbours(col,row);

	
	if( current_state == 0){
		// cell is dead
		if( num == 3){
			new_state = 1;
		}

	}else if( current_state == 1){
		// cell is alive
		if( num < 2){ 
			new_state = 0;
		}else if( num  >= 2 && num <= 3){ 
			new_state = 1;
		}else if( num >= 3){
			new_state = 0;
		}else {
			// do nothing
		}
	}
		
	// apply the new state
	map[row*width + col] = new_state;
}

int Model::number_of_live_neighbours(int col,int row){
	int count= 0;

	for (int c = 0; c < 3 ; ++c){
		for( int r = 0;  r< 3; ++r){
			int c2 = col-1 + c;
			int r2 = row-1 + r;
			if( c2 == col && r2 == row){continue;}
			if( c2 < 0 || c2 >= width){ continue;}
			if( r2 < 0 || r2 >= height ){ continue;}

			if( map[r2*width + c2] == 1){count++;}
		}
	}
	return count;
}


void Model::step(){
	// initialize the backbuffer
	int num_rows = Backbuffer::num_rows;
	backbuffer->row_start = -1;
	memset(backbuffer->array[0],0, sizeof(int)* width);
	for( int i = 0; i < num_rows-1; ++i){
		if( i < height){
			memcpy(backbuffer->array[i+1],map+(width*i),sizeof(int)*width);
		}
	}

	
	// iterate through each position in the map and update the cells
	int last = num_rows-2;
	for( int row = 0 ; row < height; ++row){
		for( int col = 0; col < width; ++col){
			apply_condition(col,row);
		}

		// slide the buffer window down one row.
		backbuffer->row_start++;
		int* temp = backbuffer->array[0];

		for( int i = 0; i < num_rows-1; ++i){
			backbuffer->array[i] = backbuffer->array[i+1];
		}
		backbuffer->array[num_rows-1] = temp;
		
		if( last > height) {continue;}
		last++;
		memcpy(backbuffer->array[num_rows-1],map + (width*last),sizeof(int)*width);
		
	}

}

void Model::draw(int dispw, int disph){
	for ( int row = 0; row <  height; ++row){
		for( int col = 0; col < width; ++col){
			int x1 = col*cell_width; 
			int y1 = row*cell_width;
			int x2 = x1 + cell_width;
			int y2 = y1 + cell_width;
			
			if( map[row*width + col] == 1){
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(200,200,200));
			}else{
				al_draw_filled_rectangle(x1,y1,x2,y2,al_map_rgb(0,0,0));

			}
		}
	}
}

// *******************************
//
// Nested class: Backbuffer
//
// *******************************
Model::Backbuffer::Backbuffer(int w,int h){
	width = w;
	height = h;
	row_start = 0;
	for( int i = 0; i< num_rows; ++i){
		array[i] = new int[width];
	}	
}
Model::Backbuffer::~Backbuffer(){
	for( int i = 0; i < num_rows; ++i){
		delete[] array[i];
	}
}
int Model::Backbuffer::get(int col, int row){
	int r = row-row_start;
	if( r >=0 && r <= num_rows-1){
		return array[r][col];
	}
	return -1;
}
int Model::Backbuffer::number_of_live_neighbours(int col, int row){
	int count= 0;

	for (int r = 0; r < 3 ; ++r){
		for( int c = 0;  c< 3; ++c){
			int c2 = col-1 + c;
			int r2 = row-1 + r;
			if( c2 == col && r2 == row){continue;}
			if( c2 < 0 || c2 >= width){ continue;}
			if( r2 < 0 || r2 >= height ){ continue;}

			if( array[r][c2] == 1 ){count++;}
		}
	}
	return count;
}

};*/