// The View class for XaoS on BeOS.
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

#include <AppDefs.h>
#include <Autolock.h>
#include <Bitmap.h>
#include <Directory.h>
#include <File.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <OS.h>
#include <Point.h>
#include <Rect.h>
#include <StorageDefs.h>
#include <SupportDefs.h>
#include <View.h>
#include <Volume.h>
#include <VolumeRoster.h>

#include "XaoSView.h"
#include "XaoSEvent.h"

#include "ui.h"
#include "version.h"

XaoSView::XaoSView(float width, float height)
:	inherited(BRect(B_ORIGIN, BPoint(width-1, height-1)),
				 "XaoS " XaoS_VERSION,
				 B_FOLLOW_ALL_SIDES,
				 B_WILL_DRAW|B_FRAME_EVENTS),
	mEventPort(create_port(100, "XaoS " XaoS_VERSION)),
	mpBuffer(0)
{
	// empty
}

XaoSView::~XaoSView(void)
{
	// empty
}

void XaoSView::AttachedToWindow(void)
{
	inherited::AttachedToWindow();

	// We will always Draw() the whole window.
	SetViewColor(B_TRANSPARENT_32_BIT);
	
	// Select the user's fixed-width font, but use ISO8859-1 encoding.
	BFont font(be_fixed_font);
	font.SetEncoding(B_ISO_8859_1);
	SetFont(&font);
	font.GetHeight(&mFontHeight);
	
	SetDrawingMode(B_OP_COPY);
	SetLowColor(255, 255, 255);
	SetHighColor(0, 0, 0);
}

void XaoSView::Draw(BRect /*updateRect*/)
{
	if (mpBuffer) {
		DrawBitmapAsync(mpBuffer, B_ORIGIN);
		Flush();
	}
}

void XaoSView::FrameResized(float /*width*/, float /*height*/)
{
	SendEvent(XaoSEvent::Resize, XaoSEvent());
}

void XaoSView::KeyDown(const char *pBytes, int32 numBytes)
{
	SendEvent(XaoSEvent::KeyDown, XaoSEvent(pBytes, numBytes));
}

void XaoSView::KeyUp(const char *pBytes, int32 numBytes)
{
	SendEvent(XaoSEvent::KeyUp, XaoSEvent(pBytes, numBytes));
}

void XaoSView::MessageReceived(BMessage *pMessage)
{
	switch (pMessage->what) {

	case B_MESSAGE_NOT_UNDERSTOOD:
		{	const BMessage *pPrevious = pMessage->Previous();
			if (pPrevious && pPrevious->what == 'PPNT') {
				// Dragged PPNT message not accepted.
				// Retry using the B_SIMPLE_DATA protocol.
				BMessage *pReply = ConvertDraggedMessage(pPrevious);
				if (pReply) {
					pMessage->SendReply(pReply);
					delete pReply;
				}
			}
		}
		break;

	default:
		inherited::MessageReceived(pMessage);
		break;
	}
}

void XaoSView::MouseDown(BPoint point)
{
	int32 clicks = 0;
	Window()->CurrentMessage()->FindInt32("clicks", &clicks);
	if (mpBuffer && clicks >= 2) {
		// Initiate dragging of the picture, in Rraster fashion.
		// Undocumented, but everybody seems to know about it.
		BMessage message('PPNT');
		BRect bounds = mpBuffer->Bounds();
		message.AddRect("bounds", bounds);
		message.AddInt32("color_space", mpBuffer->ColorSpace());
		message.AddInt32("bits_length", mpBuffer->BitsLength());
		message.AddData("bits", B_RAW_TYPE,
						  mpBuffer->Bits(), mpBuffer->BitsLength());
		
		BBitmap *pDragImage = new BBitmap(bounds, B_COLOR_8_BIT, TRUE);
		BView *const pView =
			new BView(bounds, "BBitmap view", B_FOLLOW_NONE, 0);
		pDragImage->AddChild(pView);
		pView->Window()->Lock();
		pView->DrawBitmap(mpBuffer, B_ORIGIN);
		pView->Sync();
		pView->Window()->Unlock();
							
		DragMessage(&message, pDragImage, point);

	} else {
		// Initiate mouse tracking.  Be recommends doing it this way,
		// leaving the window locked; we must be able to draw from the
		// main thread, so we temporarily unlock the window.
		// We also must delay sending the first event until after
		// we're sure that this is not the start of a double-click.
		// I'm not entirely happy about this.
		Window()->Unlock();

		const bigtime_t sample_interval = 50 * 1000;
		bigtime_t delay;
		if (get_click_speed(&delay) != B_NO_ERROR) {
			delay = 50 * 1000;
		}
		int32 buttons = 0;
		Window()->CurrentMessage()->FindInt32("buttons", &buttons);
		while (buttons) {
			if (delay <= 0) {
				SendEvent(XaoSEvent::Mouse,
							 XaoSEvent((int)point.x, (int)point.y,
											buttons, modifiers()));
			}
			snooze(sample_interval);
			delay -= sample_interval;
			{	BAutolock locker(Window());
				GetMouse(&point, (uint32 *)&buttons, TRUE);
			}
		}
		Window()->Lock();

		SendEvent(XaoSEvent::Mouse,
					 XaoSEvent((int)point.x, (int)point.y, 0, 0));
	}
}

