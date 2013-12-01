// Wrappers.h
// $Date$
// $Revision$

#ifndef WRAPPERS_H
#define WRAPPERS_H


// Wrapper functions for SCA_MoveNode() to allow effective type checking

static void INLINE SCA_MoveIconNode(struct ScaIconNode **SrcList, struct ScaIconNode **DestList,
	struct ScaIconNode *iconNode)
{
	SCA_MoveNode((struct ScalosNodeList *) SrcList, 
		(struct ScalosNodeList *) DestList, &iconNode->in_Node);
}


static void INLINE SCA_MoveDevIconNode(struct ScaDeviceIcon **SrcList, struct ScaDeviceIcon **DestList,
	struct ScaDeviceIcon *diNode)
{
	SCA_MoveNode((struct ScalosNodeList *) SrcList, 
		(struct ScalosNodeList *) DestList, &diNode->di_Node);
}


static void INLINE SCA_MoveBobNode(struct ScaBob **SrcList, struct ScaBob **DestList,
	struct ScaBob *bobNode)
{
	SCA_MoveNode((struct ScalosNodeList *) SrcList, 
		(struct ScalosNodeList *) DestList, &bobNode->scbob_Node);
}

static void INLINE SCA_MoveWSNode(struct ScaWindowStruct **SrcList, struct ScaWindowStruct **DestList,
	struct ScaWindowStruct *wsNode)
{
	SCA_MoveNode((struct ScalosNodeList *) SrcList, 
		(struct ScalosNodeList *) DestList, &wsNode->ws_Node);
}

/* ------------------------------------------------- */

// Wrapper functions for SCA_AllocStdNode() to allow effective type checking

static struct ScaIconNode INLINE *SCA_AllocIconNode(struct ScaIconNode **IconList)
{
	return (struct ScaIconNode *) SCA_AllocStdNode((struct ScalosNodeList *) IconList, NTYP_IconNode);
}

/* ------------------------------------------------- */

#endif	/* WRAPPERS_H */
