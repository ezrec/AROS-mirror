// :ts=4 (Tabsize)

// we need to export our struct instance data because maintask needs its size

struct PopupMenuInst
{
	APTR popupmenu;                     // from popup menu library
	Object *windowobj;
	struct MinList itemlist;
};

struct MenuItemNode
{
	struct MinNode node;
	Object *itemobj;
	APTR item;                          // popup menu library item
};
