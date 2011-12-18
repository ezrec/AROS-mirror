#ifndef CONFIG_H
#define CONFIG_H

#include <float.h> /* for DBL_MAX definition */
#include <stdlib.h> /* for qsort */
#include <unistd.h> /* for unlink */

/*
 * These two are used if POV is being called from within another program
 * like a GUI interface.
 */
#ifndef MAIN_RETURN_TYPE
#define MAIN_RETURN_TYPE int
#endif

#ifndef MAIN_RETURN_STATEMENT
#define MAIN_RETURN_STATEMENT
#endif


/*
 * Functions that POV calls once per render to do various initializations,
 * in the order that they are normally called.
 */
#ifndef STARTUP_POVRAY  /* First function called in main() for each render */
#define STARTUP_POVRAY
#endif

#ifndef PRINT_CREDITS   /* Prints POV-Ray version information banner */
#define PRINT_CREDITS Print_Credits();
#endif

#ifndef PRINT_OTHER_CREDITS /* Prints credits for custom POV versions */
#define PRINT_OTHER_CREDITS
#endif

/*
 * These read the INI files.  READ_ENV_VAR reads an INI file specified by
 * (usually) the POVINI environment variable instead of the default file.
 * PROCESS_POVRAY_INI reads the INI file from the default location if
 * READ_ENV_VAR wasn't successful.  ALT_WRITE_INI_FILE writes out a new
 * INI file with the values as specified by the used for this render.
 */
#ifndef READ_ENV_VAR
#define READ_ENV_VAR Warning(0.0,"Environment variable not implemented on this platform.\n");
#endif

#ifndef PROCESS_POVRAY_INI
#define PROCESS_POVRAY_INI Warning(0.0,"Reading 'povray.ini' not implemented on this platform.\n");
#endif

#ifndef ALT_WRITE_INI_FILE
#define ALT_WRITE_INI_FILE
#endif

#ifndef FINISH_POVRAY   /* The last call that POV makes to exit */
#define FINISH_POVRAY(n) exit(n);
#endif


/*
 * Functions that POV calls once per frame to do varios (de)initializations,
 * in the order they are normally called.
 */
#ifndef POV_PRE_RENDER    /* Called just prior to the start of rendering */
#define POV_PRE_RENDER
#endif

#ifndef CONFIG_MATH       /* Macro for setting up any special FP options */
#define CONFIG_MATH
#endif

#ifndef POV_PRE_PIXEL     /* Called before each pixel is rendered */
#define POV_PRE_PIXEL(x,y,c)
#endif

#ifndef POV_POST_PIXEL    /* Called after each pixel is rendered */
#define POV_POST_PIXEL(x,y,c)
#endif

#ifndef POV_PRE_SHUTDOWN  /* Called before memory and objects are freed */
#define POV_PRE_SHUTDOWN
#endif

#ifndef POV_POST_SHUTDOWN /* Called after memory and objects are freed */
#define POV_POST_SHUTDOWN
#endif

#ifndef PRINT_STATS
#define PRINT_STATS(a) Print_Stats(a);
#endif

/* Platform-specific User Interface callbacks, allowing the UI to get status information */
#ifndef UICB_PARSE_ERROR  /* Alert UI that a parse error occurred */
#define UICB_PARSE_ERROR
#endif

#ifndef UICB_USER_ABORT  /* alert UI that user aborted the render */
#define UICB_USER_ABORT
#endif

#ifndef UICB_OPEN_INCLUDE_FILE  /* alert UI that an include file was opened during parse */
#define UICB_OPEN_INCLUDE_FILE
#endif

#ifndef UICB_CLOSE_INCLUDE_FILE  /* alert UI that an include file was closed during parse */
#define UICB_CLOSE_INCLUDE_FILE
#endif


/* Various numerical constants that are used in the calculations */
#ifndef EPSILON     /* A small value used to see if a value is nearly zero */
#define EPSILON 1.0e-10
#endif

#ifndef HUGE_VAL    /* A very large value, can be considered infinity */
#define HUGE_VAL 1.0e+17
#endif

/*
 * If the width of a bounding box in one dimension is greater than
 * the critical length, the bounding box should be set to infinite.
 */

#ifndef CRITICAL_LENGTH
#define CRITICAL_LENGTH 1.0e6
#endif

