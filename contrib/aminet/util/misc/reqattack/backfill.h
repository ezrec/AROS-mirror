struct LayerHookMsg
{
	struct Layer *layer;
	struct Rectangle bounds;
	LONG offsetx;
	LONG offsety;
};


void MyBackfillFunc(struct Hook *hook,struct RastPort *rp,struct LayerHookMsg *msg);

