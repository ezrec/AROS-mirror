void InitWindow ()
{
  win = OpenWindowTags(0, WA_Title, "Bomber",
    	    	    	  WA_CloseGadget, TRUE,
			  WA_DragBar, TRUE,
			  WA_DepthGadget, TRUE,
			  WA_InnerWidth, WIDTH,
			  WA_InnerHeight, HEIGHT,
			  WA_Activate, TRUE,
			  WA_RMBTrap, TRUE,
			  WA_SimpleRefresh, TRUE,
			  WA_NoCareRefresh, TRUE,
			  WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
			  TAG_DONE);

  if (!win) cleanup("Can't open window!");

  if (GetBitMapAttr(win->RPort->BitMap, BMA_DEPTH) < 15)
  {
    cleanup("Need a hi or true color screen!");
  }
}