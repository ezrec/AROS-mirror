/* vars.h */

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct UtilityBase *UtilityBase;
extern struct Library *DiskfontBase;
extern struct Library *LayersBase;

extern char s[300];

extern struct IClass *areaclass;
extern struct IClass *groupclass;
extern struct IClass *buttonclass;
extern struct IClass *stringclass;
extern struct IClass *labelclass;
extern struct IClass *sysimageclass;
extern struct IClass *coolimageclass;
extern struct IClass *cycleclass;
extern struct IClass *propclass;
extern struct IClass *classes[PUIC_NumClasses];

extern struct RastPort temprp;
extern struct SignalSemaphore temprpsem;

extern WORD group_defaultspacingx;
extern WORD group_defaultspacingy;

extern WORD window_defaultspacingl;
extern WORD window_defaultspacingr;
extern WORD window_defaultspacingo;
extern WORD window_defaultspacingu;

extern WORD image_text_spacing;

extern BOOL grouptitles_center;
extern BOOL grouptitles_3d;
extern BOOL popup_usewindow;

extern struct PUI_FrameInfo frameinfo[NUM_FRAMES];
extern struct PUI_RealFrameInfo realframeinfo[NUM_REALFRAMES];

