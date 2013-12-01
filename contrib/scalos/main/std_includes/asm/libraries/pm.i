;TAG_USER EQU $80000000

PM_SubMenuTimer			EQU TAG_USER+1	; Delay before opening submenus
PM_RecessSelected		EQU TAG_USER+2	; Recess selected item
PM_WideSelectBar		EQU TAG_USER+3	; Wide selectbar

PM_Menu				EQU (TAG_USER+4)	; (struct PopupMenu *) Pointer to menulist initialized by MakeMenu()		*/
PM_Top				EQU (TAG_USER+12)	; (LONG) Top (Y) position							*/
PM_Left				EQU (TAG_USER+13)	; (LONG) Left (X) position							*/
PM_Code				EQU (TAG_USER+14)	; (UWORD) Obsolete.								*/
PM_Right			EQU (TAG_USER+15)	; (LONG) X position relative to right edge					*/
PM_Bottom			EQU (TAG_USER+16)	; (LONG) Y position relative to bottom edge					*/
PM_MinWidth			EQU (TAG_USER+17)	; (LONG) Minimum width								*/
PM_MinHeight			EQU (TAG_USER+18)	; (LONG) Minimum height							*/
PM_ForceFont			EQU (TAG_USER+19)	; (struct TextFont *tf) Use this font instead of user preferences.		*/
PM_PullDown			EQU (TAG_USER+90)	; (BOOL) Turn the menu into a pulldown menu.					*/
PM_MenuHandler			EQU (TAG_USER+91)	; (struct Hook *) Hook that is called for each selected item, after the	*/
							; menu has been closed. This tag turns on MultiSelect.				*/
PM_AutoPullDown			EQU (TAG_USER+92)	; (BOOL) Automatic pulldown menu. (PM_FilterIMsg only)				*/
PM_LocaleHook			EQU (TAG_USER+93)	; (struct Hook *) Locale "GetString()" hook. (Not yet implemented)		*/
PM_CenterScreen			EQU (TAG_USER+94)	; (BOOL) Center menu on the screen						*/
PM_UseLMB			EQU (TAG_USER+95)	; (BOOL) Left mouse button should be used to select an item			*/
							; (right button selects multiple items)					*/
PM_DRIPensOnly			EQU (TAG_USER+96)	; (BOOL) Only use the screen's DRI pens, revert to system images if necessary.	*/
							; Use with care as it overrides the user's prefs!				*/
PM_HintBox			EQU (TAG_USER+97)	; (BOOL) Close the menu when the mouse is moved.				*/


; Tags passed to MakeItem

PM_Title			EQU (TAG_USER+20)	; (STRPTR) Item title								*/
PM_UserData			EQU (TAG_USER+21)	; (void *) Anything, returned by OpenPopupMenu when this item is selected	*/
PM_ID				EQU (TAG_USER+22)	; (ULONG) ID number, if you want an easy way to access this item later		*/
PM_CommKey			EQU (TAG_USER+47)	; (char) Keyboard shortcut for this item.					*/
PM_TitleID			EQU (TAG_USER+49)	; (ULONG) Locale string ID 							*/
PM_Object			EQU (TAG_USER+43)	; (Object *) BOOPSI object with the abillity to render this item		*/

; Submenu & Layout tags */
; PM_Sub & PM_Members are mutally exclusive */
PM_Sub				EQU (TAG_USER+23)	; (PopupMenu *) Pointer to submenu list (from PM_MakeMenu)			*/
PM_Members			EQU (TAG_USER+65)	; (PopupMenu *) Members for this group (list created by PM_MakeMenu)		*/
PM_LayoutMode			EQU (TAG_USER+64)	; (ULONG) Layout method (PML_Horizontal / PML_Vertical)			*/

; Text attributes */
PM_FillPen			EQU (TAG_USER+26)	; (BOOL) Make the item appear in FILLPEN					*/
PM_Italic			EQU (TAG_USER+29)	; (BOOL) Italic text								*/
PM_Bold				EQU (TAG_USER+30)	; (BOOL) Bold text								*/
PM_Underlined			EQU (TAG_USER+31)	; (BOOL) Underlined text							*/
PM_ShadowPen			EQU (TAG_USER+34)	; (BOOL) Draw text in SHADOWPEN						*/
PM_ShinePen			EQU (TAG_USER+35)	; (BOOL) Draw text in SHINEPEN							*/
PM_Centre			EQU (TAG_USER+36)	; (BOOL) Center the text of this item						*/
PM_Center			EQU PM_Centre	; American version... */
PM_TextPen			EQU (TAG_USER+45)	; (ULONG) Pen number for text colour of this item				*/
PM_Shadowed			EQU (TAG_USER+48)	; (BOOL) Draw a shadow behind the text						*/

