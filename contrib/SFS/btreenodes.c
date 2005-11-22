#include <exec/types.h>
#include <proto/exec.h>

#include "btreenodes.h"
#include "btreenodes_protos.h"

#include "adminspaces_protos.h"
#include "cachebuffers_protos.h"
#include "debug.h"

#include "globals.h"

extern void setchecksum(struct CacheBuffer *);
extern LONG readcachebuffercheck(struct CacheBuffer **,ULONG,ULONG);
extern void dreq(UBYTE *fmt, ... );
extern void outputcachebuffer(struct CacheBuffer *cb);

/*

The idea behind a multiway tree is that multiway trees do
not require a very good balance to be effective.

x-Way trees

We store new nodes in a leaf-container which can contain
upto x nodes.  When the leaf-container is full it is split
into two seperate leaf-containers containing x/2 and x/2+1
nodes.  A node-container is created which keeps track of the
two leaf-containers.  The new leaf-containers will be split
when they are full as well, and so on.

At some point the node-container will be full.  When this
happens we split it in much the same way we split the
leaf-containers.  In effect this will mean adding a new
level to the tree.

When deleting a leaf from a leaf-container we will check if
the container contains less than x/2 leafs.  If this is the
case we attempt to merge the leaf-container with the next or
previous leaf-container (or both).  For this to work the
total number of free leafs in the next and previous
container must be equal to the number of leafs in the
current leaf-container.

*/



/* Internal functions */

LONG splitbtreecontainer(BLCK rootblock,struct CacheBuffer *cb);
WORD getbnodeindex(struct BTreeContainer *btc);
struct BNode *searchforbnode(ULONG key,struct BTreeContainer *btc);
struct BNode *insertbnode(ULONG key,struct BTreeContainer *btc);
void removebnode(ULONG key,struct BTreeContainer *btc);



void __inline copywordsforward(UWORD *src,UWORD *dst,UWORD len) {
  while(len-->0) {
    *dst++=*src++;
  }
}

void __inline copywordsbackward(UWORD *src,UWORD *dst,UWORD len) {
  while(len-->0) {
    *--dst=*--src;
  }
}



LONG getparentbtreecontainer(BLCK rootblock,struct CacheBuffer **io_cb) {
  ULONG childkey=((struct fsBNodeContainer *)((*io_cb)->data))->btc.bnode[0].key;
  BLCK childblock=(*io_cb)->blckno;
  LONG errorcode=0;

  _XDEBUG((DEBUG_NODES,"getparentbtreecontainer: Getting parent of block %ld\n",(*io_cb)->blckno));

  /* This function gets the BTreeContainer parent of the passed in CacheBuffer.  If
     there is no parent this function sets io_cb to 0 */

  if(rootblock!=childblock) {
    while((errorcode=readcachebuffercheck(io_cb,rootblock,BNODECONTAINER_ID))==0) {
      struct fsBNodeContainer *bnc=(*io_cb)->data;
      struct BTreeContainer *btc=&bnc->btc;
      struct BNode *bn;
      WORD n=btc->nodecount;

      if(btc->isleaf==TRUE) {
        #ifdef CHECKCODE_BNODES
          dreq("getparentbtreecontainer() couldn't find parent for bnodecontainer at block %ld!\nPlease notify the author!",childblock);
          outputcachebuffer(*io_cb);
          errorcode=INTERR_BTREE;
        #endif

        break;
      }

      while(n-->0) {
        if(btc->bnode[n].data==childblock) {
          return(0);    /* Found parent!! */
        }
      }

      bn=searchforbnode(childkey,btc);    /* This searchforbnode() doesn't have to get EXACT key matches. */
      rootblock=bn->data;
    }
  }

  *io_cb=0;

  return(errorcode);
}



