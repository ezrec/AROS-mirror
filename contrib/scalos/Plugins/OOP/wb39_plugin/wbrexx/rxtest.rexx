/* rxtest.rexx */
/* Test file for WBRexx.plugin */
/* 11 Jan 2001 17:41:47 */

options results

address workbench

say "-----------------------------------------------------"

say "Application.Version"
getattr application.version var xyzzy
res = result

say "result = <" || res || ">"
say "xyzzy = <" || xyzzy || ">"



say "Application.screen"
getattr application.screen var "xyzzy1"
res = result

say "result = <" || res || ">"
say "xyzzy1 = <" || xyzzy1 || ">"


say "Aplication.arexx"
getattr application.arexx var xyzzy2
res = result

say "result = <" || res || ">"
say "xyzzy = <" || xyzzy2 || ">"

if rc > 0 then do
	getattr application.lasterror
	res = result
	fault code res
	say "lasterror =" result
	end


say "Aplication.iconborder"
getattr application.iconborder
res = result
say "result = <" || res || ">"



say "-----------------------------------------------------"

say "windows"
getattr windows.count VAR wincount
say "Windows.Count = <" || wincount || ">"

getattr windows stem win
res = result

do n = 0 to wincount-1
	say "win." || n || " = <" || win.n || ">"
end

if rc > 0 then do
	getattr application.lasterror
	res = result
	fault code res
	say "lasterror =" result
	end
say


wn = wincount - 1

getattr windows.wn var win3
res = result
say "windows.3 = <" || win3 || ">"
say


say "this one ought to return an error:"
wn = wincount + 4
getattr windows.wn var win4
res = result
say "windows.13 = <" || win4 || ">"
if rc > 0 then do
	getattr application.lasterror
	res = result
	fault code res
	say "lasterror =" result
	end
say

/*
say "application"
getattr application stem app1
say "Version =" app1.version
say "LastError =" app1.lasterror
say "IconBorder =" app1.iconborder
*/


say "-----------------------------------------------------"

say "Selected Icons list in Root window"
getattr window.icons.selected name "root" stem rootwin
res = result
if rc > 0 then do
	getattr application.lasterror
	res = result
	fault code res
	say "lasterror =" result
	end

say "Count =" rootwin.count

do n = 0 to rootwin.count-1
	say n
	say "Name =" rootwin.n.name
	say "Left =" rootwin.n.left
	say "Top =" rootwin.n.top
	say "Width =" rootwin.n.width
	say "Height =" rootwin.n.height
	say "Type =" rootwin.n.type
	say "Status =" rootwin.n.status
end
say


say "-----------------------------------------------------"
say "KeyCommands"
getattr keycommands.count VAR kccount
say "keycommands.Count = <" || kccount || ">"

if kccount > 0 then do
	getattr keycommands.0.name var kc
	say "<" || kc || ">"
	getattr keycommands.0.key var kc2
	say "<" || kc2 || ">"
	end


getattr keycommands stem kcc
res = result

do n = 0 to kccount-1
	say "kcc." || n || " Name= <" || kcc.n.name || ">  Key= <" || kcc.n.key || ">  Cmd= <" || kcc.n.command || ">"
end

say "-----------------------------------------------------"
say "MenuCommands"
getattr menucommands.count VAR mccount
say "Menucommands.Count = <" || mccount || ">"

getattr Menucommands stem mcc
res = result

do n = 0 to mccount-1
	say "mcc." || n || " Name= <" || mcc.n.name || ">  Title= <" || mcc.n.title || ">  Shortcut= <" || mcc.n.shortcut || ">  Cmd= <" || mcc.n.command || ">"
end


say "-----------------------------------------------------"

getattr window.screen  name "root" stem rootscreen
say "Screen.name = " rootscreen.name
say "Screen.width = " rootscreen.width
say "Screen.height = " rootscreen.height

say

getattr application.font.icon stem iconfont name "Text"
say "font.icon.name = " iconfont.name
say "font.icon.width = " iconfont.width
say "font.icon.height = " iconfont.height
say "font.icon.size = " iconfont.size
say

getattr application.font.screen stem screenfont name "Text"
say "font.screen.name = " screenfont.name
say "font.screen.width = " screenfont.width
say "font.screen.height = " screenfont.height
say "font.screen.size = " screenfont.size
say

/*
address command "wait"

getattr windows.active
res = result
say "Active Window = <" || res || ">"
*/


/*
icon window "HD1-6GB:Bilder" names "normal spezial" right 30

icon window "HD1-6GB:Bilder" names "normal" select

do n = 0 to 3
	icon window "HD1-6GB:Bilder" cycle previous
	if rc > 0 then do
		getattr application.lasterror
		res = result
		fault code res
		say "lasterror =" result
		leave
		end
	address command "wait"
end
say
*/


icon window "HD1-6GB:PD/BasiliskII"  names "src" open
menu window "HD1-6GB:PD/BasiliskII/src" invoke "WINDOW.SHOW.ALLFILES"

address command "wait"
icon window "HD1-6GB:PD/BasiliskII/src" names "xpram.cpp" makevisible select

view window "HD1-6GB:PD/BasiliskII" up


/*
menu window "HD1-6GB:PD/BasiliskII" invoke "WINDOW.OPENPARENT"
if rc > 0 then do
	fault code workbench.lasterror
	say "lasterror =" result
	end
*/


window "HD1-6GB:PD/BasiliskII" activate
window "active" cycle previous


window "RAM:" open
newdrawer "RAM:xyzzy"
rename "RAM:xyzzy" "RAM:BlahBlah"
if rc > 0 then do
	fault code workbench.lasterror
	say "lasterror =" result
	end


delete "RAM:BlahBlah"
delete "RAM:Spezial" all
