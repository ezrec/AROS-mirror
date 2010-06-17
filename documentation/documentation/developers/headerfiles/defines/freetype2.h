#ifndef DEFINES_FREETYPE2_H
#define DEFINES_FREETYPE2_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/freetype/src/freetype2.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for freetype2
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __OpenEngine_WB(__FreeType2Base) \
        AROS_LC0(struct GlyphEngine *, OpenEngine, \
        struct Library *, (__FreeType2Base), 5, FreeType2)

#define OpenEngine() \
    __OpenEngine_WB(FreeType2Base)

#define __CloseEngine_WB(__FreeType2Base, __arg1) \
        AROS_LC1NR(void, CloseEngine, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
        struct Library *, (__FreeType2Base), 6, FreeType2)

#define CloseEngine(arg1) \
    __CloseEngine_WB(FreeType2Base, (arg1))

#define __SetInfoA_WB(__FreeType2Base, __arg1, __arg2) \
        AROS_LC2(ULONG, SetInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__FreeType2Base), 7, FreeType2)

#define SetInfoA(arg1, arg2) \
    __SetInfoA_WB(FreeType2Base, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(FREETYPE2_NO_INLINE_STDARG)
#define SetInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ObtainInfoA_WB(__FreeType2Base, __arg1, __arg2) \
        AROS_LC2(ULONG, ObtainInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__FreeType2Base), 8, FreeType2)

#define ObtainInfoA(arg1, arg2) \
    __ObtainInfoA_WB(FreeType2Base, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(FREETYPE2_NO_INLINE_STDARG)
#define ObtainInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ObtainInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ReleaseInfoA_WB(__FreeType2Base, __arg1, __arg2) \
        AROS_LC2(ULONG, ReleaseInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__FreeType2Base), 9, FreeType2)