LONG createbnode(BLCK rootblock,ULONG key,struct CacheBuffer **returned_cb,struct BNode **returned_bnode) {
  LONG errorcode;

  _XDEBUG((DEBUG_NODES,"createbnode: Creating BNode with key %ld, using %ld as root.\n",key,rootblock));

  while((errorcode=findbnode(rootblock, key, returned_cb, returned_bnode))==0) {
    struct fsBNodeContainer *bnc=(*returned_cb)->data;
    struct BTreeContainer *btc=&bnc->btc;
    LONG extbranches=(globals->bytes_block-sizeof(struct fsBNodeContainer))/btc->nodesize;

    #ifdef CHECKCODE_BNODES
      if(*returned_bnode!=0 && (*returned_bnode)->key==key) {
        dreq("createbnode: findbnode() says key %ld already exists!",key);
        outputcachebuffer(*returned_cb);
      }
    #endif

    _XDEBUG((DEBUG_NODES,"createbnode: findbnode found block %ld\n",(*returned_cb)->blckno));

    if(btc->nodecount<extbranches) {
      /* Simply insert new node in this BTreeContainer */

      _XDEBUG((DEBUG_NODES,"createbnode: Simple insert\n"));

      preparecachebuffer(*returned_cb);

      *returned_bnode=insertbnode(key,btc);

      setchecksum(*returned_cb);
     //  errorcode=storecachebuffer(*returned_cb);
      break;
    }
    else if((errorcode=splitbtreecontainer(rootblock,*returned_cb))!=0) {
      break;
    }

    /* Loop and try insert it the normal way again :-) */
  }

  return(errorcode);
}



LONG splitbtreecontainer(BLCK rootblock,struct CacheBuffer *cb) {
  struct CacheBuffer *cbparent;
  struct BNode *bn;
  LONG errorcode;

  _XDEBUG((DEBUG_NODES,"splitbtreecontainer: splitting block %ld\n",cb->blckno));

  lockcachebuffer(cb);

  cbparent=cb;
  if((errorcode=getparentbtreecontainer(rootblock,&cbparent))==0) {
    if(cbparent==0) {
      /* We need to create Root tree-container */

      _XDEBUG((DEBUG_NODES,"splitbtreecontainer: creating root tree-container\n"));

      cbparent=cb;
      if((errorcode=allocadminspace(&cb))==0) {
        struct fsBNodeContainer *bnc=cb->data;
        struct fsBNodeContainer *bncparent=cbparent->data;
        struct BTreeContainer *btcparent=&bncparent->btc;

        CopyMemQuick(cbparent->data,cb->data,globals->bytes_block);
        bnc->bheader.ownblock=AROS_LONG2BE(cb->blckno);

        _XDEBUG((DEBUG_NODES,"splitbtreecontainer: allocated admin space for root\n"));

        lockcachebuffer(cb);          /* Lock cachebuffer which now contains the data previously in root.
                                         It must be locked here, otherwise preparecachebuffer below could
                                         kill it. */

        if((errorcode=storecachebuffer(cb))==0) {
          preparecachebuffer(cbparent);
          clearcachebuffer(cbparent);        /* Not strictly needed, but makes things more clear. */

          bncparent->bheader.id=AROS_LONG2BE(BNODECONTAINER_ID);
          bncparent->bheader.ownblock=AROS_LONG2BE(cbparent->blckno);

          btcparent->isleaf=FALSE;
          btcparent->nodesize=sizeof(struct BNode);
          btcparent->nodecount=0;

          bn=insertbnode(0,btcparent);
          bn->data=cb->blckno;

          errorcode=storecachebuffer(cbparent);
        }

        unlockcachebuffer(cbparent);  /* Unlock cachebuffer which was root, and still is root */
      }
    }

    if(errorcode==0) {
      struct fsBNodeContainer *bncparent=cbparent->data;
      struct BTreeContainer *btcparent=&bncparent->btc;
      LONG branches=(globals->bytes_block-sizeof(struct fsBNodeContainer))/btcparent->nodesize;

      if(btcparent->nodecount==branches) {
        /* We need to split the parent tree-container first! */

        errorcode=splitbtreecontainer(rootblock,cbparent);

        if(errorcode==0) {
          cbparent=cb;
          if((errorcode=getparentbtreecontainer(rootblock,&cbparent))==0) {
            /* cbparent might have changed after the split */
            bncparent=cbparent->data;
            btcparent=&bncparent->btc;
          }
        }
      }

      if(errorcode==0) {
        struct CacheBuffer *cbnew;

        /* We can split this container and add it to the parent
           because the parent has enough room. */

        lockcachebuffer(cbparent);

        if((errorcode=allocadminspace(&cbnew))==0) {
          struct fsBNodeContainer *bncnew=cbnew->data;
          struct BTreeContainer *btcnew=&bncnew->btc;
          struct fsBNodeContainer *bnc=cb->data;
          struct BTreeContainer *btc=&bnc->btc;
          LONG branches=(globals->bytes_block-sizeof(struct fsBNodeContainer))/btc->nodesize;
          ULONG newkey;
          ULONG newblckno=cbnew->blckno;

          bncnew->bheader.id=AROS_LONG2BE(BNODECONTAINER_ID);
          bncnew->bheader.ownblock=AROS_LONG2BE(cbnew->blckno);

          btcnew->isleaf=btc->isleaf;
          btcnew->nodesize=btc->nodesize;

          btcnew->nodecount=branches-branches/2;

          copywordsforward((UWORD *)((UBYTE *)btc->bnode+branches/2*btc->nodesize),(UWORD *)btcnew->bnode,(branches-branches/2)*btc->nodesize/2);

          newkey=btcnew->bnode[0].key;

          if((errorcode=storecachebuffer(cbnew))==0) {

            preparecachebuffer(cb);

            btc->nodecount=branches/2;

            if((errorcode=storecachebuffer(cb))==0) {
              preparecachebuffer(cbparent);

              bn=insertbnode(newkey,btcparent);
              bn->data=newblckno;

              errorcode=storecachebuffer(cbparent);
            }
          }
        }

        unlockcachebuffer(cbparent);
      }
    }
  }

  unlockcachebuffer(cb);

  return(errorcode);
}



