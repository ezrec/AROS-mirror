#define PUIF_Group_SameWidth				1
#define PUIF_Group_SameHeight				2
#define PUIF_Group_IsRoot					4
#define PUIF_Group_IsHoriz					8
#define PUIF_Group_PageMode				16

struct groupdata
{
	struct MinList	childs;
	Object			*activeobj;
	Object			*activepageobj;
	ULONG				flags;
	WORD				numchilds;
	WORD				spacing;
	WORD				activepage;
};

