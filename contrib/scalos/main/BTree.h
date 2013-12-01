// BTree.h
// $Date$
// $Revision$

#ifndef BTREE_H
#define BTREE_H

//----------------------------------------------------------------------------

struct BTAVLnode
{
	const void *avln_Key;
	void *avln_Data;
	int avln_Balance;
	BOOL avln_Hidden;
	struct BTAVLnode *avln_Left;
	struct BTAVLnode *avln_Right;
};

struct BTree
{
	struct BTAVLnode *btr_Root;
	size_t btr_NodeCount;
	void (*btr_DisposeData)(void *);
	void (*btr_DisposeKey)(void *);
	int (*btr_Compare)(const void *, const void *);
};

//----------------------------------------------------------------------------

struct BTree *BTreeCreate(void (*DisposeData)(void *),
			   void (*DisposeKey)(void *),
			   int (*Compare)(const void *, const void *));
void BTreeDispose(struct BTree *tree);
int BTreeInsert(struct BTree *tree, const void *key, void *data);
BOOL BTreeHide(struct BTree *tree, const void *key);
void *BTreeFind(struct BTree *tree, const void *key);
size_t BTreeNumberOfNodes(struct BTree *tree);
void BTreeDumpTree(struct BTree *tree, void *topass,
		void (*print)(const void *key, const void *data, void *topass));

//----------------------------------------------------------------------------

#endif /* BTREE_H */
