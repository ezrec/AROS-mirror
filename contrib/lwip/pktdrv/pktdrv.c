/* pktdrv.c - dummy packet driver */

int eth_addr()
{
/*hmm */
}

int htonl()
{
/*convert values between host
       and network byte order*/
}

int htons()
{
/*convert values between host
       and network byte order*/
}

int ntohl()
{
/*convert values between host
       and network byte order*/
}

int ntohs()
{
/*convert values between host
       and network byte order*/
}

int kbhit()
{
/* terminal check keyboard function*/
/* would be nice with kbhit, gotoxy and all that in aros (termio.h)*/
}

static int get_packet_interrupt(int pkt_intr_arg)
{
}

int pkt_init(int pkt_intr_arg)
{
}

int pkt_release()
{
}

int pkt_send(int *buffer)
{
}

int pkt_get_current_length()
{
}

void pkt_drop_current()
{
}

int pkt_read_current(unsigned char *buffer, unsigned int start, unsigned int length)
{
}

int *pkt_received()
{
}

