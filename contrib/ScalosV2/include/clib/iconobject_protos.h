struct Iconobject *	NewIconObject( APTR Name, APTR Taglist );
void						DisposeIconObject( struct Iconobject *iconobject );
struct Iconobject *	GetDefIconObject( ULONG IconType, APTR TagList );
void						PutIconObject( struct Iconobject *iconobject, APTR path, APTR TagList );
LONG						IsIconName( APTR filename );
struct Iconobject *	Convert2IconObject( struct DiskObject *diskobject );	// private!
