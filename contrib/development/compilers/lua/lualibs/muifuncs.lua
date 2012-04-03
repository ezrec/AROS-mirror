--[[
/******************************************************************************
* $Id$
*
* Copyright (C) 2006 Matthias Rustler.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
]]

function mui.MUIV_Window_AltHeight_MinMax(p) return -p end
function mui.MUIV_Window_AltHeight_Visible(p) return -100-p end
function mui.MUIV_Window_AltHeight_Screen(p) return -200-p end
function mui.MUIV_Window_AltTopEdge_Delta(p) return -3-p end
function mui.MUIV_Window_AltWidth_MinMax(p) return -p end
function mui.MUIV_Window_AltWidth_Visible(p) return -100-p end
function mui.MUIV_Window_AltWidth_Screen(p) return -200-p end
function mui.MUIV_Window_Height_MinMax(p) return -p end
function mui.MUIV_Window_Height_Visible(p) return -100-p end
function mui.MUIV_Window_Height_Screen(p) return -200-p end
function mui.MUIV_Window_TopEdge_Delta(p) return -3-p end
function mui.MUIV_Window_Width_MinMax(p) return -p end
function mui.MUIV_Window_Width_Visible(p) return -100-p end
function mui.MUIV_Window_Width_Screen(p) return -200-p end

-- Some nice functions for creating your object tree
function mui.MenustripObject(...)
  return mui.new(mui.MUIC_Menustrip, ...)
end

function mui.MenuObject(...)
  return mui.new(mui.MUIC_Menu, ...)
end

function mui.MenuObjectT(name, ...)
  return mui.new(mui.MUIC_Menu, mui.MUIA_Menu_Title, name, ...)
end

function mui.MenuitemObject(...)
  return mui.new(mui.MUIC_Menuitem, ...)
end

function mui.WindowObject(...)
  return mui.new(mui.MUIC_Window, ...)
end

function mui.ImageObject(...)
  return mui.new(mui.MUIC_Image, ...)
end

function mui.ImagedisplayObject(...)
  return mui.new(mui.MUIC_Imagedisplay, ...)
end

function mui.BitmapObject(...)
  return mui.new(mui.MUIC_Bitmap, ...)
end

function mui.BodychunkObject(...)
  return mui.new(mui.MUIC_Bodychunk, ...)
end

function mui.ChunkyImageObject(...)
  return mui.new(mui.MUIC_ChunkyImage, ...)
end

function mui.NotifyObject(...)
  return mui.new(mui.MUIC_Notify, ...)
end

function mui.ApplicationObject(...)
  return mui.new(mui.MUIC_Application, ...)
end

function mui.TextObject(...)
  return mui.new(mui.MUIC_Text, ...)
end

function mui.RectangleObject(...)
  return mui.new(mui.MUIC_Rectangle, ...)
end

function mui.BalanceObject(...)
  return mui.new(mui.MUIC_Balance, ...)
end

function mui.ListObject(...)
  return mui.new(mui.MUIC_List, ...)
end

function mui.PropObject(...)
  return mui.new(mui.MUIC_Prop, ...)
end

function mui.StringObject(...)
  return mui.new(mui.MUIC_String, ...)
end

function mui.ScrollbarObject(...)
  return mui.new(mui.MUIC_Scrollbar, ...)
end

function mui.ListviewObject(...)
  return mui.new(mui.MUIC_Listview, ...)
end

function mui.RadioObject(...)
  return mui.new(mui.MUIC_Radio, ...)
end

function mui.VolumelistObject(...)
  return mui.new(mui.MUIC_Volumelist, ...)
end

function mui.FloattextObject(...)
  return mui.new(mui.MUIC_Floattext, ...)
end

function mui.DirlistObject(...)
  return mui.new(mui.MUIC_Dirlist, ...)
end

function mui.CycleObject(...)
  return mui.new(mui.MUIC_Cycle, ...)
end

function mui.GaugeObject(...)
  return mui.new(mui.MUIC_Gauge, ...)
end

function mui.ScaleObject(...)
  return mui.new(mui.MUIC_Scale, ...)
end

function mui.NumericObject(...)
  return mui.new(mui.MUIC_Numeric, ...)
end

function mui.SliderObject(...)
  return mui.new(mui.MUIC_Slider, ...)
end

function mui.NumericbuttonObject(...)
  return mui.new(mui.MUIC_Numericbutton, ...)
end

function mui.KnobObject(...)
  return mui.new(mui.MUIC_Knob, ...)
end

function mui.LevelmeterObject(...)
  return mui.new(mui.MUIC_Levelmeter, ...)
end

function mui.BoopsiObject(...)
  return mui.new(mui.MUIC_Boopsi, ...)
end

function mui.ColorfieldObject(...)
  return mui.new(mui.MUIC_Colorfield, ...)
end

function mui.PenadjustObject(...)
  return mui.new(mui.MUIC_Penadjust, ...)
end

function mui.ColoradjustObject(...)
  return mui.new(mui.MUIC_Coloradjust, ...)
end


function mui.PaletteObject(...)
  return mui.new(mui.MUIC_Palette, ...)
end

function mui.GroupObject(...)
  return mui.new(mui.MUIC_Group, ...)
end

function mui.RegisterObject(...)
  return mui.new(mui.MUIC_Register, ...)
end

function mui.VirtgroupObject(...)
  return mui.new(mui.MUIC_Virtgroup, ...)
end

function mui.ScrollgroupObject(...)
  return mui.new(mui.MUIC_Scrollgroup, ...)
end

function mui.PopstringObject(...)
  return mui.new(mui.MUIC_Popstring, ...)
end

function mui.PopobjectObject(...)
  return mui.new(mui.MUIC_Popobject, ...)
end

function mui.PoplistObject(...)
  return mui.new(mui.MUIC_Poplist, ...)
end

function mui.PopscreenObject(...)
  return mui.new(mui.MUIC_Popscreen, ...)
end

function mui.PopaslObject(...)
  return mui.new(mui.MUIC_Popasl, ...)
end

function mui.PendisplayObject(...)
  return mui.new(mui.MUIC_Pendisplay, ...)
end

function mui.PoppenObject(...)
  return mui.new(mui.MUIC_Poppen, ...)
end

function mui.CrawlingObject(...)
  return mui.new(mui.MUIC_Crawling, ...)
end

-- The following in zune only
function mui.PopimageObject(...)
  return mui.new(mui.MUIC_Popimage, ...)
end

function mui.PopframeObject(...)
  return mui.new(mui.MUIC_Popframe, ...)
end

function mui.AboutmuiObject(...)
  return mui.new(mui.MUIC_Aboutmui, ...)
end

function mui.ScrmodelistObject(...)
  return mui.new(mui.MUIC_Scrmodelist, ...)
end

function mui.KeyentryObject(...)
  return mui.new(mui.MUIC_Keyentry, ...)
end

function mui.VGroup(...)
  return mui.new(mui.MUIC_Group, ...)
end

function mui.HGroup(...)
  return mui.new(mui.MUIC_Group, mui.MUIA_Group_Horiz, true, ...)
end

function mui.ColGroup(columns, ...)
  return mui.new(mui.MUIC_Group, mui.MUIA_Group_Columns, columns, ...)
end

function mui.RowGroup(rows, ...)
  return mui.new(mui.MUIC_Group, mui.MUIA_Group_Rows, rows, ...)
end

function mui.PageGroup(...)
  return mui.new(mui.MUIC_Group, mui.MUIA_Group_PageMode, true, ...)
end

function mui.VGroupV(...)
  return mui.new(mui.MUIC_Virtgroup, ...)
end

function mui.HGroupV(...)
  return mui.new(mui.MUIC_Virtgroup, mui.MUIA_Group_Horiz, true, ...)
end

function mui.ColGroupV(columns, ...)
  return mui.new(mui.MUIC_Virtgroup, mui.MUIA_Group_Columns, columns, ...)
end

function mui.RowGroupV(rows, ...)
  return mui.new(mui.MUIC_Virtgroup, mui.MUIA_Group_Rows, rows, ...)
end

function mui.PageGroupV(...)
  return mui.new(mui.MUIC_Virtgroup, mui.MUIA_Group_PageMode, true, ...)
end

function mui.RegisterGroup(ts, ...)
  return mui.new(mui.MUIC_Register, mui.MUIA_Register_Titles, ts, ...)
end

function mui.IconListObject(...)
  return mui.new(mui.MUIC_IconList, ...)
end

function mui.IconVolumeListObject(...)
  return mui.new(mui.MUIC_IconVolumeList, ...)
end

function mui.IconDrawerListObject(...)
  return mui.new(mui.MUIC_IconDrawerList, ...)
end

function mui.IconListviewObject(...)
  return mui.new(mui.MUIC_IconListview, ...)
end


-- Space objects
function mui.HVSpace()
  return mui.new(mui.MUIC_Rectangle)
end

function mui.HSpace(x)
  return mui.make(mui.MUIO_HSpace, x)
end

function mui.VSpace(x)
  return mui.make(mui.MUIO_VSpace, x)
end

function mui.HBar(x)
  return mui.make(mui.MUIO_HBar, x)
end

function mui.VBar(x)
  return mui.make(mui.MUIO_VBar, x)
end

function mui.HCenter(obj)
  return mui.HGroup(
    mui.MUIA_Group_Spacing, 0,
    mui.Child, mui.HSpace(0),
    mui.Child, obj,
    mui.Child, mui.HSpace(0)
  )
end

function mui.VCenter(obj)
  return mui.VGroup(
    mui.MUIA_Group_Spacing, 0,
    mui.Child, mui.VSpace(0),
    mui.Child, obj,
    mui.Child, mui.VSpace(0)
  )
end


-- These macros will create a simple button. It's simply calling mui.make
function mui.SimpleButton(label)
  return mui.make(mui.MUIO_Button, label)
end

function mui.ImageButton(label, imagePath)
  return mui.make(mui.MUIO_ImageButton, label, imagePath)
end

function mui.CoolImageButton(label,image)
  return mui.make(mui.MUIO_CoolButton, label, image, 0)
end

function mui.CoolImageIDButton(label,imageid)
  return mui.make(mui.MUIO_CoolButton, label, imageid, MUIO_CoolButton_CoolImageID)
end

-- Use this for getting a pop button
function mui.PopButton(img)
  return mui.make(mui.MUIO_PopButton, img)
end


--[[
 Macros for Labelobjects
 Use them for example in a group containing 2 columns, in the first
 columns the label and in the second columns the object.

 These objects should be uses because the user might have set strange
 values.

 xxxLabel() is suited for Objects without frame
 xxxLabel1() is suited for objects with a single frame, like buttons
 xxxLabel2() is suited for objects with with double frames, like string gadgets
]]

-- Right aligned
function mui.Label(label)
  return mui.make(mui.MUIO_Label, label, 0)
end

function mui.Label1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_SingleFrame)
end

function mui.Label2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_DoubleFrame)
end

--Left aligned
function mui.LLabel(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned)
end

function mui.LLabel1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned + mui.MUIO_Label_SingleFrame)
end

function mui.LLabel2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned + mui.MUIO_Label_DoubleFrame)
end

-- Centered
function mui.CLabel(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered)
end

function mui.CLabel1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered + mui.MUIO_Label_SingleFrame)
end

function mui.CLabel2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered + mui.MUIO_Label_DoubleFrame)
end

-- Freevert - Right aligned
function mui.FreeLabel(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert)
end

function mui.FreeLabel1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_SingleFrame)
end

function mui.FreeLabel2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_DoubleFrame)
end

-- Freevert - Left aligned
function mui.FreeLLabel(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned)
end

function mui.FreeLLabel1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned + mui.MUIO_Label_SingleFrame)
end

function mui.FreeLLabel2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned + mui.MUIO_Label_DoubleFrame)
end

-- Freevert - Centered
function mui.FreeCLabel(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered)
end

function mui.FreeCLabel1(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered + mui.MUIO_Label_SingleFrame)
end

function mui.FreeCLabel2(label)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered + mui.MUIO_Label_DoubleFrame)
end

-- The same as above + keys
function mui.KeyLabel(label,key)
  return mui.make(mui.MUIO_Label, label, string.byte(key))
end

function mui.KeyLabel1(label,key)
  return mui.make(mui.MUIO_Label, (label), mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.KeyLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_DoubleFrame + string.byte(key))
end

function mui.KeyLLabel(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned + string.byte(key))
end

function mui.KeyLLabel1(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned + mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.KeyLLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_LeftAligned + mui.MUIO_Label_DoubleFrame + string.byte(key))
end

function mui.KeyCLabel(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered + string.byte(key))
end

function mui.KeyCLabel1(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered + mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.KeyCLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_Centered + mui.MUIO_Label_DoubleFrame + string.byte(key))
end

function mui.FreeKeyLabel(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + string.byte(key))
end

function mui.FreeKeyLabel1(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.FreeKeyLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_DoubleFrame + string.byte(key))
end

function mui.FreeKeyLLabel(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned + string.byte(key))
end

function mui.FreeKeyLLabel1(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned + mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.FreeKeyLLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_LeftAligned + mui.MUIO_Label_DoubleFrame + string.byte(key))
end

function mui.FreeKeyCLabel(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered + string.byte(key))
end

function mui.FreeKeyCLabel1(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered + mui.MUIO_Label_SingleFrame + string.byte(key))
end

function mui.FreeKeyCLabel2(label,key)
  return mui.make(mui.MUIO_Label, label, mui.MUIO_Label_FreeVert + mui.MUIO_Label_Centered + mui.MUIO_Label_DoubleFrame + string.byte(key))
end

-- Misc
function mui.nnset(obj, attr, value)
  obj:set(mui.MUIA_NoNotify, true, attr, value)
end

function mui.setmutex(obj, n)
  obj:set(mui.MUIA_Radio_Active, n)
end

function mui.setcycle(obj, n)
  obj:set(mui.MUIA_Cycle_Active, n)
end

function mui.setstring(obj, s)
  obj:set(mui.MUIA_String_Contents, s)
end

function mui.setcheckmark(obj, b)
  obj:set(mui.MUIA_Selected, b)
end

function mui.setslider(obj, l)
  obj:set(mui.MUIA_Numeric_Value, l)
end

