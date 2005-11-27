#include <dos/dos.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "nodes.h"

#include "adminspaces_protos.h"
#include "cachebuffers_protos.h"
#include "debug.h"
#include "support_protos.h"

#include "globals.h"

extern LONG readcachebuffercheck(struct CacheBuffer **,ULONG,ULONG);
extern void setchecksum(struct CacheBuffer *);

extern LONG req(UBYTE *fmt, UBYTE *gads, ... );
extern LONG req_unusual(UBYTE *fmt, ... );

LONG parentnodecontainer(BLCK noderoot, struct CacheBuffer **io_cb) {
  BLCK childblock=(*io_cb)->blckno;
  NODE nodenumber=((struct fsNodeContainer *)((*io_cb)->data))->nodenumber;
  LONG errorcode=0;

  /* Looks for the parent of the passed-in CacheBuffer (fsNodeContainer)
     starting from the root.  It returns an error if any error occured.
     If error is 0 and io_cb is 0 as well, then there was no parent (ie,
     you asked parent of the root).  Otherwise io_cb should contain the
     parent of the passed-in NodeContainer. */

  *io_cb=0;

  while(noderoot!=childblock && (errorcode=readcachebuffercheck(io_cb, noderoot, NODECONTAINER_ID))==0) {
    struct fsNodeContainer *nc=(*io_cb)->data;

    if(nc->nodes==1) {
      /* We've descended the tree to a leaf NodeContainer, something
         which should never happen if the passed-in CacheBuffer had
         contained a valid fsNodeContainer. */

      req_unusual("Failed to locate the parent NodeContainer\n"\
                  "of block %ld.");

      *io_cb=0;

      return(INTERR_NODES);
    }
    else {
      UWORD containerentry=(nodenumber - nc->nodenumber)/nc->nodes;

      noderoot=nc->node[containerentry]>>globals->shifts_block32;
    }
  }

  return(errorcode);
}



int isfull(struct fsNodeContainer *nc) {
  BLCKn *p=nc->node;
  WORD n=globals->node_containers;

  while(--n>=0) {
    if(*p==0 || (*p & 0x00000001)==0) {
      break;
    }
    p++;
  }

  return(n<0);
}



LONG markparentfull(BLCK noderoot, struct CacheBuffer *cb) {
  NODE nodenumber=((struct fsNodeContainer *)(cb->data))->nodenumber;
  LONG errorcode;

  if((errorcode=parentnodecontainer(noderoot, &cb))==0 && cb!=0) {
    struct fsNodeContainer *nc=cb->data;

    preparecachebuffer(cb);

    nc->node[(nodenumber-nc->nodenumber)/nc->nodes]|=0x00000001;

    if((errorcode=storecachebuffer(cb))==0) {
      if(isfull(nc)) {        /* This container now is full as well!  Mark the next higher up container too then! */
        return(markparentfull(noderoot, cb));
      }
    }
  }

  return(errorcode);
}



LONG markparentempty(BLCK noderoot, struct CacheBuffer *cb) {
  NODE nodenumber=((struct fsNodeContainer *)(cb->data))->nodenumber;
  LONG errorcode;

  if((errorcode=parentnodecontainer(noderoot, &cb))==0 && cb!=0) {
    struct fsNodeContainer *nc=cb->data;
    int wasfull;

    wasfull=isfull(nc);

    preparecachebuffer(cb);

    nc->node[(nodenumber-nc->nodenumber)/nc->nodes]&=~0x00000001;

    if((errorcode=storecachebuffer(cb))==0) {
      if(wasfull) {
        /* This container was completely full before!  Mark the next higher up container too then! */

        return(markparentempty(noderoot, cb));
      }
    }
  }

  return(errorcode);
}



LONG freecontainer(BLCK noderoot, struct CacheBuffer *cb) {
  NODE nodenumber=((struct fsNodeContainer *)(cb->data))->nodenumber;
  LONG errorcode;

  if((errorcode=parentnodecontainer(noderoot, &cb))==0 && cb!=0) {   /* This line also prevents the freeing of the noderoot. */
    struct fsNodeContainer *nc=cb->data;
    UWORD containerindex=(nodenumber-nc->nodenumber)/nc->nodes;

    lockcachebuffer(cb);

    if((errorcode=freeadminspace(nc->node[containerindex]>>globals->shifts_block32))==0) {

      unlockcachebuffer(cb);

      preparecachebuffer(cb);

      nc->node[containerindex]=0;

      if((errorcode=storecachebuffer(cb))==0) {
        BLCKn *p=nc->node;
        WORD n=globals->node_containers;

        while(n-->0) {
          if(*p++!=0) {
            break;
          }
        }

        if(n<0) {          /* This container is now completely empty!  Free this NodeIndexContainer too then! */
          return(freecontainer(noderoot, cb));
        }
      }
    }
    else {
      unlockcachebuffer(cb);
    }
  }

  return(errorcode);
}



