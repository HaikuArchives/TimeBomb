//	AppView.h


#ifndef APPVIEW_H
#define APPVIEW_H


#include <View.h>
#include <StringView.h>

class AppView: public BView
{
	private:		
		BStringView		*appSV;				//Used to display time
		BStringView		*messageSV;			//Used to show scrolling message


		thread_id		alarm_thread;
		int				min,hour,ampm;
		char 			buf1[3];
		bool			FirstRun;			//toggles if this is first time 
											//through code
		bool			TimeUp;				//time to set off alarm
		int 			LastMin;			//holds last minute
		int				LastHour;			//holds last hour
		bool			CDPlaying;			//flag for if the CD is playing or not			

		char			totaltime[10];		//string used to display current time
		int8			RingCount;			//num of times alarm has rung
		int				WhichAlarm;			//keeps track of which alarm is going off
		
		char 			tmppath[B_PATH_NAME_LENGTH+B_FILE_NAME_LENGTH];		

		void			MessageReceived(BMessage *message);
		void			Pulse();

	public:
			AppView(BRect r);
};

#endif