void InitWindow ()
{
  int wx, wy;
  
  scr = LockPubScreen(NULL);
  if (!scr) cleanup("Can't lock pub screen!");

  if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) < 15)
  {
    cleanup("Need a hi or true color screen!");
  }
  
  wx = (scr->Width - WIDTH - scr->WBorLeft - scr->WBorRight) / 2;
  wy = (scr->Height - HEIGHT - scr->WBorBottom - scr->WBorTop - scr->Font->ta_YSize - 1) / 2;
  
  win = OpenWindowTags(0, WA_PubScreen, (IPTR)scr,
    	    	    	  WA_Title, (IPTR)"Bomber",
    	    	    	  WA_CloseGadget, TRUE,
			  WA_DragBar, TRUE,
			  WA_DepthGadget, TRUE,
			  WA_Left, wx,
			  WA_Top, wy,
			  WA_InnerWidth, WIDTH,
			  WA_InnerHeight, HEIGHT,
			  WA_Activate, TRUE,
			  WA_RMBTrap, TRUE,
			  WA_SimpleRefresh, TRUE,
			  WA_NoCareRefresh, TRUE,
			  WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
			  TAG_DONE);

  if (!win) cleanup("Can't open window!");

}
