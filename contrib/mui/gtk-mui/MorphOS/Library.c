/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: Library.c,v 1.2 2005/08/04 20:06:54 itix Exp $
 *
 *****************************************************************************/

#include <exec/resident.h>

#include <proto/exec.h>

#include "Library.h"

#define COMPILE_VERSION  1
#define COMPILE_REVISION 0
#define COMPILE_DATE     "(4.8.2005)"
#define PROGRAM_VER      "1.0"

	// just for testing, library is not re-entrant

void exit(void)
{
}

void printf(void)
{
}

/**********************************************************************
	Function prototypes
**********************************************************************/

static struct Library *LIB_Open(void);
static BPTR            LIB_Close(void);
static BPTR            LIB_Expunge(void);
static ULONG           LIB_Reserved(void);
static struct Library *LIB_Init(struct MyLibrary *LibBase, BPTR SegList, struct ExecBase *MySysBase);

/**********************************************************************
	Library Header
**********************************************************************/

static const char VerString[]	= "\0$VER: gtk-mui.library " PROGRAM_VER " "COMPILE_DATE;
static const char LibName[]	= "gtk-mui.library";

void	gtk_adjustment_new(void);
void	gtk_arrow_new(void);
void	gtk_box_pack_start(void);
void	gtk_button_new(void);
void	gtk_button_new_with_label(void);
void	gtk_check_button_new_with_label(void);
void	gtk_container_add(void);
void	gtk_container_set_border_width(void);
void	gtk_entry_get_text(void);
void	gtk_entry_new(void);
void	gtk_entry_set_text(void);
void	gtk_file_selection_get_filename(void);
void	gtk_file_selection_new(void);
void	gtk_file_selection_set_filename(void);
void	gtk_hbox_new(void);
void	gtk_hscrollbar_new(void);
void	gtk_hseparator_new(void);
void	gtk_label_new(void);
void	gtk_main(void);
void	gtk_main_quit(void);
void	gtk_menu_bar_new(void);
void	gtk_menu_item_new_with_label(void);
void	gtk_menu_item_set_submenu(void);
void	gtk_menu_new(void);
void	gtk_menu_shell_append(void);
void	gtk_radio_button_group(void);
void	gtk_radio_button_new_with_label(void);
void	gtk_table_attach(void);
void	gtk_table_new(void);
void	gtk_table_set_col_spacings(void);
void	gtk_table_set_row_spacings(void);
void	gtk_toggle_button_set_active(void);
void	gtk_tooltips_force_window(void);
void	gtk_tooltips_new(void);
void	gtk_vbox_new(void);
void	gtk_vscrollbar_new(void);
void	gtk_widget_destroy(void);
void	gtk_widget_grab_default(void);
void	gtk_widget_set_sensitive(void);
void	gtk_widget_show(void);
void	gtk_widget_show_all(void);
void	gtk_widget_hide(void);
void	gtk_widget_hide_all(void);
void	gtk_window_new(void);
void	gtk_window_set_title(void);
void	gtk_adjustment_set_value(void);
void	gtk_box_pack_end(void);
void	gtk_button_new_from_stock(void);
void	gtk_editable_get_chars(void);
void	gtk_editable_insert_text(void);
void	gtk_editable_select_region(void);
void	gtk_editable_set_editable(void);
void	gtk_entry_set_max_length(void);
void	gtk_entry_set_visibility(void);
void	gtk_hscale_new(void);
void	gtk_image_new(void);
void	gtk_image_new_from_file(void);
void	gtk_init(void);
void	gtk_menu_get_attach_widget(void);
void	gtk_option_menu_new(void);
void	gtk_option_menu_set_menu(void);
void	gtk_radio_button_get_group(void);
void	gtk_radio_button_new_with_label_from_widget(void);
void	gtk_range_set_update_policy(void);
void	gtk_scale_set_digits(void);
void	gtk_scale_set_draw_value(void);
void	gtk_scale_set_value_pos(void);
void	gtk_table_attach_defaults(void);
void	gtk_tooltips_disable(void);
void	gtk_tooltips_enable(void);
void	gtk_tooltips_set_tip(void);
void	gtk_vscale_new(void);
void	gtk_widget_ref(void);
void	gtk_widget_set_size_request(void);
void	gtk_widget_unref(void);
void	gtk_window_set_resizable(void);