LONG findbnode(BLCK rootblock, ULONG key, struct CacheBuffer **returned_cb, struct BNode **returned_bnode) {
  LONG errorcode;

  /* This function finds the BNode with the given key.  If no exact match can be
     found then this function will return either the next or previous closest
     match (don't rely on this).

     If there were no BNode's at all, then *returned_cb will be the Rootblock
     and *returned_bnode will be zero.

     Any error will be returned.  If non-zero then don't rely on the contents
     of *returned_cb and *returned_bnode. */

  _XDEBUG((DEBUG_NODES,"findbnode: Looking for BNode with key %ld, from root %ld\n",key,rootblock));

  while((errorcode=readcachebuffercheck(returned_cb, rootblock, BNODECONTAINER_ID))==0) {
    struct fsBNodeContainer *bnc=(*returned_cb)->data;
    struct BTreeContainer *btc=&bnc->btc;

    if(btc->nodecount==0) {
      *returned_bnode=0;
      break;
    }

    *returned_bnode=searchforbnode(key, btc);
    if(btc->isleaf==TRUE) {
      break;
    }
    rootblock=(*returned_bnode)->data;
  }

  _XDEBUG((DEBUG_NODES,"findbnode: *returned_cb->blckno = %ld, Exiting with errorcode %ld\n",(*returned_cb)->blckno,errorcode));

  return(errorcode);
}



struct BNode *searchforbnode(ULONG key, struct BTreeContainer *tc) {
  struct BNode *tn;
  WORD n=tc->nodecount-1;

  /* This function looks for the BNode equal to the key.  If no
     exact match is available then the BNode which is slightly
     lower than key will be returned.  If no such BNode exists
     either, then the first BNode in this block is returned.

     This function will return the first BNode even if there
     are no BNode's at all in this block (this can only happen
     for the Root of the tree).  Be sure to check if the Root
     is not empty before calling this function. */

  tn=(struct BNode *)((UBYTE *)tc->bnode+n*tc->nodesize);

  for(;;) {
    if(n<=0 || key >= tn->key) {
      return(tn);
    }
    tn=(struct BNode *)((UBYTE *)tn-tc->nodesize);
    n--;
  }
}



struct BNode *insertbnode(ULONG key,struct BTreeContainer *btc) {
  struct BNode *bn=btc->bnode;
  UWORD *src;
  UWORD *dst;

  bn=(struct BNode *)((UBYTE *)bn+btc->nodesize*(btc->nodecount-1));

  src=(UWORD *)((UBYTE *)bn+btc->nodesize);
  dst=src+btc->nodesize/2;

  /* This routine inserts a node sorted into a BTreeContainer.  It does
     this by starting at the end, and moving the nodes one by one to
     a higher slot until the empty slot has the correct position for
     this key.  Donot use this function on completely filled
     BTreeContainers! */

  for(;;) {
    if(bn<btc->bnode || key > bn->key) {
      bn=(struct BNode *)((UBYTE *)bn+btc->nodesize);
      bn->key=key;
      btc->nodecount++;
      break;
    }
    else {
      WORD l=btc->nodesize/2;

      while(l-->0) {
        *--dst=*--src;
      }
    }

    bn=(struct BNode *)((UBYTE *)bn-btc->nodesize);
  }

