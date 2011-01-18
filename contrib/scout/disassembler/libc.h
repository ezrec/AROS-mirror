#define EXCEPTION_ABORT     1
#define EXCEPTION_NO_MEMORY 2

struct LibcData
{
    jmp_buf abort_buf;
    char *exception_arg1;
    unsigned int exception_arg2;
    void *pool;
    void *userdata;
};

void libc_setcontext(struct LibcData *data);
void libc_unsetcontext(void);