static const APTR FuncTable[] =
{
	(APTR)   FUNCARRAY_BEGIN,

	// Start m68k block for system functions.

	(APTR)   FUNCARRAY_32BIT_NATIVE, 
	(APTR)   LIB_Open,
	(APTR)   LIB_Close,
	(APTR)   LIB_Expunge,
	(APTR)   LIB_Reserved,
	(APTR)   -1,

	// Start sysv block for user functions.

	(APTR)   FUNCARRAY_32BIT_SYSTEMV,
	(APTR)	gtk_adjustment_new,
	(APTR)	gtk_arrow_new,
	(APTR)	gtk_box_pack_start,
	(APTR)	gtk_button_new,
	(APTR)	gtk_button_new_with_label,
	(APTR)	gtk_check_button_new_with_label,
	(APTR)	gtk_container_add,
	(APTR)	gtk_container_set_border_width,
	(APTR)	gtk_entry_get_text,
	(APTR)	gtk_entry_new,
	(APTR)	gtk_entry_set_text,
	(APTR)	gtk_file_selection_get_filename,
	(APTR)	gtk_file_selection_new,
	(APTR)	gtk_file_selection_set_filename,
	(APTR)	gtk_hbox_new,
	(APTR)	gtk_hscrollbar_new,
	(APTR)	gtk_hseparator_new,
	(APTR)	gtk_label_new,
	(APTR)	gtk_main,
	(APTR)	gtk_main_quit,
	(APTR)	gtk_menu_bar_new,
	(APTR)	gtk_menu_item_new_with_label,
	(APTR)	gtk_menu_item_set_submenu,
	(APTR)	gtk_menu_new,
	(APTR)	gtk_menu_shell_append,
	(APTR)	gtk_radio_button_group,
	(APTR)	gtk_radio_button_new_with_label,
	(APTR)	gtk_table_attach,
	(APTR)	gtk_table_new,
	(APTR)	gtk_table_set_col_spacings,
	(APTR)	gtk_table_set_row_spacings,
	(APTR)	gtk_toggle_button_set_active,
	(APTR)	gtk_tooltips_force_window,
	(APTR)	gtk_tooltips_new,
	(APTR)	gtk_vbox_new,
	(APTR)	gtk_vscrollbar_new,
	(APTR)	gtk_widget_destroy,
	(APTR)	gtk_widget_grab_default,
	(APTR)	gtk_widget_set_sensitive,
	(APTR)	gtk_widget_show,
	(APTR)	gtk_widget_show_all,
	(APTR)	gtk_widget_hide,
	(APTR)	gtk_widget_hide_all,
	(APTR)	gtk_window_new,
	(APTR)	gtk_window_set_title,

	(APTR)	gtk_adjustment_set_value,
	(APTR)	gtk_box_pack_end,
	(APTR)	gtk_button_new_from_stock,
	(APTR)	gtk_editable_get_chars,
	(APTR)	gtk_editable_insert_text,
	(APTR)	gtk_editable_select_region,
	(APTR)	gtk_editable_set_editable,
	(APTR)	gtk_entry_set_max_length,
	(APTR)	gtk_entry_set_visibility,
	(APTR)	gtk_hscale_new,
	(APTR)	gtk_image_new,
	(APTR)	gtk_image_new_from_file,
	(APTR)	gtk_init,
	(APTR)	gtk_menu_get_attach_widget,
	(APTR)	gtk_option_menu_new,
	(APTR)	gtk_option_menu_set_menu,
	(APTR)	gtk_radio_button_get_group,
	(APTR)	gtk_radio_button_new_with_label_from_widget,
	(APTR)	gtk_range_set_update_policy,
	(APTR)	gtk_scale_set_digits,
	(APTR)	gtk_scale_set_draw_value,
	(APTR)	gtk_scale_set_value_pos,
	(APTR)	gtk_table_attach_defaults,
	(APTR)	gtk_tooltips_disable,
	(APTR)	gtk_tooltips_enable,
	(APTR)	gtk_tooltips_set_tip,
	(APTR)	gtk_vscale_new,
	(APTR)	gtk_widget_ref,
	(APTR)	gtk_widget_set_size_request,
	(APTR)	gtk_widget_unref,
	(APTR)	gtk_window_set_resizable,

	(APTR)   -1,

	(APTR)   FUNCARRAY_END
};