#ifndef BOUND_HUGE  /* Maximum lengths of a bounding box. */
#define BOUND_HUGE 2.0e10
#endif

/*
 * These values determine the minumum and maximum distances
 * that qualify as ray-object intersections.
 */

#define Small_Tolerance 0.001
#define Max_Distance 1.0e7


#ifndef DBL_FORMAT_STRING
#define DBL_FORMAT_STRING "%lf"
#endif

#ifndef DBL
#define DBL double
#endif

#ifndef SNGL
#define SNGL float
#endif

#ifndef COLC
#define COLC float
#endif

#ifndef M_PI
#define M_PI   3.1415926535897932384626
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef TWO_M_PI
#define TWO_M_PI 6.283185307179586476925286766560
#endif

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576
#endif

#ifndef M_PI_360
#define M_PI_360 0.00872664625997164788
#endif

/* Some implementations of scanf return 0 on failure rather than EOF */
#ifndef SCANF_EOF
#define SCANF_EOF EOF
#endif

/* Get minimum/maximum of two values. */

#ifndef min
#define min(x,y) (((x)>(y))?(y):(x))
#endif

#ifndef max
#define max(x,y) (((x)<(y))?(y):(x))
#endif

/* Get minimum/maximum of three values. */

#define max3(x,y,z) (((x)>(y))?(((x)>(z))?(x):(z)):(((y)>(z))?(y):(z)))
#define min3(x,y,z) (((x)<(y))?(((x)<(z))?(x):(z)):(((y)<(z))?(y):(z)))

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef CONST       /* How to define a local variable - normally 'const' */
#define CONST	const
#endif

#ifndef CDECL
#define CDECL
#endif

#ifndef NEW_LINE_STRING
#define NEW_LINE_STRING "\n"
#endif

/* If compiler version is undefined, then make it 'u' for unknown */
#ifndef COMPILER_VER
#define COMPILER_VER ".u"
#endif

#ifndef QSORT
#define QSORT(a,b,c,d) qsort((a),(b),(c),(d))
#endif


/*
 * POV_NAME_MAX is for file systems that have a separation of the filename
 * into name.ext.  The POV_NAME_MAX is the name part.  FILE_NAME_LENGTH
 * is the sum of name + extension.
 */
#ifndef POV_NAME_MAX
#define POV_NAME_MAX 8
#endif

#ifndef FILE_NAME_LENGTH
#define FILE_NAME_LENGTH 150
#endif

#ifndef FILENAME_SEPARATOR
#define FILENAME_SEPARATOR '/'
#endif

#ifndef DRIVE_SEPARATOR
#define DRIVE_SEPARATOR ':'
#endif

/*
 * Splits a given string into the path and file components using the
 * FILENAME_SEPARATOR and DRIVE_SEPARATOR
 */
#ifndef POV_SPLIT_PATH
#define POV_SPLIT_PATH(s,p,f) POV_Split_Path((s),(p),(f))
#endif

/* How to read, write and append to files using fopen() */
/* -- Binary -- */
#ifndef READ_BINFILE_STRING
#define READ_BINFILE_STRING "rb"
#endif

#ifndef WRITE_BINFILE_STRING
#define WRITE_BINFILE_STRING "wb"
#endif

#ifndef APPEND_BINFILE_STRING
#define APPEND_BINFILE_STRING "ab"
#endif
/* -- Text -- */
#ifndef READ_TXTFILE_STRING
#define READ_TXTFILE_STRING "r"
#endif

#ifndef WRITE_TXTFILE_STRING
#define WRITE_TXTFILE_STRING "w"
#endif

#ifndef APPEND_TXTFILE_STRING
#define APPEND_TXTFILE_STRING "a"
#endif

/* The output file format used if the user doesn't specify one */
#ifndef DEFAULT_OUTPUT_FORMAT
#define DEFAULT_OUTPUT_FORMAT   't'
#endif

/* System specific image format like BMP for Windows or PICT for Mac */
#ifndef READ_SYS_IMAGE
#define READ_SYS_IMAGE(i,n) Read_Targa_Image((i),(n))
#endif

#ifndef GET_SYS_FILE_HANDLE
#define GET_SYS_FILE_HANDLE Get_Targa_File_Handle
#endif

#ifndef SYS_DEF_EXT
#define SYS_DEF_EXT ".tga"
#endif

