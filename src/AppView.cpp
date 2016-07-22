// AppView.cpp

#include <Application.h>

//For string manipulation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "MsgVals.h"
#include "AppView.h"
#include "AppWindow.h"
#include "SoundWork.h"
#include "cd.h"

#include "SettingsClass.h"

//Global settings
extern SettingsClass		settings;
extern D_CD					*cd;

//////////////////////////////////////////////////////////////////////////

AppView::AppView(BRect r)
			: BView(r, "TimeBombView", B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED)
{			

	time_t  	now;
	struct 		tm *ptr;
	int			loop=0;
	
	cd = new D_CD();
	cd->Init(settings.CDdevice);

	//Set up program settings
	FirstRun		= true;
	min	= hour = ampm = 90;
	TimeUp      	= false;
	CDPlaying		= false;
	LastMin     	= 90;
	LastHour		= 90;
	RingCount		= 0;
	WhichAlarm		= 0;
	

	//Set up and add the string view used to display the current time
	appSV = new BStringView(BRect(r.left,r.top+24,r.right,r.top+48), "TimeString", "", B_FOLLOW_NONE, B_WILL_DRAW);
	appSV->SetHighColor(settings.FontColor);
	appSV->SetFontSize(24);
	appSV->SetAlignment(B_ALIGN_CENTER);
	AddChild(appSV);
	
	//Now get the current time and display it
	time(&now);
	ptr = localtime(&now);

	strftime(buf1,3,"%I",ptr);
	hour = atoi(buf1);
	
	strftime(buf1,3,"%M",ptr);
	min = atoi(buf1);
	
	strftime(buf1,3,"%p",ptr);
	if(strcmp(buf1,"AM") == 0)
		ampm = 0;
	else
		ampm = 1;
		
	for(loop=0;loop<(signed)strlen(buf1);loop++)
	{
		buf1[loop]=tolower(buf1[loop]);
					
	}//end for loop convert to lowercase
		
	if(min<10)
		sprintf(totaltime,"%d:%d%d %s",hour,0,min,buf1);
	else
		sprintf(totaltime,"%d:%d %s",hour,min,buf1);
			
	LastMin = min;
	LastHour = hour;
	appSV->SetText(totaltime);
	
	//Set up and add the string view used to display the current time
	messageSV = new BStringView(BRect(r.left,r.top+44,r.right,r.top+65), "MessageString", "", B_FOLLOW_NONE, B_WILL_DRAW);
	messageSV->SetHighColor(settings.FontColor);
	messageSV->SetFontSize(14);
	messageSV->SetAlignment(B_ALIGN_CENTER);
	AddChild(messageSV);

	
	Invalidate();				//Re-Draw view
	
}// end AppView

//////////////////////////////////////////////////////////////////////////

