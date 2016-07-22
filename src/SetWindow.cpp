// SetWindow.cpp

#include <Application.h>
#include <CheckBox.h>
#include <Box.h>
#include <Slider.h>
#include <TextControl.h>
#include <TextView.h>
#include <RadioButton.h>
#include <StringView.h>
#include <ColorControl.h>

#include <stdio.h>
#include <stdlib.h>

#include "cd.h"
#include "SetWindow.h"
#include "MsgVals.h"

#include "SettingsClass.h"

//Global settings
extern SettingsClass		settings;
extern D_CD					*cd;

SetWindow::SetWindow(BWindow *hand)
	: BWindow(BRect(0,0,380,400), "Settings",B_FLOATING_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,
			  B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	BRect		MainAppFrame;
	char 		buf[3];	
	
	handler = hand;
	
    MainAppFrame =	handler->Frame();
	if(MainAppFrame.left - 150 > 0)	//if settings window is to the left of the left screen boundry move on screen
		MoveTo(MainAppFrame.left - 150,MainAppFrame.top + handler->Bounds().Height() + 9);
	else
		MoveTo(4,MainAppFrame.top + handler->Bounds().Height() + 24);
	
	BView *sv = new BView(Bounds(), "settingview", B_FOLLOW_ALL, B_WILL_DRAW);
	
	// Ring options setting box
	BBox *genBox = new BBox(BRect(10,10,175,190));
	genBox->SetLabel("Ring Options");
	sv->AddChild(genBox);
	
	// create sliders for mass, drag, etc... 
	volume = new BSlider(BRect(5,20,155,50), "Volume", "Volume",new BMessage(TIMEBOMB_VOL_CHANGE), 0, 10);
	
	rgb_color 	blue ={5,5,255,55};
	volume->UseFillColor(true,&blue);
	volume->SetStyle(B_BLOCK_THUMB);
	volume->SetLimitLabels("Min", "Max");
	volume->SetValue(settings.Volume * 10);
	genBox->AddChild(volume);
	
	// create checkbox for audible 
	audible = new BCheckBox(BRect(5,90,75,100), "Audible", "Audible",new BMessage(TIMEBOMB_AUDIBLE_CHANGE));
	audible->SetValue(settings.Audible);
	genBox->AddChild(audible);

	//Create textcontrol to input number of alarm rings
	ringNum = new BTextControl(BRect(5,112,115,130),"NumRings","Seconds to ring","",new BMessage(TIMEBOMB_NUMRINGS_CHANGE),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.NumAlarmRings);
	ringNum->SetText(buf);
	textView = ringNum->TextView();
	textView->SetMaxBytes(2);
	ringNum->SetDivider(80);
	genBox->AddChild(ringNum);
	
	//Label for alarm sound text control
	BStringView *AlarmNameString = new BStringView(BRect(6,140,90,150),"AlarmNameString","Alarm Sound", B_FOLLOW_NONE, B_WILL_DRAW);
	AlarmNameString->SetHighColor(0,0,0);
	AlarmNameString->SetFontSize(10);
	genBox->AddChild(AlarmNameString);	
	
	//Create textcontrol to input alarm sound name
	soundName = new BTextControl(BRect(5,150,155,160),"Sound","","",new BMessage(TIMEBOMB_SOUND_CHANGE),B_FOLLOW_NONE);
	soundName->SetText(settings.SoundName);
	textView = soundName->TextView();
	textView->SetMaxBytes(B_FILE_NAME_LENGTH);
	soundName->SetDivider(0);
	genBox->AddChild(soundName);			

	//Alarm setting box
	BBox *alarmBox = new BBox(BRect(185,10,370,190));
	alarmBox->SetLabel("Alarm Times");
	sv->AddChild(alarmBox);
	
	// create textcontrol for inputing alarm1 time
	BStringView *alarmString = new BStringView(BRect(5,20,45,30),"Alarm1","Alarm 1", B_FOLLOW_NONE, B_WILL_DRAW);
	alarmString->SetHighColor(0,0,255);
	alarmString->SetFontSize(10);
	alarmBox->AddChild(alarmString);
	
	hourText = new BTextControl(BRect(5,32,40,50),"Hour","Hr","",new BMessage(TIMEBOMB_HOUR_CHANGE),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.AlarmHour[0]);
	hourText->SetText(buf);
	textView = hourText->TextView();  //getting parent TextView and limiting input to 2 digits
	textView->SetMaxBytes(2);
	hourText->ResizeToPreferred();
	alarmBox->AddChild(hourText);
	
	minText = new BTextControl(BRect(45,32,80,50),"Min","Mn","",new BMessage(TIMEBOMB_MIN_CHANGE),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.AlarmMin[0]);
	minText->SetText(buf);
	textView = minText->TextView();   //getting parent TextView and limiting input to 2 digits
	textView->SetMaxBytes(2);	
	minText->ResizeToPreferred();
	alarmBox->AddChild(minText);

	amRadio = new BRadioButton(BRect(105,33,140,50),"AM","am",new BMessage(TIMEBOMB_AMPM_CHANGE),B_FOLLOW_NONE);
	pmRadio = new BRadioButton(BRect(145,33,180,50),"PM","pm",new BMessage(TIMEBOMB_AMPM_CHANGE),B_FOLLOW_NONE);
	if(settings.AlarmAMPM[0] == 0)
		amRadio->SetValue(1);
	else
		pmRadio->SetValue(1);
	alarmBox->AddChild(amRadio);
	alarmBox->AddChild(pmRadio);
	
	//Label for alarm 1 message text control
	BStringView *string = new BStringView(BRect(6,60,90,70),"Alarm1MSG","Alarm 1 message", B_FOLLOW_NONE, B_WILL_DRAW);
	string->SetHighColor(0,0,255);
	string->SetFontSize(10);
	alarmBox->AddChild(string);
		
	//Create textcontrol to input alarm1 message
	alarm1MSG = new BTextControl(BRect(5,70,178,90),"MSG1","","",new BMessage(TIMEBOMB_ALARM_MSG_CHANGE_1),B_FOLLOW_NONE);
	alarm1MSG->SetText(settings.AlarmMSG_1);
	textView = alarm1MSG->TextView();
	textView->SetMaxBytes(100);
	alarm1MSG->SetDivider(0);
	alarmBox->AddChild(alarm1MSG);

	// create textcontrol for inputing alarm2 time
	BStringView *alarmString2 = new BStringView(BRect(5,100,45,110),"Alarm2","Alarm 2", B_FOLLOW_NONE, B_WILL_DRAW);
	alarmString2->SetHighColor(0,90,90);
	alarmString2->SetFontSize(10);
	alarmBox->AddChild(alarmString2);
	
	hourText2 = new BTextControl(BRect(5,112,40,130),"Hour","Hr","",new BMessage(TIMEBOMB_HOUR_CHANGE_2),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.AlarmHour[1]);
	hourText2->SetText(buf);
	textView = hourText2->TextView();  //getting parent TextView and limiting input to 2 digits
	textView->SetMaxBytes(2);
	hourText2->ResizeToPreferred();
	alarmBox->AddChild(hourText2);
	
	minText2 = new BTextControl(BRect(45,112,80,130),"Min","Mn","",new BMessage(TIMEBOMB_MIN_CHANGE_2),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.AlarmMin[1]);
	minText2->SetText(buf);
	textView = minText2->TextView();   //getting parent TextView and limiting input to 2 digits
	textView->SetMaxBytes(2);	
	minText2->ResizeToPreferred();
	alarmBox->AddChild(minText2);

	BBox *ampmBox = new BBox(BRect(100,113,180,130),NULL,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FRAME_EVENTS | 
               				 B_NAVIGABLE_JUMP,B_NO_BORDER);
	alarmBox->AddChild(ampmBox);

	amRadio2 = new BRadioButton(BRect(5,0,40,20),"AM","am",new BMessage(TIMEBOMB_AMPM_CHANGE_2),B_FOLLOW_NONE);
	pmRadio2 = new BRadioButton(BRect(45,0,185,20),"PM","pm",new BMessage(TIMEBOMB_AMPM_CHANGE_2),B_FOLLOW_NONE);

	if(settings.AlarmAMPM[1] == 0)
		amRadio2->SetValue(1);
	else
		pmRadio2->SetValue(1);

	ampmBox->AddChild(amRadio2);
	ampmBox->AddChild(pmRadio2);

	//Label for alarm 2 message text control
	BStringView *string2 = new BStringView(BRect(6,140,90,150),"Alarm2MSG","Alarm 2 message", B_FOLLOW_NONE, B_WILL_DRAW);
	string2->SetHighColor(0,90,90);
	string2->SetFontSize(10);
	alarmBox->AddChild(string2);
	
	//Create textcontrol to input alarm2 message
	alarm2MSG = new BTextControl(BRect(5,150,178,170),"MSG2","","",new BMessage(TIMEBOMB_ALARM_MSG_CHANGE_2),B_FOLLOW_NONE);
	alarm2MSG->SetText(settings.AlarmMSG_2);
	textView = alarm2MSG->TextView();
	textView->SetMaxBytes(100);
	alarm2MSG->SetDivider(0);
	alarmBox->AddChild(alarm2MSG);

//CD setting box
	BBox *cdBox = new BBox(BRect(10,200,370,290));
	cdBox->SetLabel("CD Options");
	sv->AddChild(cdBox);
// TESTING
	
	//Create textcontrol to input number of track to play
	trackNum = new BTextControl(BRect(5,20,80,30),"NumTracks","CD Track","",new BMessage(TIMEBOMB_CD_TRACK_CHANGE),B_FOLLOW_NONE);
	sprintf(buf,"%d",settings.CDTrack);
	trackNum->SetText(buf);
	textView = trackNum->TextView();
	textView->SetMaxBytes(2);
	trackNum->SetDivider(50);
	cdBox->AddChild(trackNum);

	// create checkbox to indicate use of CD alarm or not
	useCD = new BCheckBox(BRect(5,45,120,55), "UseCD", "Play CD for alarm",new BMessage(TIMEBOMB_CD_CHANGE));
	useCD->SetValue(settings.PlayCD);
	cdBox->AddChild(useCD);
	
	// create checkbox for playing whole song when alarm goes off or not
	wholeSong = new BCheckBox(BRect(5,65,160,80), "wholesong", "Play until manually stopped",new BMessage(TIMEBOMB_WHOLE_SONG_CHANGE));
	wholeSong->SetValue(settings.WholeSong);
	cdBox->AddChild(wholeSong);

	BStringView *cdString = new BStringView(BRect(170,20,270,30),"CD Device Str","Choose CD Device", B_FOLLOW_NONE, B_WILL_DRAW);
	cdString->SetHighColor(0,0,255);
	cdString->SetFontSize(10);
	cdBox->AddChild(cdString);

	//make device list for CD player(s)
	cdListView = new BListView(BRect(170,30,270,80),"CD List");
	
	for(int i=0;i < cd->GetNumDevices();i++)
	{
		char CDdevice[20];

		sprintf(CDdevice,"CD Device %d",i+1);	
		cdDevice[i] = new BStringItem(CDdevice);
		
		cdListView->AddItem(cdDevice[i]);		
	}
	cdListView->SetSelectionMessage(new BMessage(TIMEBOMB_CD_DEVICE_CHANGE));
		
	cdBox->AddChild(cdListView);

// END TEST

	
	
	//Other setting box
	BBox *otherBox = new BBox(BRect(10,300,370,390));
	otherBox->SetLabel("Color");
	sv->AddChild(otherBox);
	
	BStringView *colorString = new BStringView(BRect(5,15,70,25),"Time Color","Time Color", B_FOLLOW_NONE, B_WILL_DRAW);
	colorString->SetHighColor(0,0,255);
	colorString->SetFontSize(10);
	otherBox->AddChild(colorString);

	//Create color control
	colorControl = new BColorControl(BPoint(5,25),B_CELLS_32x8,6,"Color Selector",
			 						 new BMessage(TIMEBOMB_COLOR_CHANGE),false);
	colorControl->SetViewColor(216,216,216);
	colorControl->SetValue(settings.FontColor);
	otherBox->AddChild(colorControl);

	// set the color to gray, add the child, and show the window 
	sv->SetViewColor(216,216,216);
	AddChild(sv);
	
	Show();	
	//Highlight the current CD device
	cdListView->Select(settings.CDdevice);

}

