#ifndef MODEL_H
#define MODEL_H

class Model{	
public:
	int population;
	int total_steps;
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


	Model();
	Model(const char* mapname);
	~Model();
	void step();
	void reset();
	void load_map(const char* mapname);

	void apply_condition(int col, int row);
	int number_of_live_neighbours(int col, int row);
private:
	// private methods
	void load_random_configuration();	
	void init(int speed, bool horiz_wrapping, bool vert_wrapping,
					int width,int height, bool enable_thread, int num_threads);

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