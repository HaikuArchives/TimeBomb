
#include "cd.h"

D_CD::D_CD()
{

	DeviceNames = new BList(4);		//List of CDROM's

	media_node cdNode;
	CurrentTrack = 1;				//May not be needed but might as well

	FindCDDevices("/dev/disk");		//Make the list of available CD drives

	BMediaRoster* roster = BMediaRoster::Roster();
	roster->GetAudioOutput(&cdNode);
	roster->GetParameterWebFor(cdNode, &ParamWeb);

	// find the CD gain parameter
	int32 numParams = ParamWeb->CountParameters();
	BParameter* param = NULL;
	for (int32 i = 0; i < numParams; i++)
	{
		BParameter* p = ParamWeb->ParameterAt(i);
		if (!strcmp(p->Kind(),B_GAIN) && !strcmp("CD",p->Group()->Name()))
		{		
			param = p;
			break;
		}
	}

	// we need it to be the right type; this will print an error message
	// if we didn't find it
	if (param->Type() == BParameter::B_CONTINUOUS_PARAMETER)
	{
		GainParam = dynamic_cast<BContinuousParameter*>(param);
	}
	else
	{
		//PUT ERROR message here
	}

	// calculate the minimum and maximum volume values for CD 
	MinVolume = GainParam->MinValue();
	MaxVolume = GainParam->MaxValue();

	Init(CDROM_1);			//Get the first CD rom ready to rock

}

void D_CD::Init(int whichCD)
{
	Stop();				//Stop any other CD drive so does not crash computer
	
	if(whichCD > CDROM_4)
		whichCD = CDROM_1;	//Set to default if out of bounds

	//setup CDROM
	devicefd = open((char*) DeviceNames->ItemAt(whichCD), O_RDONLY);
	if (devicefd < 0)
	{
		//PUT ERROR message here
	}
	
}

D_CD::~D_CD()
{
	// Free the names of the CD player devices.
	int32 countItems = DeviceNames->CountItems(); 
	for (int i=0; i<countItems; ++i) 
	{
		delete DeviceNames->RemoveItem((int32) 0);
    }
	delete DeviceNames;
	
	if(devicefd >= 0)
		close(devicefd);
		
	delete ParamWeb;
}


void
D_CD::PlayOrPause()
{
	// if paused, or stopped, plays, if playing, pauses
	UpdateState();
	switch (CurrentState) 
	{
		case NoCD:
			 Play();
			 return;
		case Stopped:
			 Play();
			 break;
		case Paused:
			 PlayContinue();
			 break;
		case Playing:
			 Pause();
			 break;
		default:
			 break;
	}
}


void 
D_CD::Pause()
{
	// pause the CD
	status_t result = ioctl(devicefd, B_SCSI_PAUSE_AUDIO);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}

void 
D_CD::Play()
{

	// play the CD
	UpdateState();
	if (CurrentState == NoCD) 
	{
		// no CD available, bail out
		ioctl(devicefd, B_LOAD_MEDIA, 0, 0);
		//PUT ERROR message here
		return;
	}
	
	scsi_play_track track;

	track.start_track = 1;
	track.start_index = 1;
	track.end_track = 99;
	track.end_index = 1;

	status_t result = ioctl(devicefd, B_SCSI_PLAY_TRACK, &track);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}

}

void
D_CD::PlayContinue()
{
	UpdateState();
	// continue after a pause
	status_t result = ioctl(devicefd, B_SCSI_RESUME_AUDIO);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}

void 
D_CD::Stop()
{
	// stop a playing CD
	status_t result = ioctl(devicefd, B_SCSI_STOP_AUDIO);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}

void 
D_CD::Eject()
{
#ifdef B_BEOS_VERSION_4_1
	// eject or load a CD
	status_t media_status = B_DEV_NO_MEDIA;
	
	// get the status first
	ioctl(devicefd, B_GET_MEDIA_STATUS, &media_status, sizeof(media_status));
	
	// if door open, load the media, else eject the CD
	status_t result = ioctl(devicefd,
		media_status == B_DEV_DOOR_OPEN ? B_LOAD_MEDIA : B_EJECT_DEVICE);
#else
	// eject CD
	status_t result = ioctl(devicefd, B_EJECT_DEVICE);
#endif
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}

void 
D_CD::SkipOneForward()
{

	// skip forward by one track
	UpdateState();
	
	if (CurrentState == NoCD)
		return;

	bool wasPaused = CurrentState == Paused
		|| CurrentState == Stopped;

	if(CurrentTrack < NumTracks)
		SelectTrack(CurrentTrack + 1);
	
	if (wasPaused)
		// make sure we don't start playing if we were paused before
		Pause();	
}