//////////////////////////////////////////////////////////////////////////////////

void SetWindow::MessageReceived(BMessage *message)
{
	
	float val;
	
	switch(message->what)
	{
	 case TIMEBOMB_VOL_CHANGE:
		val = volume->Value();
		settings.Volume = val / 10;
	 	break;	
	 case TIMEBOMB_AUDIBLE_CHANGE:
		settings.Audible = audible->Value();
	 	break;
	 case TIMEBOMB_CD_CHANGE:
		settings.PlayCD = useCD->Value();
	    break;
	 case TIMEBOMB_CD_TRACK_CHANGE:
		settings.CDTrack = atoi(trackNum->Text());		
	 	break;   
	 case TIMEBOMB_WHOLE_SONG_CHANGE:
		settings.WholeSong = wholeSong->Value();
	    break;
	 case TIMEBOMB_CD_DEVICE_CHANGE:
		switch(cdListView->CurrentSelection())
		{
			case 0:
				cd->Init(CDROM_1);
				settings.CDdevice = 0;
				break;
			case 1:
				cd->Init(CDROM_2);
				settings.CDdevice = 1;				
				break;			
			case 2:
				cd->Init(CDROM_3);
				settings.CDdevice = 2;				
				break;
			case 3:
				cd->Init(CDROM_4);
				settings.CDdevice = 3;				
				break;			
		}
	 	break;   
	 case TIMEBOMB_HOUR_CHANGE:
		settings.AlarmHour[0] = atoi(hourText->Text());
	 	break;
	 case TIMEBOMB_MIN_CHANGE:
		settings.AlarmMin[0] = atoi(minText->Text());		
	 	break;
	case TIMEBOMB_AMPM_CHANGE:
		if (amRadio->Value()==1)
			settings.AlarmAMPM[0] = 0;
		else
			settings.AlarmAMPM[0] = 1;
	 	break;	 		 
	 case TIMEBOMB_HOUR_CHANGE_2:
		settings.AlarmHour[1] = atoi(hourText2->Text());
	 	break;
	 case TIMEBOMB_MIN_CHANGE_2:
		settings.AlarmMin[1] = atoi(minText2->Text());
	 	break;
	case TIMEBOMB_AMPM_CHANGE_2:
		if (amRadio2->Value()==1)
			settings.AlarmAMPM[1] = 0;
		else
			settings.AlarmAMPM[1] = 1;
	 	break;		 	
	 case TIMEBOMB_SOUND_CHANGE:
		strcpy(settings.SoundName,soundName->Text());
	 	break;
	 case TIMEBOMB_ALARM_MSG_CHANGE_1:
		strcpy(settings.AlarmMSG_1,alarm1MSG->Text());
	    break;
	 case TIMEBOMB_ALARM_MSG_CHANGE_2:	
		strcpy(settings.AlarmMSG_2,alarm2MSG->Text());
	 	break;	
	 case TIMEBOMB_COLOR_CHANGE:
	 	settings.FontColor = colorControl->ValueAsColor(); 	
	 	handler->PostMessage(message);
	 	break;
	 case TIMEBOMB_NUMRINGS_CHANGE:
		settings.NumAlarmRings = atoi(ringNum->Text());
	 	break;
	 	
	 default:
	 	BWindow::MessageReceived(message);
	 	break;
	}
}

//////////////////////////////////////////////////////////////////////////////////

void SetWindow::Quit() 
{
	// alert the application of our impending demise 
	handler->PostMessage(TIMEBOMB_SETWIN_QUIT);
	BWindow::Quit();
}
