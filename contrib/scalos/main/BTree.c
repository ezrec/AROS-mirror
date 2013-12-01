// BTree.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <stdlib.h>

#include "debug.h"
#include <proto/scalos.h>

#include "BTree.h"
#include "scalos_structures.h"
#include "functions.h"

//----------------------------------------------------------------------------

#define BALANCED        0
#define LEFT_HEAVY      1
#define RIGHT_HEAVY     -1

//----------------------------------------------------------------------------

static BOOL BTreeInternalInsert(struct BTree *tree, struct BTAVLnode **t,
		   BOOL *balanced, const void *key, void *data);
static void rotate_right(struct BTAVLnode **t);
static void rotate_left(struct BTAVLnode **t);
static void BTreeInternalDisposeNode(struct BTree *tree, struct BTAVLnode *t);
static BOOL BTreeInternalHideNode(struct BTree *tree, struct BTAVLnode *t, const void *key);
static void *BTreeInternalFindNode(struct BTree *tree, struct BTAVLnode *t, const void *key);
static void BTreeInternalTraverseTree(struct BTAVLnode *n, void *UserData,
		    void (*print)(const void *key, const void *data, void *UserData));

//----------------------------------------------------------------------------

struct BTree *BTreeCreate(void (*DisposeData)(void *),
			   void (*DisposeKey)(void *),
			   int (*Compare)(const void *, const void *))
{
	struct BTree *t = ScalosAlloc(sizeof(struct BTree));

	if (t)
		{
	        t->btr_Root = NULL;
	        t->btr_NodeCount = 0;
		t->btr_DisposeData = DisposeData;
		t->btr_DisposeKey = DisposeKey;
		t->btr_Compare = Compare;
		}
        
        return t;
}

    
void BTreeDispose(struct BTree *tree)
{
	BTreeInternalDisposeNode(tree, tree->btr_Root);
	ScalosFree(tree);
}


int BTreeInsert(struct BTree *tree, const void *key, void *data)
{
	BOOL balanced = 0;
	int retval = BTreeInternalInsert(tree, &tree->btr_Root, &balanced, key, data);

        if (!retval)
            tree->btr_NodeCount++;

        return retval;
}


BOOL BTreeHide(struct BTree *tree, const void *key)
{
	return BTreeInternalHideNode(tree, tree->btr_Root, key);
}


void *BTreeFind(struct BTree *tree, const void *key)
{
	return BTreeInternalFindNode(tree, tree->btr_Root, key);
}


size_t BTreeNumberOfNodes(struct BTree *tree)
{
	return tree->btr_NodeCount;
}


void BTreeDumpTree(struct BTree *tree, void *UserData,
		void (*print)(const void *key, const void *data, void *UserData))
{
	BTreeInternalTraverseTree(tree->btr_Root, UserData, print);
}


static BOOL BTreeInternalInsert(struct BTree *tree, struct BTAVLnode **t, BOOL *balanced,
		   const void *key, void *data)
{
	int comp;
	BOOL retval;
	struct BTAVLnode *node;

	if (!(*t))
		{
		node = ScalosAlloc(sizeof(struct BTAVLnode));

		if (node)
			{
		        node->avln_Key = key;
		        node->avln_Data = data;
		        node->avln_Balance = BALANCED;
		        node->avln_Hidden = 0;
			node->avln_Left = node->avln_Right = NULL;
		        *t = node;
			}

		return FALSE;
		}
	else
		{
		comp = tree->btr_Compare(key, (*t)->avln_Key);
		if (comp < 0)
			{
			// Add to left subtree
			if (!(*t)->avln_Left)
				{
				node = ScalosAlloc(sizeof(struct BTAVLnode));

				if (node)
					{
			                node->avln_Key = key;
			                node->avln_Data = data;
			                node->avln_Balance = BALANCED;
			                node->avln_Hidden = 0;
					node->avln_Left = node->avln_Right = NULL;
			                (*t)->avln_Left = node;
					*balanced = FALSE;
					}
				}
			else
				{
				retval = BTreeInternalInsert(tree, &((*t)->avln_Left), balanced, key, data);
				if (retval != 0)
					return retval;
				}
			if (!(*balanced))
				{
				switch ((*t)->avln_Balance)
					{
				case LEFT_HEAVY:
					rotate_left(t);
					*balanced = TRUE;
					break;
				case BALANCED:
					(*t)->avln_Balance = LEFT_HEAVY;
					break;
				case RIGHT_HEAVY:
					(*t)->avln_Balance = BALANCED;
					*balanced = TRUE;
					break;
					}
				}
			}
		else if (comp > 0)
			{
			/// try the right subtree
			if (!(*t)->avln_Right)
				{
				node = ScalosAlloc(sizeof(struct BTAVLnode));
				if (node)
					{
					node->avln_Key = key;
					node->avln_Data = data;
					node->avln_Balance = BALANCED;
					node->avln_Hidden = 0;
					node->avln_Left = node->avln_Right = NULL;
					(*t)->avln_Right = node;
					*balanced = FALSE;
					}
				}
			else
				{
				retval = BTreeInternalInsert(tree, &((*t)->avln_Right), balanced, key, data);
				if (retval)
					return retval;
				}
			if (!(*balanced))
				{
				switch ((*t)->avln_Balance)
					{
				case LEFT_HEAVY:
		                        (*t)->avln_Balance = BALANCED;
					*balanced = TRUE;
		                        break;
				case BALANCED:
		                        (*t)->avln_Balance = RIGHT_HEAVY;
		                        break;
				case RIGHT_HEAVY:
					rotate_right(t);
					*balanced = TRUE;
					break;
					}
				}
			}
		else 	
			{
			//  data has been previously hidden
			if ((*t)->avln_Hidden == 0)
				return TRUE;
			else
				{
		                if (tree->btr_DisposeData)
					tree->btr_DisposeData((*t)->avln_Data);
		                (*t)->avln_Data = data;
				(*t)->avln_Hidden = FALSE;
				*balanced = TRUE;
				}
			}
		}

	return FALSE;
}

