#define JMP_ABS	0x4EF9
#define JSR_ABS	0x4EB9

struct Wedge
{
	UWORD	Command;
	APTR	Address;
	UWORD	Pad;
};
