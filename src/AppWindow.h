//	AppWindow.h


#ifndef APPWINDOW_H
#define APPWINDOW_H


#include <Window.h>
#include "SetWindow.h"


class AppWindow : public BWindow 
{
	private:
		SetWindow		*setWindow;			//window for settings		
		BView 			*appView;
		BMenuBar  		*MenuBar;
		BRect       	MenuRect;				
		BButton			*offButton;
		BWindow 		*handler;
		
		void 			AboutRequested();		
		bool			QuitRequested();
		void			MessageReceived(BMessage *message);
		bool 			LoadConfigurationFile(char *filename);
		bool			SaveConfigurationFile(char *filename);			

	public:
			
			AppWindow(BRect frame,uint32 workspaces);			

};

#endif
