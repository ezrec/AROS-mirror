struct DisasmBase
{
    struct Library lib;
    BPTR seglist;
    struct SignalSemaphore sem;
    bfd default_bfd;
};
