/*
** Startup code: AROS version
**
** unsigned long _start(char *argstr,                 -- A0
**                      unsigned long argsize,        -- D0
**                      struct ExecBase *sysbase);    -- A6 ??
**
** esp+12  sysbase
** esp+8   argsize
** esp+4   argstr
** esp+0   return address
**
*/

  /*----------------------------------------------------*/

   .text
   .align 4

   .globl _start
   .globl start

_start:
start:

  /* Save stack pointer for exit() routine */
  movl	   %esp,STKPTR

  /* Save the exec library base */
  movl     12(%esp),SYSBASE

  /* Save the command line pointer length to CommandLineLen */
  movl     8(%esp),%ecx
  movl     %ecx,CommandLineLen

  /* Save the command line pointer to CommandLine */
  movl     4(%esp),%eax
  movl     %eax,CommandLine
  movl     %eax,__ARGS
  test     %eax,%eax
  jz       .Ldont_nullit


  /* Remove $0a character from end of string */
  cmpb   #0x0a,-1(%eax+%ecx:w)
  jne    .Lcontt

  /* Decrement count by one to remove the $0a character */
  subl   #1,%ecx
 
.Lcontt:
  movb   #0,0(%eax+%ecx:w)	   /* null terminate it */
  movw   %ecx,__ARGC

.Ldont_nullit:
    call   PASCALMAIN
    movl   STKPTR,%esp
    ret

  /*----------------------------------------------------*/

    .data

    .global __ARGS              ; pointer to the arguments
    .global __ARGC              ; number of arguments
    .global CommandLineLen      ; byte length of command line
    .global CommandLine         ;
    .global STKPTR              ; Used to terminate the program, initial SP
    .global SYSBASE             ; exec library base

    .align 4

 __ARGS:        .long   0
 __ARGC:        .word   0
CommandLine:    .long   0
CommandLineLen: .long   0
STKPTR:         .long   0
SYSBASE:        .long   0




