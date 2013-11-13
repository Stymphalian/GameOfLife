#ifndef ALLEGROSHELL_H
#define ALLEGROSHELL_H

class Model;
class View;


class _Mouse{
public:
	ALLEGRO_MOUSE_STATE old_mouse;
	ALLEGRO_MOUSE_STATE mouse;
	int start_x;
	int start_y;
	int pressed;
	bool draw_mouse;
	unsigned current_state;

	_Mouse() {;}
	~_Mouse(){;}
	void update(ALLEGRO_EVENT* ev);
private:
};


class AllegroShell {
	public:
		// shell variables
		int draw_flag;
		int run_flag;
		ALLEGRO_DISPLAY* display;
		ALLEGRO_EVENT_QUEUE* queue;
		ALLEGRO_TIMER* timer;
		ALLEGRO_KEYBOARD_STATE keyboard;
		_Mouse* mouse;

		// Variables
		bool step_flag;
		bool step_once_flag;
		Model* model;
		View* view;


		AllegroShell();
		~AllegroShell();
		bool isKeyboardEvent(ALLEGRO_EVENT* ev);
		bool isMouseEvent(ALLEGRO_EVENT* ev);
		void handle_keyboard(ALLEGRO_EVENT* ev);
		void handle_mouse(ALLEGRO_EVENT* ev);
		void draw();
		void run();
	private:
		int display_w;
		int display_h;
		int fps;

		void zoom_in_mouse();
		void zoom_out_mouse();
		
	protected:
};
#endif