  return(bn);
}



LONG deleteinternalnode(BLCK rootblock,struct CacheBuffer *cb,ULONG key) {
  struct fsBNodeContainer *bnc=cb->data;
  struct BTreeContainer *btc=&bnc->btc;
  UWORD branches=(globals->bytes_block-sizeof(struct fsBNodeContainer))/btc->nodesize;
  LONG errorcode;

  /* Deletes specified internal node. */

  preparecachebuffer(cb);

  removebnode(key,btc);

  if((errorcode=storecachebuffer(cb))==0) {

    /* Now checks if the container still contains enough nodes,
       and takes action accordingly. */

    _XDEBUG((DEBUG_NODES,"deleteinternalnode: branches = %ld, btc->nodecount = %ld\n",branches,btc->nodecount));

    if(btc->nodecount<(branches+1)/2) {
      struct CacheBuffer *cbparent=cb;

      /* nodecount has become to low.  We need to merge this Container
         with a neighbouring Container, or we need to steal a few nodes
         from a neighbouring Container. */

      lockcachebuffer(cb);

      /* We get the parent of the container here, so we can find out what
         containers neighbour the container which currently hasn't got enough nodes. */

      if((errorcode=getparentbtreecontainer(rootblock,&cbparent))==0) {
        if(cbparent!=0) {
          struct fsBNodeContainer *bncparent=cbparent->data;
          struct BTreeContainer *btcparent=&bncparent->btc;
          WORD n;

          _XDEBUG((DEBUG_NODES,"deleteinternalnode: get parent returned block %ld.\n",cbparent->blckno));

          for(n=0; n<btcparent->nodecount; n++) {
            if(btcparent->bnode[n].data==cb->blckno) {
              break;
            }
          }

          /* n is now the offset of our own bnode. */

          lockcachebuffer(cbparent);

          if(n<btcparent->nodecount-1) {      // Check if we have a next neighbour.
            struct CacheBuffer *cb_next;

            _XDEBUG((DEBUG_NODES,"deleteinternalnode: using next container.\n"));

            if((errorcode=readcachebuffercheck(&cb_next,btcparent->bnode[n+1].data,BNODECONTAINER_ID))==0) {
              struct fsBNodeContainer *bnc_next=cb_next->data;
              struct BTreeContainer *btc_next=&bnc_next->btc;

              lockcachebuffer(cb_next);

              if(btc_next->nodecount+btc->nodecount>branches) {    // Check if we need to steal nodes.
                WORD nodestosteal=(btc_next->nodecount+btc->nodecount)/2-btc->nodecount;

                /* Merging them is not possible.  Steal a few nodes then. */

                preparecachebuffer(cb);

                copywordsforward((UWORD *)btc_next->bnode,(UWORD *)((UBYTE *)btc->bnode+btc->nodecount*btc->nodesize),nodestosteal*btc->nodesize/2);
                btc->nodecount+=nodestosteal;

                if((errorcode=storecachebuffer(cb))==0) {
                  preparecachebuffer(cb_next);

                  copywordsforward((UWORD *)((UBYTE *)btc_next->bnode+btc_next->nodesize*nodestosteal),(UWORD *)btc_next->bnode,(btc->nodesize/2)*(btc_next->nodecount - nodestosteal));
                  btc_next->nodecount-=nodestosteal;

                  if((errorcode=storecachebuffer(cb_next))==0) {
                    preparecachebuffer(cbparent);

                    btcparent->bnode[n+1].key=btc_next->bnode[0].key;

                    errorcode=storecachebuffer(cbparent);
                  }
                }
              }
              else {   // Merging is possible.
                preparecachebuffer(cb);

                copywordsforward((UWORD *)btc_next->bnode,(UWORD *)((UBYTE *)btc->bnode+btc->nodesize*btc->nodecount),btc->nodesize*btc_next->nodecount/2);
                btc->nodecount+=btc_next->nodecount;

                if((errorcode=storecachebuffer(cb))==0) {
                  if((errorcode=freeadminspace(cb_next->blckno))==0) {
                    errorcode=deleteinternalnode(rootblock,cbparent,btcparent->bnode[n+1].key);
                  }
                }
              }

              unlockcachebuffer(cb_next);
            }
          }
          else if(n>0) {       // Check if we have a previous neighbour.
            struct CacheBuffer *cb2;

            _XDEBUG((DEBUG_NODES,"deleteinternalnode: using prev container.\n"));

            if((errorcode=readcachebuffercheck(&cb2,btcparent->bnode[n-1].data,BNODECONTAINER_ID))==0) {
              struct fsBNodeContainer *bnc2=cb2->data;
              struct BTreeContainer *btc2=&bnc2->btc;

              lockcachebuffer(cb2);

              if(btc2->nodecount+btc->nodecount>branches) {
                WORD nodestosteal=(btc2->nodecount+btc->nodecount)/2-btc->nodecount;

                /* Merging them is not possible.  Steal a few nodes then. */

                preparecachebuffer(cb);

                copywordsbackward((UWORD *)((UBYTE *)btc->bnode+btc->nodecount*btc->nodesize),(UWORD *)((UBYTE *)btc->bnode+(btc->nodecount+nodestosteal)*btc->nodesize),btc->nodecount*btc->nodesize/2);
                btc->nodecount+=nodestosteal;
                copywordsforward((UWORD *)((UBYTE *)btc2->bnode+(btc2->nodecount-nodestosteal)*btc2->nodesize),(UWORD *)btc->bnode,nodestosteal*btc->nodesize/2);

                if((errorcode=storecachebuffer(cb))==0) {

                  preparecachebuffer(cb2);

                  btc2->nodecount-=nodestosteal;

                  if((errorcode=storecachebuffer(cb2))==0) {
                    preparecachebuffer(cbparent);

                    btcparent->bnode[n].key=btc->bnode[0].key;

                    errorcode=storecachebuffer(cbparent);
                  }
                }
              }
              else {
                preparecachebuffer(cb2);

                copywordsforward((UWORD *)btc->bnode,(UWORD *)((UBYTE *)btc2->bnode+btc2->nodecount*btc2->nodesize),btc->nodecount*btc->nodesize/2);
                btc2->nodecount+=btc->nodecount;

                if((errorcode=storecachebuffer(cb2))==0) {
                  if((errorcode=freeadminspace(cb->blckno))==0) {
                    errorcode=deleteinternalnode(rootblock,cbparent,btcparent->bnode[n].key);
                  }
                }
              }

              unlockcachebuffer(cb2);
            }
          }
          /*
          else {
            // Never happens, except for root and then we don't care.
          }
          */

          unlockcachebuffer(cbparent);

        }
        else if(btc->nodecount==1) {
          /* No parent, so must be root. */

          _XDEBUG((DEBUG_NODES,"deleteinternalnode: no parent so must be root\n"));

          if(btc->isleaf==FALSE) {
            struct CacheBuffer *cb2;
            struct fsBNodeContainer *bnc=cb->data;

            /* The current root has only 1 node.  We now copy the data of this node into the
               root and promote that data to be the new root.  The rootblock number stays the
               same that way. */

            preparecachebuffer(cb);

            if((errorcode=readcachebuffercheck(&cb2,btc->bnode[0].data,BNODECONTAINER_ID))==0) {

              CopyMemQuick(cb2->data,cb->data,globals->bytes_block);
              bnc->bheader.ownblock=AROS_LONG2BE(cb->blckno);

              if((errorcode=storecachebuffer(cb))==0) {
                errorcode=freeadminspace(cb2->blckno);
              }
            }
            else {
              dumpcachebuffer(cb);
            }
          }
          /* If not, then root contains leafs. */
        }

        _XDEBUG((DEBUG_NODES,"deleteinternalnode: almost done\n"));

        /* otherwise, it must be the root, and the root is allowed
           to contain less than the minimum amount of nodes. */

      }

      unlockcachebuffer(cb);
    }
  }

  return(errorcode);
}



