/***************************************
*        
* opti_cvrt.h
*
* http://optimum.citeweb.net/index.html 
*                                       
***************************************/

#ifndef OPTI_CVRT_H
#define OPTI_CVRT_H




/* quelques routines de conversion... 
 * ne pas vous affoler en voyant toutes ces declaration:
 * Un meme codage couleur peut posseder 4 noms differents, selon
 * le point de vue ou on le decrit :
 * par exemple pour un mode RGB (depth 24bits, mais alignement 32 bits):
 *             lARGB pour un int [ARGB] sur une machine Less significant byte first
 *             mBGRA pour un int [BGRA]                 Most
 *             bBGRA pour 4 octets B-G-R-A
 *              ARGB pour un int [ARGB] sur la machine actuellement utilisee
 * => les 3 premieres denominations sont identiques, ainsi que la 4eme sur un PC
 * 
 * Donc en pratique, on utilisera plutot le nom sans prefixe "par defaut"
 * sauf pour specifier sans ambiguité un format(enfin c'est vous qui voyez,
 * et puis de toute facon tout sera bien explique dans l'optilib finale... ;) )
 * 
 * Evidemment ca se casse la gueule pour le 16bits, vu qu'une composante est
 * a cheval sur 2octets... donc ici RGB_565 = un short [RGB] vu par un PC
 */

/* 24/32 bits vers 16 bits */
extern void SOpti_cvrt_RGB_888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);   /* not optimized yet ! */
extern void SOpti_cvrt_BGR_888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);   /* not optimized yet ! */
 

extern void SOpti_cvrt_ARGB_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_bBGRA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_mBGRA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_lARGB_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
	
/* 16 bits vers 24/32 */
extern void SOpti_cvrt_RGB_565_ARGB_8888(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_RGB_565_bBGRA_8888(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_RGB_565_mBGRA_8888(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_RGB_565_lARGB_8888(unsigned char *src, unsigned char *dst, int nbpixels);

/* 24/32 bits exotique ? */
extern void SOpti_cvrt_ABGR_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_bRGBA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_mRGBA_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);
extern void SOpti_cvrt_lABGR_8888_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels);

/* convertit du 8 bits en 16 bits a partir d'une palette 16b */
extern void SOpti_cvrt_PAL_8_RGB_565(unsigned char *src, unsigned char *dst, int nbpixels, unsigned short *palette);

/* convertit du 8 bits en 32 bits a partir d'une palette 32b */
extern void SOpti_cvrt_PAL_8_RGBA_8888(unsigned char *src, unsigned char *dst, int nbpixels, unsigned int *palette);

#endif
