/*
        A small library of files routines.
        Bill N. Vlachoudis   V.Vlachoudis@cern.ch
*/

say "This is a library file, and cannot run alone..."
exit 1

/* ----------- return file size, or -1 if file not found --------------- */
filesize: procedure
	trace o
	file = open(arg(1),"rb")
	if file = -1 then return -1
	size = seek(file,0,"eof")
	call close file
return size

/* ----------------- return 0 or 1 if file exists -------------- */
state: procedure
	trace o
	file = open(arg(1),"rb")
	if file = -1 then return 0
	call close file
return 1

/* --------------------- readword ------------------------------ */
/* Read one word from file */
readword: procedure
	trace o
	fin = arg(1)
	word = ""
	do forever
		if eof(fin) then leave
		ch = read(fin,1)
		if c2d(ch) <= 32 then do
			if word ^== "" then leave
		end; else
			word = word || ch
	end
return word

/* the same as state */
exist: return state(arg(1))