static const struct MyInitData InitData	=
{
	0xa0,8,  NT_LIBRARY,0,
	0xa0,9,  0xfb,0,					/* 0xfb -> priority -5 */
	0x80,10, (ULONG)&LibName[0],
	0xa0,14, LIBF_SUMUSED|LIBF_CHANGED,0,
	0x90,20, COMPILE_VERSION,
	0x90,22, COMPILE_REVISION,
	0x80,24, (ULONG)&VerString[7],
	0
};

static const ULONG InitTable[] =
{
	sizeof(struct MyLibrary),
	(ULONG)	FuncTable,
	(ULONG)	&InitData,
	(ULONG)	LIB_Init
};

const struct Resident RomTag	=
{
	RTC_MATCHWORD,
	(struct Resident *)&RomTag,
	(struct Resident *)&RomTag+1,
	RTF_AUTOINIT | RTF_PPC | RTF_EXTENDED,
	COMPILE_VERSION,
	NT_LIBRARY,
	0,
	(char *)&LibName[0],
	(char *)&VerString[7],
	(APTR)&InitTable[0],
	COMPILE_REVISION, NULL
};

/**********************************************************************
	Globals
**********************************************************************/

#ifdef __MORPHOS__
const ULONG __abox__	= 1;
#endif

struct ExecBase *SysBase;

/**********************************************************************
	LIB_Reserved
**********************************************************************/

static ULONG LIB_Reserved(void)
{
	return 0;
}

/**********************************************************************
	LIB_Init
**********************************************************************/

static struct Library *LIB_Init(struct MyLibrary *LibBase, BPTR SegList, struct ExecBase *MySysBase)
{
	LibBase->SegList	= SegList;
	SysBase				= MySysBase;
	return &LibBase->Library;
}

/**********************************************************************
	RemoveLibrary
**********************************************************************/

static VOID RemoveLibrary(struct MyLibrary *LibBase)
{
	Remove(&LibBase->Library.lib_Node);
	FreeMem((APTR)((ULONG)(LibBase) - (ULONG)(LibBase->Library.lib_NegSize)), LibBase->Library.lib_NegSize + LibBase->Library.lib_PosSize);
}

/**********************************************************************
	LIB_Expunge
 ********************************************************************/

static BPTR LIB_Expunge(void)
{
	struct MyLibrary *LibBase = (struct MyLibrary *)REG_A6;

	if (LibBase->Library.lib_OpenCnt == 0)
	{
		BPTR seglist = LibBase->SegList;

		RemoveLibrary(LibBase);
		return seglist;
	}

	LibBase->Library.lib_Flags |= LIBF_DELEXP;
	return 0;
}

/**********************************************************************
	LIB_Close
**********************************************************************/

static BPTR LIB_Close(void)
{
	struct MyLibrary *LibBase = (struct MyLibrary *)REG_A6;
	BPTR	SegList	= 0;

	LibBase->Library.lib_OpenCnt--;

	if (LibBase->Library.lib_Flags & LIBF_DELEXP && LibBase->Library.lib_OpenCnt == 0)
	{
		SegList = LibBase->SegList;
		RemoveLibrary(LibBase);
	}

	return SegList;
}

/**********************************************************************
	LIB_Open
**********************************************************************/

static struct Library *LIB_Open(void)
{
	struct MyLibrary *LibBase = (struct MyLibrary *)REG_A6;
	LibBase->Library.lib_Flags &= ~LIBF_DELEXP;
	LibBase->Library.lib_OpenCnt++;
	return &LibBase->Library;
}
