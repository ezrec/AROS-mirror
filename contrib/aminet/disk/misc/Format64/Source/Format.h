
/* Format.h */

typedef struct DriveLayout
{
   ULONG lowCyl,highCyl;
   ULONG surfaces;
   ULONG BPT;
   ULONG blockSize;
   ULONG unit;
   ULONG flags;
   ULONG memType;
   char  devName[256];
} DriveLayout;

typedef enum prepResult {eOK,eCancel,eQuit} prepResult;

/* Prototypes for functions defined in Format.c */
int main(void);
void formatVolume(BPTR *volumeLock,
		  char *volumeName,
		  char *newName,
		  ULONG ffs,
		  BOOL quick,
		  BOOL verify,
		  BOOL icon,
		  char *statString);
BOOL doFullFormat(DriveLayout *layout,char *statString,char *devName,
		     struct IOExtTD *io1);
BOOL askAreYouSure(char *volumeName,
		   BOOL truncColon);
BOOL alertIsWriteProtected(char *devName);
void printError(char *first,
		char *second,
		char *third);
void getVolumeName(char *name,
		   struct WBArg *argList,
		   UWORD disk);
void cleanup(ULONG err);
prepResult getPrepInput(void);
BOOL volumeToDevName(BPTR volumeLock,
		     char *dev,
		     DriveLayout *layout);
struct IOExtTD *OpenDrive(char *driveDevName,
			  ULONG unit,
			  ULONG flags);
void CloseDrive(struct IOExtTD *diskRequest);
BSTR makeBSTR(char *in,
	      char *out);
void parseArgs(char *drive,
	       char *newName,
	       BOOL *ffs,
			 BOOL *intl,
	       BOOL *icons,
	       BOOL *quick,
	       BOOL *verify);
BOOL updateStatWindow(char *string,
		      UWORD percent);