static LONG addnewnodelevel(BLCK noderoot, UWORD nodesize) {
  struct CacheBuffer *cb;
  LONG errorcode;

  /* Adds a new level to the Node tree. */

  _XDEBUG((DEBUG_NODES,"addnewnodelevel: Entry\n"));

  if((errorcode=readcachebuffercheck(&cb, noderoot, NODECONTAINER_ID))==0) {
    struct CacheBuffer *newcb;

    lockcachebuffer(cb);   /* Locking cb, because allocadminspace & storecachebuffer below may destroy it otherwise */

    if((errorcode=allocadminspace(&newcb))==0) {
      struct fsNodeContainer *nc=cb->data;
      struct fsNodeContainer *newnc=newcb->data;
      BLCK newblock=newcb->blckno;

      /* The newly allocated block will become a copy of the current root. */

      newnc->bheader.id=NODECONTAINER_ID;
      newnc->bheader.ownblock=newcb->blckno;
      newnc->nodenumber=nc->nodenumber;
      newnc->nodes=nc->nodes;
      CopyMemQuick(nc->node, newnc->node, globals->bytes_block-sizeof(struct fsNodeContainer));

      if((errorcode=storecachebuffer(newcb))==0) {

        /* The current root will now be transformed into a new root. */

        preparecachebuffer(cb);

        if(nc->nodes==1) {
          nc->nodes=(globals->bytes_block-sizeof(struct fsNodeContainer))/nodesize;
        }
        else {
          nc->nodes*=globals->node_containers;
        }

        nc->node[0]=(newblock<<globals->shifts_block32)+1;  /* Tree is full from that point! */

        ClearMemQuick(&nc->node[1], globals->bytes_block-sizeof(struct fsNodeContainer)-4);

        errorcode=storecachebuffer(cb);
      }
    }

    unlockcachebuffer(cb);
  }

  _XDEBUG((DEBUG_NODES,"addnewnodelevel: Exiting with errorcode %ld\n",errorcode));

  return(errorcode);
}



LONG deletenode(BLCK noderoot, struct CacheBuffer *cb, struct fsNode *n, UWORD nodesize) {
  struct fsNodeContainer *nc=cb->data;
  UWORD nodecount=(globals->bytes_block-sizeof(struct fsNodeContainer))/nodesize;
  WORD i=nodecount;
  WORD empty=0;
  LONG errorcode;

  preparecachebuffer(cb);

  n->data=0;

  n=(struct fsNode *)nc->node;

  while(i-->0) {
    if(n->data==0) {
      empty++;
    }
    n=(struct fsNode *)((UBYTE *)n+nodesize);
  }

  if((errorcode=storecachebuffer(cb))==0) {
    if(empty==1) {      /* NodeContainer was completely full before, so we need to mark it empty now. */
      errorcode=markparentempty(noderoot, cb);
    }
    else if(empty==nodecount) {         /* NodeContainer is now completely empty!  Free it! */
      errorcode=freecontainer(noderoot, cb);
    }
  }

  return(errorcode);
}



static LONG createnodecontainer(ULONG nodenumber, ULONG nodes, BLCK *returned_block) {
  struct CacheBuffer *cb;
  LONG errorcode;

  _XDEBUG((DEBUG_NODES,"createnodecontainer: nodenumber = %ld, nodes = %ld\n",nodenumber,nodes));

  if((errorcode=allocadminspace(&cb))==0) {
    struct fsNodeContainer *nc=cb->data;

    nc->bheader.id=NODECONTAINER_ID;
    nc->bheader.ownblock=cb->blckno;

    nc->nodenumber=nodenumber;
    nc->nodes=nodes;

    errorcode=storecachebuffer(cb);
    *returned_block=cb->blckno;
  }

  _XDEBUG((DEBUG_NODES,"createnodecontainer: Exiting with errorcode %ld\n",errorcode));

  return(errorcode);
}



