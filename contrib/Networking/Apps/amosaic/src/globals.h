/*
#undef IntuitionBase
#undef GfxBase
#undef DOSBase
#undef IconBase
#undef GadToolsBase
#undef AslBase
#undef LayersBase
#undef DiskFontBase
#undef UtilityBase

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct DosLibrary *DOSBase;
extern struct Library *IconBase, *GadToolsBase, *AslBase, *MUIMasterBase,
                      *LayersBase, *DiskFontBase, *DataTypesBase;
extern struct UtilityBase *UtilityBase;
*/
extern struct Library *MUIMasterBase;

extern struct TextAttr ta_norm, ta_h1, ta_h2, ta_h3, ta_h4, ta_h5,
                       ta_h6, ta_bold, ta_ital, ta_fixed, ta_address,
                       ta_listing, ta_plain;
extern struct TextFont *tf_norm, *tf_h1, *tf_h2, *tf_h3, *tf_h4,
                       *tf_h5, *tf_h6, *tf_bold, *tf_ital, *tf_fixed,
                       *tf_address, *tf_listing, *tf_plain;

extern Object * App       ; /* Application object       */
extern Object * WI_Main   ; /* Window object            */
extern Object * BT_Home   ; /* Edit Button object       */
extern Object * BT_Back   ; /* Delete Button object     */
extern Object * BT_Forward ; /* Delete Button object     */
extern Object * BT_Open   ; /* Save Button object       */
extern Object * BT_Reload   ; /* Save Button object       */
extern Object * BT_Quit   ; /* Save Button object       */
extern Object * BT_Amiga   ;
extern Object * BT_AMosaic ;
extern Object * TX_Title  ; /* Title */
extern Object * TX_URL    ; /* URL   */
extern Object * TX_Dest   ; /* Dest URL   */
extern Object * GG_Progress ;
extern Object * SC_Progress ;
extern Object * TX_Progress ;
extern Object * PG_Bottom ;
extern Object * BT_Interrupt;
// extern Object * SB_Vert  ; /* scrollbar (Handled internaly) */
// extern Object * SB_Horiz ; /* scrollbar (Handled internaly) */
extern Object *BT_Search, *STR_Search;
extern Object *G_Norm;
extern Object *G_Virt;
extern Object *G_Forms;

extern Object * HTML_Gad ;
extern Object *WI_About;
extern Object *WI_Wait;
extern Object *WI_ImagesReady;

extern struct MUI_CustomClass *MUI_HTMLGadClass;
extern struct MUI_CustomClass *MUI_NewGroupClass;
extern Class *NewGroupClass;
extern Class *HTMLGadClass;
extern Class *SpinnerClass;
extern Class *BoingTransferClass;
extern Class *TextEditFieldClass;

#define ID_DISPLAY 1
#define ID_EDIT    2
#define ID_DELETE  3
#define ID_SAVE    4

#ifndef LIBRARIES_IFFPARSE_H
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

extern struct Hook interrupt_hook, goback_hook, goforward_hook, debug_hook;

extern struct MUI_RenderInfo *GlobalRenderInfo;
