//	App.h


#ifndef APP_H
#define APP_H

#include "SetWindow.h"
#include "AppWindow.h"
#include "Application.h"

class App : public BApplication 
{
	private:
		AppWindow			*appWindow;			//Main window

		void				MessageReceived(BMessage *message);
		bool            	QuitRequested();
	
	public:	
		App(char *STARTUP_TEXT);
		

};
#endif