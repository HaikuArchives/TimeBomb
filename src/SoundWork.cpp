// Used in playing sound events

//#include "SimpleGameSound.h"
#include "SoundWork.h"
//#include <stdio.h>

//For a wav
int32 playsound(char *path)
{ 
	float			VolumeLevel;
	char			num;
	int				length;
      
    //get volumelevel off end of path string and convert to float
	length = strlen(path);	 
	num = path[length-1];
	VolumeLevel = (float)num;
	VolumeLevel /= 10;
	path[length-1] = (char)NULL;	

	entry_ref	ref;
	BEntry		entry(path,true);
	if(entry.InitCheck()  == B_OK)
	{
		if(entry.GetRef(&ref) == B_OK)
		{
			BSimpleGameSound *sound = new BSimpleGameSound(&ref);
			
			if(sound->InitCheck() == B_OK)
			{
				sound->SetGain(VolumeLevel);
				sound->StartPlaying();
	 
				while (sound->IsPlaying()==true)
				{
					;
				}
				sound->StopPlaying();
		//		snooze(10000);  // Wait a bit
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;

	return true;     

//OLD CODE BELOW
      
/*
      BSound 			*sound; 
      BSoundPlayer 		*player; 


      entry_ref 	ref;
      BEntry 		entry(path, true);	
		
      BSoundPlayer::play_id id; 
   
      if (entry.InitCheck() == B_OK) 
      { 
         if (entry.GetRef(&ref) == B_OK) 
         { 
            sound = new BSound(&ref); 
            
            media_raw_audio_format	fmt = sound->Format();
            if (sound->InitCheck() == B_OK) 
            { 		              
               player = new BSoundPlayer( &fmt, ref.name );  
               player->Start();         
               player->SetVolume(VolumeLevel);
               id = player->StartPlaying(sound); 
//               player->WaitForSound(id);                
               sound->ReleaseRef();
               
               player->WaitForSound(id);                
               
               player->Stop();
               delete player;
            }
            else
            	return false; 
         }
         else
            	return false; 
      } 
      else
            	return false;


      return true;
*/


}//end playsound



