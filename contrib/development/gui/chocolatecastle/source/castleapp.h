/* CastleApp class header */

/* Code generated with ChocolateCastle 0.1 */
/* written by Grzegorz "Krashan" Kraszewski <krashan@teleinfo.pb.bialystok.pl> */

extern struct MUI_CustomClass *CastleAppClass;

struct MUI_CustomClass *CreateCastleAppClass(void);
void DeleteCastleAppClass(void);


struct CastleAppData
{
	Object *ProjectSelectRadio;
};


#define CAAM_Generate           0x6EDA9475
#define CAAM_RemoveGenerator    0x6EDA9476


struct opCAA_Generate
{
	ULONG MethodID;
	ULONG ProjectType;
};

struct CAAP_RemoveGenerator
{
	ULONG MethodID;
	Object *Generator;
};

