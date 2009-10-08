#if !defined(DRM_REDEFINES_H)
#define DRM_REDEFINES_H

/* THESE REDEFINES ARE NECESSARY AS WE BUILD USER AND KERNEL INTO ONE MODULE
   AND THEY HAVE OVERLAPPING FUNCTION NAMES! */

#define nouveau_dma_channel_init            _redefined_nouveau_dma_channel_init
#define nouveau_dma_wait                    _redefined_nouvau_dma_wait
#define nouveau_notifier_alloc              _redefined_nouveau_notifier_alloc

#endif