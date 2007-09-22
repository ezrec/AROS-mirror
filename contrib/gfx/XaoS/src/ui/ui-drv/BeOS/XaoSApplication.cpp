// The Application class for XaoS on BeOS.
// Copyright © 1997  Jens Kilian (jjk@acm.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <cstring>

#include <Alert.h>
#include <Application.h>
#include <Archivable.h>
#include <Bitmap.h>
#include <File.h>
#include <Message.h>
#include <OS.h>
#include <Point.h>
#include <Rect.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>

#include "XaoSApplication.h"
#include "XaoSSplashView.h"
#include "XaoSSplashWindow.h"

#include "version.h"

// Application signature.
const char kSignature[] = "application/x-jjk-xaos";

XaoSApplication::XaoSApplication(main_function_ptr pMainFunction)
:	inherited(kSignature),
	mMainThreadID(B_ERROR),
	mpMainFunction(pMainFunction),
	mExitStatus(-1),
	mArgc(0),
	mppArgv(0)
{
	// empty
}

XaoSApplication::~XaoSApplication(void)
{
	if (mppArgv) {
		// Delete command line arguments.
		for (int i = 0; i < mArgc; ++i) {
			delete [] mppArgv[i];
		}
		delete [] mppArgv;
		mppArgv = 0;
	}
}

void
XaoSApplication::AboutRequested(void)
{
	(new BAlert("About XaoS " XaoS_VERSION,
					"XaoS " XaoS_VERSION "\n"
					"Copyright © 1996,1997  Jan Hubicka, Thomas Marsh\n"
					"BeOS driver by Jens Kilian <jjk@acm.org>",
					"OK"))->Go();
}

void
XaoSApplication::ArgvReceived(int32 argc, char **ppArgv)
{
	if (mppArgv) {
		// This shouldn't happen (as long as XaoS is not made
		// a single-launch application, which won't happen soon :-)
	} else {
		// Copy the command line arguments.
		mArgc = (int)argc;
		char **p = mppArgv = new char*[mArgc+1];
		
		for (int i = 0; i < mArgc; ++i) {
			*p++ = strcpy(new char [strlen(*ppArgv)+1], *ppArgv);
			++ppArgv;
		}
		*p = 0;
	}
}

bool
XaoSApplication::QuitRequested(void)
{
	// Cleanly shutting down a BeOS application is hard work.
	// This function may be called multiple times before it finally
	// returns 'true'.  In an extreme case, the following
	// interactions may occur:
	//
	//		User			Application			Window			XaoS thread
	//		 |	B_QUIT_REQ.	  |				  |				    |
	//		 | ##########>	  | QuitReq.	  |				    |
	//		 |					  | =========>	  | Quit cmd.	    |
	//		 |					  | false		  | ############>	 |
	//		 |					  | <---------	  |				    |
	//		 |					  |				  | AllowQuit	    |
	//		 |					  |				  | <===========	 |
	//		 |					  |				  | B_QUIT_REQ.	 |
	//		 |					  | B_QUIT_REQ	  | <############	 |
	//		 |					  | <#########	  |				    |
	//		 |					  | QuitReq.	  |				(thread ends)
	//		 |					  | =========>	  |				    .
	//		 |					  | true			  |				    .
	//		 |					  | <---------	  |				    .
	//		 |					  | Quit			  |				    .
	//		 |					  | =========>	  |				    .
	//		 |					  |				  .				    .
	//		 |			(wait_for_thread)		  .				    .
	//		 |					  | < - - - - - - - - - - - - -  .
	//		 |					  |				  .				    .
	//
	//		#######> BMessage or other asynchronous event
	//		=======> function call
	//		-------> function return value
	
	const bool mayQuit = inherited::QuitRequested();
	if (mayQuit && mMainThreadID >= B_NO_ERROR) {
		// Wait for the main thread to finish.
		int32 dummy;
		(void)wait_for_thread(mMainThreadID, &dummy);
		mMainThreadID = B_ERROR;
	}
	
	return mayQuit;
}


void
XaoSApplication::ReadyToRun(void)
{
	if (!mppArgv) {
		// User didn't pass any command line options.
		// ### TO DO: LET THE USER SELECT MODE, SIZE ETC.
		XaoSSplashView *pSplashView =
			new XaoSSplashView(MakeSplashBitmap());
		if ((new XaoSSplashWindow(pSplashView))->Go()) {
			static const char kArg0[] = "XaoS";
			mArgc = 1;
			mppArgv = new char*[mArgc+1];
			mppArgv[0] = strcpy(new char [strlen(kArg0)+1], kArg0);
			mppArgv[1] = 0;
		}
	}
	
	// Spawn and start the thread running the real main program.
	if (!mppArgv
		 || (mMainThreadID = spawn_thread(MainThread,
		 											  "XaoS main program",
		 											  B_NORMAL_PRIORITY,
		 											  this)) < B_NO_ERROR
		 || resume_thread(mMainThreadID) < B_NO_ERROR)
	{
		PostMessage(B_QUIT_REQUESTED);
	}
}

long
XaoSApplication::MainThread(void *data)
{
	XaoSApplication *pApp = (XaoSApplication *)data;
	pApp->mExitStatus =
		(pApp->mpMainFunction)(pApp->mArgc, pApp->mppArgv);

	return 0;
}

BBitmap *
XaoSApplication::MakeSplashBitmap(void)
{
	// Get application info.
	app_info info;
	if (GetAppInfo(&info) != B_NO_ERROR) {
		return 0;
	}
	
	// Get the executable.
	BFile appFile(&(info.ref), O_RDONLY);
	if (appFile.InitCheck() != B_NO_ERROR) {
		return 0;
	}
	
	// Get resources.
	BResources resources;
	if (resources.SetTo(&appFile) != B_NO_ERROR) {
		return 0;
	}

	// Find the splash screen resource.	
	size_t length;
	void *pResource = resources.FindResource('XaSp', 1, &length);
	if (!pResource) {
		return 0;
	}
	
	// Unflatten the resource.
	BMessage archived;
	status_t status = archived.Unflatten((const char *)pResource);
	delete pResource;
	if (status != B_NO_ERROR) {
		return 0;
	}
	
	// Instantiate & return a bitmap.
	BArchivable *pResult = instantiate_object(&archived);
	if (!pResult) {
		return 0;
	}
	BBitmap *pSplashBitmap = dynamic_cast<BBitmap *>(pResult);
	if (!pSplashBitmap) {
		delete pResult;
	}
	
	return pSplashBitmap;
}
