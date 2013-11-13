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


	bool thread_enable_flag;
	int num_threads;
	ALLEGRO_THREAD** threads;
	struct model_thread_step_t* thread_args;


	Model(int w,int h,bool enable_thread);
	~Model();
	void step();
	void reset();

	void apply_condition(int col, int row);
	int number_of_live_neighbours(int col, int row);
private:
	// private methods
	
	void load_random_configuration();	

public:
	static const int ALIVE = 100;
	static const int DEAD = 0;
};


struct model_thread_step_t{
	Model* model;
	int start_row;
	int end_row;
	int population;
};

#endif