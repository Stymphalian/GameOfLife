#ifndef VIEW_H
#define VIEW_H

#include "GameOfLife.h"
class AllegroShell;
class Model;

class View{
public:
	int disp_w;
	int disp_h;
	AllegroShell* control;
	Model* model;
	ALLEGRO_EVENT_SOURCE event_source;

	int cam_x;
	int cam_y;
	int cam_w;
	int cam_h;
	int cam_move_rate;

	View(AllegroShell* control, Model* model);
	~View();
	void draw();
	bool emit_event();
	ALLEGRO_EVENT_SOURCE* get_event_source();
	void zoom_camera(int x, int y, int w, int h, float cw, float ch, int dir);
	void make_cam_within_bounds();


private:
	//variables
	ALLEGRO_FONT* _font_carbon_12;
	ALLEGRO_COLOR _gold;

	void draw_model();
	void draw_status_bar();
	void draw_mouse();
};

#endif
