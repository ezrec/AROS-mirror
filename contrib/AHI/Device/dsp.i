; $Id$

;    AHI - Hardware independent audio subsystem
;    Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
;     
;    This library is free software; you can redistribute it and/or
;    modify it under the terms of the GNU Library General Public
;    License as published by the Free Software Foundation; either
;    version 2 of the License, or (at your option) any later version.
;     
;    This library is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;    Library General Public License for more details.
;     
;    You should have received a copy of the GNU Library General Public
;    License along with this library; if not, write to the
;    Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
;    MA 02139, USA.

	IFND    _AHI_DSP_I_
_AHI_DSP_I_	SET	1

	include	exec/types.i

	STRUCTURE Echo,0
	ULONG	ahiecho_Delay
	FPTR	ahiecho_Code		;The echo routine
	Fixed	ahiecho_FeedbackDS	;Delayed signal to same channel
	Fixed	ahiecho_FeedbackDO	;Delayed signal to other channel
	Fixed	ahiecho_FeedbackNS	;Normal signal to same channel
	Fixed	ahiecho_FeedbackNO	;Normal signal to other channel
	Fixed	ahiecho_MixN		;Normal signal
	Fixed	ahiecho_MixD		;Delayed signal
	ULONG	ahiecho_Offset		;(&Buffer-&SrcPtr)/sizeof(ahiecho_Buffer[0])
	APTR	ahiecho_SrcPtr		;Pointer to &Buffer
	APTR	ahiecho_DstPtr		;Pointer to &(Buffer[Delay])
	APTR	ahiecho_EndPtr		;Pointer to address after buffer
	ULONG	ahiecho_BufferLength	;Delay buffer length in samples
	ULONG	ahiecho_BufferSize	;Delay buffer size in bytes
	LABEL	ahiecho_Buffer		;Delay buffer
	LABEL	Echo_SIZEOF

	ENDC
