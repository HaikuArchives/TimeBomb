#ifndef SETTINGSCLASS_H
#define SETTINGSCLASS_H

#include "Application.h"

class SettingsClass
{
	public:
		char				AppPath[B_PATH_NAME_LENGTH+B_FILE_NAME_LENGTH];
		char				AppLeaf[B_FILE_NAME_LENGTH];
		char				SoundPath[B_PATH_NAME_LENGTH+B_FILE_NAME_LENGTH];		
		char				SoundName[B_FILE_NAME_LENGTH];
		char				AlarmMSG_1[100];
		char				AlarmMSG_2[100];
		float 				Volume;
		bool				Audible;
		int					CDdevice;
		bool				PlayCD;
		bool				WholeSong;
		int					CDTrack;
		int					AlarmHour[3];
		int					AlarmMin[3];
		bool				AlarmAMPM[3];	//0 = AM , 1 = PM 
		int					NumAlarmRings;
		BPoint				TopLeft;
		rgb_color			FontColor;

};

#endif