void 
D_CD::SkipOneBackward()
{

	// skip backward by one track
	UpdateState();
		
	if (CurrentState == NoCD)
		return;

	bool wasPaused = CurrentState == Paused
		|| CurrentState == Stopped;

	
	if(CurrentTrack > 1)
		SelectTrack(CurrentTrack - 1);

	if (wasPaused)
		// make sure we don't start playing if we were paused before
		Pause();

}


void 
D_CD::StartSkippingBackward()
{
	// start skipping
	UpdateState();
		
	if (CurrentState == NoCD)
		return;

	scsi_scan scan;
	scan.direction = -1;
	scan.speed = 1;
	status_t result = ioctl(devicefd, B_SCSI_SCAN, &scan);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}



void 
D_CD::StartSkippingForward()
{
	// start skipping
	UpdateState();
	
	if (CurrentState == NoCD)
		return;

	scsi_scan scan;
	scan.direction = 1;
	scan.speed = 1;
	status_t result = ioctl(devicefd, B_SCSI_SCAN, &scan);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}



void 
D_CD::StopSkipping()
{
	// stop skipping
	UpdateState();
	
	if (CurrentState == NoCD)
		return;

	scsi_scan scan;
	scan.direction = 0;
	scan.speed = 1;
	status_t result = ioctl(devicefd, B_SCSI_SCAN, &scan);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}

	result = ioctl(devicefd, B_SCSI_RESUME_AUDIO);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}

}


void 
D_CD::SelectTrack(int32 trackNumber)
{	
	// go to a selected track
	UpdateState();

	if (CurrentState == NoCD)
		return;


	scsi_play_track track;

	track.start_track = trackNumber;
	track.start_index = 1;
	track.end_track = 99;
	track.end_index = 1;

	status_t result = ioctl(devicefd, B_SCSI_PLAY_TRACK, &track);
	if (result != B_NO_ERROR) 
	{
		//PUT ERROR message here
		return;
	}
}


int
D_CD::FindCDDevices(const char *directory)
{ 

	BDirectory dir; 
	dir.SetTo(directory); 
	if(dir.InitCheck() != B_NO_ERROR) 
	{ 
		//PUT ERROR message here
		return false; 
	} 
	dir.Rewind(); 
	BEntry entry; 
	while(dir.GetNextEntry(&entry) >= 0) 
	{ 
		BPath path; 
		const char *name; 
		entry_ref e; 
		
		if(entry.GetPath(&path) != B_NO_ERROR) 
			continue; 
		name = path.Path(); 
		
		
		if(entry.GetRef(&e) != B_NO_ERROR) 
			continue; 

		if(entry.IsDirectory()) 
		{ 
			if(strcmp(e.name, "floppy") == 0) 
				continue; // ignore floppy (it is not silent) 

			int devfd = FindCDDevices(name);
			if(devfd >= 0)
				return devfd;
		} 
		else 
		{ 
			int devfd; 
			device_geometry g; 

			if(strcmp(e.name, "raw") != 0) 
				continue; // ignore partitions 

			devfd = open(name, O_RDONLY); 

			if(devfd < 0) 
				continue; 

			if(ioctl(devfd, B_GET_GEOMETRY, &g, sizeof(g)) >= 0) 
			{
				if(g.device_type == B_CD)
				{ 	
				    // Store the device's name in the list and close the device. 
				    DeviceNames->AddItem(strdup(name));				
				
					//return devfd;
				}
			}
			close(devfd);
		} 
	}//end while
	return B_ERROR;

}


//bool
void
D_CD::UpdateState()
{
	// check the current CD play state and force a notification to
	// be sent if it changed from last time
	NumTracks = GetNumTracks();
	
	scsi_position pos;
	status_t media_status = B_DEV_NO_MEDIA;
	ioctl(devicefd, B_GET_MEDIA_STATUS, &media_status, sizeof(media_status));
	if (media_status != B_NO_ERROR)
		CurrentState = NoCD;
		
	status_t result = ioctl(devicefd, B_SCSI_GET_POSITION, &pos);

	if (result != B_NO_ERROR)
		CurrentState = NoCD;
		
	else if ((!pos.position[1]) || (pos.position[1] >= 0x13) ||
	   ((pos.position[1] == 0x12) && (!pos.position[6])))
		CurrentState = Stopped;
		
	else if (pos.position[1] == 0x11)
		CurrentState = Playing;
	else
		CurrentState = Paused;


	// check the current CD track number		
	if (!pos.position[1] || pos.position[1] >= 0x13
		|| (pos.position[1] == 0x12 && !pos.position[6]))
		 CurrentTrack = 0;
	else
		 CurrentTrack = pos.position[6];

}

