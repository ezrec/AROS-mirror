-- tags for mui.filerequest
mui.ASLFR_Window= -2146959358             -- struct Window *
mui.ASLFR_Screen= -2146959320             -- struct Screen *
mui.ASLFR_PubScreenName= -2146959319      -- STRPR
mui.ASLFR_PrivateIDCMP= -2146959318       -- BOOL
mui.ASLFR_IntuiMsgFunc= -2146959290       -- struct Hook *
mui.ASLFR_SleepWindow= -2146959317        -- BOOL
mui.ASLFR_UserData= -2146959308           -- APTR
mui.ASLFR_PopToFront= -2146959229         -- BOOL
mui.ASLFR_Activate= -2146959228           -- BOOL
mui.ASLFR_TextAttr= -2146959309           -- struct TextAttr *
mui.ASLFR_Locale= -2146959310             -- struct Locale *
mui.ASLFR_TitleText= -2146959359          -- STRPTR
mui.ASLFR_PositiveText= -2146959342       -- STRPTR
mui.ASLFR_NegativeText= -2146959341       -- STRPTR
mui.ASLFR_InitialLeftEdge= -2146959357    -- WORD
mui.ASLFR_InitialTopEdge= -2146959356     -- WORD
mui.ASLFR_InitialWidth= -2146959355       -- WORD
mui.ASLFR_InitialHeight= -2146959354      -- WORD
mui.ASLFR_InitialFile= -2146959352        -- STRPTR
mui.ASLFR_InitialDrawer= -2146959351      -- STRPTR
mui.ASLFR_InitialPattern= -2146959350     -- STRPTR
mui.ASLFR_InitialShowVolumes= -2146959230 -- BOOL
mui.ASLFR_Flags1= -2146959340             -- ULONG
mui.ASLFR_Flags2= -2146959338             -- ULONG
mui.ASLFR_DoSaveMode= -2146959316         -- BOOL
mui.ASLFR_DoMultiSelect= -2146959315      -- BOOL
mui.ASLFR_DoPatterns= -2146959314         -- BOOL
mui.ASLFR_DrawersOnly= -2146959313        -- BOOL
mui.ASLFR_FilterFunc= -2146959311         -- struct Hook *
mui.ASLFR_RejectIcons= -2146959300        -- BOOL
mui.ASLFR_RejectPattern= -2146959299      -- UBYTE *
mui.ASLFR_AcceptPattern= -2146959298      -- UBYTE *
mui.ASLFR_FilterDrawers= -2146959297      -- BOOL
mui.ASLFR_HookFunc= -2146959353           -- struct Hook *
mui.ASLFR_SetSortBy= -2146959236          -- ULONG
-- mui.ASLFR_GetSortBy= -2146959235 
mui.ASLFR_SetSortDrawers= -2146959234     -- ULONG
-- mui.ASLFR_GetSortDrawers= -2146959233 
mui.ASLFR_SetSortOrder= -2146959232       -- ULONG
-- mui.ASLFR_GetSortOrder= -2146959231 

-- Flags for ASLFR_Flags1
mui.FRF_FILTERFUNC= 128 
mui.FRF_INTUIFUNC= 64 
mui.FRF_DOSAVEMODE= 32 
mui.FRF_PRIVATEIDCMP= 16 
mui.FRF_DOMULTISELECT= 8 
mui.FRF_DOPATTERNS= 1 

-- Flags for ASLSM_Flags2
mui.FRF_DRAWERSONLY= 1 
mui.FRF_FILTERDRAWERS= 2 
mui.FRF_REJECTICONS= 4 

-- Sort criteria for ASLFR_SetSortBy/ASLFR_GetSortBy tags
mui.ASLFRSORTBY_Name= 0 
mui.ASLFRSORTBY_Date= 1 
mui.ASLFRSORTBY_Size= 2 

-- Drawer placement for ASLFR_SetSortDrawers/ASLFR_GetSortDrawers tags
mui.ASLFRSORTDRAWERS_First= 0 
mui.ASLFRSORTDRAWERS_Mix= 1 
mui.ASLFRSORTDRAWERS_Last= 2 

-- Sort order for the ASLFR_SetSortOrder/ASLFR_GetSortOrder tags
mui.ASLFRSORTORDER_Ascend= 0 
mui.ASLFRSORTORDER_Descend= 1 




