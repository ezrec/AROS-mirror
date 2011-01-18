APTR CreateDisassemblerObject(bfd *abfd);
void DeleteDisassemblerObject(APTR ob);
APTR DisassembleObject(APTR ob, struct DisData *ds, struct DisasmBase *DisassemblerBase);
