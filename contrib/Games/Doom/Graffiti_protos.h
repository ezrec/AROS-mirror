#ifndef LIB_PROTOS_H
#define LIB_PROTOS_H

#ifdef __cplusplus
extern "C" {
#endif

struct GRF_Screen* OpenChunkyScreen(int);
void       CloseChunkyScreen(struct GRF_Screen* GrfStr);
void        SetChunkyPalette(struct GRF_Screen* GrfStr,long *pal);
void        CopyChunkyScreen(struct GRF_Screen* GrfStr,char *pic);

#ifdef __cplusplus
};
#endif

#endif
