/*
 * tooltypes.c
 * ===========
 * Handles tooltypes.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/dos.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "game.h"
#include "tooltypes.h"


#define NUM_TOOLTYPES   11


struct Library  *IconBase;

/* filename and dirlock */
static BPTR   dir_lock = NULL;
static char   prg_name[256];

/* where to put the toolvalues */
extern char     pubscr_name[];    /* PUBSCREEN=<name of public screen> */
extern UBYTE    current_level;    /* LEVEL=<0-3> */
extern BOOL     place_warnings;   /* WARNINGS */
extern UBYTE    auto_opening;     /* AUTOOPEN=<0-10> */
extern UBYTE    chosen_task;      /* TASK=<ALL, PATH> */
extern BOOL     display_colors;   /* NOCOLORS */
extern LONG     win_left;         /* LEFT=<left edge> */
extern LONG     win_top;          /* TOP=<top edge */
extern struct Window  *main_win;
extern struct level   levels[];   /* OPTIONALROWS... */

/* reads the tooltypes */
static void
read_tooltypes (void)
{
   struct DiskObject  *disk_obj;
   char  *tool_val;
   BPTR   old_dir = -1;
   
   if (NULL != (IconBase = OpenLibrary ("icon.library", 33L)))
   {
      if (dir_lock != NULL)
         old_dir = CurrentDir (dir_lock);
      
      if (NULL != (disk_obj = GetDiskObject (prg_name)))
      {
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "PUBSCREEN")))
            strncpy (pubscr_name, tool_val, 128);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "LEFT")))
            win_left = atoi (tool_val);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "TOP")))
            win_top = atoi (tool_val);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "LEVEL")))
         {
            if (MatchToolValue (tool_val, "Novice"))
               current_level = 1;
            else if (MatchToolValue (tool_val, "Amateur"))
               current_level = 2;
            else if (MatchToolValue (tool_val, "Expert"))
               current_level = 3;
            else if (MatchToolValue (tool_val, "Optional"))
               current_level = 0;
         }
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "OPTIONALROWS")))
            levels[0].rows = atoi (tool_val);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "OPTIONALCOLS")))
            levels[0].columns = atoi (tool_val);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes,
                                      "OPTIONALMINES")))
         {
            levels[0].bombs = atoi (tool_val);
         }
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "TASK")))
         {
            if (MatchToolValue (tool_val, "All"))
               chosen_task = 0;
            else if (MatchToolValue (tool_val, "Path"))
               chosen_task = 1;
         }
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "AUTOOPEN")))
            auto_opening = atoi (tool_val);
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "WARNINGS")))
            place_warnings = TRUE;
         if (NULL != (tool_val = FindToolType (disk_obj->do_ToolTypes, "NOCOLORS")))
            display_colors = FALSE;
         FreeDiskObject (disk_obj);
      }
      
      if (old_dir != -1)
         CurrentDir (old_dir);
      
      CloseLibrary (IconBase);
   }
}