; Other item attributes */
PM_TitleBar			EQU (TAG_USER+32)	; (BOOL) Horizontal separator bar						*/
PM_WideTitleBar			EQU (TAG_USER+33)	; (BOOL) Same as above, but full width						*/
PM_NoSelect			EQU (TAG_USER+25)	; (BOOL) Make the item unselectable (without visual indication)		*/
PM_Disabled			EQU (TAG_USER+38)	; (BOOL) Disable an item							*/
PM_Hidden			EQU (TAG_USER+63)	; (BOOL) This item is not to be drawn (nor used in the layout process)		*/

; Images & Icons */
PM_ImageSelected		EQU (TAG_USER+39)	; (struct Image *) Image when selected, title will be rendered on top it	*/
PM_ImageUnselected		EQU (TAG_USER+40)	; (struct Image *) Image when unselected					*/
PM_IconSelected			EQU (TAG_USER+41)	; (struct Image *) Icon when selected						*/
PM_IconUnselected		EQU (TAG_USER+42)	; (struct Image *) Icon when unselected					*/

; Check/MX items */
PM_Checkit			EQU (TAG_USER+27)	; (BOOL) Leave space for a checkmark						*/
PM_Checked			EQU (TAG_USER+28)	; (BOOL) Make this item is checked						*/
PM_AutoStore			EQU (TAG_USER+44)	; (BOOL *) Pointer to BOOL reflecting the current state of the item		*/
PM_Exclude			EQU (TAG_USER+37)	; (PM_IDLst *) Items to unselect or select when this gets selected		*/
PM_ExcludeShared		EQU (TAG_USER+101)	; (BOOL) Used if the list is shared between two or more items			*/
PM_Toggle			EQU (TAG_USER+100)	; (BOOL) Enable/disable toggling of check/mx items. Default: TRUE		*/

; Dynamic construction/destruction */
PM_SubConstruct			EQU (TAG_USER+61)	; (struct Hook *) Constructor hook for submenus. Called before menu is opened.	*/
PM_SubDestruct			EQU (TAG_USER+62)	; (struct Hook *) Destructor hook for submenus. Called after menu has closed.	*/

; Special/misc. stuff */
PM_UserDataString		EQU (TAG_USER+46)	; (STRPTR) Allocates memory and copies the string to UserData.			*/
PM_Flags			EQU (TAG_USER+24)	; (UlONG) For internal use							*/
PM_ColourBox			EQU (TAG_USER+60)	; (UlONG) Filled rectangle (for palettes etc.)					*/
PM_ColorBox			EQU PM_ColourBox	; For Americans... */

; Tags passed to MakeMenu

PM_Item				EQU TAG_USER+50	; Item pointer from MakeItem

; Tags passed to MakeIDList

PM_ExcludeID			EQU TAG_USER+55	; ID number of menu to deselect when this gets selected
PM_IncludeID			EQU TAG_USER+56	; ID number of menu to select when this gets selected
PM_ReflectID			EQU TAG_USER+57	; ID number of menu that should reflect the state of this one
PM_InverseID			EQU TAG_USER+58	; ID number of menu to inverse reflect the state of this one

; Built-in Images

IMG_CHECKMARK			EQU $10		; Checkmark		\/
IMG_RIGHTARROW			EQU $0C		; SysIClass Arrow	|>|
IMG_MXIMAGE			EQU $0F		; SysIClass MX		(*)
IMG_CHECKIMAGE			EQU $0E		; SysIClass Checkbox	| |
IMG_AMIGAKEY			EQU $11		; Amiga Key Image	|A|

IMG_BULLET_A			EQU $8000	; bullet		·
IMG_BULLET_B			EQU $8100	; 3d bullet		·
IMG_ARROW_A			EQU $8200	; Arrow		>
IMG_ARROW_B			EQU $8300	; 3d arrow		>
IMG_ARROW_C			EQU $8400	; Arrow		->

