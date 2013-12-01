ULONG mRollover_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
	struct MUI_RollOver_InstanceData *data = INST_DATA(cl, obj);
	struct TagItem *tags;

	if(!( obj = DoSuperNew( cl, obj, TAG_MORE, ((struct opSet *)msg)->ops_AttrList ) ))
	{
		return(0);
	}

	DEBUGLOG(printf("OM_NEW, getting any tag values...\n");)

	// --- Set Any Tags Found At Creation Time
	tags = msg->ops_AttrList;
	data->iMouseOver = 0;
	data->Item = (struct PanelItem *)GetTagData(MUIA_RollOver_ItemInfo, NULL, tags);
	data->app = (struct MUIApp *)GetTagData(MUIA_RollOver_AppStruct, NULL, tags);

	return((ULONG)obj);
}




SAVEDS(ULONG) ASM RollOverImageDispatcher(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, Msg msg))
{
	switch (msg->MethodID)
	{
		case OM_NEW						: return(mRollover_New          (cl, obj, (APTR)msg));
		case OM_SET						: return(mRollover_Set          (cl, obj, (APTR)msg));
		case MUIM_RollOver_Execute	: return(mRollover_Execute      (cl, obj, (APTR)msg));
		case MUIM_Setup				: return(mRollover_Setup        (cl, obj, (APTR)msg));
		case MUIM_Cleanup				: return(mRollover_Cleanup      (cl, obj, (APTR)msg));
		case MUIM_HandleEvent		: return(mRollover_HandleEvent  (cl, obj, (APTR)msg));

		// --- This sub-class doesn't support the method sent, so pass it up to
		//     its parent class.
		default							: return(DoSuperMethodA         (cl, obj, msg));
	}
}