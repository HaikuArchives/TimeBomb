// main.cpp

#include "App.h"
#include "cd.h"

//GLOBALS
D_CD		*cd;

int main(int argc,char *argv[])
{
	bool hide=false;			//  Show/Hide window flag
	int	 index;

	for(index=1;index<argc-1;index++)
	{
		if( (argv[index][0] == '-' && argv[index][1] == 'h') || 
        	(argv[index][0] == '-' && argv[index][1] == 'H') )
		
			hide = true;
	}

	App		myApp( argc>1 ?  argv[argc-1] : "");	
		
	myApp.Run();
	return B_NO_ERROR;	
}