#include "includes.h"

struct Library *MUIMasterBase, *DataTypesBase;

struct TextAttr   *ta, ta_norm, ta_h1, ta_h2, ta_h3, ta_h4, ta_h5, ta_h6,
                       ta_bold, ta_ital, ta_fixed, ta_address, ta_listing,
                       ta_plain;
struct TextFont   *tf, *tf_norm, *tf_h1, *tf_h2, *tf_h3, *tf_h4, *tf_h5,
                       *tf_h6, *tf_bold, *tf_ital, *tf_fixed, *tf_address,
                       *tf_listing, *tf_plain;


Object * App        ; /* Application object       */
Object * WI_Main    ; /* Window object            */
Object * BT_Home    ; /* Edit Button object       */
Object * BT_Back    ; /* Delete Button object     */
Object * BT_Forward ; /* Delete Button object     */
Object * BT_Open    ; /* Save Button object       */
Object * BT_Reload  ; /* Save Button object       */
Object * BT_Quit    ; /* Save Button object       */
Object * BT_Amiga   ;
Object * BT_AMosaic ;
Object * TX_Title   ;  /* Title */
Object * TX_URL     ; /* URL   */
Object * TX_Dest    ; /* Dest URL  */
Object * GG_Progress;
Object * SC_Progress;
Object * TX_Progress;
Object * PG_Bottom  ;
Object * BT_Interrupt;
// Object * SB_Vert ; /* scrollbar (Handled internaly) */
// Object * SB_Horiz; /* scrollbar (Handled internaly) */
Object * HTML_Gad   ; /* the magic MUI HTML gadget */
Object *BT_Search, *STR_Search;
Object *G_Norm;
Object *G_Virt;
Object *G_Forms;
Object *WI_About;
Object *WI_Wait;
Object *WI_ImagesReady;

struct MUI_CustomClass *MUI_HTMLGadClass;
struct MUI_CustomClass *MUI_NewGroupClass;
Class *NewGroupClass;
Class *HTMLGadClass;
Class *SpinnerClass;
Class *BoingTransferClass;
Class *TextEditFieldClass;

struct Hook interrupt_hook, goback_hook, goforward_hook, debug_hook;