--[[
Font requester (currently not supported by Zulu)

mui.ASLFO_Window= -2146959358 
mui.ASLFO_Screen= -2146959320 
mui.ASLFO_PubScreenName= -2146959319 
mui.ASLFO_PrivateIDCMP= -2146959318 
mui.ASLFO_IntuiMsgFunc= -2146959290 
mui.ASLFO_SleepWindow= -2146959317 
mui.ASLFO_UserData= -2146959308 
mui.ASLFO_PopToFront= -2146959229 
mui.ASLFO_Activate= -2146959228 
mui.ASLFO_TextAttr= -2146959309 
mui.ASLFO_Locale= -2146959310 
mui.ASLFO_TitleText= -2146959359 
mui.ASLFO_PositiveText= -2146959342 
mui.ASLFO_NegativeText= -2146959341 
mui.ASLFO_InitialLeftEdge= -2146959357 
mui.ASLFO_InitialTopEdge= -2146959356 
mui.ASLFO_InitialWidth= -2146959355 
mui.ASLFO_InitialHeight= -2146959354 
mui.ASLFO_InitialName= -2146959350 
mui.ASLFO_InitialSize= -2146959349 
mui.ASLFO_InitialStyle= -2146959348 
mui.ASLFO_InitialFlags= -2146959347 
mui.ASLFO_InitialFrontPen= -2146959346 
mui.ASLFO_InitialBackPen= -2146959345 
mui.ASLFO_InitialDrawMode= -2146959301 
mui.ASLFO_Flags= -2146959340 
mui.ASLFO_DoFrontPen= -2146959316 
mui.ASLFO_DoBackPen= -2146959315 
mui.ASLFO_DoStyle= -2146959314 
mui.ASLFO_DoDrawMode= -2146959313 
mui.ASLFO_SampleText= -2146959227 
mui.ASLFO_FixedWidthOnly= -2146959312 
mui.ASLFO_MinHeight= -2146959344 
mui.ASLFO_MaxHeight= -2146959343 
mui.ASLFO_FilterFunc= -2146959311 
mui.ASLFO_HookFunc= -2146959353 
mui.ASLFO_MaxFrontPen= -2146959294 
mui.ASLFO_MaxBackPen= -2146959293 
mui.ASLFO_ModeList= -2146959339 
mui.ASLFO_FrontPens= -2146959296 
mui.ASLFO_BackPens= -2146959295 
mui.FOB_DOFRONTPEN= 0 
mui.FOB_DOBACKPEN= 1 
mui.FOB_DOSTYLE= 2 
mui.FOB_DODRAWMODE= 3 
mui.FOB_FIXEDWIDTHONLY= 4 
mui.FOB_PRIVATEIDCMP= 5 
mui.FOB_INTUIFUNC= 6 
mui.FOB_FILTERFUNC= 7 
mui.FOF_DOFRONTPEN= 1 
mui.FOF_DOBACKPEN= 2 
mui.FOF_DOSTYLE= 4 
mui.FOF_DODRAWMODE= 8 
mui.FOF_FIXEDWIDTHONLY= 16 
mui.FOF_PRIVATEIDCMP= 32 
mui.FOF_INTUIFUNC= 64 
mui.FOF_FILTERFUNC= 128 
]]





--[[
Screenmode requester (currently not supported by Zulu)

mui.ASLSM_Window= -2146959358 
mui.ASLSM_Screen= -2146959320 
mui.ASLSM_PubScreenName= -2146959319 
mui.ASLSM_PrivateIDCMP= -2146959318 
mui.ASLSM_IntuiMsgFunc= -2146959290 
mui.ASLSM_SleepWindow= -2146959317 
mui.ASLSM_UserData= -2146959308 
mui.ASLSM_PopToFront= -2146959229 
mui.ASLSM_Activate= -2146959228 
mui.ASLSM_TextAttr= -2146959309 
mui.ASLSM_Locale= -2146959310 
mui.ASLSM_TitleText= -2146959359 
mui.ASLSM_PositiveText= -2146959342 
mui.ASLSM_NegativeText= -2146959341 
mui.ASLSM_InitialLeftEdge= -2146959357 
mui.ASLSM_InitialTopEdge= -2146959356 
mui.ASLSM_InitialWidth= -2146959355 
mui.ASLSM_InitialHeight= -2146959354 
mui.ASLSM_InitialDisplayID= -2146959260 
mui.ASLSM_InitialDisplayWidth= -2146959259 
mui.ASLSM_InitialDisplayHeight= -2146959258 
mui.ASLSM_InitialDisplayDepth= -2146959257 
mui.ASLSM_InitialOverscanType= -2146959256 
mui.ASLSM_InitialAutoScroll= -2146959255 
mui.ASLSM_InitialInfoOpened= -2146959254 
mui.ASLSM_InitialInfoLeftEdge= -2146959253 
mui.ASLSM_InitialInfoTopEdge= -2146959252 
mui.ASLSM_DoWidth= -2146959251 
mui.ASLSM_DoHeight= -2146959250 
mui.ASLSM_DoDepth= -2146959249 
mui.ASLSM_DoOverscanType= -2146959248 
mui.ASLSM_DoAutoScroll= -2146959247 
mui.ASLSM_PropertyFlags= -2146959246 
mui.ASLSM_PropertyMask= -2146959245 
mui.ASLSM_MinWidth= -2146959244 
mui.ASLSM_MaxWidth= -2146959243 
mui.ASLSM_MinHeight= -2146959242 
mui.ASLSM_MaxHeight= -2146959241 
mui.ASLSM_MinDepth= -2146959240 
mui.ASLSM_MaxDepth= -2146959239 
mui.ASLSM_FilterFunc= -2146959238 
mui.ASLSM_CustomSMList= -2146959237
]]

