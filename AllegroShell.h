#ifndef ALLEGROSHELL_H
#define ALLEGROSHELL_H

class Model;
class View;
class _Mouse;

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

		void init(int display_w, int display_h, int step_flag);
		void destroy();
		bool prompt_for_file(char* buffer,unsigned bufsize, const char* pattern);
		void _zoom_mouse(int direction);
		void zoom_in_mouse();
		void zoom_out_mouse();

	protected:
};


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
	void update(ALLEGRO_EVENT* ev){
		old_mouse = mouse;
		al_get_mouse_state(&mouse);
	}
private:
};

#endif