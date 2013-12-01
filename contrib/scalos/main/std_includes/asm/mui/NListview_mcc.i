;;;/*
;;  NListview.mcc (c) Copyright 1996 by Gilles Masson
;;  Registered MUI class, Serial Number: 1d51                            $9d510020 to $9d51002F
;;  *** use only YOUR OWN Serial Number for your public custom class ***
;;  NListview_mcc.h
;;*/

	IFND	MUI_NListview_MCC_I
MUI_NListview_MCC_I	set	1

	IFND	LIBRARIES_MUI_H
	include "libraries/mui.i"
	ENDIF

	IFND	MUI_NList_MCC_H
	include "MUI/NList_mcc.i"
	ENDIF

;;#define MUIC_NListview "NListview.mcc"
;;#define NListviewObject MUI_NewObject(MUIC_NListview


;/* Attributes */

MUIA_NListview_NList:		equ	$9d510020 ;/* GM  i.g Object *          */

MUIA_NListview_Vert_ScrollBar:		equ	$9d510021 ;/* GM  isg LONG              */
MUIA_NListview_Horiz_ScrollBar:		equ	$9d510022 ;/* GM  isg LONG              */
MUIA_NListview_VSB_Width:		equ	$9d510023 ;/* GM  ..g LONG              */
MUIA_NListview_HSB_Height:		equ	$9d510024 ;/* GM  ..g LONG              */

MUIV_NListview_ScrollerPos_Default:		equ	0
MUIV_NListview_ScrollerPos_Left:		equ	1
MUIV_NListview_ScrollerPos_Right:		equ	2
MUIV_NListview_ScrollerPos_None:		equ	3

MUIM_NListview_QueryBeginning:		equ	MUIM_NList_QueryBeginning ;/* obsolete */

MUIV_NListview_VSB_Always:		equ	1
MUIV_NListview_VSB_Auto:		equ	2
MUIV_NListview_VSB_FullAuto:		equ	3
MUIV_NListview_VSB_None:		equ	4
MUIV_NListview_VSB_Default:		equ	5
MUIV_NListview_VSB_Left:		equ	6

MUIV_NListview_HSB_Always:		equ	1
MUIV_NListview_HSB_Auto:		equ	2
MUIV_NListview_HSB_FullAuto:		equ	3
MUIV_NListview_HSB_None:		equ	4
MUIV_NListview_HSB_Default:		equ	5

MUIV_NListview_VSB_On:		equ	$0030
MUIV_NListview_VSB_Off:		equ	$0010

MUIV_NListview_HSB_On:		equ	$0300
MUIV_NListview_HSB_Off:		equ	$0100


	ENDIF ;/* MUI_NListview_MCC_I */
