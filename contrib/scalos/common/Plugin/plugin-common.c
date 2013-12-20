// plugin-common.c
// $Date$
// $Revision$

//----------------------------------------------------------------------------
// Check the preprocessor symbols provided by plugin_data.h

#if !defined(LIB_VERSION) || !defined(LIB_REVISION) || !defined(LIB_NAME) || !defined(LIB_VERSTRING)
#error Library version, revision, name or version string not defined
#endif

#ifndef LIB_BASETYPE
#define LIB_BASETYPE struct PluginBase
#endif

#if PLUGIN_TYPE == OOP

#if !defined(CI_CLASSNAME) || !defined(CI_PLUGINNAME) || !defined(CI_DESCRIPTION) \
	|| !defined(CI_AUTHOR) || !defined(CI_HOOKFUNC)
#error classname, pluginname, description, author or hookfunc not define for ClassInfo
#endif

#ifndef CI_SUPERCLASSNAME
#define CI_SUPERCLASSNAME CI_CLASSNAME
#endif
#ifndef CI_PRIORITY
#define CI_PRIORITY 0
#endif
#ifndef CI_NEEDEDVERSION
#define CI_NEEDEDVERSION 39
#endif
#ifndef CI_INSTSIZE
#define CI_INSTSIZE 2*sizeof(ULONG)
#endif

#elif PLUGIN_TYPE == FILETYPE

#ifndef FT_INFOSTRING
#error FT_INFOSTRING not defined for FileType plugin
#endif

#elif PLUGIN_TYPE == PREVIEW

#ifndef PV_GENERATE
#ifdef __AROS__
#define PV_GENERATE PluginBase_0_LIBSCAPreviewGenerate
#else
#define PV_GENERATE LIBSCAPreviewGenerate
#endif
#endif

#elif PLUGIN_TYPE == PREFS

// Nothing to do

#else

#error Unknown or undefined PLUGIN_TYPE

#endif

//----------------------------------------------------------------------------
// Code common to plugin-aos4.c and plugin-classic.c

static char ALIGNED libName[] = LIB_NAME;
static char ALIGNED libVerString[] = LIB_VERSTRING;
#ifdef LIB_IDSTRING
static char ALIGNED libIdString[] = LIB_IDSTRING;
#endif

#if PLUGIN_TYPE == OOP

STATIC_PATCHFUNC(CI_HOOKFUNC)

/* structure which holds the data that Scalos needs for this class/plugin */
static struct ScaClassInfo ClassInfo =
{
	/* Next three lines constitute the Hook structure within this structure */
	{
	{ 0,0 }, 	/* previous and successor pointers, set to NULL */
	(HOOKFUNC) PATCH_NEWFUNC(CI_HOOKFUNC), /* pointer to hook function which does the real work in this plugin */
	0,0            	/* h_SubEntry and h_Data are 0 by default */
	},

	CI_PRIORITY,    /* Priority of this class */
	CI_INSTSIZE,    /* Instance size */
	CI_NEEDEDVERSION, /* Required version of Scalos */
	0,              /* Reserved */
	CI_CLASSNAME,	/* name of this class */
	CI_SUPERCLASSNAME, /* name of the superclass */
	CI_PLUGINNAME,  /* real name of the plugin */
	CI_DESCRIPTION, /* description of plugin */
	CI_AUTHOR	/* name of author */
};

#ifdef __AROS__
AROS_LH0(struct ScaClassInfo *, GetClassInfo,
struct Library *, libbase, 5, PluginBase
)
{
	AROS_LIBFUNC_INIT
#else
static LIBFUNC(GetClassInfo, libbase, struct ScaClassInfo *)
#endif
{
	(void)libbase;
	return &ClassInfo;
}
LIBFUNC_END

#endif

//----------------------------------------------------------------------------

