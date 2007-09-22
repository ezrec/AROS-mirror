// The Window class for XaoS on BeOS.
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

#include <Application.h>
#include <GraphicsDefs.h>
#include <Message.h>
#include <MessageQueue.h>
#include <Point.h>
#include <Rect.h>
#include <Screen.h>
#include <View.h>
#include <Window.h>

#include "XaoSWindow.h"

#include "version.h"

const BPoint kDefaultLocation(100, 100);

static BRect calcFrame(BRect viewBounds)
{
	viewBounds.OffsetBy(kDefaultLocation);
	return viewBounds;
}

XaoSWindow::XaoSWindow(BView *const pView)
:	inherited(calcFrame(pView->Bounds()),
				 "XaoS " XaoS_VERSION,
				 B_TITLED_WINDOW,
				 0),
	mpView(pView),
	mQuitAllowed(FALSE)
{
	// Attach the view.
	AddChild(mpView);
}

XaoSWindow::~XaoSWindow(void)
{
	// empty
}

void XaoSWindow::FrameResized(float width, float height)
{
      // Remove queued-up resize-messages.
      BMessageQueue *pQueue = MessageQueue();
      BMessage *pMessage;
      while ((pMessage = pQueue->FindMessage(B_WINDOW_RESIZED, 0)) != 0) {
              pQueue->RemoveMessage(pMessage);
      }
      
//    // Check to see if the top BView is the same size as the window, and if
//    // not, repost a resize message.  The latter case happens sometimes
//    // because the app_server starts dropping resize messages when the
//    // application can't keep up.
//    const BRect bounds = Bounds();
//    BView *pTopView = ChildAt(0);
//    if (pTopView && pTopView->Bounds() != bounds) {
//            PostMessage(&BMessage(B_WINDOW_RESIZED));
//    }

      inherited::FrameResized(width, height);
}

bool XaoSWindow::QuitRequested(void)
{
	// Are we allowed to quit?
	if (!mQuitAllowed) {
		// Not yet ...
		mpView->KeyDown("q", 1);
		mpView->KeyDown("1", 1);
		
	} else {
		// Yep!
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
	
	return mQuitAllowed;
}

void XaoSWindow::AllowQuit()
{
	// Allow quitting; called from the main thread just before
	// it posts B_QUIT_REQUESTED.
	mQuitAllowed = TRUE;
}