static void rotate_right(struct BTAVLnode **t)
{
	struct BTAVLnode *ri = (*t)->avln_Right;
	struct BTAVLnode *gchild;
    
	if (ri->avln_Balance == RIGHT_HEAVY)
		{
		// RR rotation
	        (*t)->avln_Right = ri->avln_Left;
	        ri->avln_Left = *t;
	        (*t)->avln_Balance = BALANCED;
	        ri->avln_Balance = BALANCED;
	        *t = ri;
		}
	else
		{
		// RL rotation
	        gchild = ri->avln_Left;
	        ri->avln_Left = gchild->avln_Right;
	        gchild->avln_Right = ri;
	        (*t)->avln_Right = gchild->avln_Left;
	        gchild->avln_Left = *t;

		switch (gchild->avln_Balance)
			{
		case LEFT_HEAVY:
	                (*t)->avln_Balance = BALANCED;
	                ri->avln_Balance = RIGHT_HEAVY;
	                break;
		case BALANCED:
	                (*t)->avln_Balance = BALANCED;
	                ri->avln_Balance = BALANCED;
	                break;
		case RIGHT_HEAVY:
	                (*t)->avln_Balance = LEFT_HEAVY;
	                ri->avln_Balance = BALANCED;
	                break;
			}
	        gchild->avln_Balance = BALANCED;
		*t = gchild;
		}
}

    
static void rotate_left(struct BTAVLnode **t)
{
	struct BTAVLnode *left = (*t)->avln_Left;
	struct BTAVLnode *gchild;

	if (left->avln_Balance == LEFT_HEAVY)
		{
		// LL rotation
	        (*t)->avln_Left = left->avln_Right;
	        left->avln_Right = *t;
	        (*t)->avln_Balance = BALANCED;
	        left->avln_Balance = BALANCED;
	        *t = left;
		}
	else
		{
		// LR rotation
	        gchild = left->avln_Right;
	        left->avln_Right = gchild->avln_Left;
	        gchild->avln_Left = left;
	        (*t)->avln_Left = gchild->avln_Right;
	        gchild->avln_Right = *t;

		switch (gchild->avln_Balance)
			{
		case LEFT_HEAVY:
	                (*t)->avln_Balance = RIGHT_HEAVY;
	                left->avln_Balance = BALANCED;
	                break;
		case BALANCED:
	                (*t)->avln_Balance = BALANCED;
	                left->avln_Balance = BALANCED;
	                break;
		case RIGHT_HEAVY:
	                (*t)->avln_Balance = BALANCED;
	                left->avln_Balance = LEFT_HEAVY;
	                break;
			}
	        gchild->avln_Balance = BALANCED;
	        *t = gchild;
		}
}    


static BOOL BTreeInternalHideNode(struct BTree *tree, struct BTAVLnode *t, const void *key)
{
	int comp;

	if (t)
		{
		comp = tree->btr_Compare(key, t->avln_Key);
		if (0 == comp)
			{
			t->avln_Hidden = TRUE;
			tree->btr_NodeCount--;

			return FALSE;
			}
		else if (comp < 0)
			return BTreeInternalHideNode(tree, t->avln_Left, key);
		else if (comp > 0)
			return BTreeInternalHideNode(tree, t->avln_Right, key);
		}
    return TRUE;
}


static void BTreeInternalDisposeNode(struct BTree *tree, struct BTAVLnode *t)
{
	if (t)
		{
	        BTreeInternalDisposeNode(tree, t->avln_Left);
	        BTreeInternalDisposeNode(tree, t->avln_Right);

	        if (tree->btr_DisposeKey)
			tree->btr_DisposeKey((void *) t->avln_Key);
	        if (tree->btr_DisposeData)
			tree->btr_DisposeData(t->avln_Data);

		ScalosFree(t);
		}
}

static void *BTreeInternalFindNode(struct BTree *tree, struct BTAVLnode *t, const void *key)
{
	int comp;

	if (t)
		{
		comp = tree->btr_Compare(key, t->avln_Key);
		if (comp < 0)
			return BTreeInternalFindNode(tree, t->avln_Left, key);
		else if (comp > 0)
			return BTreeInternalFindNode(tree, t->avln_Right, key);
		else if (!t->avln_Hidden)
			return t->avln_Data;
		else
			return NULL;
		}
	else
		return NULL;
}

static void BTreeInternalTraverseTree(struct BTAVLnode *n, void *UserData,
		    void (*print)(const void *key, const void *data, void *UserData))
{
	if (n)
		{
		BTreeInternalTraverseTree(n->avln_Left, UserData, print);

		if (!n->avln_Hidden)
			{
			print(n->avln_Key, n->avln_Data, UserData);
			}
		BTreeInternalTraverseTree(n->avln_Right, UserData, print);
		}
}

//----------------------------------------------------------------------------