void XaoSView::WindowActivated(bool active)
{
	inherited::WindowActivated(active);
	if (active) {
		MakeFocus();
	}
}

void XaoSView::SetBuffer(BBitmap *pBuffer)
{
	mpBuffer = pBuffer;
}

void XaoSView::GetTextSize(float &width, float &height)
{
	// Assumes a fixed-width font, of course.
	width = StringWidth("W");
	height = TextHeight();
}

void XaoSView::DrawText(
	BPoint leftTop, const char *const pText, int32 length)
{
	if (length > 0) {
		float width = StringWidth(pText, length);
		float height = TextHeight();
		FillRect(BRect(leftTop, leftTop + BPoint(width, height)),
					B_SOLID_LOW);
		if (length >= 4 && 0 == strncmp(pText, "BeOS", 4)) {
			// A little surprise :-)
			SetHighColor(0, 0, 255);
			DrawChar('B', leftTop + BPoint(0, mFontHeight.ascent));
			SetHighColor(255, 0, 0);
			DrawChar('e');
			SetHighColor(0, 0, 0);
			DrawString(pText + 2, length - 2);
	
		} else {
			DrawString(pText, length, leftTop + BPoint(0, mFontHeight.ascent));
		}
		
		Sync();
	}
}

void XaoSView::Clear(void)
{
	FillRect(Bounds());
	Sync();
}

BMessage *XaoSView::ConvertDraggedMessage(const BMessage *pOriginal)
{
	// The receiver of a dragged PPNT message didn't understand it.
	// Try to send a B_SIMPLE_DATA message referencing a temporary file.
	
// ### This doesn't work (yet).
//	
//	BVolumeRoster vol_roster;
//	BVolume boot_vol;
//	BDirectory root_dir;
//	BEntry home_entry;
//	BDirectory home_dir;
//	status_t status;
//	
//	// ### SHOULD USE find_directory() TO FIND THE DIRECTORY.
//	vol_roster.GetBootVolume(&boot_vol);
//	boot_vol.GetRootDirectory(&root_dir);
//	status = root_dir.FindEntry("home", &home_entry);
//	if (status == B_NO_ERROR) {
//		status = home_dir.SetTo(&home_entry);
//	}
//	
//	if (status == B_NO_ERROR) {
//		// ### SHOULD ALLOW MULTIPLE FILES.
//		const char kFilename[] = "Fractal";
//		
//		BEntry entry(&home_dir, kFilename);
//		BFile file(&entry, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
//		
//		// Wow, we actually managed to open the file.
//		// Now to write some stuff into it.
//		
//		// ### SHOULD USE DATATYPES LIBRARY.
//		struct {
//			int32 left, top, right, bottom;
//			int32 color_space;
//			int32 bits_length;
//		} BITMheader;
//			
//		BRect bounds;
//		pOriginal->FindRect("bounds", &bounds);
//		BITMheader.left = bounds.left;
//		BITMheader.top = bounds.top;
//		BITMheader.right = bounds.right;
//		BITMheader.bottom = bounds.bottom;
//			
//		pOriginal->FindInt32("color_space", &BITMheader.color_space);
//		pOriginal->FindInt32("bits_length", &BITMheader.bits_length);
//			
//		file.Write(&BITMheader, sizeof(BITMheader));
//		void *pData;
//		ssize_t dummy;
//		pOriginal->FindData("bits", B_RAW_TYPE, &pData, &dummy);
//		file.Write(pData, BITMheader.bits_length);
//		// ### SHOULD SET MIME TYPE OF FILE.
//			
//		// Create and return the B_SIMPLE_DATA message.
//		BMessage *pMessage = new BMessage(B_SIMPLE_DATA);
//		entry_ref file_ref;
//		entry.GetRef(&file_ref);
//		pMessage->AddRef("refs", &file_ref);
//		return pMessage;
//	}
//	
	// Oops, something went wrong.
	return 0;
}
