/* Wazp3D - Alain THELLIER - Paris - FRANCE 							*/
/* Adaptation to AROS from Matthias Rustler							*/
/* Code clean-up and library enhancements from Gunther Nikl					*/
/* LICENSE: GNU General Public License (GNU GPL) for this file				*/

/* OpenGL-wrapper: functions definitions								*/

void HARD3D_AllocImageBuffer(void *gc,unsigned short large,unsigned short high);
void HARD3D_AllocZbuffer(void *gc,unsigned short large,unsigned short high);
void HARD3D_ClearImageBuffer(void *gc,unsigned short x,unsigned short y,unsigned short large,unsigned short high);
void HARD3D_ClearZBuffer(void *gc,float fz);
void HARD3D_CreateTexture(void *gc,void *gt,unsigned char *pt,unsigned short large,unsigned short high,unsigned short bits,unsigned char UseMipMap,unsigned char UseFiltering);
void HARD3D_Current(void *gc);
void HARD3D_DoUpdate(void *gc);
void HARD3D_DrawPrimitive(void *hc,struct point3D *P,unsigned long Pnb,unsigned long primitive,unsigned char UsePersp);
void HARD3D_End(void *gc);
void HARD3D_Flush(void *gc);
void HARD3D_Fog(void *gc,unsigned char FogMode,float FogZmin,float FogZmax,float FogDensity,unsigned char *FogRGBA);
void HARD3D_FreeTexture(void *gc,void *gt);
void HARD3D_ReadZSpan(void *gc, unsigned short x, unsigned short y,unsigned long n, double *dz);
void HARD3D_SetBackColor(void *gc,unsigned char  *RGBA);
void HARD3D_SetBitmap(void *gc,void *bm,unsigned char *Image8,unsigned long format,unsigned short x,unsigned short y,unsigned short large,unsigned short high);
void HARD3D_SetClipping(void *gc,unsigned short xmin,unsigned short xmax,unsigned short ymin,unsigned short ymax);
void HARD3D_SetCulling(void *gc,unsigned char CullingMode);
void HARD3D_SetCurrentColor(void *gc,unsigned char  *RGBA);
void HARD3D_SetDrawFunctions(void *hc,void *gt,unsigned char ZMode,unsigned char BlendMode,unsigned char TexEnvMode,unsigned char UseGouraud,unsigned char UseFog,unsigned char PerspMode,unsigned char *EnvRGBA);
void HARD3D_SetDrawStates(void *gc,void *gt,unsigned char ZMode,unsigned char BlendMode,unsigned char TexEnvMode,unsigned char UseGouraud,unsigned char UseFog,unsigned char PerspMode);
void HARD3D_SetEnvColor(void *gc,unsigned char  *RGBA);
void HARD3D_SetPointSize(void *gc,unsigned short PointSize);
void HARD3D_Start(void *gc);
void HARD3D_UpdateTexture(void *gc,void *gt,unsigned char *pt,unsigned short large,unsigned short high,unsigned char bits);
void HARD3D_WriteZSpan(void *gc, unsigned short x, unsigned short y,unsigned long n, double *dz, unsigned char *mask);

/*=============================================================*/
struct HARD3D_context{
	void *hdc;				/* handle to Display Context  */
	void *hglrc;			/* handle to GL Rendering Context */
	void *instance;			/* handle to window-instance */
	void *glwnd;			/* handle to window for GL */
	void *winbm;			/* off-screen Windows' bitmap */
	void *RGBA32;			/* off-screen Windows' bitmap data */
	void *Image8;
	void *awin;				/* Amiga's current window at startup */
	void *overwin;			/* Amiga's window created for Aros-Mesa overlay */
	void *hackrastport;		/* Copy of current window->rport */
	unsigned short large,high;			/* to convert origin to lower-left corner */
	unsigned char glstarted;			/* OpenGL ready ? */
	unsigned char registered;			/* window-instance registered ?*/
	unsigned char UseAntiAlias;			/* enable POLYGON/POINT/LINE_SMOOTH  */
	unsigned char UseDoubleBuffer;		/* use OS_SwapBuffers else not    */
	unsigned char UseOverlay;			/* create a sub-window("overlay") else use WinUAE's window for GL context */
	unsigned char DebugHard;			/* for debugging */
	};
/*=============================================================*/
struct HARD3D_texture{
	unsigned long gltex;	/* GL texture */
	};
/*=============================================================*/
