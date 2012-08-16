/* Generator class header */

/* Code generated with ChocolateCastle 0.1 */
/* written by Grzegorz "Krashan" Kraszewski <krashan@teleinfo.pb.bialystok.pl> */

#ifndef CHC_GENERATOR_H
#define CHC_GENERATOR_H


extern struct MUI_CustomClass *GeneratorClass;

struct MUI_CustomClass *CreateGeneratorClass(void);
void DeleteGeneratorClass(void);

#define INPUT_LINE_MAX_LEN     512   // in project file


#define GENM_Generate          0x6EDA8367
#define GENM_InsertRemark      0x6EDA8368
#define GENM_IndentIn          0x6EDA8369
#define GENM_IndentOut         0x6EDA836A
#define GENM_DoIndent          0x6EDA836B
#define GENM_Setup             0x6EDA836C
#define GENM_Cleanup           0x6EDA836D
#define GENM_Signature         0x6EDA836E
#define GENM_Text              0x6EDA836F
#define GENM_TextParam         0x6EDA8370
#define GENM_Conditional       0x6EDA8371
#define GENM_Save              0x6EDA8372   // save the class to a project file
#define GENM_SaveAction        0x6EDA8373
#define GENM_LoadAction        0x6EDA8374
#define GENM_Load              0x6EDA8375   // load a class from a project file
#define GENM_MethodHeader      0x6EDA8376
#define GENM_MethodFooter      0x6EDA8377
#define GENM_Verify            0x6EDA8378   // verify general module settings
#define GENM_GenerateAction    0x6EDA8379
#define GENM_MakefileSignature 0x6EDA837A   // signature for makefiles
#define GENM_CreateLibGroup    0x6EDA837B   // MUI group for library parameters
#define GENM_DisableLibGroup   0x6EDA837C   // change disabled state of library group
#define GENM_LibVersionH       0x6EDA837D   // generates library/class version header
#define GENM_LibraryC          0x6EDA837E   // generates shared library body
#define GENM_LibraryH          0x6EDA837F   // generates shared library body header
#define GENM_LeadingComment    0x6EDA8380   // generates leading comment before function
#define GENM_TrailingComment   0x6EDA8381   // generates trailing comment after function


#define GENA_SubclassSpace     0x6EDA9983
#define GENA_UnitName          0x6EDA9984
#define GENA_ProjectType       0x6EDA9985  // [..G]


struct GENP_InsertRemark
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  Text;
};


struct GENP_Setup
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  NamePattern;
};


struct GENP_Text
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  Text;
};


struct GENP_TextParam
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  Text;
	STACKED IPTR    Param;
};

struct GENP_Save
{
	STACKED ULONG   MethodID;
	STACKED IPTR    Handle;
};

struct GENP_Load
{
	STACKED ULONG   MethodID;
	STACKED IPTR    Handle;
	STACKED struct Parser *Parser;
	STACKED STRPTR  LineBuf;
};

struct GENP_MethodHeader
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  MethodName;
	STACKED STRPTR  FuncName;
	STACKED STRPTR  StructName;
	STACKED STRPTR  Identifier;
	STACKED IPTR    Instance;       // only generate INST_DATA if TRUE
	STACKED IPTR    ExtClass;       // TRUE if external class, lib name in autodoc header
};

struct GENP_MethodFooter
{
	STACKED ULONG   MethodID;
	STACKED IPTR    ReturnZero;     // output "return 0;" statement if TRUE
};

struct GENP_DisableLibGroup
{
	STACKED ULONG   MethodID;
	STACKED IPTR    Disabled;
};

struct GENP_LibVersionH
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  FileName;
};

struct GENP_LibraryC
{
	STACKED ULONG   MethodID;
	STACKED IPTR    ProjectType;
};

struct GENP_LeadingComment
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  VarText;
};

struct GENP_TrailingComment
{
	STACKED ULONG   MethodID;
	STACKED STRPTR  VarText;
};

#define PARAM_CLASSNAME    0
#define PARAM_CLASSNAMELC  1
#define PARAM_LIBNAME      2
#define PARAM_VERSION      3
#define PARAM_REVISION     4
#define PARAM_COPYRIGHT    5

#define OBJ_LIBG_NAME       0x6EDA83A0   // shared library name
#define OBJ_LIBG_VERSION    0x6EDA83A1   // shared library version
#define OBJ_LIBG_REVISION   0x6EDA83A2   // shared library revision
#define OBJ_LIBG_COPYRIGHT  0x6EDA83A3   // shared library copyrigth


#endif
