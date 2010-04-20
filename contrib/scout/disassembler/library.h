struct DisasmBase
{
    struct Library lib;
    BPTR seglist;
    struct SignalSemaphore sem;
};
