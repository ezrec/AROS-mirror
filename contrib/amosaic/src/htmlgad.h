#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

/*------------------------------------------------------------------------
  Programmer-visible tags.   I=can initialize, S=can set, G=can get
------------------------------------------------------------------------*/

#define TAG(x)				(TAG_USER + x)
#define HTMLA_title			TAG(1)		// --G
#define HTMLA_raw_text		TAG(2)		// -SG
#define HTMLA_objects		TAG(3)		// ---
#define HTMLA_new_start_pos	TAG(4)		// -S-
#define HTMLA_window		TAG(5)		// I--
#define HTMLA_font			TAG(6)		// I--
#define HTMLA_italic_font	TAG(7)		// I--
#define HTMLA_bold_font		TAG(8)		// I--
#define HTMLA_fixed_font	TAG(9)		// I--
#define HTMLA_header1_font	TAG(10)		// I--
#define HTMLA_header2_font	TAG(11)		// I--
#define HTMLA_header3_font	TAG(12)		// I--
#define HTMLA_header4_font	TAG(13)		// I--
#define HTMLA_header5_font	TAG(14)		// I--
#define HTMLA_header6_font	TAG(15)		// I--
#define HTMLA_address_font	TAG(16)		// I--
#define HTMLA_plain_font	TAG(17)		// I--
#define HTMLA_listing_font	TAG(18)		// I--
#define HTMLA_doc_height    TAG(19)		// --G
#define HTMLA_new_href      TAG(20)		// --G
#define HTMLA_view_height   TAG(21)		// --G
#define HTMLA_scroll_y      TAG(22)		// -SG
#define HTMLA_scroll_x      TAG(23)		// -SG
#define HTMLA_doc_width     TAG(24)		// --G
#define HTMLA_view_width    TAG(25)		// --G
#define HTMLA_visited_fg    TAG(26)     // -S-
#define HTMLA_anchor_fg     TAG(27)     // -S-
#define HTMLA_active_fg     TAG(28)     // -S-
#define HTMLA_image_func    TAG(29)     // I--
#define HTMLA_visit_func    TAG(30)     // I--
#define HTMLA_inst          TAG(31)     // --G
#define HTMLA_image_load    TAG(32)     // --G
#define HTMLA_mui_resize    TAG(33)     // ---Notify

#define HTMLA_active        TAG(102)
#define HTMLA_SB_Vert		TAG(103)	// -S-
#define HTMLA_SB_Horiz		TAG(104)	// -S-
#define HTMLA_HTML_Data		TAG(106)	// -S-
#define HTMLA_Top			TAG(107)	// -S-
#define HTMLA_Left			TAG(108)	// -S-

#define HTMLA_post_text		TAG(110)	// -SG
#define HTMLA_post_href		TAG(111)	// -SG
#define HTMLA_get_href		TAG(112)	// -SG
#define HTMLA_ScrollKeys	TAG(113)	// .S. /* BOOL */
#define HTMLM_SetText       TAG(101)

#define HTMLM_ResetTopLeft	TAG(109)	// --- /* OBSOLETE */
#define HTMLM_ResetAmigaGadgets TAG(105)
#define HTMLM_ResetAmigaGadgetsNoScroll TAG(114)
