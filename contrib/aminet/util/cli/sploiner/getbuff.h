void *buffer_init(FILE *fp, size_t buf_size);
int getcbuf(void *buf);
int putcbuf(char c, void *buf);
int flush(void *buf);
void discard(void *buf);
