/***************************/
/*			   */
/* Localized Text Routines */
/*			   */
/***************************/

#include "typeface.h"

void SetLabelKey(Object *win, Object *gadg, ULONG text)
{
extern char *KeyLabelTable;
char *uscore;

  if ((uscore = strchr(GetString(text),'_')) != NULL)
    GadgetKey(win,gadg,KeyLabelTable+(uscore[1]*2));
}

void SetupMenus(struct NewMenu *menus)
{
  while (menus->nm_Type != NM_END)
  {
    switch ((IPTR)(menus->nm_UserData))
    {
      case ID_PROJECTMENU:
	menus->nm_Label = GetString(msgMenuProjectTitle);
	break;
      case ID_OPEN:
	menus->nm_Label = GetString(msgMenuOpen);
	break;
      case ID_SAVE:
	menus->nm_Label = GetString(msgMenuSave);
	break;
      case ID_CHANGEDIR:
	menus->nm_Label = GetString(msgMenuChangeDir);
	break;
      case ID_PREVIEW:
	menus->nm_Label = GetString(msgMenuPreview);
	break;
      case ID_PREFS:
	menus->nm_Label = GetString(msgMenuPrefs);
	break;
      case ID_ABOUT:
	menus->nm_Label = GetString(msgMenuAbout);
	break;
      case ID_QUIT:
	menus->nm_Label = GetString(msgMenuQuit);
	break;

      case ID_FONTMENU:
	menus->nm_Label = GetString(msgMenuFontTitle);
	break;
      case ID_EDITFONT:
	menus->nm_Label = GetString(msgMenuEditFont);
	break;
      case ID_EDITCHAR:
	menus->nm_Label = GetString(msgMenuEditChar);
	break;
      case ID_MOVEFL:
      case ID_MOVEL:
	menus->nm_Label = GetString(msgMenuMoveLeft);
	break;
      case ID_MOVEFR:
      case ID_MOVER:
	menus->nm_Label = GetString(msgMenuMoveRight);
	break;
      case ID_MOVEFU:
      case ID_MOVEU:
	menus->nm_Label = GetString(msgMenuMoveUp);
	break;
      case ID_MOVEFD:
      case ID_MOVED:
	menus->nm_Label = GetString(msgMenuMoveDown);
	break;
      case ID_RESETKERN:
	menus->nm_Label = GetString(msgMenuResetKern);
	break;
      case ID_CASCADE:
 	menus->nm_Label = GetString(msgMenuCascade);
	break;

      case ID_CHARMENU:
	menus->nm_Label = GetString(msgMenuCharTitle);
	break;
      case ID_UNDO:
	menus->nm_Label = GetString(msgMenuUndo);
	break;
      case ID_CUT:
	menus->nm_Label = GetString(msgMenuCut);
	break;
      case ID_COPY:
	menus->nm_Label = GetString(msgMenuCopy);
	break;
      case ID_PASTE:
	menus->nm_Label = GetString(msgMenuPaste);
	break;
      case ID_ERASE:
	menus->nm_Label = GetString(msgMenuErase);
	break;
      case ID_FLIPH:
	menus->nm_Label = GetString(msgMenuFlipHoriz);
	break;
      case ID_FLIPV:
	menus->nm_Label = GetString(msgMenuFlipVert);
	break;
      case ID_ZOOMIN:
	menus->nm_Label = GetString(msgMenuZoomIn);
	break;
      case ID_ZOOMOUT:
	menus->nm_Label = GetString(msgMenuZoomOut);
	break;
      case ID_WIDTHC:
	menus->nm_Label = GetString(msgMenuChangeWidth);
	break;
      case ID_KERNING:
	menus->nm_Label = GetString(msgMenuKerning);
	break;

      case ID_PREVIEWMENU:
	menus->nm_Label = GetString(msgMenuPreviewTitle);
	break;
      case ID_UPDATE:
	menus->nm_Label = GetString(msgMenuUpdate);
	break;
      case ID_ALL:
	menus->nm_Label = GetString(msgMenuShowAll);
	break;
      case ID_CLEAR:
	menus->nm_Label = GetString(msgMenuClear);
	break;
    }
    menus++;
  }
}
