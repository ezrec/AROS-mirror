/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef STRUCTURES_H
#define STRUCTURES_H 1

/*------------------------------------------------------------------------*/

/* pre-defines for AutoDocMessage */
struct AutoDocFunc;
struct AutoDocFile;

/*------------------------------------------------------------------------*/

enum Types
{
	TYP_ROOT = 0,
	TYP_GROUP,
	TYP_FILE,
	TYP_DRAWER,
	TYP_PARENT,
	TYP_FUNC
};

enum Groups
{
	GRP_NDK = 0,
	GRP_MCC,
	GRP_MUI,
	GRP_OTHER,
	GRP_FILTER
};

enum Modes
{
	MODE_AUTOGROUP = 0,
	MODE_NORMAL
};

struct AutoDocMessage
{
	struct MUI_NListtree_TreeNode 	*adm_Node;
	struct AutoDocFile					*adm_File;
	struct AutoDocFunc					*adm_Func;
	enum Types 					 			 adm_Type;
	enum Groups								 adm_Group;
};

struct AutoDocFunc
{
	struct MinNode 			 adf_Node;
	struct AutoDocMessage	 adf_ListMsg;

	UBYTE 						 adf_FuncName[SIZE_NAME];
	UBYTE 						 adf_FuncNameShort[SIZE_NAME];
	UBYTE 						*adf_Data;
	UBYTE							*adf_High;
	LONG 							 adf_Size;
	enum Types					 adf_Type;
	UBYTE 						 adf_Flags;
};

struct AutoDocFile
{
	struct MinNode 			 adf_Node;
	struct AutoDocMessage	 adf_ListMsg;
	UWORD 					 	 adf_ListFlags;

	struct MinList 			 adf_Funcs;
	UBYTE 						 adf_FileName[SIZE_NAME];
	UBYTE 						 adf_FileNameShort[SIZE_NAME];
	enum Types					 adf_Type;
	enum Groups 				 adf_Group;
	UBYTE 						 adf_Flags;
};

struct AutoDocs
{
	struct MinList 			 ad_Files;
	UBYTE 						 ad_Path[SIZE_PATH];
	UBYTE 						 ad_Filter[SIZE_NAME];
	LONG 							 ad_TotalFiles;
	LONG 							 ad_TotalFuncs;
	enum Modes 					 ad_Mode;
	UBYTE 						 ad_Flags;
};

struct AutoDocsPrefs
{
	UBYTE 						 adp_Path[SIZE_PATH];
	enum Modes 					 adp_Mode;
};

/*------------------------------------------------------------------------*/

#endif /* STRUCTURES_H */

