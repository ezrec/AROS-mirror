	;;
	;; Our startup signature is:
	;;   unsigned long program(char *argstr,
	;;                         unsigned long argsize,
	;;                         struct ExecBase *sysbase);
	;;
	;;
	;;    sysbase    16(ebp)
	;;    argsize    12(ebp)
	;;    argstr      8(ebp)
	;;    return      4(ebp)
	;;    old ebp     0(ebp)
	;; 
	;;

; Macro ?
DOSLIBVER	EQU	0	; 37

LIBVECTSIZE	EQU	4

;
; Exec functions
;	
LVOOpenLibrary	EQU	(92 * LIBVECTSIZE)
LVOCloseLibrary	EQU	(69 * LIBVECTSIZE)


;
; Dos functions
;
LVOOpen		EQU	(5 * LIBVECTSIZE)
LVOClose	EQU	(6 * LIBVECTSIZE)
LVORead		EQU	(7 * LIBVECTSIZE)
LVOWrite	EQU	(8 * LIBVECTSIZE)

Mode_old	EQU	1005

	
	global _start
	section .text
_start
	push	ebp
	mov	ebp,esp

.getSysBase
	mov	eax,[ebp+16]
	mov	[sys_base],eax

.Exec_openDos
	push	eax

	push	byte DOSLIBVER
	push	long DOSNAME
	
	mov	eax,[eax-LVOOpenLibrary]
	call	eax
	add	esp,byte 12
	
	mov	[dos_base],eax
	test	eax,eax
	je	near .theend 

.Dos_openCon
	mov	eax,[dos_base]
	push	eax
	
	push	long	Mode_old
	push	long	CONNAME
	
	mov	eax,[eax-LVOOpen]
	call	eax
	add	esp,byte 12
	
	mov	[con_handle],eax
	test	eax,eax
	je	near .Exec_closeDos

.Dos_writeMessage
	mov	eax,[dos_base]
	push	eax
	
	push	long ttexte-ttext
	push	long ttext
	push	long [con_handle]
	
	mov	eax,[eax-LVOWrite]
	call	eax
	add	esp,byte 16

.Dos_waitForReturn
	mov	eax,[dos_base]
	push	eax
	
	push	byte 80
	push	long buffer
	push	long [con_handle]
	
	mov	eax,[eax-LVORead]
	call	eax
	add	esp,byte 16

.Dos_closeCon
	mov	eax,[dos_base]
	push	eax
	
	push	long [con_handle]
	
	mov	eax,[eax-LVOClose]
	call	eax
	add	esp,byte 8

.Exec_closeDos
	mov	eax,[sys_base]
	push	eax
	
	push	long [dos_base]
	
	mov	eax,[eax-LVOCloseLibrary]
	call	eax
	add	esp,byte 8

.theend
	leave
	ret
;------------------------------------------------------------------
part2
	ret
;------------------------------------------------------------------
;------------------------------------------------------------------	
	section .rodata
DOSNAME		db	'dos.library',0
CONNAME		db	'CON:20/10/200/100/** Test console ',0
ttext		db	'Enter some text!   ',0
ttexte

ver		db	'$VER:	test 1.1 (20/2/2002)',0x0a,0
name		db	'test'

	section .data

	section .bss
sys_base	resd	1
dos_base	resd	1
con_handle	resd	1

buffer		resb	80

	end
