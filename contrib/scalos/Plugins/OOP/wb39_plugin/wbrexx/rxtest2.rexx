/* rxtest2.rexx */

OPTIONS RESULTS

ADDRESS WORKBENCH

activeWin = "HD1-6GB:Bilder"

GETATTR	WINDOW.ICONS.UNSELECTED.0 NAME '"'||activeWin||'"' STEM IconInfo.0
say IconInfo.0.name

GETATTR	WINDOW.ICONS.SELECTED.0 NAME '"'||activeWin||'"' STEM IconInfo.0
say IconInfo.0.name

GETATTR	WINDOW.ICONS.ALL.0 NAME '"'||activeWin||'"' STEM IconInfo.0
say IconInfo.0.name

