#ifndef RANDOMLOGO_H
#define RANDOMLOGO_H

#define RA_RANDOMIZE 	1
#define RA_APPICON		2
#define RA_RUNRAPREFS 	3
#define RA_KEYPREFS		4

struct RandomLogoMsg
{
	struct Message msg;
	char	*filename;
	BYTE	code;
	ULONG *data;
	ULONG	*result;
};

extern ULONG randomlogomask;

void InitRandomLogo(void);
void CleanupRandomLogo(void);
ULONG SendRandomLogo(char *logo,BYTE code,ULONG *data);
void HandleRandomLogo(void);

#endif

