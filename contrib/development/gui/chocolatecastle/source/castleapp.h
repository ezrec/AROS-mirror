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


#define CAAM_Generate           0x6EDA9475ul
#define CAAM_RemoveGenerator    0x6EDA9476ul


struct opCAA_Generate
{
	STACKED ULONG MethodID;
	STACKED ULONG ProjectType;
};

struct CAAP_RemoveGenerator
{
	STACKED ULONG MethodID;
	STACKED Object *Generator;
};

