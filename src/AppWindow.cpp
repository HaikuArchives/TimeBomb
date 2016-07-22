// AppWindow.cpp

#include <Alert.h>			//For AboutBox
#include <Application.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Button.h>
#include <Screen.h>

#include <stdio.h>

#include "MsgVals.h"
#include "AppWindow.h"
#include "AppView.h"
#include "SoundWork.h"

#include "SetWindow.h"
#include "SettingsClass.h"

#include "Constants.h"		//For AboutBox text

//Global settings
extern SettingsClass		settings;

AppWindow::AppWindow(BRect frame,uint32 workspaces)
			: BWindow(frame, "TimeBomb",B_MODAL_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
					   B_NOT_ZOOMABLE | B_NOT_RESIZABLE, workspaces )
{			
	
	BMenu 			*tempMenu;
	BMenuItem 		*tempMenuItem;
	
	//Initialize global settings
	if(LoadConfigurationFile("config.file") ==false)
	{
		//Use generic settings if the config.file is missing	
		settings.Volume          = .5;
		settings.Audible         = true;
		settings.CDdevice		 = 0;
		settings.PlayCD			 = false;
		settings.WholeSong		 = true;
		settings.CDTrack		 = 1;
		settings.TopLeft.x       = 5;
		settings.TopLeft.y 	     = 5;
		settings.AlarmHour[0]    = 12;
		settings.AlarmHour[1]    = 12;
		settings.AlarmMin[0]     = 0;
		settings.AlarmMin[1]     = 0;
		settings.AlarmAMPM[0]    = 1;
		settings.AlarmAMPM[1]    = 1;		
		settings.FontColor.red	 = 0;
		settings.FontColor.green = 0;
		settings.FontColor.blue	 = 0;
		settings.NumAlarmRings   = 5;
		strcpy(settings.SoundName,"alarm1");
		strcpy(settings.AlarmMSG_1,"ALARM 1 is going!");
		strcpy(settings.AlarmMSG_2,"ALARM 2 is going!");
	}	
	
	//If the last saved window position is off the screen due to a screen resize put app window
	//on the far right edge or bottom of screen if needed.
	BRect screen;
	BScreen *theScreen = new BScreen(B_MAIN_SCREEN_ID);
	screen = theScreen->Frame();

	if(settings.TopLeft.x > screen.right)
		settings.TopLeft.x = screen.right-129;
	if(settings.TopLeft.y > screen.bottom)
		settings.TopLeft.y = screen.bottom-95;
	theScreen->~BScreen();		
	
	MoveTo(settings.TopLeft);
	
	appView = new AppView(Bounds());
	appView->SetViewColor(216,216,216);
	AddChild(appView);

	handler = appView->Window();
	
	SetPulseRate(1000000);		// 1 second pulse rate


// creating and adding menu bar
	MenuRect = Bounds();
	MenuRect.bottom = 19;
	MenuBar = new BMenuBar(MenuRect, "Menu", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_ITEMS_IN_ROW, true);

	//Layout the File menu
	tempMenu = new BMenu("File");
	
	tempMenuItem = new BMenuItem("About TimeBomb", new BMessage(B_ABOUT_REQUESTED));
	tempMenu->AddItem(tempMenuItem);
	tempMenu->AddSeparatorItem();
	tempMenuItem = new BMenuItem("Quit", new BMessage(TIMEBOMB_END),'Q'); 	
    tempMenuItem->SetTarget(appView);
	tempMenu->AddItem(tempMenuItem);
	MenuBar->AddItem(tempMenu);
	
	//Layout the Window menu

	tempMenu = new BMenu("Window");	
	tempMenuItem = new BMenuItem("Settings", new BMessage(TIMEBOMB_SETTINGS));
	tempMenu->AddItem(tempMenuItem);
	MenuBar->AddItem(tempMenu);
	
	//Now create the menu
	appView->AddChild(MenuBar);


	//Create the button
	offButton = new BButton(BRect(23,67,60,75),"off","Stop Alarm",new BMessage(TIMEBOMB_STOP_ALARM),B_FOLLOW_NONE);
	offButton->ResizeToPreferred();
	appView->AddChild(offButton);


	handler->Show();
	
	setWindow = NULL;

}// end AppWindow

