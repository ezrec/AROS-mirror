 /* Copyright © 1996 Kai Hofmann. All rights reserved. */

#ifndef _PHONELOG_H
#define _PHONELOG_H 1

 #ifndef PHONELOG
   struct PhoneLogEntry {
                         char		Number[30+1];
                         char		Name[80+1];
                         char		Reason[80+1];

                         char		ProgramName[32];
                         unsigned short	ProgramVersion;
                         unsigned short	ProgramRevision;

                         unsigned short	StartDay;
                         unsigned short	StartMonth;
                         int		StartYear;
                         unsigned short	StartHour;
                         unsigned short	StartMin;
                         unsigned short	StartSec;

                         unsigned short	EndDay;
                         unsigned short	EndMonth;
                         int		EndYear;
                         unsigned short	EndHour;
                         unsigned short	EndMin;
                         unsigned short	EndSec;

                         unsigned short	Hours;
                         unsigned short	Mins;
                         unsigned short	Secs;
                        };


   struct PhoneLogMarker {
                          char           ProgramName[32];
                          unsigned short ProgramVersion;
                          unsigned short ProgramRevision;
                          char           MarkName[256];
                         };

   #define PHONELOG
 #endif

/*
******* PhoneLog/struct PhoneLogEntry ***************************************
*
*   NAME
*	struct PhoneLogEntry -- Structure for log file entries (V33)
*
*   RECORD
*	struct PhoneLogEntry {
*	                      char            Number[30+1];
*	                      char            Name[80+1];
*	                      char            Reason[80+1];
*
*	                      char            ProgramName[32];
*	                      unsigned short  ProgramVersion;
*	                      unsigned short  ProgramRevision;
*
*	                      unsigned short  StartDay;
*	                      unsigned short  StartMonth;
*	                      int             StartYear;
*	                      unsigned short  StartHour;
*	                      unsigned short  StartMin;
*	                      unsigned short  StartSec;
*	                      unsigned short  EndDay;
*	                      unsigned short  EndMonth;
*	                      int             EndYear;
*	                      unsigned short  EndHour;
*	                      unsigned short  EndMin;
*	                      unsigned short  EndSec;
*
*	                      unsigned short  Hours;
*	                      unsigned short  Mins;
*	                      unsigned short  Secs;
*                            };
*
*   FUNCTION
*	Used by PhoneLogGenerator and PhoneLogParser
*
*   ELEMENTS
*	Number          - Phone Number that was called
*	Name            - Name of the host that was called
*	Reason          - Reason why the host is called
*	ProgramName     - Name of the program that makes the call
*	ProgramVersion  - Version of the program that makes the call
*	ProgramRevision - Revision of the program that makes the call
*	StartDay        - Day of the start of the call
*	StartMonth      - Month of the start of the call
*	StartYear       - Year of the start of the call
*	StartHour       - Hour of the start of the call
*	StartMin        - Min of the start of the call
*	StartSec        - Sec of the start of the call
*	EndDay          - Day of the end of the call
*	EndMonth        - Month of the end of the call
*	EndYear         - Year of the end of the call
*	EndHour         - Hour of the end of the call
*	EndMin          - Min of the end of the call
*	EndSec          - Sec of the end of the call
*	Hours           - Hours of the call duration
*	Mins            - Minutes of the call duration
*	Secs            - Seconds of the call duration
*
*   EXAMPLE
*	See PhoneLogGenerator and PhoneLogParser
*
*   NOTES
*	None
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	PhoneLogGenerator,PhoneLogParser, struct PhoneLogMarker
*
*****************************************************************************
*
*
*/

/*
******* PhoneLog/struct PhoneLogMarker **************************************
*
*   NAME
*	struct PhoneLogMarker -- Structure for log file marks (V33)
*
*   RECORD
*	struct PhoneLogMarker {
*	                       char           ProgramName[32];
*	                       unsigned short ProgramVersion;
*	                       unsigned short ProgramRevision;
*	                       char           MarkName[256];
*                             };
*
*   FUNCTION
*	Used by PhoneLogGenerator and PhoneLogParser
*
*   ELEMENTS
*	ProgramName     - Name of the program that wrote the mark
*	ProgramVersion  - Version of the program that wrote the mark
*	ProgramRevision - Revision of the program that wrote the mark
*	MarkName        - Name of the mark
*
*   EXAMPLE
*	See PhoneLogGenerator and PhoneLogParser
*
*   NOTES
*	None
*
*   BUGS
*	No known bugs.
*
*   SEE ALSO
*	PhoneLogGenerator,PhoneLogParser, struct PhoneLogEntry
*
*****************************************************************************
*
*
*/

#endif	/* _PHONELOG_H */
