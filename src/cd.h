#ifndef CD_H
#define CD_H

#include <ParameterWeb.h>
#include <TimeSource.h>
#include <MediaRoster.h>
#include <Application.h>
#include <Directory.h>
#include <Path.h>
#include <List.h>
#include <scsi.h>
#include <unistd.h>
#include <string.h>
//#include <Debug.h>

#define CDROM_1		0
#define CDROM_2		1
#define CDROM_3		2
#define CDROM_4		3

enum CDState 
{
	NoCD,
	Stopped,
	Paused,
	Playing,
	Skipping
};


class D_CD 
{

public:

			D_CD();
	virtual ~D_CD();
	
	void 	Init(int whichCD);			//Initializes the specified CDROM 1 - 4
	int 	FindCDDevices(const char *directory);
										//Finds the CD drives

	void 	PlayOrPause();				//Play is paused or pause if playing
	void 	PlayContinue();				//Continue playing after pause
	void 	Play();						//Play the current track
	void 	Pause();					//Pause the CD
	void 	Stop();						//Stop the CD
	void 	Eject();					//Spit the CD out
	void 	SkipOneForward();			//Change to the next track
	void 	SkipOneBackward();			//Change to the previous track
	void 	StartSkippingBackward();
	void 	StartSkippingForward();		
	void 	StopSkipping();				//Stop the track skipping
	void 	SelectTrack(int32);			//Select track to play
	void 	UpdateState();				//Gets current state of the CDROM

	int32	GetNumDevices();			//Returns the number of CD drives in system
	int32 	GetNumTracks();				//Returns the number of tracks on CD
	int32	GetCurrentTrack();			//Returns the current track being played
	
	void 	VolumeUp();					//Raise volume level by 1
	void 	VolumeDown();				//Lower volume level by 1
	void 	VolumeSet(float level);		//Set volume level
		
private:

	BList		*DeviceNames;			//List of CDROM device paths up to 4

	int 		devicefd;				//Device number?
	float 		MaxVolume;				//Maximum allowed CD volume
	float		MinVolume;				//Minimum allowed CD volume
	
	CDState		CurrentState;			//The state of the CDROM
	int32		CurrentTrack;			//The track currently selected for play or currently playing
	int32		NumTracks;				//Number of tracks on the CD

	BParameterWeb *ParamWeb;			//Used to get the current volume from the media controls
	BContinuousParameter *GainParam;	//and used in changing those values


};


#endif