//////////////////////////////////////////////////////////////////////////



void AppWindow::MessageReceived(BMessage *message)
{
	switch ( message->what )
	{

	    case B_ABOUT_REQUESTED:
	    	 AboutRequested();
			 break;	 		 
	 		 
		case TIMEBOMB_SETTINGS:
			 if(setWindow != NULL)
			 	{
			 		setWindow->Minimize(false);
			 	
			 	break;			 	
				}
 
	         setWindow = new SetWindow(handler);
			 break;	 		 
	 		 
	 	case TIMEBOMB_COLOR_CHANGE:
			 BWindow::DispatchMessage(message,appView);
	 		 break;
	 		 
	 	case TIMEBOMB_STOP_ALARM:
	 		 BWindow::DispatchMessage(message,appView);
	 		 break; 
	 		 		 
        case TIMEBOMB_SETWIN_QUIT:
			 setWindow = NULL;
       	 	 break;			 
			 
   		default:
			 BWindow::MessageReceived(message);
			 break;
	}
	
}// end MessageReceived


//////////////////////////////////////////////////////////////////////////

void AppWindow::AboutRequested()
{
	(new BAlert(STR_ABOUT_TITLE,STR_ABOUT_DESC, STR_ABOUT_BUTTON))->Go();

}//end AboutRequested


//////////////////////////////////////////////////////////////////////////


bool AppWindow::QuitRequested()
{	
	
	//saved to disk

	BRect			top_left;
	
	//Sending the top left hand point of the current window position
	top_left = Frame();
	settings.TopLeft.x = top_left.left;
	settings.TopLeft.y = top_left.top;	
	
	SaveConfigurationFile("config.file");
	be_app -> PostMessage(B_QUIT_REQUESTED);
		
	return(true);

}


//////////////////////////////////////////////////////////////////////////

bool AppWindow::LoadConfigurationFile(char *filename)
{

//Load general configuration file from disk

	FILE				*fp;			//File pointer used in reading config file
	char				tmpstr[20];		//holding string for labels within the config file
	char 				*configpath = new char[strlen(settings.AppPath)+strlen(filename)+1];
	int					temp;
	
	bool 				trip_wire	= false;
	char 				temp_char;
	int 				counter=0;
	int 				loop=0;
	int					red,green,blue;
	
	strcpy(configpath,settings.AppPath);
	strcat(configpath,filename);

	//open the disk file
	if(!(fp=fopen(configpath,"rb")))
	{	
		delete configpath;
		return false;	
	}

	fscanf(fp,"%s%f %f\n",tmpstr,&settings.TopLeft.x,&settings.TopLeft.y);
	fscanf(fp,"%s%d %d %s\n",tmpstr,&settings.AlarmHour[0],&settings.AlarmMin[0],tmpstr);
	if(strcmp(tmpstr,"am")==0)	settings.AlarmAMPM[0] = 0; else settings.AlarmAMPM[0] = 1;
	
	fscanf(fp,"%s%d %d %s\n",tmpstr,&settings.AlarmHour[1],&settings.AlarmMin[1],tmpstr); 
	if(strcmp(tmpstr,"am")==0)	settings.AlarmAMPM[1] = 0; else settings.AlarmAMPM[1] = 1;
	
	fscanf(fp,"%s%d\n",tmpstr,&temp); settings.Audible = temp;
	fscanf(fp,"%s%d\n",tmpstr,&temp); settings.CDdevice = temp;
	fscanf(fp,"%s%d\n",tmpstr,&temp); settings.PlayCD = temp;
	fscanf(fp,"%s%d\n",tmpstr,&temp); settings.WholeSong = temp;
	fscanf(fp,"%s%d\n",tmpstr,&temp); settings.CDTrack = temp;
	fscanf(fp,"%s%f\n",tmpstr,&settings.Volume);
	
	
	fscanf(fp,"%s ",tmpstr);	
	while (trip_wire == false)
	{
		fscanf(fp,"%c",&temp_char);
		if(temp_char == '"')
		{
			counter++;
			if(counter == 2)
				trip_wire =true;
		}
		else
		{
			settings.SoundName[loop] = temp_char;
			loop++;
		}
	}
	settings.SoundName[loop++] = '\0';
	
	fscanf(fp,"%s%d\n",tmpstr,&settings.NumAlarmRings);
	fscanf(fp,"%s%u %u %u\n",tmpstr,&red,&green,&blue);	//did it this way to get rid of compile warnings
	settings.FontColor.red   = red;
	settings.FontColor.green = green;
	settings.FontColor.blue  = blue;
				
	//get the alarm1 msg character by character
	trip_wire = false;
	loop 	  = 0;
	counter   = 0;
	
	fscanf(fp,"%s ",tmpstr);	
	while (trip_wire == false)
	{
		fscanf(fp,"%c",&temp_char);
		if(temp_char == '"')
		{
			counter++;
			if(counter == 2)
				trip_wire =true;
		}
		else
		{
			settings.AlarmMSG_1[loop] = temp_char;
			loop++;
		}
	}
	settings.AlarmMSG_1[loop++] = '\0';
	
	//reset state vars and get the alarm2 msg character by character
	trip_wire = false;
	loop 	  = 0;
	counter   = 0;
	
	fscanf(fp,"\n%s ",tmpstr);	
	while (trip_wire == false)
	{
		fscanf(fp,"%c",&temp_char);
		if(temp_char == '"')
		{
			counter++;
			if(counter == 2)
				trip_wire =true;
		}
		else
		{
			settings.AlarmMSG_2[loop] = temp_char;
			loop++;
		}
	}
	settings.AlarmMSG_2[loop++] = '\0';
	
	//close the file, re-alocate ram and return success
	fclose(fp);

	delete configpath;
	return true;

}//end LoadConfigurationFile