LONG deletebnode(BLCK rootblock,ULONG key) {
  struct CacheBuffer *cb;
  struct BNode *bn;
  LONG errorcode;

  if((errorcode=findbnode(rootblock,key,&cb,&bn))==0) {
    #ifdef CHECKCODE_BNODES
      if(bn==0 || bn->key!=key) {
        dreq("deletebnode: key %ld doesn't exist!",key);
        outputcachebuffer(cb);
        return(INTERR_BTREE);
      }
    #endif

    _XDEBUG((DEBUG_NODES,"deletebnode: key %ld\n",key));

    errorcode=deleteinternalnode(rootblock,cb,key);
  }

  return(errorcode);
}



void removebnode(ULONG key,struct BTreeContainer *btc) {
  struct BNode *bn=btc->bnode;
  WORD n=0;

  _XDEBUG((DEBUG_NODES,"removebnode: key %ld\n",key));

  /* This routine removes a node from a BTreeContainer indentified
     by its key.  If no such key exists this routine does nothing.
     It correctly handles empty BTreeContainers. */

  while(n<btc->nodecount) {
    if(bn->key == key) {
      btc->nodecount--;

      copywordsforward((UWORD *)((UBYTE *)bn+btc->nodesize),(UWORD *)bn,(btc->nodecount-n)*btc->nodesize/2);
      break;
    }
    bn=(struct BNode *)((UBYTE *)bn+btc->nodesize);
    n++;
  }
}



