#ifndef ALLEGROSHELL_H
#define ALLEGROSHELL_H

#include "Model.h"

class AllegroShell {
	public:
		// shell variables
		int draw_flag;
		int run_flag;
		ALLEGRO_DISPLAY* display;
		ALLEGRO_FONT* font_carbon_12;
		ALLEGRO_EVENT_QUEUE* queue;
		ALLEGRO_TIMER* timer;
		ALLEGRO_KEYBOARD_STATE keyboard;
		ALLEGRO_MOUSE_STATE mouse;

		// Variables
		bool step_flag;
		bool step_once_flag;
		Model* model;

		AllegroShell();
		~AllegroShell();
		bool isKeyboardEvent(ALLEGRO_EVENT* ev);
		bool isMouseEvent(ALLEGRO_EVENT* ev);
		void handle_keyboard();
		void handle_mouse();
		void draw();
		void run();
	private:
		int display_w;
		int display_h;
		int fps;
	protected:
};
#endif