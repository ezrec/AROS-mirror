Before you start working with this port, please read FPC FAQ:s and manuals. Also download 1.0.4 version of compiler for 
linux!

Compiler have target AROS now. Left to do is hundreds of m68k asm parts of system includes, mainly libcall wrappers.
To find places where to make AROS specifig version, seek {AROS VERSION HERE} string and write i386 version of function or
procedure.

HOW TO CALL 5th LIB FUNC WITH 3 PARAMS IN AROS:

movl	<libbase>,%eax
pushl	%eax
pushl	<param3>
pushl	<param2>
pushl	<param1>
movl	-20(%eax),%eax	{5*4=20}
call	*%eax
addl	$16,%esp	{ 4 pushl's --> 4 * 4 = 16}


Startup code of proggies is needed also. It's equivalent in c can be found in:

/??/st.c

Also it's untested that it produces right kind of executables. Use option switch in compiler to leave generated ASM files 
so you can see what went wrong. If asm is allright problem might be in linking. Disable smartlinking and check if there
is some special linker switces not used what aros needs (As it uses host spefic defaults) 

-r -nostartfiles - nostdlib.

should be added.



Files changed in compiler sources 

Files changed in RTL



I hope that somebody has more time to work with this port and more knowledge of both i386&m68k asm than I have.
Sasu