/* Functions to delete and rename a file */
#ifndef DELETE_FILE_ERR
#define DELETE_FILE_ERR -1
#endif

#ifndef DELETE_FILE
#define DELETE_FILE(name) unlink(name)
#endif

#ifndef RENAME_FILE_ERR
#define RENAME_FILE_ERR -1
#endif

#ifndef RENAME_FILE
#define RENAME_FILE(orig,new) rename(orig,new)
#endif

#ifndef MAX_BUFSIZE  /* The maximum size of the output file buffer */
#define MAX_BUFSIZE INT_MAX
#endif


/*
 * The TIME macros are used when displaying the rendering time for the user.
 * These are called in such a manner that STOP_TIME can be called multiple
 * times for a givn START_TIME in order to get intermediate TIME_ELAPSED
 * values.  TIME_ELAPSED is often defined as (tstop - tstart).
 */
#ifndef START_TIME
#define START_TIME time(&tstart);     
#endif

#ifndef STOP_TIME
#define STOP_TIME  time(&tstop);
#endif

#ifndef TIME_ELAPSED
#define TIME_ELAPSED difftime (tstop, tstart);
#endif

#ifndef SPLIT_TIME
#define SPLIT_TIME(d,h,m,s) POV_Std_Split_Time ((d),(h),(m),(s))
#endif


/*
 * The PRECISION_TIMER macros are used in generating histogram images on
 * systems that have very accurate timers (usually in the microsecond range).
 */
#ifndef PRECISION_TIMER_AVAILABLE
#define PRECISION_TIMER_AVAILABLE 0
#endif

#ifndef PRECISION_TIMER_INIT  /* Called once to initialize the timer */
#define PRECISION_TIMER_INIT
#endif

#ifndef PRECISION_TIMER_START
#define PRECISION_TIMER_START ;
#endif

#ifndef PRECISION_TIMER_STOP
#define PRECISION_TIMER_STOP
#endif

#ifndef PRECISION_TIMER_COUNT  /* The difference between START and STOP times */
#define PRECISION_TIMER_COUNT 0
#endif


/*
 * The COOPERATE macros are used on co-operative multi-tasking systems to
 * return control to the GUI or OS.  COOPERATE is the old form, and one
 * or both of COOPERATE_0 and COOPERATE_1 should be defined instead.
 */
#ifdef COOPERATE
#define COOPERATE_0     COOPERATE
#define COOPERATE_1     COOPERATE
#endif

#ifndef COOPERATE_0    /* Called less frequently */
#define COOPERATE_0
#endif

#ifndef COOPERATE_1    /* Called more frequently */
#define COOPERATE_1
#endif


/* How to get input from the user */
#ifndef TEST_ABORT
#define TEST_ABORT
#endif

#ifndef WAIT_FOR_KEYPRESS
#define WAIT_FOR_KEYPRESS
#else
#define WAIT_FOR_KEYPRESS_EXISTS
#endif

/*
 * Functions that write text for the user to see.  These functions will
 * usually be customized for GUI environments so that POV outputs its
 * messages to a status bar or popup window.
 */
#ifndef POV_BANNER
#define POV_BANNER(s) POV_Std_Banner(s)
#endif

#ifndef POV_WARNING
#define POV_WARNING(s) POV_Std_Warning(s)
#endif

#ifndef POV_RENDER_INFO
#define POV_RENDER_INFO(s) POV_Std_Render_Info(s)
#endif

#ifndef POV_STATUS_INFO
#define POV_STATUS_INFO(s) POV_Std_Status_Info(s)
#endif

#ifndef POV_DEBUG_INFO
#define POV_DEBUG_INFO(s) POV_Std_Debug_Info(s)
#endif

#ifndef POV_FATAL
#define POV_FATAL(s) POV_Std_Fatal(s)
#endif

#ifndef POV_STATISTICS
#define POV_STATISTICS(s) POV_Std_Statistics(s)
#endif


/*
 * Functions that handle the graphical display preview.  These functions
 * will be customeized for all versions of POV that want to do any sort
 * of rendering preview.  The default functions will create a 80x25 text
 * "rendering" using crude ASCII graphics.
 */
#ifndef POV_DISPLAY_INIT     /* Initializes display for each frame rendered */
#define POV_DISPLAY_INIT(w,h) POV_Std_Display_Init((w),(h));
#endif

