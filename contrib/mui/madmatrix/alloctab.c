#include <stdlib.h>

/*#define DEBUG*/

void **AllocTab(size_t taille,int nblignes, int nbcolonnes)
{
  void **nouveau;
  int i,j;

  nouveau= ( void **) calloc(nblignes,sizeof(void *));

#ifdef DEBUG
  printf("Allocation de %p !\n",nouveau);
#endif

  if ( nouveau == NULL )
    return(NULL);

  for ( i=0; i< nblignes ; i ++ )
    if ( NULL == (nouveau[i] = calloc(nbcolonnes,taille) ) )
      break;
#ifdef DEBUG
    else
      printf("Allocation de %p\n",nouveau[i]);
#endif

  if ( i < nblignes )
  {
    for ( j=0; j < i ; j++ )
      free(nouveau[j]);

    free(nouveau);
    return(NULL);
  }
  return(nouveau);
}

void FreeTab(void **tab, int nblignes)
{
  int j;

  for ( j=0; j < nblignes ; j++ )
  {
#ifdef DEBUG
    printf("Liberation de %p\n",tab[j]);
#endif
    free(tab[j]);
  }

#ifdef DEBUG
  printf("Liberation de %p !\n",tab);
#endif
  free(tab);
}


