
extern LANGSPEC bool GFX_SelectEditFont(struct Tracker_Windows *tvisual);

extern LANGSPEC void GFX_SetWindowTitle(struct Tracker_Windows *tvisual,char *title);
extern LANGSPEC void SetNormalPointer(struct Tracker_Windows *tvisual);
extern LANGSPEC void SetResizePointer(struct Tracker_Windows *tvisual);

extern LANGSPEC ReqType GFX_OpenReq(struct Tracker_Windows *tvisual,int width,int height,char *title);
extern LANGSPEC void GFX_CloseReq(struct Tracker_Windows *tvisual,ReqType reqtype);

extern LANGSPEC int GFX_GetInteger(struct Tracker_Windows *tvisual,ReqType reqtype,char *text,int min,int max);

extern LANGSPEC float GFX_GetFloat(struct Tracker_Windows *tvisual,ReqType reqtype,char *text,float min,float max);

extern LANGSPEC char *GFX_GetString(struct Tracker_Windows *tvisual,ReqType reqtype,char *text);

extern LANGSPEC int GFX_Menu(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	int num_sel,
	char **menutext
);

extern LANGSPEC char *GFX_GetLoadFileName(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	char *dir
);

extern LANGSPEC char *GFX_GetSaveFileName(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	char *dir
);