struct BNode *next(struct BTreeContainer *tc, struct BNode *tn) {
  if((UBYTE *)tn == (UBYTE *)tc->bnode + (tc->nodecount-1)*tc->nodesize) {
    return(0);
  }
  else {
    return((struct BNode *)((UBYTE *)tn+tc->nodesize));
  }
}



struct BNode *previous(struct BTreeContainer *tc, struct BNode *tn) {
  if(tn==tc->bnode) {
    return(0);
  }
  else {
    return((struct BNode *)((UBYTE *)tn-tc->nodesize));
  }
}



LONG nextbnode(BLCK rootblock, struct CacheBuffer **io_cb, struct BNode **io_bnode) {
  struct BNode *bn;
  ULONG key=(*io_bnode)->key;
  BLCK blockwithnext=0;
  LONG errorcode;

  /* This function locates the next BNode in the tree.  If there is no next
     BNode then this function will return no error and zero *io_bnode. */

  while((errorcode=readcachebuffercheck(io_cb, rootblock, BNODECONTAINER_ID))==0) {
    struct fsBNodeContainer *bnc=(*io_cb)->data;
    struct BTreeContainer *btc=&bnc->btc;

    bn=searchforbnode(key, btc);

    if((*io_bnode=next(btc, bn))!=0) {
      blockwithnext=(*io_bnode)->data;
    }

    if(btc->isleaf==TRUE) {
      /* Traversed the tree to the end. */

      if(*io_bnode!=0 || blockwithnext==0) {  // Checks if next is in this block, or no next was found ever.
        break;
      }

      errorcode=findbnode(blockwithnext, key, io_cb, io_bnode);
      break;
    }

    rootblock=bn->data;
  }

  return(errorcode);
}



LONG previousbnode(BLCK rootblock, struct CacheBuffer **io_cb, struct BNode **io_bnode) {
  struct BNode *bn;

  /* This function locates the previous BNode in the tree.  If there is no previous
     BNode then this function will return no error and zero *io_bnode.

     This function will not walk the tree again if the Previous BNode is in the same
     block as the current one. */

  if((bn=previous(&((struct fsBNodeContainer *)((*io_cb)->data))->btc, *io_bnode))!=0) {   // Quick check if there is a direct Previous available.
    *io_bnode=bn;
    return(0);
  }
  else {
    ULONG key=(*io_bnode)->key;
    BLCK blockwithprevious=0;
    LONG errorcode;

    /* No direct previous was available, so we walk the tree: */

    while((errorcode=readcachebuffercheck(io_cb, rootblock, BNODECONTAINER_ID))==0) {
      struct fsBNodeContainer *bnc=(*io_cb)->data;
      struct BTreeContainer *btc=&bnc->btc;

      bn=searchforbnode(key, btc);

      if((*io_bnode=previous(btc, bn))!=0) {
        blockwithprevious=(*io_bnode)->data;
      }

      if(btc->isleaf==TRUE) {
        /* Traversed the tree to the end. */

        if(*io_bnode!=0 || blockwithprevious==0) {  // Checks if previous is in this block, or no previous was found ever.
          break;
        }

        errorcode=findbnode(blockwithprevious, key, io_cb, io_bnode);
        break;
      }

      rootblock=bn->data;
    }

    return(errorcode);
  }
}



LONG lastbnode(BLCK rootblock, struct CacheBuffer **returned_cb, struct BNode **returned_bnode) {
  return(findbnode(rootblock, 0xFFFFFFFF, returned_cb, returned_bnode));
}