void AppView::Pulse()
{

    static char 	test_string[105];
    static char		temp_string[105];

	char 			output_string[25];
	
	int32			ntracks; 				//number of tracks on current CD
	
	static int		count=0;
	static int 		max=19;					//Maximum amount of message to display at one time
	int				scroll_speed=3;
	int				index=0;

	
	time_t  		now;
	struct 			tm *ptr;
	int				loop=0;
	char 			temp[2];
	
	time(&now);
	ptr = localtime(&now);

	strftime(buf1,3,"%I",ptr);
	hour = atoi(buf1);
	
	strftime(buf1,3,"%M",ptr);
	min = atoi(buf1);
	
	strftime(buf1,3,"%p",ptr);
	if(strcmp(buf1,"AM") == 0)
		ampm = 0;
	else
		ampm = 1;
	

	if(hour == settings.AlarmHour[0] && min == settings.AlarmMin[0] && ampm == settings.AlarmAMPM[0] && LastMin != min)
	{
		WhichAlarm = 1;
		TimeUp = true;
	}
		
	if(hour == settings.AlarmHour[1] && min == settings.AlarmMin[1] && ampm == settings.AlarmAMPM[1] && LastMin != min)
	{
		WhichAlarm = 2;
		TimeUp = true;	
	}

	if(LastMin != min || LastHour != hour || FirstRun == true)
	{
			
		for(loop=0;loop<(signed)strlen(buf1);loop++)
		{
			buf1[loop]=tolower(buf1[loop]);
					
		}//end for loop convert to lowercase
		
		if(min<10)
			sprintf(totaltime,"%d:%d%d %s",hour,0,min,buf1);
		else
			sprintf(totaltime,"%d:%d %s",hour,min,buf1);
			
		LastMin = min;
		LastHour = hour;
		appSV->SetHighColor(settings.FontColor);
		appSV->SetText(totaltime);
		appSV->Invalidate();
	}

	if(TimeUp == true)
	{
		//if over 20 characters in message scroll 

		if(RingCount == 0 && WhichAlarm == 1)
			strcpy(temp_string,settings.AlarmMSG_1);
		
		if(RingCount == 0 && WhichAlarm == 2)
			strcpy(temp_string,settings.AlarmMSG_2);		
		
		if(WhichAlarm == 1)
			count = strlen(settings.AlarmMSG_1);
		
		if(WhichAlarm == 2)
			count = strlen(settings.AlarmMSG_2);
		
		if(count > max+1)
		{
		
			if(RingCount == 0)			//Do not shift if first display of message
			{
				strcpy(test_string,temp_string);
			}		
			else
			{
				for(index=0;index < scroll_speed;index++)
				{	
					strcpy(test_string,temp_string);
					test_string[count+1] = test_string[0];
					for(loop=0;loop < count;loop++)
					{	
						if(test_string[loop+1] == '\0')
							test_string[loop+1] = ' ';			
				
						temp_string[loop] = test_string[loop+1];		
					}
					temp_string[loop] = test_string[count+1];
				}//end for the more times this is run, faster the scroll (to a point)
			}
			
			strncpy(output_string,test_string,max);
			output_string[max] = '\0';
			messageSV->SetHighColor(settings.FontColor);					
			messageSV->SetText(output_string);
			messageSV->Invalidate();
	
		}//if string too big need to scroll
		else
		{
			if(WhichAlarm == 1)
				strncpy(output_string,settings.AlarmMSG_1,max+1);
			if(WhichAlarm == 2)
				strncpy(output_string,settings.AlarmMSG_2,max+1);
							
			messageSV->SetHighColor(settings.FontColor);			
			messageSV->SetText(output_string);
			messageSV->Invalidate();
	
		}//	no need to scroll if 20 or under characters


	}//end if TimeUp show message

	if(TimeUp == true && settings.Audible == true)	//Time for alarm and alarm sound
	{
		Window()->Minimize(false);			//Show window if it is minimized
		
		if(settings.PlayCD == false)
		{
			strcpy(tmppath,settings.SoundPath);
			strcat(tmppath,settings.SoundName); 	
			temp[0] = (int)(settings.Volume*10); //adding number to string sent with thread
			temp[1] = (char)NULL;
	    	strcat(tmppath,temp);
	
	   	 	//Creating thread for alarm sound
			alarm_thread = spawn_thread(playsound,"AlarmThread",B_NORMAL_PRIORITY,(void*)tmppath);
			resume_thread(alarm_thread);	//play alarm sound
		}
		else
		{
			if(CDPlaying == false)	//Only start the play once not each pulse cycle
			{
				cd->Play();
				ntracks = cd->GetNumTracks();
				if(settings.CDTrack > ntracks)		//play last track if invalid track requested
					cd->SelectTrack(ntracks);
				else
					cd->SelectTrack(settings.CDTrack);
				CDPlaying = true;
			}
		}
		
		RingCount++;
		if(RingCount >= settings.NumAlarmRings)
		{
			RingCount = 0;
			TimeUp    = false;
			messageSV->SetHighColor(216,216,216);	//Erase the message when alarm is over
			messageSV->SetText("");//output_string);
			messageSV->Invalidate();
			if(settings.WholeSong==false)
				cd->Stop();	
			CDPlaying = false;
		}

	}
	
	if(TimeUp == true && settings.Audible == false)	//Time for alarm and NO sound
	{	       
	    Window()->Minimize(false);			//Show window if it is minimized
	    
	    for(loop=0;loop<(signed)strlen(buf1);loop++)
		{
			buf1[loop]=tolower(buf1[loop]);
					
		}//end for loop convert to lowercase
		
		if(min<10)
			sprintf(totaltime,"%d:%d%d %s",hour,0,min,buf1);
		else
			sprintf(totaltime,"%d:%d %s",hour,min,buf1);
	    
	    if(RingCount % 2)	//Change color every other second
	    {
			appSV->SetHighColor(settings.FontColor);
			appSV->SetText(totaltime);	
			appSV->Invalidate();
		}
		else
		{
			appSV->SetHighColor(216,216,216);
			appSV->SetText(totaltime);	
			appSV->Invalidate();
		}
		
		RingCount++;
		if(RingCount >= (settings.NumAlarmRings))
		{
			RingCount = 0;
			TimeUp    = false;
			appSV->SetHighColor(settings.FontColor);
			appSV->SetText(totaltime);		
			messageSV->SetHighColor(216,216,216);		//Erase the message when alarm is over
			messageSV->SetText("");		
			appSV->Invalidate();
			messageSV->Invalidate();
		}


	}//end timeup and no sound	
	
	if(FirstRun)
	{
		LastMin = min;
		LastHour = hour;
		FirstRun = false;
	}

	
}//AppView::Pulse


//////////////////////////////////////////////////////////////////////////

void AppView::MessageReceived(BMessage *message)
{

	status_t		status;
	
	switch ( message->what )
	{

//App specific messages

		case TIMEBOMB_STOP_ALARM:			 
			 LastMin   = min;
			 TimeUp    = false;
			 RingCount = 0;
			 
			 appSV->SetHighColor(settings.FontColor);
			 appSV->SetText(totaltime);	
			 appSV->Invalidate(); 		
				 
			 messageSV->SetHighColor(216,216,216);		//Erase the message when alarm is over
			 messageSV->SetText("");		
		 	 messageSV->Invalidate();
 
			 cd->Stop();
			 CDPlaying = false; 
			 break;
			 
		case TIMEBOMB_COLOR_CHANGE:	 		 
			 appSV->SetHighColor(settings.FontColor);
			 appSV->SetText(totaltime);
			 Invalidate();
			 break;			 		 
			 
//General messages
		case TIMEBOMB_END:
			 wait_for_thread(alarm_thread,&status);
			 kill_thread(alarm_thread);
			 (Window())->PostMessage(B_QUIT_REQUESTED);	 		 
			 break;
		
   		default:
			 BView::MessageReceived(message);
			 break;
	}
	
}// end MessageReceived

//////////////////////////////////////////////////////////////////////////
