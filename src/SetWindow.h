// SetWindow.h


#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <ListItem.h>
#include <ListView.h>

#include <Window.h>
#include <StorageKit.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <SupportKit.h>

class SetWindow : public BWindow
{

	 private:
 			BWindow 			*handler;
			BSlider 			*volume;
			BCheckBox 			*audible;		
			BTextControl 		*hourText,*hourText2;
			BTextControl 		*minText,*minText2;
			BTextView 			*textView;
			BRadioButton		*amRadio,*amRadio2;
			BRadioButton		*pmRadio,*pmRadio2;
			BTextControl 		*soundName;
			BTextControl 		*ringNum;	
			BColorControl		*colorControl;
			BTextControl 		*alarm1MSG;
			BTextControl 		*alarm2MSG;
			
			BCheckBox 			*useCD;
			BTextControl 		*trackNum;
			BCheckBox 			*wholeSong;				
			
			BListView			*cdListView;
			BStringItem			*cdDevice[4];
			
			void 				MessageReceived(BMessage *message);
			void 				Quit();


	 public:
 			SetWindow(BWindow *hand);
 	
	
};
#endif
