# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

abbreviations = {
    'everything' : 'Everything',
    'libraries'  : 'Libraries',
    'devices'    : 'Devices',
    'classes'    : 'Classes',
    'gadgets'    : 'Gadgets',
    'linklibs'   : 'Linklibs',
    'hidds'      : 'HIDDs',
    'resources'  : 'Resources',
    'programs'   : 'Programs',
    'utilities'  : 'Utilities',
    'system'     : 'System',

    'boopsi' : 'BOOPSI Classes',
    'alib_commodities' : 'Amiga.lib - Commodities',
    'alib_stdio' : 'Amiga.lib - Standard I/O',
    'alib' : 'Amiga.lib',
    'amigaguide' : 'AmigaGuide',
    'arp' : 'ARP',
    'asl' : 'Asl',
    'battclock' : 'Battery backedup clock',
    'battmem' : 'Battery backedup memory',
    'bullet' : 'Bullet.library (Fonts)',
    'c' : 'Shell commands',
    'camd' : 'Camd.library',
    'cardres' : 'Card.resource',
    'cia' : 'Cia.resource',
    'clib' : 'ANSI C link library',
    'colorwheel' : 'Colorwheel Gadget',
    'commodities' : 'Commodities.library',
    'console' : 'Console.device',
    'DataType' : 'Datatypes',
    'datatypes' : 'Datatypes.library',
    'DevCMD' : 'Device commands',
    'disk' : 'Disk.resource',
    'diskfont' : 'Diskfont.library',
    'dos' : 'Dos.library',
    'dtclass' : 'DTClass',
    'exec' : 'Exec.library',
    'expansion' : 'Expansion.library',
    'gadtools' : 'GadTools.library',
    'gradientslider' : 'Gradientslider Gadget',
    'graphics' : 'Graphics.library',
    'HID' : 'Design a new HIDD',
    'HIDG' : 'Graphics.HIDD',
    'icon' : 'Icon.library',
    'iffparse' : 'IFFParse.library',
    'input' : 'Input.device',
    'intuition' : 'Intuition.library',
    'keymap' : 'Keymap.library',
    'layers' : 'Layers.library',
    'locale' : 'Locale.library',
    'lowlevel' : 'LowLevel.library',
    'mathffp' : 'MathFFP.library',
    'mathieeedoubbas' : 'MathIEEEDoubleBase',
    'mathieeedoubtrans' : 'MathIEEEDoubleTrans',
    'mathieeesingbas' : 'MathIEEESingleBase',
    'mathieeesingtrans' : 'MathIEEESingleTrans',
    'mathtrans' : 'MathTrans',
    'misc' : 'Misc.resource',
    'multiview' : 'MultiView',
    'nonvolatile' : 'NonVolatile.library',
    'other' : 'Other tasks',
    'potgo' : 'Potgo.resource',
    'prefs' : 'Preferences',
    'ramdrive' : 'Ramdrive.device',
    'realtime' : 'RealTime.library',
    'reqtools' : 'ReqTools.library',
    'rexxsyslib' : 'RexxSys.library',
    'rexxsupport' : 'RexxSupport.library',
    'rexxmast' : 'RexxMast Program',
    'timer' : 'Timer.device',
    'translator' : 'Translator.library',
    'utility' : 'Utility.library',
    'wb' : 'Workbench.library'
}

categories = {
    'libraries' : [
        'intuition',
        'graphics',
        'dos',
        'translator',
        'utility',
        'wb',
        'rexxsyslib',
        'reqtools',
        'realtime',
        'nonvolatile',
        'mathtrans',
        'icon',
        'iffparse',
        'input',
        'keymap',
        'layers',
        'locale',
        'lowlevel',
        'mathffp',
        'mathieeedoubbas',
        'mathieeedoubtrans',
        'mathieeesingbas',
        'mathieeesingtrans',
        'mathtrans',
        'gadtools',
        'expansion',
        'exec',
        'diskfont',
        'datatypes',
        'commodities',
        'camd',
        'bullet',
        'asl',
        'arp',
        'amigaguide'
    ],
    
    'devices' : [
        'serial',
        'timer',
        'console',
        'ramdrive',
        'DevCMD'
    ],
    
    'resources' : [
        'potgo',
        'misc',
        'disk',
        'cia',
        'cardres',
        'battmem',
        'battclock'
    ],
    
    'gadgets' : [
        'gradientslider',
        'colorwheel'
    ],
        
    'classes' : [
        'gadgets',
        'DataType',
        'dtclass',
        'boopsi'
    ],
    
    'hidds' : [
        'HID',
        'HIDG'
    ],
    
    'utilities' : [
        'multiview'
    ],
    
    'system' : [
        'rexxmast'
    ],
    
    'programs' : [
        'prefs',
        'system',
        'utilities',
        'tools',
        'c'
    ],
    
    'linklibs' : [
        'clib',
        'alib',
        'alib_stdio',
        'alib_commodities'
    ],
    
    'everything' : [
        'libraries',
        'classes',
        'devices',
        'resources',
        'hidds',
        'programs',
        'linklibs',
        'other'
    ]   
}

