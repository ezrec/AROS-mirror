/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

/* "dopus.library"                                             */
/*                                                             */
/* Support routines for Directory Opus and associated programs */
/* (c) Copyright 1992 Jonathan Potter                          */
/*                                                             */
#pragma libcall DOpusBase FileRequest 1E 801
#pragma libcall DOpusBase Do3DBox 24 543210807
#pragma libcall DOpusBase Do3DStringBox 2A 543210807
#pragma libcall DOpusBase Do3DCycleBox 30 543210807
#pragma libcall DOpusBase DoArrow 36 6543210808
/*pragma libcall DOpusBase LSprintf 3C 0*/
#pragma libcall DOpusBase LCreateExtIO 42 0802
#pragma libcall DOpusBase LCreatePort 48 0802
#pragma libcall DOpusBase LDeleteExtIO 4E 801
#pragma libcall DOpusBase LDeletePort 54 801
#pragma libcall DOpusBase LToUpper 5A 001
#pragma libcall DOpusBase LToLower 60 001
#pragma libcall DOpusBase LStrCat 66 9802
#pragma libcall DOpusBase LStrnCat 6C 09803
#pragma libcall DOpusBase LStrCpy 72 9802
#pragma libcall DOpusBase LStrnCpy 78 09803
#pragma libcall DOpusBase LStrCmp 7E 9802
#pragma libcall DOpusBase LStrnCmp 84 09803
#pragma libcall DOpusBase LStrCmpI 8A 9802
#pragma libcall DOpusBase LStrnCmpI 90 09803
#pragma libcall DOpusBase StrCombine 96 0A9804
#pragma libcall DOpusBase StrConcat 9C 09803
#pragma libcall DOpusBase LParsePattern A2 9802
#pragma libcall DOpusBase LMatchPattern A8 9802
#pragma libcall DOpusBase LParsePatternI AE 9802
#pragma libcall DOpusBase LMatchPatternI B4 9802
#pragma libcall DOpusBase BtoCStr BA 09803
#pragma libcall DOpusBase Assign C0 9802
#pragma libcall DOpusBase BaseName C6 801
#pragma libcall DOpusBase CompareLock CC 9802
#pragma libcall DOpusBase PathName D2 09803
#pragma libcall DOpusBase SendPacket D8 190804
#pragma libcall DOpusBase TackOn DE 09803
#pragma libcall DOpusBase StampToStr E4 801
#pragma libcall DOpusBase StrToStamp EA 801
#pragma libcall DOpusBase AddListView F0 0802
#pragma libcall DOpusBase ListViewIDCMP F6 9802
#pragma libcall DOpusBase RefreshListView FC 0802
#pragma libcall DOpusBase RemoveListView 102 0802
#pragma libcall DOpusBase DrawCheckMark 108 210804
#pragma libcall DOpusBase FixSliderBody 10E 2109805
#pragma libcall DOpusBase FixSliderPot 114 32109806
#pragma libcall DOpusBase GetSliderPos 11A 10803
#pragma libcall DOpusBase LAllocRemember 120 10803
#pragma libcall DOpusBase LFreeRemember 126 801
#pragma libcall DOpusBase SetBusyPointer 12C 801
#pragma libcall DOpusBase GetWBScreen 132 801
#pragma libcall DOpusBase SearchPathList 138 09803
#pragma libcall DOpusBase CheckExist 13E 9802
#pragma libcall DOpusBase CompareDate 144 9802
#pragma libcall DOpusBase Seed 14A 001
#pragma libcall DOpusBase Random 150 001
#pragma libcall DOpusBase StrToUpper 156 9802
#pragma libcall DOpusBase StrToLower 15C 9802
#pragma libcall DOpusBase RawkeyToStr 162 2981005
#pragma libcall DOpusBase DoRMBGadget 168 9802
#pragma libcall DOpusBase AddGadgets 16E 3210A9807
#pragma libcall DOpusBase ActivateStrGad 174 9802
#pragma libcall DOpusBase RefreshStrGad 17A 9802
#pragma libcall DOpusBase CheckNumGad 180 109804
#pragma libcall DOpusBase CheckHexGad 186 109804
#pragma libcall DOpusBase Atoh 18C 0802
#pragma libcall DOpusBase HiliteGad 192 9802
#pragma libcall DOpusBase DoSimpleRequest 198 9802
/*                                                             */
/* Directory Opus private functions                            */
/*                                                             */
#pragma libcall DOpusBase ReadConfig 19E 9802
#pragma libcall DOpusBase SaveConfig 1A4 9802
#pragma libcall DOpusBase DefaultConfig 1AA 801
#pragma libcall DOpusBase GetDevices 1B0 801
#pragma libcall DOpusBase AssignGadget 1B6 A910805
#pragma libcall DOpusBase AssignMenu 1BC A90804
#pragma libcall DOpusBase FindSystemFile 1C2 109804
/*                                                             */
/*                                                             */
/*                                                             */
#pragma libcall DOpusBase Do3DFrame 1C8 5493210808
#pragma libcall DOpusBase FreeConfig 1CE 801
#pragma libcall DOpusBase DoCycleGadget 1D4 0A9804
#pragma libcall DOpusBase UScoreText 1DA 2109805
#pragma libcall DOpusBase DisableGadget 1E0 109804
#pragma libcall DOpusBase EnableGadget 1E6 109804
#pragma libcall DOpusBase GhostGadget 1EC 109804
#pragma libcall DOpusBase DrawRadioButton 1F2 543210807
#pragma libcall DOpusBase GetButtonImage 1F8 854321007
#pragma libcall DOpusBase ShowSlider 1FE 9802
#pragma libcall DOpusBase CheckConfig 204 801
#pragma libcall DOpusBase GetCheckImage 20A 821004
/*                                                             */
#pragma libcall DOpusBase OpenRequester 210 801
#pragma libcall DOpusBase CloseRequester 216 801
#pragma libcall DOpusBase AddRequesterObject 21C 9802
#pragma libcall DOpusBase RefreshRequesterObject 222 9802
#pragma libcall DOpusBase ObjectText 228 493210807
/**/
#pragma libcall DOpusBase DoGlassImage 22E 2109805
#pragma libcall DOpusBase Decode_RLE 234 09803
#pragma libcall DOpusBase ReadStringFile 23A 9802
#pragma libcall DOpusBase FreeStringFile 240 801
#pragma libcall DOpusBase LFreeRemEntry 246 9802
/**/
#pragma libcall DOpusBase AddGadgetBorders 24C 2109805
#pragma libcall DOpusBase CreateGadgetBorders 252 432A910808
#pragma libcall DOpusBase SelectGadget 258 9802
/*                                                             */
/* end "dopus.library" pragmas */