#ifndef POV_DISPLAY_FINISHED  /* Waits for user input after rendering done */
#define POV_DISPLAY_FINISHED POV_Std_Display_Finished();
#endif

#ifndef POV_DISPLAY_CLOSE     /* Closes the display window after each frame */
#define POV_DISPLAY_CLOSE POV_Std_Display_Close();
#endif

#ifndef POV_DISPLAY_PLOT      /* Plots a single pixel */
#define POV_DISPLAY_PLOT(x,y,r,g,b,a) POV_Std_Display_Plot((x),(y),(r),(g),(b),(a));
#endif

#ifndef POV_DISPLAY_PLOT_RECT  /* Plots a filled rectangle */
#define POV_DISPLAY_PLOT_RECT(x1,y1,x2,y2,r,g,b,a) POV_Std_Display_Plot_Rect((x1),(y1),(x2),(y2),(r),(g),(b),(a));
#endif

#ifndef POV_DISPLAY_PLOT_BOX   /* Plots a hollow box */
#define POV_DISPLAY_PLOT_BOX(x1,y1,x2,y2,r,g,b,a) POV_Std_Display_Plot_Box((x1),(y1),(x2),(y2),(r),(g),(b),(a));
#endif

#ifndef POV_GET_FULL_PATH      /* returns full pathspec */
#define POV_GET_FULL_PATH(f,p,b) if (b) strcpy(b,p);
#endif

#ifndef POV_WRITE_LINE         /* write the current line to something */
#define POV_WRITE_LINE(line,y)
#endif

#ifndef POV_ASSIGN_PIXEL       /* assign the colour of a pixel */
#define POV_ASSIGN_PIXEL(x,y,colour)
#endif

/* The next two are palette modes, for normal and grayscale display */
#ifndef NORMAL
#define NORMAL '0'
#endif

#ifndef GREY
#define GREY   'G'
#endif

/*
 * The DEFAULT_DISPLAY_GAMMA is used when there isn't one specified by the
 * user in the POVRAY.INI.  For those systems that are very savvy, this
 * could be a function which returns the current display gamma.  The
 * DEFAULT_ASSUMED_GAMMA should be left alone.
 */
#ifndef DEFAULT_DISPLAY_GAMMA
#define DEFAULT_DISPLAY_GAMMA 2.2
#endif

#ifndef DEFAULT_ASSUMED_GAMMA
#define DEFAULT_ASSUMED_GAMMA 1.0
#endif


/*****************************************************************************
 *
 * MEMIO.C Memory macros
 *
 *****************************************************************************/

#ifndef __FILE__
#define __FILE__ ""
#endif

#ifndef __LINE__
#define __LINE__ (-1)
#endif

/*
 * These functions define macros which do checking for memory allocation,
 * and can also do other things.  Check mem.c before you change them, since
 * they aren't simply replacements for malloc, calloc, realloc, and free.
 */
#ifndef POV_MALLOC
#define POV_MALLOC(size,msg)        pov_malloc ((size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_CALLOC
#define POV_CALLOC(nitems,size,msg) pov_calloc ((nitems), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_REALLOC
#define POV_REALLOC(ptr,size,msg)   pov_realloc ((ptr), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_FREE
#define POV_FREE(ptr)               pov_free ((void *)(ptr), __FILE__, __LINE__)
#endif

#ifndef POV_MEM_INIT
#define POV_MEM_INIT()              mem_init()
#endif

#ifndef POV_MEM_RELEASE_ALL
#define POV_MEM_RELEASE_ALL(log)    mem_release_all(log)
#endif

#ifndef POV_STRDUP
#define POV_STRDUP(str)             pov_strdup(str)
#endif

/* For those systems that don't have memmove, this can also be pov_memmove */
#ifndef POV_MEMMOVE
#define POV_MEMMOVE(dst,src,len)    memmove((dst),(src),(len))
#endif


/*
 * Functions which invoke external programs to do work for POV, generally
 * at the request of the user.
 */
#ifndef POV_SHELLOUT
#define POV_SHELLOUT(string) pov_shellout(string)
#endif

#ifndef POV_MAX_CMD_LENGTH
#define POV_MAX_CMD_LENGTH 250
#endif

#ifndef POV_SYSTEM
#define POV_SYSTEM(string) system(string)
#endif


#endif