#define ReleaseInfoA(arg1, arg2) \
    __ReleaseInfoA_WB(FreeType2Base, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(FREETYPE2_NO_INLINE_STDARG)
#define ReleaseInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ReleaseInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */
#define FT_Init_FreeType ((FT_Error (*)(FT_Library *alibrary))__AROS_GETVECADDR(FreeType2Base,15))
#define FT_Done_FreeType ((FT_Error (*)(FT_Library library))__AROS_GETVECADDR(FreeType2Base,16))
#define FT_Library_Version ((void (*)(FT_Library library, FT_Int *amajor, FT_Int *aminor, FT_Int *apatch))__AROS_GETVECADDR(FreeType2Base,17))
#define FT_New_Face ((FT_Error (*)(FT_Library library, const char *pathname, FT_Long face_index, FT_Face *aface))__AROS_GETVECADDR(FreeType2Base,18))
#define FT_Done_Face ((FT_Error (*)(FT_Face face))__AROS_GETVECADDR(FreeType2Base,19))
#define FT_New_Memory_Face ((FT_Error (*)(FT_Library library, const FT_Byte *file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface))__AROS_GETVECADDR(FreeType2Base,20))
#define FT_Open_Face ((FT_Error (*)(FT_Library library, const FT_Open_Args *args, FT_Long face_index, FT_Face *aface))__AROS_GETVECADDR(FreeType2Base,21))
#define FT_Attach_File ((FT_Error (*)(FT_Face face, const char *filepathname))__AROS_GETVECADDR(FreeType2Base,22))
#define FT_Attach_Stream ((FT_Error (*)(FT_Face face, FT_Open_Args *parameters))__AROS_GETVECADDR(FreeType2Base,23))
#define FT_Set_Char_Size ((FT_Error (*)(FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution))__AROS_GETVECADDR(FreeType2Base,24))
#define FT_Set_Pixel_Sizes ((FT_Error (*)(FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height))__AROS_GETVECADDR(FreeType2Base,25))
#define FT_Set_Transform ((void (*)(FT_Face face, FT_Matrix *matrix, FT_Vector *delta))__AROS_GETVECADDR(FreeType2Base,26))
#define FT_Load_Glyph ((FT_Error (*)(FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags))__AROS_GETVECADDR(FreeType2Base,27))
#define FT_Get_Char_Index ((FT_UInt (*)(FT_Face face, FT_ULong charcode))__AROS_GETVECADDR(FreeType2Base,28))
#define FT_Get_Name_Index ((FT_UInt (*)(FT_Face face, FT_String *glyph_name))__AROS_GETVECADDR(FreeType2Base,29))
#define FT_Load_Char ((FT_Error (*)(FT_Face face, FT_ULong char_code, FT_Int32 load_flags))__AROS_GETVECADDR(FreeType2Base,30))
#define FT_Render_Glyph ((FT_Error (*)(FT_GlyphSlot slot, FT_Render_Mode render_mode))__AROS_GETVECADDR(FreeType2Base,31))
#define FT_Get_Kerning ((FT_Error (*)(FT_Face face, FT_UInt left_glyph, FT_UInt right_glyph, FT_UInt kern_mode, FT_Vector *akerning))__AROS_GETVECADDR(FreeType2Base,32))
#define FT_Get_Glyph_Name ((FT_Error (*)(FT_Face face, FT_UInt glyph_index, FT_Pointer buffer, FT_UInt buffer_max))__AROS_GETVECADDR(FreeType2Base,33))
#define FT_Get_Postscript_Name ((const char * (*)(FT_Face face))__AROS_GETVECADDR(FreeType2Base,34))
#define FT_Select_Charmap ((FT_Error (*)(FT_Face face, FT_Encoding encoding))__AROS_GETVECADDR(FreeType2Base,35))
#define FT_Set_Charmap ((FT_Error (*)(FT_Face face, FT_CharMap charmap))__AROS_GETVECADDR(FreeType2Base,36))
#define FT_Get_First_Char ((FT_ULong (*)(FT_Face face, FT_UInt *agindex))__AROS_GETVECADDR(FreeType2Base,37))
#define FT_Get_Next_Char ((FT_ULong (*)(FT_Face face, FT_ULong charcode, FT_UInt *agindex))__AROS_GETVECADDR(FreeType2Base,38))
#define FT_Get_Glyph ((FT_Error (*)(FT_GlyphSlot slot, FT_Glyph *aglyph))__AROS_GETVECADDR(FreeType2Base,39))
#define FT_Glyph_Copy ((FT_Error (*)(FT_Glyph source, FT_Glyph *target))__AROS_GETVECADDR(FreeType2Base,40))
#define FT_Glyph_Transform ((FT_Error (*)(FT_Glyph glyph, FT_Matrix *matrix, FT_Vector *delta))__AROS_GETVECADDR(FreeType2Base,41))
#define FT_Glyph_Get_CBox ((void (*)(FT_Glyph glyph, FT_UInt bbox_mode, FT_BBox *acbox))__AROS_GETVECADDR(FreeType2Base,42))
#define FT_Glyph_To_Bitmap ((FT_Error (*)(FT_Glyph *the_glyph, FT_Render_Mode render_mode, FT_Vector *origin, FT_Bool destroy))__AROS_GETVECADDR(FreeType2Base,43))
#define FT_Done_Glyph ((void (*)(FT_Glyph glyph))__AROS_GETVECADDR(FreeType2Base,44))
#define FT_New_Size ((FT_Error (*)(FT_Face face, FT_Size *asize))__AROS_GETVECADDR(FreeType2Base,45))
#define FT_Done_Size ((FT_Error (*)(FT_Size size))__AROS_GETVECADDR(FreeType2Base,46))
#define FT_Activate_Size ((FT_Error (*)(FT_Size size))__AROS_GETVECADDR(FreeType2Base,47))
#define FTC_Manager_New ((FT_Error (*)(FT_Library library, FT_UInt max_faces, FT_UInt max_sizes, FT_ULong max_bytes, FTC_Face_Requester requester, FT_Pointer req_data, FTC_Manager *amanager))__AROS_GETVECADDR(FreeType2Base,48))
#define FTC_Manager_Lookup_Face ((FT_Error (*)(FTC_Manager manager, FTC_FaceID face_id, FT_Face *aface))__AROS_GETVECADDR(FreeType2Base,49))
#define FTC_Manager_Lookup_Size ((FT_Error (*)(FTC_Manager manager, FTC_Font font, FT_Face *aface, FT_Size *asize))__AROS_GETVECADDR(FreeType2Base,50))
#define FTC_Node_Unref ((void (*)(FTC_Node node, FTC_Manager manager))__AROS_GETVECADDR(FreeType2Base,51))
#define FTC_ImageCache_New ((FT_Error (*)(FTC_Manager manager, FTC_ImageCache *acache))__AROS_GETVECADDR(FreeType2Base,52))
#define FTC_ImageCache_Lookup ((FT_Error (*)(FTC_ImageCache cache, FTC_ImageType type, FT_UInt gindex, FT_Glyph *aglyph, FTC_Node *anode))__AROS_GETVECADDR(FreeType2Base,53))
#define FTC_SBitCache_New ((FT_Error (*)(FTC_Manager manager, FTC_SBitCache *acache))__AROS_GETVECADDR(FreeType2Base,54))
#define FTC_SBitCache_Lookup ((FT_Error (*)(FTC_SBitCache cache, FTC_ImageType type, FT_UInt gindex, FTC_SBit *ansbit, FTC_Node *anode))__AROS_GETVECADDR(FreeType2Base,55))
#define FTC_Manager_Done ((void (*)(FTC_Manager manager))__AROS_GETVECADDR(FreeType2Base,58))
#define FTC_Manager_Reset ((void (*)(FTC_Manager manager))__AROS_GETVECADDR(FreeType2Base,59))
#define FTC_CMapCache_New ((FT_Error (*)(FTC_Manager manager, FTC_CMapCache *acache))__AROS_GETVECADDR(FreeType2Base,60))
#define FTC_CMapCache_Lookup ((FT_UInt (*)(FTC_CMapCache cache, FTC_FaceID face_id, FT_Int cmap_index, FT_UInt32 char_code))__AROS_GETVECADDR(FreeType2Base,61))
#define FT_MulDiv ((FT_Long (*)(FT_Long a, FT_Long b, FT_Long c))__AROS_GETVECADDR(FreeType2Base,65))
#define FT_MulFix ((FT_Long (*)(FT_Long a, FT_Long b))__AROS_GETVECADDR(FreeType2Base,66))
#define FT_DivFix ((FT_Long (*)(FT_Long a, FT_Long b))__AROS_GETVECADDR(FreeType2Base,67))
#define FT_RoundFix ((FT_Fixed (*)(FT_Fixed a))__AROS_GETVECADDR(FreeType2Base,68))
#define FT_CeilFix ((FT_Fixed (*)(FT_Fixed a))__AROS_GETVECADDR(FreeType2Base,69))
#define FT_FloorFix ((FT_Fixed (*)(FT_Fixed a))__AROS_GETVECADDR(FreeType2Base,70))
#define FT_Vector_Transform ((void (*)(FT_Vector *vector, const FT_Matrix *matrix))__AROS_GETVECADDR(FreeType2Base,71))
#define FT_Matrix_Multiply ((void (*)(const FT_Matrix *a, FT_Matrix *b))__AROS_GETVECADDR(FreeType2Base,72))
#define FT_Matrix_Invert ((FT_Error (*)(FT_Matrix *matrix))__AROS_GETVECADDR(FreeType2Base,73))
#define FT_Sin ((FT_Fixed (*)(FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,74))
#define FT_Cos ((FT_Fixed (*)(FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,75))
#define FT_Tan ((FT_Fixed (*)(FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,76))
#define FT_Atan2 ((FT_Angle (*)(FT_Fixed dx, FT_Fixed dy))__AROS_GETVECADDR(FreeType2Base,77))
#define FT_Angle_Diff ((FT_Angle (*)(FT_Angle angle1, FT_Angle angle2))__AROS_GETVECADDR(FreeType2Base,78))
#define FT_Vector_Unit ((void (*)(FT_Vector *vec, FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,79))
#define FT_Vector_Rotate ((void (*)(FT_Vector *vec, FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,80))
#define FT_Vector_Length ((FT_Fixed (*)(FT_Vector *vec))__AROS_GETVECADDR(FreeType2Base,81))
#define FT_Vector_Polarize ((void (*)(FT_Vector *vec, FT_Fixed *length, FT_Angle *angle))__AROS_GETVECADDR(FreeType2Base,82))
#define FT_Vector_From_Polar ((void (*)(FT_Vector *vec, FT_Fixed length, FT_Angle angle))__AROS_GETVECADDR(FreeType2Base,83))
#define FT_List_Add ((void (*)(FT_List list, FT_ListNode node))__AROS_GETVECADDR(FreeType2Base,84))
#define FT_List_Insert ((void (*)(FT_List list, FT_ListNode node))__AROS_GETVECADDR(FreeType2Base,85))
#define FT_List_Find ((FT_ListNode (*)(FT_List list, void *data))__AROS_GETVECADDR(FreeType2Base,86))
#define FT_List_Remove ((void (*)(FT_List list, FT_ListNode node))__AROS_GETVECADDR(FreeType2Base,87))
#define FT_List_Up ((void (*)(FT_List list, FT_ListNode node))__AROS_GETVECADDR(FreeType2Base,88))
#define FT_List_Iterate ((FT_Error (*)(FT_List list, FT_List_Iterator iterator, void *user))__AROS_GETVECADDR(FreeType2Base,89))
#define FT_List_Finalize ((void (*)(FT_List list, FT_List_Destructor destroy, FT_Memory memory, void *user))__AROS_GETVECADDR(FreeType2Base,90))
#define FT_Outline_New ((FT_Error (*)(FT_Library library, FT_UInt numPoints, FT_Int numContours, FT_Outline *anoutline))__AROS_GETVECADDR(FreeType2Base,91))
#define FT_Outline_New_Internal ((FT_Error (*)(FT_Memory memory, FT_UInt numPoints, FT_Int numContours, FT_Outline *anoutline))__AROS_GETVECADDR(FreeType2Base,92))
#define FT_Outline_Done ((FT_Error (*)(FT_Library library, FT_Outline *outline))__AROS_GETVECADDR(FreeType2Base,93))
#define FT_Outline_Done_Internal ((FT_Error (*)(FT_Memory memory, FT_Outline *outline))__AROS_GETVECADDR(FreeType2Base,94))
#define FT_Outline_Copy ((FT_Error (*)(FT_Outline *source, FT_Outline *target))__AROS_GETVECADDR(FreeType2Base,95))
#define FT_Outline_Translate ((void (*)(FT_Outline *outline, FT_Pos xOffset, FT_Pos yOffset))__AROS_GETVECADDR(FreeType2Base,96))
#define FT_Outline_Transform ((void (*)(FT_Outline *outline, FT_Matrix *matrix))__AROS_GETVECADDR(FreeType2Base,97))
#define FT_Outline_Reverse ((void (*)(FT_Outline *outline))__AROS_GETVECADDR(FreeType2Base,98))
#define FT_Outline_Check ((FT_Error (*)(FT_Outline *outline))__AROS_GETVECADDR(FreeType2Base,99))
#define FT_Outline_Get_CBox ((void (*)(FT_Outline *outline, FT_BBox *acbox))__AROS_GETVECADDR(FreeType2Base,100))
#define FT_Outline_Get_BBox ((FT_Error (*)(FT_Outline *outline, FT_BBox *abbox))__AROS_GETVECADDR(FreeType2Base,101))
#define FT_Outline_Get_Bitmap ((FT_Error (*)(FT_Library library, FT_Outline *outline, FT_Bitmap *abitmap))__AROS_GETVECADDR(FreeType2Base,102))
#define FT_Outline_Render ((FT_Error (*)(FT_Library library, FT_Outline *outline, FT_Raster_Params *params))__AROS_GETVECADDR(FreeType2Base,103))
#define FT_Outline_Decompose ((FT_Error (*)(FT_Outline *outline, const FT_Outline_Funcs *func_interface, void *user))__AROS_GETVECADDR(FreeType2Base,104))
#define FT_Add_Module ((FT_Error (*)(FT_Library library, const FT_Module_Class *clazz))__AROS_GETVECADDR(FreeType2Base,105))
#define FT_Get_Module ((FT_Module (*)(FT_Library library, const char *module_name))__AROS_GETVECADDR(FreeType2Base,106))
#define FT_Remove_Module ((FT_Error (*)(FT_Library library, FT_Module module))__AROS_GETVECADDR(FreeType2Base,107))
#define FT_New_Library ((FT_Error (*)(FT_Memory memory, FT_Library *alibrary))__AROS_GETVECADDR(FreeType2Base,108))
#define FT_Done_Library ((FT_Error (*)(FT_Library library))__AROS_GETVECADDR(FreeType2Base,109))
#define FT_Set_Debug_Hook ((void (*)(FT_Library library, FT_UInt hook_index, FT_DebugHook_Func debug_hook))__AROS_GETVECADDR(FreeType2Base,110))
#define FT_Add_Default_Modules ((void (*)(FT_Library library))__AROS_GETVECADDR(FreeType2Base,111))
#define FT_Get_Renderer ((FT_Renderer (*)(FT_Library library, FT_Glyph_Format format))__AROS_GETVECADDR(FreeType2Base,112))
#define FT_Set_Renderer ((FT_Error (*)(FT_Library library, FT_Renderer renderer, FT_UInt num_params, FT_Parameter *parameters))__AROS_GETVECADDR(FreeType2Base,113))
#define FT_Stream_OpenGzip ((FT_Error (*)(FT_Stream stream, FT_Stream source))__AROS_GETVECADDR(FreeType2Base,114))
#define FT_Get_Sfnt_Table ((void * (*)(FT_Face face, FT_Sfnt_Tag tag))__AROS_GETVECADDR(FreeType2Base,115))

__END_DECLS

#endif /* DEFINES_FREETYPE2_H*/
