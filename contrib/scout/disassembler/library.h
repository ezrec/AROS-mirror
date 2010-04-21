struct DisasmBase
{
    struct Library lib;
    BPTR seglist;
    struct SignalSemaphore sem;
};

struct AbortContext
{
    jmp_buf buf;
    char *file;
    unsigned int line;
};