void
save_tooltypes (void)
{
   int    i, j, n = 0;
   char **new_toolarray;
   char **old_toolarray;
   char  *old_toolval[NUM_TOOLTYPES];
   char   tooltypes[NUM_TOOLTYPES][129];
   BPTR   old_dir = -1;
   struct DiskObject  *disk_obj;
   
   if (NULL != (IconBase = OpenLibrary ("icon.library", 33L)))
   {
      if (dir_lock != NULL)
         old_dir = CurrentDir (dir_lock);
      
      if (NULL != (disk_obj = GetDiskObject (prg_name)))
      {
         old_toolarray = disk_obj->do_ToolTypes;
         while (old_toolarray[n] != NULL)
            ++n;
         
         old_toolval[0] = FindToolType (old_toolarray, "PUBSCREEN");
         old_toolval[1] = FindToolType (old_toolarray, "LEFT");
         old_toolval[2] = FindToolType (old_toolarray, "TOP");
         old_toolval[3] = FindToolType (old_toolarray, "LEVEL");
         old_toolval[4] = FindToolType (old_toolarray, "OPTIONALROWS");
         old_toolval[5] = FindToolType (old_toolarray, "OPTIONALCOLS");
         old_toolval[6] = FindToolType (old_toolarray, "OPTIONALMINES");
         old_toolval[7] = FindToolType (old_toolarray, "TASK");
         old_toolval[8] = FindToolType (old_toolarray, "AUTOOPEN");
         old_toolval[9] = FindToolType (old_toolarray, "WARNINGS");
         old_toolval[10] = FindToolType (old_toolarray, "NOCOLORS");
         for (i = 0; i < NUM_TOOLTYPES; ++i)
         {
            if (old_toolval[i] == NULL)
               ++n;
         }
         if (NULL != (new_toolarray = malloc ((n + 1) * sizeof (*new_toolarray))))
         {
            if (pubscr_name[0] != '\0')
            {
               strncpy (tooltypes[0], "PUBSCREEN=", 128);
               strncat (tooltypes[0], pubscr_name, 128);
            }
            else
            {
               strncpy (tooltypes[0],
                        "(PUBSCREEN=<name of public screen>)", 128);
            }
            
            sprintf (tooltypes[1], "LEFT=%d", main_win->LeftEdge);
            
            sprintf (tooltypes[2], "TOP=%d", main_win->TopEdge);
            
            strncpy (tooltypes[3], "LEVEL=", 128);
            if (current_level == 1)
               strncat (tooltypes[3], "Novice", 128);
            else if (current_level == 2)
               strncat (tooltypes[3], "Amature", 128);
            else if (current_level == 3)
               strncat (tooltypes[3], "Expert", 128);
            else if (current_level == 0)
               strncat (tooltypes[3], "Optional", 128);
            
            sprintf (tooltypes[4], "OPTIONALROWS=%d", levels[0].rows);
            
            sprintf (tooltypes[5], "OPTIONALCOLS=%d", levels[0].columns);
            
            sprintf (tooltypes[6], "OPTIONALMINES=%d", levels[0].bombs);
            
            strncpy (tooltypes[7], "TASK=", 128);
            if (chosen_task == 0)
               strncat (tooltypes[7], "All", 128);
            else if (chosen_task == 1)
               strncat (tooltypes[7], "Path", 128);
            
            sprintf (tooltypes[8], "AUTOOPEN=%d", auto_opening);
            
            if (place_warnings)
               strncpy (tooltypes[9], "WARNINGS", 128);
            else
               strncpy (tooltypes[9], "(WARNINGS)", 128);
            
            if (display_colors)
               strncpy (tooltypes[10], "(NOCOLORS)", 128);
            else
               strncpy (tooltypes[10], "NOCOLORS", 128);
            
            i = 0;
            while (old_toolarray[i] != NULL)
            {
               if (strstr (old_toolarray[i], "PUBSCREEN=") ==
                   old_toolarray[i] ||
                   strstr (old_toolarray[i], "(PUBSCREEN=") ==
                   old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[0];
                  old_toolval[0] = tooltypes[0];
               }
               else if (strstr (old_toolarray[i], "LEFT=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(LEFT=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[1];
                  old_toolval[1] = tooltypes[1];
               }
               else if (strstr (old_toolarray[i], "TOP=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(TOP=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[2];
                  old_toolval[2] = tooltypes[2];
               }
               else if (strstr (old_toolarray[i], "LEVEL=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(LEVEL=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[3];
                  old_toolval[3] = tooltypes[3];
               }
               else if (strstr (old_toolarray[i], "OPTIONALROWS=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(OPTIONALROWS=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[4];
                  old_toolval[4] = tooltypes[4];
               }
               else if (strstr (old_toolarray[i], "OPTIONALCOLS=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(OPTIONALCOLS=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[5];
                  old_toolval[5] = tooltypes[5];
               }
               else if (strstr (old_toolarray[i], "OPTIONALMINES=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(OPTIONALMINES=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[6];
                  old_toolval[6] = tooltypes[6];
               }
               else if (strstr (old_toolarray[i], "TASK=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(TASK=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[7];
                  old_toolval[7] = tooltypes[7];
               }
               else if (strstr (old_toolarray[i], "AUTOOPEN=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(AUTOOPEN=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[8];
                  old_toolval[8] = tooltypes[8];
               }
               else if (strstr (old_toolarray[i], "WARNINGS") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(WARNINGS)") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[9];
                  old_toolval[9] = tooltypes[9];
               }
               else if (strstr (old_toolarray[i], "NOCOLORS") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(NOCOLORS)") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[10];
                  old_toolval[10] = tooltypes[10];
               }
               else
                  new_toolarray[i] = old_toolarray[i];
               
               ++i;
            }
            if (n > i)
            {
               for (j = 0; j < NUM_TOOLTYPES; ++j)
               {
                  if (old_toolval[j] == NULL)
                  {
                     new_toolarray[i] = tooltypes[j];
                     ++i;
                  }
               }
            }
            new_toolarray[i] = NULL;
            
            disk_obj->do_ToolTypes = new_toolarray;
            PutDiskObject (prg_name, disk_obj);
            disk_obj->do_ToolTypes = old_toolarray;
            
            free (new_toolarray);
         }
         FreeDiskObject (disk_obj);
      }
      
      if (old_dir != -1)
         CurrentDir (old_dir);
      
      CloseLibrary (IconBase);
   }
}

void
handle_startup_msg (
   char **args,
   BOOL   from_wb)
{
   struct WBArg  *wb_arg;
   
   if (from_wb)
   {
      wb_arg = ((struct WBStartup *)args)->sm_ArgList;
      dir_lock = wb_arg->wa_Lock;
      strncpy (prg_name, wb_arg->wa_Name, 255);
   }
   else
      strncpy (prg_name, args[0], 255);
   
   read_tooltypes ();
}
