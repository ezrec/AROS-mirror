/* Locale initializations. */

#include "locale.h"

CONST_STRPTR TodoRadioEntries[3];
CONST_STRPTR SuperclassRadioEntries[3];
CONST_STRPTR ReggaeClassTypeEntries[9];

void InitializeStrings(void)
{
	TodoRadioEntries[0] = LS(MSG_TODO_REMARKS_RADIO_WARNINGS, "remarks as preprocessor #warnings");
	TodoRadioEntries[1] = LS(MSG_TODO_REMARKS_RADIO_COMMENTS, "remarks as plain comments");
	TodoRadioEntries[2] = NULL;

	SuperclassRadioEntries[0] = LS(MSG_SUPERCLASS_RADIO_PRIVATE, "private superclass");
	SuperclassRadioEntries[1] = LS(MSG_SUPERCLASS_RADIO_PUBLIC, "public superclass");
	SuperclassRadioEntries[2] = NULL;

	ReggaeClassTypeEntries[0] = LS(MSG_REGGAE_CLASS_TYPE_STREAM, "stream");
	ReggaeClassTypeEntries[1] = LS(MSG_REGGAE_CLASS_TYPE_DEMUXER, "demultiplexer");
	ReggaeClassTypeEntries[2] = LS(MSG_REGGAE_CLASS_TYPE_DECODER, "decoder");
	ReggaeClassTypeEntries[3] = LS(MSG_REGGAE_CLASS_TYPE_FILTER, "filter");
	ReggaeClassTypeEntries[4] = LS(MSG_REGGAE_CLASS_TYPE_ENCODER, "encoder");
	ReggaeClassTypeEntries[5] = LS(MSG_REGGAE_CLASS_TYPE_MULTIPLEXER, "multiplexer");
	ReggaeClassTypeEntries[6] = LS(MSG_REGGAE_CLASS_TYPE_OUTPUT, "output");
	ReggaeClassTypeEntries[7] = LS(MSG_REGGAE_CLASS_TYPE_NO_TYPE, "no type");
	ReggaeClassTypeEntries[8] = NULL;
}
