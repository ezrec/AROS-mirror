#ifndef REQATTACK_H
#define REQATTACK_H

struct DeveloperAttack
{
	char	*da_Logo; /* file name of logo to display */
	WORD	da_StartButton; /* number of button to highlight at startup */
	char  *da_AppIcon; /* file name of the appicon (without .info!) */
	ULONG	da_Flags; /* set of flags described below */
};

#define DA_FLAGS_LOGOFROMMEM 1 /* logo is an address to a loaded logo file
											 in ReqAttack RAIM format - other types
											 not allowed! */
/* more flags will come in future */

/*
**	Please note that the button number of da_StartButton is NOT
**	counted same as the standard EasyRequestArgs return value!
**	Here buttons are numbered 0, 1, ... ,n !
**
** Please also note that setting da_Logo to "" will be taken as
** setting it to NULL - this means the dev logo will be IGNORED.
** The same goes to da_AppIcon and all possible strings.
*/

#endif
