/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_FREETYPE_H
#define _PPCINLINE_FREETYPE_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef FREETYPE_BASE_NAME
#define FREETYPE_BASE_NAME FreetypeBase
#endif /* !FREETYPE_BASE_NAME */

#define FT_Request_Size(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Size_Request ))*(void**)((long)(FREETYPE_BASE_NAME) - 796))(__p0, __p1))

#define FT_Stroker_Rewind(__p0) \
	(((void (*)(FT_Stroker ))*(void**)((long)(FREETYPE_BASE_NAME) - 850))(__p0))

#define FT_Load_Sfnt_Table(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Face , FT_ULong , FT_Long , FT_Byte *, FT_ULong *))*(void**)((long)(FREETYPE_BASE_NAME) - 640))(__p0, __p1, __p2, __p3, __p4))

#define FT_Done_Size(__p0) \
	(((FT_Error (*)(FT_Size ))*(void**)((long)(FREETYPE_BASE_NAME) - 532))(__p0))

#define FT_Outline_GetOutsideBorder(__p0) \
	(((FT_StrokerBorder (*)(FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 832))(__p0))

#define FT_Get_Charmap_Index(__p0) \
	(((FT_Int (*)(FT_CharMap ))*(void**)((long)(FREETYPE_BASE_NAME) - 700))(__p0))

#define FT_Library_Version(__p0, __p1, __p2, __p3) \
	(((void (*)(FT_Library , FT_Int *, FT_Int *, FT_Int *))*(void**)((long)(FREETYPE_BASE_NAME) - 34))(__p0, __p1, __p2, __p3))

#define FT_Glyph_Get_CBox(__p0, __p1, __p2) \
	(((void (*)(FT_Glyph , FT_UInt , FT_BBox *))*(void**)((long)(FREETYPE_BASE_NAME) - 280))(__p0, __p1, __p2))

#define FT_New_Size(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Size *))*(void**)((long)(FREETYPE_BASE_NAME) - 526))(__p0, __p1))

#define FT_Remove_Module(__p0, __p1) \
	(((FT_Error (*)(FT_Library , FT_Module ))*(void**)((long)(FREETYPE_BASE_NAME) - 388))(__p0, __p1))

#define FT_Glyph_StrokeBorder(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Glyph *, FT_Stroker , FT_Bool , FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 928))(__p0, __p1, __p2, __p3))

#define FT_Stroker_EndSubPath(__p0) \
	(((FT_Error (*)(FT_Stroker ))*(void**)((long)(FREETYPE_BASE_NAME) - 868))(__p0))

#define FT_TrueTypeGX_Validate(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Bytes *, FT_UInt ))*(void**)((long)(FREETYPE_BASE_NAME) - 946))(__p0, __p1, __p2, __p3))

#define FT_Get_Track_Kerning(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_Fixed , FT_Int , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 802))(__p0, __p1, __p2, __p3))

#define FT_Stroker_ParseOutline(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Stroker , FT_Outline *, FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 856))(__p0, __p1, __p2))

#define FT_Get_PFR_Metrics(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Face , FT_UInt *, FT_UInt *, FT_Fixed *, FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 496))(__p0, __p1, __p2, __p3, __p4))

#define FT_Sfnt_Table_Info(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_ULong *, FT_ULong *))*(void**)((long)(FREETYPE_BASE_NAME) - 742))(__p0, __p1, __p2, __p3))

#define FT_Outline_Render(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Library , FT_Outline *, FT_Raster_Params *))*(void**)((long)(FREETYPE_BASE_NAME) - 490))(__p0, __p1, __p2))

#define FT_Outline_Done(__p0, __p1) \
	(((FT_Error (*)(FT_Library , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 436))(__p0, __p1))

#define FT_Get_WinFNT_Header(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_WinFNT_HeaderRec *))*(void**)((long)(FREETYPE_BASE_NAME) - 616))(__p0, __p1))

#define FT_Outline_Get_CBox(__p0, __p1) \
	(((void (*)(const FT_Outline *, FT_BBox *))*(void**)((long)(FREETYPE_BASE_NAME) - 454))(__p0, __p1))

#define FT_Get_Sfnt_Table(__p0, __p1) \
	(((void *(*)(FT_Face , FT_Sfnt_Tag ))*(void**)((long)(FREETYPE_BASE_NAME) - 634))(__p0, __p1))

#define FT_Outline_EmboldenXY(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Outline *, FT_Pos , FT_Pos ))*(void**)((long)(FREETYPE_BASE_NAME) - 1102))(__p0, __p1, __p2))

#define FT_Outline_New_Internal(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Memory , FT_UInt , FT_Int , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 430))(__p0, __p1, __p2, __p3))

#define FT_Load_Glyph(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Int32 ))*(void**)((long)(FREETYPE_BASE_NAME) - 94))(__p0, __p1, __p2))

#define FT_Bitmap_New(__p0) \
	(((void (*)(FT_Bitmap *))*(void**)((long)(FREETYPE_BASE_NAME) - 754))(__p0))

#define FT_Attach_Stream(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Open_Args *))*(void**)((long)(FREETYPE_BASE_NAME) - 70))(__p0, __p1))

#define FT_Face_SetUnpatentedHinting(__p0, __p1) \
	(((FT_Bool (*)(FT_Face , FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 976))(__p0, __p1))

#define FT_Vector_Polarize(__p0, __p1, __p2) \
	(((void (*)(FT_Vector *, FT_Fixed *, FT_Angle *))*(void**)((long)(FREETYPE_BASE_NAME) - 604))(__p0, __p1, __p2))

#define FTC_Manager_New(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	(((FT_Error (*)(FT_Library , FT_UInt , FT_UInt , FT_ULong , FTC_Face_Requester , FT_Pointer , FTC_Manager *))*(void**)((long)(FREETYPE_BASE_NAME) - 232))(__p0, __p1, __p2, __p3, __p4, __p5, __p6))

#define FT_Get_CMap_Format(__p0) \
	(((FT_Long (*)(FT_CharMap ))*(void**)((long)(FREETYPE_BASE_NAME) - 994))(__p0))

#define FT_Stroker_Done(__p0) \
	(((void (*)(FT_Stroker ))*(void**)((long)(FREETYPE_BASE_NAME) - 916))(__p0))

#define FTC_Manager_Reset(__p0) \
	(((void (*)(FTC_Manager ))*(void**)((long)(FREETYPE_BASE_NAME) - 238))(__p0))

#define FT_List_Insert(__p0, __p1) \
	(((void (*)(FT_List , FT_ListNode ))*(void**)((long)(FREETYPE_BASE_NAME) - 328))(__p0, __p1))

#define FT_Reference_Face(__p0) \
	(((FT_Error (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 1090))(__p0))

#define FT_Outline_Reverse(__p0) \
	(((void (*)(FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 478))(__p0))

#define FT_Glyph_Transform(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Glyph , FT_Matrix *, FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 274))(__p0, __p1, __p2))

#define FT_Bitmap_Copy(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Library , const FT_Bitmap *, FT_Bitmap *))*(void**)((long)(FREETYPE_BASE_NAME) - 760))(__p0, __p1, __p2))

#define FT_Get_Module(__p0, __p1) \
	(((FT_Module (*)(FT_Library , const char *))*(void**)((long)(FREETYPE_BASE_NAME) - 382))(__p0, __p1))

#define FT_Face_GetVariantsOfChar(__p0, __p1) \
	(((FT_UInt32 *(*)(FT_Face , FT_ULong ))*(void**)((long)(FREETYPE_BASE_NAME) - 1024))(__p0, __p1))

#define FT_Get_Advance(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Int32 , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 1036))(__p0, __p1, __p2, __p3))

#define FT_Property_Set(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , const FT_String *, const FT_String *, const void *))*(void**)((long)(FREETYPE_BASE_NAME) - 1108))(__p0, __p1, __p2, __p3))

#define FT_Set_Var_Design_Coordinates(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 712))(__p0, __p1, __p2))

#define FT_Get_BDF_Property(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , const char *, BDF_PropertyRec *))*(void**)((long)(FREETYPE_BASE_NAME) - 226))(__p0, __p1, __p2))

#define FT_Angle_Diff(__p0, __p1) \
	(((FT_Angle (*)(FT_Angle , FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 580))(__p0, __p1))

#define FT_Outline_Transform(__p0, __p1) \
	(((void (*)(const FT_Outline *, const FT_Matrix *))*(void**)((long)(FREETYPE_BASE_NAME) - 472))(__p0, __p1))

#define FTC_Manager_LookupSize(__p0, __p1, __p2) \
	(((FT_Error (*)(FTC_Manager , FTC_Scaler , FT_Size *))*(void**)((long)(FREETYPE_BASE_NAME) - 256))(__p0, __p1, __p2))

#define FT_Init_FreeType(__p0) \
	(((FT_Error (*)(FT_Library *))*(void**)((long)(FREETYPE_BASE_NAME) - 28))(__p0))

#define FT_Stroker_BeginSubPath(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Stroker , FT_Vector *, FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 862))(__p0, __p1, __p2))

#define FT_Get_PFR_Advance(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Pos *))*(void**)((long)(FREETYPE_BASE_NAME) - 508))(__p0, __p1, __p2))

#define FT_Get_Glyph(__p0, __p1) \
	(((FT_Error (*)(FT_GlyphSlot , FT_Glyph *))*(void**)((long)(FREETYPE_BASE_NAME) - 262))(__p0, __p1))

#define FT_Vector_Unit(__p0, __p1) \
	(((void (*)(FT_Vector *, FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 586))(__p0, __p1))

#define FT_Get_CID_Registry_Ordering_Supplement(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , const char **, const char **, FT_Int *))*(void**)((long)(FREETYPE_BASE_NAME) - 1054))(__p0, __p1, __p2, __p3))

#define FT_Reference_Library(__p0) \
	(((FT_Error (*)(FT_Library ))*(void**)((long)(FREETYPE_BASE_NAME) - 1084))(__p0))

#define FT_Outline_New(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , FT_UInt , FT_Int , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 424))(__p0, __p1, __p2, __p3))

#define FT_Done_Library(__p0) \
	(((FT_Error (*)(FT_Library ))*(void**)((long)(FREETYPE_BASE_NAME) - 400))(__p0))

#define FT_Get_Kerning(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_UInt , FT_UInt , FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 118))(__p0, __p1, __p2, __p3, __p4))

#define FT_Stroker_GetCounts(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Stroker , FT_UInt *, FT_UInt *))*(void**)((long)(FREETYPE_BASE_NAME) - 904))(__p0, __p1, __p2))

#define FT_Glyph_Copy(__p0, __p1) \
	(((FT_Error (*)(FT_Glyph , FT_Glyph *))*(void**)((long)(FREETYPE_BASE_NAME) - 268))(__p0, __p1))

#define FTC_ImageCache_New(__p0, __p1) \
	(((FT_Error (*)(FTC_Manager , FTC_ImageCache *))*(void**)((long)(FREETYPE_BASE_NAME) - 676))(__p0, __p1))

#define FT_Cos(__p0) \
	(((FT_Fixed (*)(FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 562))(__p0))

#define FT_Glyph_To_Bitmap(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Glyph *, FT_Render_Mode , FT_Vector *, FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 286))(__p0, __p1, __p2, __p3))

#define FT_Get_FSType_Flags(__p0) \
	(((FT_UShort (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 1000))(__p0))

#define FT_Set_Char_Size(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Face , FT_F26Dot6 , FT_F26Dot6 , FT_UInt , FT_UInt ))*(void**)((long)(FREETYPE_BASE_NAME) - 82))(__p0, __p1, __p2, __p3, __p4))

#define FT_Done_Face(__p0) \
	(((FT_Error (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 76))(__p0))

#define FT_Select_Size(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Int ))*(void**)((long)(FREETYPE_BASE_NAME) - 790))(__p0, __p1))

#define FT_Get_Char_Index(__p0, __p1) \
	(((FT_UInt (*)(FT_Face , FT_ULong ))*(void**)((long)(FREETYPE_BASE_NAME) - 148))(__p0, __p1))

#define FT_Render_Glyph(__p0, __p1) \
	(((FT_Error (*)(FT_GlyphSlot , FT_Render_Mode ))*(void**)((long)(FREETYPE_BASE_NAME) - 112))(__p0, __p1))

#define FT_Stroker_Set(__p0, __p1, __p2, __p3, __p4) \
	(((void (*)(FT_Stroker , FT_Fixed , FT_Stroker_LineCap , FT_Stroker_LineJoin , FT_Fixed ))*(void**)((long)(FREETYPE_BASE_NAME) - 844))(__p0, __p1, __p2, __p3, __p4))

#define FT_CeilFix(__p0) \
	(((FT_Fixed (*)(FT_Fixed ))*(void**)((long)(FREETYPE_BASE_NAME) - 196))(__p0))

#define FT_Get_Name_Index(__p0, __p1) \
	(((FT_Uint (*)(FT_Face , FT_String *))*(void**)((long)(FREETYPE_BASE_NAME) - 166))(__p0, __p1))

#define FTC_Manager_LookupFace(__p0, __p1, __p2) \
	(((FT_Error (*)(FTC_Manager , FTC_FaceID , FT_Face *))*(void**)((long)(FREETYPE_BASE_NAME) - 250))(__p0, __p1, __p2))

#define FTC_Node_Unref(__p0, __p1) \
	(((void (*)(FTC_Node , FTC_Manager ))*(void**)((long)(FREETYPE_BASE_NAME) - 658))(__p0, __p1))

#define FTC_Manager_Done(__p0) \
	(((void (*)(FTC_Manager ))*(void**)((long)(FREETYPE_BASE_NAME) - 244))(__p0))

#define FT_Outline_Get_Bitmap(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Library , FT_Outline *, const FT_Bitmap *))*(void**)((long)(FREETYPE_BASE_NAME) - 484))(__p0, __p1, __p2))

#define FT_Get_Renderer(__p0, __p1) \
	(((FT_Renderer (*)(FT_Library , FT_Glyph_Format ))*(void**)((long)(FREETYPE_BASE_NAME) - 514))(__p0, __p1))

#define FTC_CMapCache_Lookup(__p0, __p1, __p2, __p3) \
	(((FT_UInt (*)(FTC_CMapCache , FTC_FaceID , FT_Int , FT_UInt32 ))*(void**)((long)(FREETYPE_BASE_NAME) - 670))(__p0, __p1, __p2, __p3))

#define FT_Vector_Rotate(__p0, __p1) \
	(((void (*)(FT_Vector *, FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 592))(__p0, __p1))

#define FT_Face_CheckTrueTypePatents(__p0) \
	(((FT_Bool (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 970))(__p0))

#define FT_Tan(__p0) \
	(((FT_Fixed (*)(FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 568))(__p0))

#define FT_Outline_GetInsideBorder(__p0) \
	(((FT_StrokerBorder (*)(FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 826))(__p0))

#define FT_Get_CID_Is_Internally_CID_Keyed(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Bool *))*(void**)((long)(FREETYPE_BASE_NAME) - 1060))(__p0, __p1))

#define FT_Bitmap_Done(__p0, __p1) \
	(((FT_Error (*)(FT_Library , FT_Bitmap *))*(void**)((long)(FREETYPE_BASE_NAME) - 778))(__p0, __p1))

#define FT_Set_MM_Blend_Coordinates(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 370))(__p0, __p1, __p2))

#define FT_MulDiv(__p0, __p1, __p2) \
	(((FT_Long (*)(FT_Long , FT_Long , FT_Long ))*(void**)((long)(FREETYPE_BASE_NAME) - 172))(__p0, __p1, __p2))

#define FT_Get_Sfnt_Name(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_SfntName *))*(void**)((long)(FREETYPE_BASE_NAME) - 550))(__p0, __p1, __p2))

#define FTC_SBitCache_Lookup(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FTC_SBitCache , FTC_ImageType , FT_UInt , FTC_SBit *, FTC_Node *))*(void**)((long)(FREETYPE_BASE_NAME) - 694))(__p0, __p1, __p2, __p3, __p4))

#define FT_OpenType_Validate(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Bytes *, FT_Bytes *, FT_Bytes *, FT_Bytes *, FT_Bytes *))*(void**)((long)(FREETYPE_BASE_NAME) - 748))(__p0, __p1, __p2, __p3, __p4, __p5, __p6))

#define FT_New_Memory_Face(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Library , const FT_Byte *, FT_Long , FT_Long , FT_Face *))*(void**)((long)(FREETYPE_BASE_NAME) - 52))(__p0, __p1, __p2, __p3, __p4))

#define FT_Get_Glyph_Name(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Pointer , FT_UInt ))*(void**)((long)(FREETYPE_BASE_NAME) - 124))(__p0, __p1, __p2, __p3))

#define FT_Activate_Size(__p0) \
	(((FT_Error (*)(FT_Size ))*(void**)((long)(FREETYPE_BASE_NAME) - 538))(__p0))

#define FT_Set_Var_Blend_Coordinates(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 718))(__p0, __p1, __p2))

#define FTC_CMapCache_New(__p0, __p1) \
	(((FT_Error (*)(FTC_Manager , FTC_CMapCache *))*(void**)((long)(FREETYPE_BASE_NAME) - 664))(__p0, __p1))

#define FT_Done_FreeType(__p0) \
	(((FT_Error (*)(FT_Library ))*(void**)((long)(FREETYPE_BASE_NAME) - 40))(__p0))

#define FT_Matrix_Multiply(__p0, __p1) \
	(((void (*)(const FT_Matrix *, FT_Matrix *))*(void**)((long)(FREETYPE_BASE_NAME) - 298))(__p0, __p1))

#define FT_Outline_Get_BBox(__p0, __p1) \
	(((FT_Error (*)(FT_Outline *, FT_BBox *))*(void**)((long)(FREETYPE_BASE_NAME) - 214))(__p0, __p1))

#define FT_Bitmap_Convert(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , const FT_Bitmap *, FT_Bitmap *, FT_Int ))*(void**)((long)(FREETYPE_BASE_NAME) - 772))(__p0, __p1, __p2, __p3))

#define FT_Set_Transform(__p0, __p1, __p2) \
	(((void (*)(FT_Face , FT_Matrix *, FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 106))(__p0, __p1, __p2))

#define FT_Get_SubGlyph_Info(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	(((FT_Error (*)(FT_GlyphSlot , FT_UInt , FT_Int *, FT_UInt *, FT_Int *, FT_Int *, FT_Matrix *))*(void**)((long)(FREETYPE_BASE_NAME) - 808))(__p0, __p1, __p2, __p3, __p4, __p5, __p6))

#define FT_Set_Renderer(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , FT_Renderer , FT_UInt , FT_Parameter *))*(void**)((long)(FREETYPE_BASE_NAME) - 520))(__p0, __p1, __p2, __p3))

#define FT_Get_X11_Font_Format(__p0) \
	(((const char *(*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 1072))(__p0))

#define FT_Set_Debug_Hook(__p0, __p1, __p2) \
	(((void (*)(FT_Library , FT_UInt , FT_DebugHook_Func ))*(void**)((long)(FREETYPE_BASE_NAME) - 406))(__p0, __p1, __p2))

#define FT_Property_Get(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , const FT_String *, const FT_String *, void *))*(void**)((long)(FREETYPE_BASE_NAME) - 1114))(__p0, __p1, __p2, __p3))

#define FT_Stroker_Export(__p0, __p1) \
	(((void (*)(FT_Stroker , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 910))(__p0, __p1))

#define FT_List_Remove(__p0, __p1) \
	(((void (*)(FT_List , FT_ListNode ))*(void**)((long)(FREETYPE_BASE_NAME) - 334))(__p0, __p1))

#define FT_GlyphSlot_Own_Bitmap(__p0) \
	(((FT_Error (*)(FT_GlyphSlot ))*(void**)((long)(FREETYPE_BASE_NAME) - 1048))(__p0))

#define FT_Vector_From_Polar(__p0, __p1, __p2) \
	(((void (*)(FT_Vector *, FT_Fixed , FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 610))(__p0, __p1, __p2))

#define FT_Get_PS_Font_Info(__p0, __p1) \
	(((FT_Error (*)(FT_Face , PS_FontInfoRec *))*(void**)((long)(FREETYPE_BASE_NAME) - 628))(__p0, __p1))

#define FT_Face_GetCharsOfVariant(__p0, __p1) \
	(((FT_UInt32 *(*)(FT_Face , FT_ULong ))*(void**)((long)(FREETYPE_BASE_NAME) - 1030))(__p0, __p1))

#define FT_TrueTypeGX_Free(__p0, __p1) \
	(((void (*)(FT_Face , FT_Bytes ))*(void**)((long)(FREETYPE_BASE_NAME) - 952))(__p0, __p1))

#define FT_OpenType_Free(__p0, __p1) \
	(((void (*)(FT_Face , FT_Bytes ))*(void**)((long)(FREETYPE_BASE_NAME) - 820))(__p0, __p1))

#define FT_Stroker_ExportBorder(__p0, __p1, __p2) \
	(((void (*)(FT_Stroker , FT_StrokerBorder , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 898))(__p0, __p1, __p2))

#define FT_Open_Face(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , const FT_Open_Args *, FT_Long , FT_Face *))*(void**)((long)(FREETYPE_BASE_NAME) - 58))(__p0, __p1, __p2, __p3))

#define FT_List_Finalize(__p0, __p1, __p2, __p3) \
	(((void (*)(FT_List , FT_List_Destructor , FT_Memory , void *))*(void**)((long)(FREETYPE_BASE_NAME) - 352))(__p0, __p1, __p2, __p3))

#define FT_Get_CID_From_Glyph_Index(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_UInt *))*(void**)((long)(FREETYPE_BASE_NAME) - 1066))(__p0, __p1, __p2))

#define FT_Matrix_Invert(__p0) \
	(((FT_Error (*)(FT_Matrix *))*(void**)((long)(FREETYPE_BASE_NAME) - 304))(__p0))

#define FT_Set_MM_Design_Coordinates(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Long *))*(void**)((long)(FREETYPE_BASE_NAME) - 364))(__p0, __p1, __p2))

#define FT_Stroker_ConicTo(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Stroker , FT_Vector *, FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 880))(__p0, __p1, __p2))

#define FT_Outline_Decompose(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Outline *, const FT_Outline_Funcs *, void *))*(void**)((long)(FREETYPE_BASE_NAME) - 418))(__p0, __p1, __p2))

#define FT_Set_Charmap(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_CharMap ))*(void**)((long)(FREETYPE_BASE_NAME) - 142))(__p0, __p1))

#define FT_Set_Pixel_Sizes(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_UInt ))*(void**)((long)(FREETYPE_BASE_NAME) - 88))(__p0, __p1, __p2))

#define FTC_SBitCache_New(__p0, __p1) \
	(((FT_Error (*)(FTC_Manager , FTC_SBitCache *))*(void**)((long)(FREETYPE_BASE_NAME) - 688))(__p0, __p1))

#define FT_Outline_Translate(__p0, __p1, __p2) \
	(((void (*)(const FT_Outline *, FT_Pos , FT_Pos ))*(void**)((long)(FREETYPE_BASE_NAME) - 460))(__p0, __p1, __p2))

#define FT_Stream_OpenGzip(__p0, __p1) \
	(((FT_Error (*)(FT_Stream , FT_Stream ))*(void**)((long)(FREETYPE_BASE_NAME) - 310))(__p0, __p1))

#define FT_Get_PS_Font_Private(__p0, __p1) \
	(((FT_Error (*)(FT_Face , PS_PrivateRec *))*(void**)((long)(FREETYPE_BASE_NAME) - 736))(__p0, __p1))

#define FT_Outline_Get_Orientation(__p0) \
	(((FT_Orientation (*)(FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 784))(__p0))

#define FT_Vector_Transform(__p0, __p1) \
	(((void (*)(FT_Vector *, const FT_Matrix *))*(void**)((long)(FREETYPE_BASE_NAME) - 208))(__p0, __p1))

#define FT_Stroker_New(__p0, __p1) \
	(((FT_Error (*)(FT_Library , FT_Stroker *))*(void**)((long)(FREETYPE_BASE_NAME) - 838))(__p0, __p1))

#define FT_Attach_File(__p0, __p1) \
	(((FT_Error (*)(FT_Face , const char *))*(void**)((long)(FREETYPE_BASE_NAME) - 64))(__p0, __p1))

#define FT_Outline_Check(__p0) \
	(((FT_Error (*)(FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 448))(__p0))

#define FT_Sin(__p0) \
	(((FT_Fixed (*)(FT_Angle ))*(void**)((long)(FREETYPE_BASE_NAME) - 556))(__p0))

#define FT_Add_Module(__p0, __p1) \
	(((FT_Error (*)(FT_Library , const FT_Module_Class *))*(void**)((long)(FREETYPE_BASE_NAME) - 376))(__p0, __p1))

#define FT_List_Iterate(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_List , FT_List_Iterator , void *))*(void**)((long)(FREETYPE_BASE_NAME) - 346))(__p0, __p1, __p2))

#define FT_Glyph_Stroke(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Glyph *, FT_Stroker , FT_Bool ))*(void**)((long)(FREETYPE_BASE_NAME) - 922))(__p0, __p1, __p2))

#define FTC_SBitCache_LookupScaler(__p0, __p1, __p2, __p3, __p4, __p5) \
	(((FT_Error (*)(FTC_SBitCache , FTC_Scaler , FT_ULong , FT_UInt , FTC_SBit *, FTC_Node *))*(void**)((long)(FREETYPE_BASE_NAME) - 988))(__p0, __p1, __p2, __p3, __p4, __p5))

#define FT_Get_First_Char(__p0, __p1) \
	(((FT_ULong (*)(FT_Face , FT_UInt *))*(void**)((long)(FREETYPE_BASE_NAME) - 154))(__p0, __p1))

#define FT_Library_SetLcdFilter(__p0, __p1) \
	(((FT_Error (*)(FT_Library , FT_LcdFilter ))*(void**)((long)(FREETYPE_BASE_NAME) - 940))(__p0, __p1))

#define FT_Face_GetCharVariantIndex(__p0, __p1, __p2) \
	(((FT_UInt (*)(FT_Face , FT_ULong , FT_ULong ))*(void**)((long)(FREETYPE_BASE_NAME) - 1006))(__p0, __p1, __p2))

#define FT_New_Face(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , const char *, FT_Long , FT_Face *))*(void**)((long)(FREETYPE_BASE_NAME) - 46))(__p0, __p1, __p2, __p3))

#define FT_Get_PFR_Kerning(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_UInt , FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 502))(__p0, __p1, __p2, __p3))

#define FT_Get_Multi_Master(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Multi_Master *))*(void**)((long)(FREETYPE_BASE_NAME) - 358))(__p0, __p1))

#define FT_Library_SetLcdFilterWeights(__p0, __p1) \
	(((FT_Error (*)(FT_Library , unsigned char *))*(void**)((long)(FREETYPE_BASE_NAME) - 1078))(__p0, __p1))

#define FT_Get_PS_Font_Value(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Long (*)(FT_Face , PS_Dict_Keys , FT_UInt , void *, FT_Long ))*(void**)((long)(FREETYPE_BASE_NAME) - 1096))(__p0, __p1, __p2, __p3, __p4))

#define FT_List_Up(__p0, __p1) \
	(((void (*)(FT_List , FT_ListNode ))*(void**)((long)(FREETYPE_BASE_NAME) - 340))(__p0, __p1))

#define FT_ClassicKern_Free(__p0, __p1) \
	(((void (*)(FT_Face , FT_Bytes ))*(void**)((long)(FREETYPE_BASE_NAME) - 964))(__p0, __p1))

#define FT_Has_PS_Glyph_Names(__p0) \
	(((FT_Int (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 622))(__p0))

#define FTC_Manager_RemoveFaceID(__p0, __p1) \
	(((void (*)(FTC_Manager , FTC_FaceID ))*(void**)((long)(FREETYPE_BASE_NAME) - 652))(__p0, __p1))

#define FT_Select_Charmap(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_Encoding ))*(void**)((long)(FREETYPE_BASE_NAME) - 136))(__p0, __p1))

#define FT_MulFix(__p0, __p1) \
	(((FT_Long (*)(FT_Long , FT_Long ))*(void**)((long)(FREETYPE_BASE_NAME) - 178))(__p0, __p1))

#define FTC_ImageCache_LookupScaler(__p0, __p1, __p2, __p3, __p4, __p5) \
	(((FT_Error (*)(FTC_ImageCache , FTC_Scaler , FT_ULong , FT_UInt , FT_Glyph *, FTC_Node *))*(void**)((long)(FREETYPE_BASE_NAME) - 982))(__p0, __p1, __p2, __p3, __p4, __p5))

#define FT_Bitmap_Embolden(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Library , FT_Bitmap *, FT_Pos , FT_Pos ))*(void**)((long)(FREETYPE_BASE_NAME) - 766))(__p0, __p1, __p2, __p3))

#define FT_DivFix(__p0, __p1) \
	(((FT_Long (*)(FT_Long , FT_Long ))*(void**)((long)(FREETYPE_BASE_NAME) - 184))(__p0, __p1))

#define FT_Get_Next_Char(__p0, __p1, __p2) \
	(((FT_ULong (*)(FT_Face , FT_ULong , FT_UInt *))*(void**)((long)(FREETYPE_BASE_NAME) - 160))(__p0, __p1, __p2))

#define FT_Stroker_CubicTo(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Stroker , FT_Vector *, FT_Vector *, FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 886))(__p0, __p1, __p2, __p3))

#define FT_Outline_Embolden(__p0, __p1) \
	(((FT_Error (*)(FT_Outline *, FT_Pos ))*(void**)((long)(FREETYPE_BASE_NAME) - 730))(__p0, __p1))

#define FT_Get_TrueType_Engine_Type(__p0) \
	(((FT_TrueTypeEngineType (*)(FT_Library ))*(void**)((long)(FREETYPE_BASE_NAME) - 814))(__p0))

#define FT_Outline_Done_Internal(__p0, __p1) \
	(((FT_Error (*)(FT_Memory , FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 442))(__p0, __p1))

#define FT_List_Add(__p0, __p1) \
	(((void (*)(FT_List , FT_ListNode ))*(void**)((long)(FREETYPE_BASE_NAME) - 322))(__p0, __p1))

#define FT_Get_Gasp(__p0, __p1) \
	(((FT_Int (*)(FT_Face , FT_UInt ))*(void**)((long)(FREETYPE_BASE_NAME) - 934))(__p0, __p1))

#define FT_Face_GetCharVariantIsDefault(__p0, __p1, __p2) \
	(((FT_Int (*)(FT_Face , FT_ULong , FT_ULong ))*(void**)((long)(FREETYPE_BASE_NAME) - 1012))(__p0, __p1, __p2))

#define FT_Load_Char(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_ULong , FT_Int32 ))*(void**)((long)(FREETYPE_BASE_NAME) - 100))(__p0, __p1, __p2))

#define FT_Vector_Length(__p0) \
	(((FT_Fixed (*)(FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 598))(__p0))

#define FT_Get_Postscript_Name(__p0) \
	(((const char *(*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 130))(__p0))

#define FT_Stroker_LineTo(__p0, __p1) \
	(((FT_Error (*)(FT_Stroker , FT_Vector *))*(void**)((long)(FREETYPE_BASE_NAME) - 874))(__p0, __p1))

#define FT_Stream_OpenLZW(__p0, __p1) \
	(((FT_Error (*)(FT_Stream , FT_Stream ))*(void**)((long)(FREETYPE_BASE_NAME) - 724))(__p0, __p1))

#define FT_Get_BDF_Charset_ID(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , const char **, const char **))*(void**)((long)(FREETYPE_BASE_NAME) - 220))(__p0, __p1, __p2))

#define FT_Get_Advances(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_UInt , FT_Int32 , FT_Fixed *))*(void**)((long)(FREETYPE_BASE_NAME) - 1042))(__p0, __p1, __p2, __p3, __p4))

#define FT_New_Library(__p0, __p1) \
	(((FT_Error (*)(FT_Memory , FT_Library *))*(void**)((long)(FREETYPE_BASE_NAME) - 394))(__p0, __p1))

#define FT_Stroker_GetBorderCounts(__p0, __p1, __p2, __p3) \
	(((FT_Error (*)(FT_Stroker , FT_StrokerBorder , FT_UInt *, FT_UInt *))*(void**)((long)(FREETYPE_BASE_NAME) - 892))(__p0, __p1, __p2, __p3))

#define FT_ClassicKern_Validate(__p0, __p1, __p2) \
	(((FT_Error (*)(FT_Face , FT_UInt , FT_Bytes *))*(void**)((long)(FREETYPE_BASE_NAME) - 958))(__p0, __p1, __p2))

#define FT_Add_Default_Modules(__p0) \
	(((void (*)(FT_Library ))*(void**)((long)(FREETYPE_BASE_NAME) - 412))(__p0))

#define FT_List_Find(__p0, __p1) \
	(((FT_ListNode (*)(FT_List , void *))*(void**)((long)(FREETYPE_BASE_NAME) - 316))(__p0, __p1))

#define FT_Get_Sfnt_Name_Count(__p0) \
	(((FT_UInt (*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 544))(__p0))

#define FTC_ImageCache_Lookup(__p0, __p1, __p2, __p3, __p4) \
	(((FT_Error (*)(FTC_ImageCache , FTC_ImageType , FT_UInt , FT_Glyph *, FTC_Node *))*(void**)((long)(FREETYPE_BASE_NAME) - 682))(__p0, __p1, __p2, __p3, __p4))

#define FT_Outline_Copy(__p0, __p1) \
	(((FT_Error (*)(const FT_Outline *, FT_Outline *))*(void**)((long)(FREETYPE_BASE_NAME) - 466))(__p0, __p1))

#define FT_Done_Glyph(__p0) \
	(((void (*)(FT_Glyph ))*(void**)((long)(FREETYPE_BASE_NAME) - 292))(__p0))

#define FT_Face_GetVariantSelectors(__p0) \
	(((FT_UInt32 *(*)(FT_Face ))*(void**)((long)(FREETYPE_BASE_NAME) - 1018))(__p0))

#define FT_Get_MM_Var(__p0, __p1) \
	(((FT_Error (*)(FT_Face , FT_MM_Var **))*(void**)((long)(FREETYPE_BASE_NAME) - 706))(__p0, __p1))

#define FT_Atan2(__p0, __p1) \
	(((FT_Angle (*)(FT_Fixed , FT_Fixed ))*(void**)((long)(FREETYPE_BASE_NAME) - 574))(__p0, __p1))

#endif /* !_PPCINLINE_FREETYPE_H */