//////////////////////////////////////////////////////////////////////////

bool AppWindow::SaveConfigurationFile(char *filename)
{

//Save general configuration file to disk


	FILE				*fp;			//File pointer used in reading config file
	char 				*configpath = new char[strlen(settings.AppPath)+strlen(filename)+1];
	
	strcpy(configpath,settings.AppPath);
	strcat(configpath,filename);

	//open the disk file
	if(!(fp=fopen(configpath,"wb")))
	{	
		(new BAlert("File load ERROR","ERROR SAVING config.file", STR_ABOUT_BUTTON))->Go();
		delete configpath;
		return false;
	}
	
	fprintf(fp,"%s %.0f %.0f\n","[WinPos]",settings.TopLeft.x,settings.TopLeft.y);
	fprintf(fp,"%s %d %d %s\n","[AlarmTime1]",settings.AlarmHour[0],settings.AlarmMin[0],settings.AlarmAMPM[0] == 0 ? "am" : "pm");
	fprintf(fp,"%s %d %d %s\n","[AlarmTime2]",settings.AlarmHour[1],settings.AlarmMin[1],settings.AlarmAMPM[1] == 0 ? "am" : "pm");
	fprintf(fp,"%s %d\n","[AudibleAlarm]",settings.Audible);
	fprintf(fp,"%s %d\n","[CDdevice]",settings.CDdevice);
	fprintf(fp,"%s %d\n","[PlayCD]",settings.PlayCD);
	fprintf(fp,"%s %d\n","[WholeSong]",settings.WholeSong);
	fprintf(fp,"%s %d\n","[CDTrack]",settings.CDTrack);
	fprintf(fp,"%s %.2f\n","[Volume]",settings.Volume);
	fprintf(fp,"%s %c%s%c\n","[AlarmSound]",'"',settings.SoundName,'"');
	fprintf(fp,"%s %d\n","[NumAlarmRings]",settings.NumAlarmRings);
	fprintf(fp,"%s %d %d %d\n","[ColorRGB]",settings.FontColor.red,settings.FontColor.green,settings.FontColor.blue);
	fprintf(fp,"%s %c%s%c\n","[AlarmMSG1]",'"',settings.AlarmMSG_1,'"');
	fprintf(fp,"%s %c%s%c","[AlarmMSG2]",'"',settings.AlarmMSG_2,'"');	
	
	//close the file, re-alocate ram and return success
	fclose(fp);
	delete configpath;
	return true;

}//end SaveConfigurationFile

//////////////////////////////////////////////////////////////////////////////
