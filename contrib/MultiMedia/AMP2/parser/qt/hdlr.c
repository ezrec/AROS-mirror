#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "quicktime.h"

void quicktime_hdlr_init(quicktime_hdlr_t *hdlr)
{
     hdlr->version = 0;
     hdlr->flags = 0;
     hdlr->component_type[0] = 'm';
     hdlr->component_type[1] = 'h';
     hdlr->component_type[2] = 'l';
     hdlr->component_type[3] = 'r';
     hdlr->component_subtype[0] = 'v';
     hdlr->component_subtype[1] = 'i';
     hdlr->component_subtype[2] = 'd';
     hdlr->component_subtype[3] = 'e';
     hdlr->component_manufacturer = 0;
     hdlr->component_flags = 0;
     hdlr->component_flag_mask = 0;
     strcpy(hdlr->component_name, "Linux Media Handler");
}

void quicktime_hdlr_init_video(quicktime_hdlr_t *hdlr)
{
     hdlr->component_subtype[0] = 'v';
     hdlr->component_subtype[1] = 'i';
     hdlr->component_subtype[2] = 'd';
     hdlr->component_subtype[3] = 'e';
     strcpy(hdlr->component_name, "Linux Video Media Handler");
}

void quicktime_hdlr_init_audio(quicktime_hdlr_t *hdlr)
{
     hdlr->component_subtype[0] = 's';
     hdlr->component_subtype[1] = 'o';
     hdlr->component_subtype[2] = 'u';
     hdlr->component_subtype[3] = 'n';
     strcpy(hdlr->component_name, "Linux Sound Media Handler");
}

void quicktime_hdlr_init_data(quicktime_hdlr_t *hdlr)
{
     hdlr->component_type[0] = 'd';
     hdlr->component_type[1] = 'h';
     hdlr->component_type[2] = 'l';
     hdlr->component_type[3] = 'r';
     hdlr->component_subtype[0] = 'a';
     hdlr->component_subtype[1] = 'l';
     hdlr->component_subtype[2] = 'i';
     hdlr->component_subtype[3] = 's';
     strcpy(hdlr->component_name, "Linux Alias Data Handler");
}

void quicktime_hdlr_delete(quicktime_hdlr_t *hdlr)
{
}

void quicktime_read_hdlr(quicktime_t *file, quicktime_hdlr_t *hdlr)
{
     hdlr->version = quicktime_read_char(file);
     hdlr->flags = quicktime_read_int24(file);
     quicktime_read_char32(file, hdlr->component_type);
     quicktime_read_char32(file, hdlr->component_subtype);
     hdlr->component_manufacturer = quicktime_read_int32(file);
     hdlr->component_flags = quicktime_read_int32(file);
     hdlr->component_flag_mask = quicktime_read_int32(file);
     quicktime_read_pascal(file, hdlr->component_name);
}

