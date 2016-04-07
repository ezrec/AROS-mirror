#ifndef ALLOCTAB_H
#define ALLOCTAB_H

void **AllocTab(size_t taille,int nblignes, int nbcolonnes);
void FreeTab(void **tab, int nblignes);

#endif
