/*
 * tooltypes.c
 * ===========
 * Handles tooltypes.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <stdlib.h>
#include <string.h>
#include "tooltypes.h"

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>


#define NUM_TOOLTYPES   5


struct Library  *IconBase;

/* filename and dirlock */
static BPTR   dir_lock = NULL;
static char   prg_name[256];

/* where to put the toolvalues */
extern char    pubscr_name[];
extern BOOL    chosen_display;
extern UBYTE   chosen_num_colors;
extern UBYTE   chosen_correction;
extern UBYTE   chosen_opponent;


/* reads the tooltypes */
static void
read_tooltypes (void)
{
   struct DiskObject  *disk_obj;
   char  *tool_val;
   BPTR   old_dir = -1;
   
   if (IconBase = OpenLibrary ("icon.library", 33L))
   {
      if (dir_lock != NULL)
         old_dir = CurrentDir (dir_lock);
      
      if (disk_obj = GetDiskObject (prg_name))
      {
         if (tool_val = FindToolType (disk_obj->do_ToolTypes, "PUBSCREEN"))
            strncpy (pubscr_name, tool_val, 128);
         if (tool_val = FindToolType (disk_obj->do_ToolTypes, "NOCOLORS"))
            chosen_display = FALSE;
         if (tool_val = FindToolType (disk_obj->do_ToolTypes, "NUMCOLORS"))
            chosen_num_colors = atoi (tool_val);
         if (tool_val = FindToolType (disk_obj->do_ToolTypes, "CORRECTION"))
         {
            if (MatchToolValue (tool_val, "Children"))
               chosen_correction = 0;
            else if (MatchToolValue (tool_val, "Adults"))
               chosen_correction = 1;
         }
         if (tool_val = FindToolType (disk_obj->do_ToolTypes, "OPPONENT"))
         {
            if (MatchToolValue (tool_val, "Human"))
               chosen_opponent = 0;
            else if (MatchToolValue (tool_val, "Computer"))
               chosen_opponent = 1;
         }
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
   char   num_str[7];
   BPTR   old_dir = -1;
   struct DiskObject  *disk_obj;
   
   if (IconBase = OpenLibrary ("icon.library", 33L))
   {
      if (dir_lock != NULL)
         old_dir = CurrentDir (dir_lock);
      
      if (disk_obj = GetDiskObject (prg_name))
      {
         old_toolarray = disk_obj->do_ToolTypes;
         while (old_toolarray[n] != NULL)
            ++n;
         
         old_toolval[0] = FindToolType (old_toolarray, "PUBSCREEN");
         old_toolval[1] = FindToolType (old_toolarray, "NOCOLORS");
         old_toolval[2] = FindToolType (old_toolarray, "NUMCOLORS");
         old_toolval[3] = FindToolType (old_toolarray, "CORRECTION");
         old_toolval[4] = FindToolType (old_toolarray, "OPPONENT");
         for (i = 0; i < NUM_TOOLTYPES; ++i)
         {
            if (old_toolval[i] == NULL)
               ++n;
         }
         if (new_toolarray = malloc ((n + 1) * sizeof (*new_toolarray)))
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
            if (!chosen_display)
               strncpy (tooltypes[1], "NOCOLORS", 128);
            else
               strncpy (tooltypes[1], "(NOCOLORS)", 128);
            strncpy (tooltypes[2], "NUMCOLORS=", 128);
#warning Changed the following line!
//            stci_d (num_str, chosen_num_colors);
sprintf(num_str, "%d" ,chosen_num_colors);
            strncat (tooltypes[2], num_str, 128);         
            strncpy (tooltypes[3], "CORRECTION=", 128);
            if (chosen_correction == 0)
               strncat (tooltypes[3], "Children", 128);
            else if (chosen_correction == 1)
               strncat (tooltypes[3], "Adults", 128);
            strncpy (tooltypes[4], "OPPONENT=", 128);
            if (chosen_opponent == 0)
               strncat (tooltypes[4], "Human", 128);
            else if (chosen_opponent == 1)
               strncat (tooltypes[4], "Computer", 128);
            
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
               else if (strstr (old_toolarray[i], "NOCOLORS") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(NOCOLORS)") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[1];
                  old_toolval[1] = tooltypes[1];
               }
               else if (strstr (old_toolarray[i], "NUMCOLORS=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(NUMCOLORS=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[2];
                  old_toolval[2] = tooltypes[2];
               }
               else if (strstr (old_toolarray[i], "CORRECTION=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(CORRECTION=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[3];
                  old_toolval[3] = tooltypes[3];
               }
               else if (strstr (old_toolarray[i], "OPPONENT=") ==
                        old_toolarray[i] ||
                        strstr (old_toolarray[i], "(OPPONENT=") ==
                        old_toolarray[i])
               {
                  new_toolarray[i] = tooltypes[4];
                  old_toolval[4] = tooltypes[4];
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
