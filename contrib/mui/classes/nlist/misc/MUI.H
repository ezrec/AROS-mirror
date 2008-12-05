
#ifndef MUI_MUI_H
#define MUI_MUI_H

#ifndef LONG_MAX
#define    LONG_MAX    0x7fffffff    /* max value for a long */
#endif
#ifndef LONG_MIN
#define    LONG_MIN    0x80000000    /* min value for a long */
#endif
#ifndef ULONG_MAX
#define    ULONG_MAX    0xffffffffU    /* max value for an unsigned long */
#endif

/*

#define MUIM_GetConfigItem                  0x80423edb
#define MUIM_Settingsgroup_ConfigToGadgets  0x80427043
#define MUIM_Settingsgroup_GadgetsToConfig  0x80425242
#define MUIM_Dataspace_Find                 0x8042832c

struct MUIP_GetConfigItem                   { ULONG MethodID; ULONG id; ULONG *storage; };
struct MUIP_Settingsgroup_ConfigToGadgets   { ULONG MethodID; Object *configdata; };
struct MUIP_Settingsgroup_GadgetsToConfig   { ULONG MethodID; Object *configdata; };
struct MUIP_Dataspace_Find                  { ULONG MethodID; ULONG id; };

#define MUIM_DrawBackground 0x804238ca
struct  MUIP_DrawBackground { ULONG MethodID; LONG left; LONG top; LONG width; LONG height; LONG res1; LONG res2; LONG res3; };

*/

#ifndef MUIM_DoDrag

struct MUI_DragImage
{
  struct BitMap *bm;
  WORD width;
  WORD height;
  WORD touchx;
  WORD touchy;
  ULONG flags;
};

#define MUIM_DoDrag 0x804216BB
struct  MUIP_DoDrag { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; };

#define MUIM_CreateDragImage 0x8042EB6F
struct  MUIP_CreateDragImage { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; };

#define MUIM_DeleteDragImage 0x80423037
struct  MUIP_DeleteDragImage { ULONG MethodID; struct MUI_DragImage *di; };

#endif

#define MUIA_Numeric_CheckAllSizes 0x80421594  /* i.. BOOL */


#define MUIA_Imagedisplay_Spec     0x8042a547 /* V11 isg struct MUI_ImageSpec * */

#define MUIA_Imageadjust_Type      0x80422f2b /* V11 i.. LONG */
/*
#define MUIV_Imageadjust_Type_All 0
#define MUIV_Imageadjust_Type_Image 1
#define MUIV_Imageadjust_Type_Background 2
#define MUIV_Imageadjust_Type_Pen 3
*/

struct MUI_ImageSpec
{
  char buf[64];
};


#define MUIA_List_Prop_Entries  0x8042a8f5
#define MUIA_List_Prop_Visible  0x804273e9
#define MUIA_List_Prop_First    0x80429df3

#define MUIA_Prop_FirstReal     0x804205DC
#define MUIA_Prop_PropRelease   0x80429839

#ifndef MUIA_Prop_DeltaFactor
#define MUIA_Prop_DeltaFactor   0x80427C5E       /* is. LONG */
#endif

#ifndef MUIA_Prop_DoSmooth
#define MUIA_Prop_DoSmooth      0x804236ce /* V4 i.. LONG */
#endif

#ifndef MUIA_Group_Forward
#define MUIA_Group_Forward      0x80421422
#endif

#define nfset(obj,attr,value) SetAttrs(obj,MUIA_Group_Forward,FALSE,attr,value,TAG_DONE)
#define nnfset(obj,attr,value) SetAttrs(obj,MUIA_Group_Forward,FALSE,MUIA_NoNotify,TRUE,attr,value,TAG_DONE)

#define MADF_DRAWFRAME          0x00000800 /* redraw frame */
#define MADF_DRAWALL            0x00000805 /* redraw all */
#define MADF_VISIBLE            0x00004000 /* redraw frame */


#define MUIMRI_IN_VIRTGROUP     0x20000000
#define MUIMRI_IS_VIRTGROUP     0x40000000


#endif /* MUI_MUI_H */
