#ifndef AMIGA_RAF_H
#define AMIGA_RAF_H
/*
 * amiga_raf.h - Register Argument Function definitions
 *
 * Last modified: Tue Mar 16 23:20:28 1993 ppessi
 * 
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:21:39  henrik
 * lots of assembler in api/amiga_api.h that needs to be fixed
 * better not do it since my knowledge of assembler i almost zero
 * think one need to change adresses to something like eax,ebx or something?
 *
 * Revision 1.1  1993/03/05  03:25:57  ppessi
 * Initial revision
 *
 * Revision 1.1  93/02/28  22:38:09  22:38:09  ppessi (Pekka Pessi)
 * 	Register Argument Function definitions
 * 
 */

#ifndef API_AMIGA_RAF_H
#define API_AMIGA_RAF_H

/*
 * RAF1...RAF7 gives consistent way to make function take arguments in
 * registers. currently __GNUC__ and __SASC supported.
 */

#if __SASC

#define RAF1(funname, type1, arg1, reg1)	\
 __asm funname(register __##reg1 type1 arg1)	\
{

#define RAF2(funname,type1, arg1, reg1,type2, arg2, reg2) \
 __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2)	\
{

#define RAF3(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3)\
 __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2,	\
	       register __##reg3 type3 arg3)	\
{

#define RAF4(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4) \
 __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2,	\
	       register __##reg3 type3 arg3,	\
	       register __##reg4 type4 arg4)	\
{

#define RAF5(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5) \
  __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2,	\
	       register __##reg3 type3 arg3,	\
	       register __##reg4 type4 arg4,	\
	       register __##reg5 type5 arg5)	\
{

#define RAF6(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5, type6, arg6, reg6) \
 __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2,	\
	       register __##reg3 type3 arg3,	\
	       register __##reg4 type4 arg4,	\
	       register __##reg5 type5 arg5,	\
	       register __##reg6 type6 arg6)	\
{

#define RAF7(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5, type6, arg6, reg6, type7, arg7, reg7) \
 __asm funname(register __##reg1 type1 arg1,	\
	       register __##reg2 type2 arg2,	\
	       register __##reg3 type3 arg3,	\
	       register __##reg4 type4 arg4,	\
	       register __##reg5 type5 arg5,	\
	       register __##reg6 type6 arg6,	\
	       register __##reg7 type7 arg7)	\
{

#elif __GNUC__

/*We'll try calling actually using parameters */
/* Also, standardize prototype declarations */
/*  The func(void) declarations are back, mainly because now all
   functions have inlines to call instead of directly calling
   the functions themselves (and needing to set registers gcc
   will complain about).  But the other type is still there,
   if needed.*/
#define RAF1(funname,type1, arg1, reg1) \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1;                     	
/*
  funname(register type1 reg1 __asm(#reg1))\
    {                                      \
      type1 arg1 = reg1;                   

#define RAFP1(funname,type1, reg1) \
  funname(register type1 reg1 __asm(#reg1))
*/

#define RAF2(funname, type1, arg1, reg1, type2, arg2, reg2) \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2;                     	

/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;
					    


#define RAFP2(funname, type1, reg1, type2, reg2) \
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2))

*/

#define RAF3(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3)\
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2; 		    \
  register type3 reg3 __asm(#reg3); \
  type3 arg3 = reg3;                     	


/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;                   \
       type3 arg3 = reg3;  


#define RAFP3(funname, type1, reg1, type2, reg2, type3, reg3)\
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3))

*/


#define RAF4(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4)		   \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2; 		    \
  register type3 reg3 __asm(#reg3); \
  type3 arg3 = reg3; 		    \
  register type4 reg4 __asm(#reg4); \
  type4 arg4 = reg4;                     	


/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;                   \
       type3 arg3 = reg3;                   \
       type4 arg4 = reg4;

#define RAFP4(funname, type1, reg1, type2, reg2, type3, reg3, type4, reg4)\
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4))
*/

#define RAF5(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5) \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2; 		    \
  register type3 reg3 __asm(#reg3); \
  type3 arg3 = reg3; 		    \
  register type4 reg4 __asm(#reg4); \
  type4 arg4 = reg4; 		    \
  register type5 reg5 __asm(#reg5); \
  type5 arg5 = reg5;                     	



/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;                   \
       type3 arg3 = reg3;                   \
       type4 arg4 = reg4;                   \
       type5 arg5 = reg5;                   

#define RAFP5(funname, type1, reg1, type2, reg2, type3, reg3, type4, reg4, type5, reg5) \
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5))
*/

#define RAF6(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5, type6, arg6, reg6) \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2; 		    \
  register type3 reg3 __asm(#reg3); \
  type3 arg3 = reg3; 		    \
  register type4 reg4 __asm(#reg4); \
  type4 arg4 = reg4; 		    \
  register type5 reg5 __asm(#reg5); \
  type5 arg5 = reg5; 		    \
  register type6 reg6 __asm(#reg6); \
  type6 arg6 = reg6;                     	



/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5),\
	   register type6 reg6 __asm(#reg6))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;                   \
       type3 arg3 = reg3;                   \
       type4 arg4 = reg4;                   \
       type5 arg5 = reg5;                   \
       type6 arg6 = reg6;


#define RAFP6(funname, type1, reg1, type2, reg2, type3, reg3, type4, reg4, type5, reg5, type6, reg6) \
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5),\
	   register type6 reg6 __asm(#reg6))
*/

#define RAF7(funname, type1, arg1, reg1, type2, arg2, reg2, type3, arg3, reg3, type4, arg4, reg4, type5, arg5, reg5, type6, arg6, reg6, type7, arg7, reg7) \
  funname(VOID)			    \
{				    \
  register type1 reg1 __asm(#reg1); \
  type1 arg1 = reg1; 		    \
  register type2 reg2 __asm(#reg2); \
  type2 arg2 = reg2; 		    \
  register type3 reg3 __asm(#reg3); \
  type3 arg3 = reg3; 		    \
  register type4 reg4 __asm(#reg4); \
  type4 arg4 = reg4; 		    \
  register type5 reg5 __asm(#reg5); \
  type5 arg5 = reg5; 		    \
  register type6 reg6 __asm(#reg6); \
  type6 arg6 = reg6; 		    \
  register type7 reg7 __asm(#reg7); \
  type7 arg7 = reg7;                     	



/*
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5),\
	   register type6 reg6 __asm(#reg6),\
	   register type7 reg7 __asm(#reg7))\
     {                                      \
       type1 arg1 = reg1;                   \
       type2 arg2 = reg2;                   \
       type3 arg3 = reg3;                   \
       type4 arg4 = reg4;                   \
       type5 arg5 = reg5;                   \
       type6 arg6 = reg6;                   \
       type7 arg7 = reg7;


#define RAFP7(funname, type1, reg1, type2, reg2, type3, reg3, type4, reg4, type5, reg5, type6, reg6, type7, reg7) \
   funname(register type1 reg1 __asm(#reg1),\
	   register type2 reg2 __asm(#reg2),\
	   register type3 reg3 __asm(#reg3),\
           register type4 reg4 __asm(#reg4),\
           register type5 reg5 __asm(#reg5),\
	   register type6 reg6 __asm(#reg6),\
	   register type7 reg7 __asm(#reg7))
*/

#endif /* __SASC | __GNUC__ */

#endif /* API_AMIGA_RAF_H */

#endif /* !defined(AMIGA_RAF_H) */

