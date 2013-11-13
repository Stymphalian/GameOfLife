#ifndef TEXTLOG_H
#define TEXTLOG_H

#include <stdio.h>
#include <stdarg.h>
#include "allegro5/allegro_native_dialog.h"


class Textlog{	
public:
	static Textlog& get(){
		static Textlog instance("jordan.log");
		return instance;
	}

	void log(const char* format,...){
		char str[1024];
   	va_list args;
   	va_start(args, format);
   	vsnprintf(str, sizeof str, format, args);
   	va_end(args);

   	fprintf(file,str);
   	fprintf(stderr,str);
	}
private:
	FILE* file;
	ALLEGRO_TEXTLOG* dialog;

	Textlog() {};
	Textlog(const char* filename){ 
		file = fopen(filename,"w");
	}
	Textlog(const Textlog& orig){}
	virtual ~Textlog(){
		fclose(file);
	}
	void operator= (Textlog const&);
};

#endif