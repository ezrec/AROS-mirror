ULONG read_addressing_mode(UBYTE mode_field, UBYTE reg_field, UBYTE size);
ULONG read_addressing_mode_nochange(UBYTE mode_field, UBYTE reg_field, UBYTE size);
ULONG getaddress_addressing_mode(UBYTE mode_field, UBYTE reg_field, UBYTE size);
VOID write_addressing_mode(UBYTE mode_field, UBYTE reg_field, ULONG val, UBYTE size);

