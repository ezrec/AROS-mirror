/* ------  Windows CE dedicated functions ---- */
say "This is a library file, and cannot run alone..."
exit 1

/* --- Initialise some common variables --- */
CeInit:
/* --- Messabe Box options --- */
MB_OK=0
MB_OKCANCEL=1
MB_ABORTRETRYIGNORE=2
MB_YESNOCANCEL=3
MB_YESNO=4
MB_RETRYCANCEL=5
MB_ICONHAND=16
MB_ICONQUESTION=32
MB_ICONEXCLAMATION=48
MB_ICONASTERISK=64

/* --- Predefined Clipboard formats --- */
CF_TEXT=1
CF_BITMAP=2
CF_SYLK=4
CF_DIF=5
CF_TIFF=6
CF_OEMTEXT=7
CF_DIB=8
CF_PALETTE=9
CF_PENDATA=10
CF_RIFF=11
CF_WAVE=12
CF_UNICODETEXT=13

return

/* --- ListFile --- */
ListFile: procedure
	trace o
	dirfiles = Dir(arg(1))
	files = ""
	do forever
		parse var files . +36 fn "0A"x files
		files = files || fn
		if dirfile<>""
			then files = files"0A"x
			else leave
	end
return
