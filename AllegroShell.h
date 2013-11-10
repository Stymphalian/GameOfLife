#ifndef ALLEGROSHELL_H
#define ALLEGROSHELL_H

class Model;
class View;

class AllegroShell {
	public:
		// shell variables
		int draw_flag;
		int run_flag;
		ALLEGRO_DISPLAY* display;
		ALLEGRO_EVENT_QUEUE* queue;
		ALLEGRO_TIMER* timer;
		ALLEGRO_KEYBOARD_STATE keyboard;
		ALLEGRO_MOUSE_STATE mouse;

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
	protected:
};
#endif