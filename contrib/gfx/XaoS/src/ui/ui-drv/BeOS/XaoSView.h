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

#ifndef XAOSVIEW_H
#define XAOSVIEW_H

#include <Bitmap.h>
#include <Font.h>
#include <OS.h>
#include <Point.h>
#include <Rect.h>
#include <SupportDefs.h>
#include <View.h>

#include "XaoSEvent.h"

class XaoSView : public BView
{
public:
	typedef BView inherited;
	
	// Constructor, destructor.
	XaoSView(float width, float height);
	virtual ~XaoSView(void);

	// Hook functions.
	virtual void AttachedToWindow(void);
	virtual void Draw(BRect updateRect);
	virtual void FrameResized(float width, float height);
	virtual void KeyDown(const char *pBytes, int32 numBytes);
	virtual void KeyUp(const char *pBytes, int32 numBytes);
	virtual void MessageReceived(BMessage *pMessage);
	virtual void MouseDown(BPoint point);
	virtual void WindowActivated(bool active);
	
	// Return the event port.
	port_id EventPort(void) const;
	
	// Set the screen buffer.
	void SetBuffer(BBitmap *pBuffer);
	
	// Auxiliary functions.
	void GetTextSize(float &width, float &height);
	void DrawText(
		BPoint leftTop, const char *const pText, int32 length);
	void Clear(void);
	
private:
	XaoSView(const XaoSView &orig);
	XaoSView &operator =(const XaoSView &orig);

	BMessage *ConvertDraggedMessage(const BMessage *pMessage);
	void SendEvent(long eventCode, const XaoSEvent &event) const;
	float TextHeight(void) const;
	
	// Data members.
	port_id mEventPort;
	BBitmap *mpBuffer;
	font_height mFontHeight;
};

// Inline functions.

inline port_id
XaoSView::EventPort(void) const
{
	return mEventPort;
}

inline void
XaoSView::SendEvent(long eventCode, const XaoSEvent &event) const
{
	(void)write_port(mEventPort, eventCode, &event, sizeof(XaoSEvent));
}

inline float
XaoSView::TextHeight(void) const
{
	return mFontHeight.ascent + mFontHeight.descent + mFontHeight.leading;
}

#endif // XAOSVIEW_H