LONG createnode(BLCK noderoot, UWORD nodesize, struct CacheBuffer **returned_cb, struct fsNode **returned_node, NODE *returned_nodeno) {
  struct CacheBuffer *cb;
  UWORD nodecount=(globals->bytes_block-sizeof(struct fsNodeContainer))/nodesize;
  BLCK nodeindex=noderoot;
  LONG errorcode;

  /* This function creates a new fsNode structure in a fsNodeContainer.  If needed
     it will create a new fsNodeContainers and a new fsNodeIndexContainer.  newoperation()
     should be called prior to calling this function. */

//  _XDEBUG((DEBUG_NODES,"createnode: Entry\n"));

  while((errorcode=readcachebuffercheck(&cb, nodeindex, NODECONTAINER_ID))==0) {
    struct fsNodeContainer *nc=cb->data;

    if(nc->nodes==1) {    // Is it a leaf-container?
      struct fsNode *n;
      WORD i=nodecount;

      n=(struct fsNode *)nc->node;

      while(i-->0) {
        if(n->data==0) {
          break;
        }
        n=(struct fsNode *)((UBYTE *)n+nodesize);
      }

      if(i>=0) {
        /* Found an empty fsNode structure! */

        preparecachebuffer(cb);

        *returned_cb=cb;
        *returned_node=n;
        *returned_nodeno=nc->nodenumber+((UBYTE *)n-(UBYTE *)nc->node)/nodesize;

        _XDEBUG((DEBUG_NODES,"createnode: Created Node %ld\n",*returned_nodeno));

        /* Below we continue to look through the NodeContainer block.  We skip the entry
           we found to be unused, and see if there are any more unused entries.  If we
           do not find any more unused entries then this container is now full. */

        n=(struct fsNode *)((UBYTE *)n+nodesize);

        while(i-->0) {
          if(n->data==0) {
            break;
          }
          n=(struct fsNode *)((UBYTE *)n+nodesize);
        }

        if(i<0) {
          /* No more empty fsNode structures in this block.  Mark parent full. */
          if((errorcode=markparentfull(noderoot, cb))!=0) {
            dumpcachebuffer(cb);
          }
        }

        break;
      }
      else {
        /* What happened now is that we found a leaf-container which was
           completely filled.  In practice this should only happen when there
           is only a single NodeContainer (only this container), or when there
           was an error in one of the full-bits in a higher level container. */

        if(noderoot!=nodeindex) {
          /*** Hmmm... it looks like there was a damaged full-bit or something.
               In this case we'd probably better call markcontainerfull. */

          // errorcode=markcontainerfull(nc->parent,nc->nodenumber);

          req("Couldn't find empty Node in NodeContainer at block %ld\n"\
              "while NodeIndexContainer indicated there should be one.\n",
              "Ok", cb->blckno);

          errorcode=ERROR_DISK_FULL;
          break;
        }
        else {
          /* Container is completely filled. */

          if((errorcode=addnewnodelevel(noderoot, nodesize))!=0) {
            break;
          }

          nodeindex=noderoot;
        }
      }
    }
    else {   // This isn't a leaf container
      BLCKn *p=nc->node;
      WORD i=globals->node_containers;

      /* We've read a normal container */

      while(i-->0) {
        if(*p!=0 && (*p & 0x00000001)==0) {
          break;
        }
        p++;
      }

      if(i>=0) {
        /* Found a not completely filled Container */

        nodeindex=*p>>globals->shifts_block32;
      }
      else {
        /* Everything in the NodeIndexContainer was completely filled.  There possibly
           are some unused pointers in this block however.  */

        _XDEBUG((DEBUG_NODES,"createnode: NodeContainer at block %ld has no empty Nodes\n",cb->blckno));

        p=nc->node;
        i=globals->node_containers;

        while(i-->0) {
          if(*p==0) {
            break;
          }
          p++;
        }

        if(i>=0) {
          BLCK newblock;
          ULONG nodes;

          /* Found an unused Container pointer */

          preparecachebuffer(cb);

          if(nc->nodes==(globals->bytes_block-sizeof(struct fsNodeContainer))/nodesize) {
            nodes=1;
          }
          else {
            nodes=nc->nodes/globals->node_containers;
          }

          if((errorcode=createnodecontainer(nc->nodenumber+(p-nc->node)*nc->nodes, nodes, &newblock))!=0) {
            dumpcachebuffer(cb);
            break;
          }

          *p=newblock<<globals->shifts_block32;

          if((errorcode=storecachebuffer(cb))!=0) {
            break;
          }
        }
        else {
          /* Container is completely filled.  This must be the top-level NodeIndex container
             as otherwise the full-bit would have been wrong! */

          if((errorcode=addnewnodelevel(noderoot, nodesize))!=0) {
            break;
          }

          nodeindex=noderoot;
        }
      }
    }
  }

//  _XDEBUG((DEBUG_NODES,"createnode: Exiting with errorcode %ld\n",errorcode));

  return(errorcode);
}



LONG findnode(BLCK nodeindex,UWORD nodesize,NODE nodeno,struct CacheBuffer **returned_cb,struct fsNode **returned_node) {
  struct CacheBuffer *cb;
  LONG errorcode;

  _XDEBUG((DEBUG_NODES,"findnode: Entry -- looking for nodeno %ld\n",nodeno));

  /* Finds a specific node by number.  It returns the cachebuffer which contains the fsNode
     structure and a pointer to the fsNode structure directly. */

  while((errorcode=readcachebuffercheck(&cb,nodeindex,NODECONTAINER_ID))==0) {
    struct fsNodeContainer *nc=cb->data;

    _XDEBUG((DDEBUG_NODES,"findnode: Read NodeContainer at block %ld with nodenumber = %ld, nodes = %ld\n",nodeindex,nc->nodenumber,nc->nodes));

    if(nc->nodes==1) {
      /* We've descended the tree to a leaf NodeContainer */

      *returned_cb=cb;
      *returned_node=(struct fsNode *)((UBYTE *)nc->node+nodesize*(nodeno-nc->nodenumber));

      return(0);
    }
    else {
      UWORD containerentry=(nodeno-nc->nodenumber)/nc->nodes;

      nodeindex=nc->node[containerentry]>>globals->shifts_block32;
    }
  }

  _XDEBUG((DEBUG_NODES,"findnode: Exiting with errorcode %ld\n",errorcode));

  return(errorcode);
}