int32 
D_CD::GetNumTracks()
{
	// get the number of tracks on the current CD

	scsi_toc toc;

	status_t result = ioctl(devicefd, B_SCSI_GET_TOC, &toc);

	if (result != B_NO_ERROR)
		return 0;
	
	return toc.toc_data[3];
}

void D_CD::VolumeUp()
{

	// support stereo (2-channel) controls
	float chanData[2];
	size_t	datasize;

	// figure out what "now" is in the performance time base associated
	// with the node that the BParameterWeb controls
	BMediaRoster* roster = BMediaRoster::Roster();
	BTimeSource* timeSource = roster->MakeTimeSourceFor(ParamWeb->Node());
	bigtime_t now = timeSource->Now();

	datasize = sizeof(chanData);
	
	// now set the BParameter!
	GainParam->GetValue(&chanData, &datasize, &now);
	// must delete the time source from BMediaRoster::MakeTimeSourceFor(...)
	timeSource->Release();			
			
	// transform the control values into their proper ranges - this is always
	// a linear mapping from the control's range to the parameter's

	if(chanData[0] < MaxVolume-1 && chanData[1] < MaxVolume-1)
	{
		chanData[0] = chanData[0] + 1;
		chanData[1] = chanData[1] + 1;
	}
	else
	{
		chanData[0] = MaxVolume;
		chanData[1] = MaxVolume;			
	}

	// figure out what "now" is in the performance time base associated
	// with the node that the BParameterWeb controls
	roster = BMediaRoster::Roster();
	timeSource = roster->MakeTimeSourceFor(ParamWeb->Node());
	now = timeSource->Now();

	// now set the BParameter!
	GainParam->SetValue(&chanData, sizeof(chanData), now);

	// must delete the time source from BMediaRoster::MakeTimeSourceFor(...)
	timeSource->Release();

}

void D_CD::VolumeDown()
{

	// support stereo (2-channel) controls
	float chanData[2];
	size_t	datasize;

	// figure out what "now" is in the performance time base associated
	// with the node that the BParameterWeb controls
	BMediaRoster* roster = BMediaRoster::Roster();
	BTimeSource* timeSource = roster->MakeTimeSourceFor(ParamWeb->Node());
	bigtime_t now = timeSource->Now();

	datasize = sizeof(chanData);

	// now set the BParameter!
	GainParam->GetValue(&chanData, &datasize, &now);
	// must delete the time source from BMediaRoster::MakeTimeSourceFor(...)
	timeSource->Release();			
			
	// transform the control values into their proper ranges - this is always
	// a linear mapping from the control's range to the parameter's

	if(chanData[0] > MinVolume+1 && chanData[1] > MinVolume+1)
	{
		chanData[0] = chanData[0] - 1;
		chanData[1] = chanData[1] - 1;
	}
	else
	{
		chanData[0] = MinVolume;
		chanData[1] = MinVolume;		
	}

	// figure out what "now" is in the performance time base associated
	// with the node that the BParameterWeb controls
	roster = BMediaRoster::Roster();
	timeSource = roster->MakeTimeSourceFor(ParamWeb->Node());
	now = timeSource->Now();

	// now set the BParameter!
	GainParam->SetValue(&chanData, sizeof(chanData), now);

	// must delete the time source from BMediaRoster::MakeTimeSourceFor(...)
	timeSource->Release();

}

void D_CD::VolumeSet(float level)
{
	// support stereo (2-channel) controls
	float chanData[2];
			
	// transform the control values into their proper ranges - this is always
	// a linear mapping from the control's range to the parameter's

	chanData[0] = level;
	chanData[1]	= level;

	if(level > MaxVolume)
	{
		chanData[0] = MaxVolume;
		chanData[1]	= MaxVolume;
	}

	if(level < MinVolume)
	{
		chanData[0] = MinVolume;
		chanData[1]	= MinVolume;
	}

	// figure out what "now" is in the performance time base associated
	// with the node that the BParameterWeb controls
	BMediaRoster* roster = BMediaRoster::Roster();
	BTimeSource* timeSource = roster->MakeTimeSourceFor(ParamWeb->Node());
	bigtime_t now = timeSource->Now();

	// now set the BParameter!
	GainParam->SetValue(&chanData, sizeof(chanData), now);

	// must delete the time source from BMediaRoster::MakeTimeSourceFor(...)
	timeSource->Release();

}

int32 D_CD::GetNumDevices()
{
	return DeviceNames->CountItems();
}

int32 D_CD::GetCurrentTrack()
{
	UpdateState();
	
	return CurrentTrack;
	
}