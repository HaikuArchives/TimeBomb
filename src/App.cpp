//App.cpp

#include <Application.h>
#include <Path.h>

#include <stdio.h>
#include <string.h>

#include "MsgVals.h"		

#include "App.h"
#include "SetWindow.h"

#include "SettingsClass.h"

//Global settings
SettingsClass		settings;

const char *app_signature = "application/x-vnd.Hailstone-TimeBomb";
		
/////////////////////////////////////////////////////////////////

App::App(char *STARTUP_TEXT)
		  : BApplication(app_signature)
{

	BEntry			entry;	
	BPath			path;
	app_info		info;
	
	unsigned int	loop=0;

	//Find path of program
	
	be_app->GetAppInfo(&info);
	entry.SetTo(&info.ref);
	entry.GetPath(&path);
	strcpy(settings.AppPath,path.Path());
	strcpy(settings.AppLeaf,path.Leaf());

	for(loop=0;loop < strlen(settings.AppPath)-strlen(settings.AppLeaf);loop++)
		settings.AppPath[loop] = settings.AppPath[loop];
	
	settings.AppPath[loop] = (char)NULL;

	strcpy(settings.SoundPath,settings.AppPath);	
	strcat(settings.SoundPath,"sounds/");	
	
	appWindow = new AppWindow(BRect(0,0,124,90),B_CURRENT_WORKSPACE);			
	
	
}//end App

/////////////////////////////////////////////////////////////////

void App::MessageReceived(BMessage *message)
{

	
	switch(message->what)
	{	
		case B_QUIT_REQUESTED:
			 be_app->PostMessage(B_QUIT_REQUESTED);
			 break;
		
   		default:
			 BApplication::MessageReceived(message);
			 break;
     
	}
	
}//end MessageRecieved

/////////////////////////////////////////////////////////////////

bool App::QuitRequested()
{
	PostMessage(B_QUIT_REQUESTED);
	return(true);
}

/////////////////////////////////////////////////////////////////
