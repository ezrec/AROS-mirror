/* ----------------------------------------------------------------------------------
 *
 * Copyright (c) 2003 by John Fitzgibbon.
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice,
 * this permission notice and the following disclaimer appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * ----------------------------------------------------------------------------------
 *
 * ttel.c
 *
 * ttel either stands for "tiny telnet" or "trivial telnet", I can't decide which.
 *
 * ttel is a lightweight general purpose telnet-like client, suitable for use in
 * embedded or diskless systems where memory or file storage space is at a premium.
 * The code is is a bit ugly, but it compiles to a small sized executable.
 *
 * On i386 GNU systems, (tested on FreeBSD 4.7 with gcc 2.95.4 and Red Hat Linux 7.3
 * with gcc 2.96), it should compile cleanly with:   gcc -Wall -O3 -o ttel ttel.c
 *
 * ttel supports a basic ascii interface, with a single line input buffer. The
 * Backspace and Delete keys, (#127, #8), are supported for command correction.
 * Input is normally buffered line-by-line, but the Esc key, (#27), can be
 * used to force currently buffered characters to be sent to the remote end.
 * (I find this useful for testing systems that should accept fragmented input.)
 * Ctrl-C, (#3), will close the current connection.
 *
 * ----------------------------------------------------------------------------------
 * history:
 * 2003-01-13  fitz          More explicit masking on echo_hex()
 * 2003-01-07  fitz          Initial version.
 * ---------------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define _DEBUG_TTEL 

#define CTRL_C 0x03
#define ESC    0x1B
#define LF     0x0A
#define CR     0x0D
#define BS     0x08
#define DEL    0x7F
#define TAB    0x09

static int hex_mode = 0;

/* ----------------------------------------------------------------------------------
 *
 * echo_hex() - prints a single byte in hex , (00 to FF)
 *
 * ---------------------------------------------------------------------------------- */
#define hex(c) ((c) & 0x0F)
#define echo_hex_digit(c) (hex(c) > 9) ? putchar ('A' - 10 + hex(c)) : putchar('0' + hex(c))
#define echo_hex(c) echo_hex_digit((c) >> 4); echo_hex_digit(c)

/* ----------------------------------------------------------------------------------
 *
 * echo() - prints the specified character to stdout, displaying hex if required.
 *
 * ---------------------------------------------------------------------------------- */
static void echo(const char ch)
{
    if (hex_mode)
    {
        putchar('[');
        echo_hex(ch);
        putchar(']');
    }
    if ((ch == TAB) || (ch == CR) || (ch == LF) ||
        ((ch >= ' ') && (ch < DEL)))
    {
        putchar(ch);
    }
    else
    {
        putchar('.');
    }
    fflush(stdout);
}

/* ----------------------------------------------------------------------------------
 *
 * reecho_str() - reechos "len" bytes from "str".
 *
 * ---------------------------------------------------------------------------------- */
#define reecho_str(str, len) do { outp = (char *)str; \
    while (outp - str < len) { echo(*outp); outp++; } } while (0)

/* ----------------------------------------------------------------------------------
 *
 * err_printf() - a very simple message printing routine, (to stdout).
 *
 * ---------------------------------------------------------------------------------- */
#define err_printf(str) do { msgp = (char *)str; \
    while (*msgp != '\0') { putchar(*msgp); msgp++; } } while (0)

/* ----------------------------------------------------------------------------------
 *
 * atou() - Converts a string to an unsigned int, (like an unsigned atoi()).
 *
 * ---------------------------------------------------------------------------------- */
#define atou(ustr,u) do {                      \
    u = 0;                                     \
    while ((*ustr >= '0') && (*ustr <= '9')) { \
        u = (u * 10) + (*ustr - '0'); ustr++;  \
    }                                          \
} while (0)

/* ----------------------------------------------------------------------------------
 *
 * atoipaddr() - Converts a string to an ip address, (like inet_aton()).
 * Note that any leading '0.' will be ignored, so 0.1.1.1.1 is the same as 1.1.1.1
 *
 * ---------------------------------------------------------------------------------- */
#define atoipaddr(ipstr,ip) do { {                                  \
    unsigned int ipbyte;                                            \
    ip = 0;                                                         \
    while (*ipstr != '\0') {                                        \
        atou(ipstr,ipbyte);                                         \
        if ((ip > 0x00FFFFFF) || (ipbyte > 255) ||                  \
            ((*ipstr != '\0') && (*ipstr != '.'))) goto err_usage;  \
        if (*ipstr != '\0') ipstr++;                                \
        ip = (ip << 8) + ipbyte;                                    \
    }                                                               \
    ip = htonl(ip);                                                 \
} } while (0)

/* ----------------------------------------------------------------------------------
 *
 * main() pretty much does it all:
 *
 * - Process the command line.
 * - Put the TTY in raw mode (char-by-char).
 * - Connect a socket to the remote ip/port.
 * - Process input from both ends in a while loop,
 *   exiting when either end closes the connection.
 * - Clean up and exit.
 *
 * Note: Using a series of code blocks in one function yields some nice optimization.
 *
 * ---------------------------------------------------------------------------------- */
#define OUTBUF_SIZE 1024
#define INBUF_SIZE  2048

int main(int argc, char **argv)
{

    int                 retval   = -1;
    int                 convert_nl = 1;
    int                 sock;
    char                *msgp;
    struct sockaddr_in  inaddr;
    struct termios      save_termattr;

    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = htonl(0x7F000001); /* 127.0.0.1 */

#ifdef _DEBUG_TTEL
    err_printf ("DEBUG: Checking command parameters.\n");
#endif

    /*
     * Step 1: Process the command line
     */
    {
        int  port = -1;
        char *opt;
        char optch;
        int  argcnt;

        for (argcnt = 1; argcnt < argc; argcnt ++)
        {
            opt = argv[argcnt];
            if (*opt != '-')
            {
                goto err_usage;
            }
            else
            {
                opt++;
                switch (*opt)
                {
                    case 'p':
                    case 'h':
                        optch = *opt;
                        opt++;
                        if (*opt == '\0')
                        {
                            if (argcnt < argc - 1)
                            {
                                argcnt++;
                                opt = argv[argcnt];
                            }
                            else
                            {
                                goto err_usage;
                            }
                        }
                        switch (optch)
                        {
                            case 'h':
#ifdef _DEBUG_TTEL
                                err_printf ("DEBUG: host=");
                                err_printf (opt); err_printf ("\n");
#endif
/*
                                if (inet_aton(opt, &(inaddr.sin_addr)) == -1)
                                    goto err_usage;
*/
                                atoipaddr(opt, inaddr.sin_addr.s_addr);
                                break;
                            case 'p':
#ifdef _DEBUG_TTEL
                                err_printf ("DEBUG: port=");
                                err_printf (opt); err_printf ("\n");
#endif
/*
                                port = atoi(opt);
*/
                                atou(opt, port);
                                if (*opt != '\0') goto err_usage;
                                break;
                        }
                        break;
                    case 'n':
#ifdef _DEBUG_TTEL
                        printf ("DEBUG: remote CR/LF conversion turned OFF\n", opt);
#endif
                        convert_nl = 0;
                        break;
                    case 'x':
#ifdef _DEBUG_TTEL
                        printf ("DEBUG: hex mode turned ON\n", opt);
#endif
                        hex_mode = 1;
                        break;
                    default:
                        goto err_usage;
                        break;
                }
            }
        }

        /* Check mandatory parameters */
        if (port == -1)
        {
            goto err_usage;
        }

        inaddr.sin_port = htons(port);
    }

#ifdef _DEBUG_TTEL
    err_printf ("DEBUG: Command parameters okay. Attempting to put TTY in raw mode.\n");
#endif

    /*
     * Step 2: Attempt to put the tty in raw character-by-character i/o mode
     */
    {
        struct termios termattr;

        if (tcgetattr (STDIN_FILENO, &termattr) < 0)
        {
            err_printf("can't get tty info.\n");
            goto err_notty;
        }
        save_termattr = termattr;

        termattr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        termattr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        termattr.c_cflag &= ~(CSIZE | PARENB);
        termattr.c_cflag |= CS8;
        termattr.c_oflag &= ~(OPOST);
        termattr.c_cc[VMIN] = 1;
        termattr.c_cc[VTIME] = 0;

        if (tcsetattr (STDIN_FILENO, TCSANOW, &termattr) < 0)
        {
            err_printf("can't set tty info.\n");
            goto err_notty;
        }
    }

#ifdef _DEBUG_TTEL
    err_printf ("DEBUG: TTY in raw mode. Attempting to connect to host.\r\n");
#endif

    /*
     * Step 3: Attempt to connect to the remote ip address/port
     */
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        err_printf("can't create socket.\r\n");
        goto err_nosock;
    }

    if (connect (sock, (struct sockaddr*) &inaddr, sizeof inaddr) == -1)
    {
        err_printf("can't connect.\r\n");
        goto err;
    }

#ifdef _DEBUG_TTEL
    err_printf ("DEBUG: Connected. Starting session.\r\n");
#endif

    /*
     * Step 4: We're now connected and in raw tty i/o mode  - run the session
     */



    {
        int    pos = 0;
        int    keeppos = 0;
        int    read_count, i;
        char   ch, prev_ch = 0;
        char   *outp;
        fd_set fdset;
        char   outbuf[OUTBUF_SIZE];
        char   inbuf[INBUF_SIZE];
        char   keepbuf[INBUF_SIZE];
int y;

        while (1)
        {
            FD_ZERO (&fdset);
            FD_SET(STDIN_FILENO, &fdset); /* local input file descriptor */
            FD_SET(sock, &fdset);         /* remote socket descriptor    */
            if (select (sock + 1, &fdset, NULL, NULL, NULL) == -1)
            {
                goto done; /* select error */
            }
            /* Check for remote input from the socket */
            if (FD_ISSET(sock, &fdset))
            {
                if ((read_count = read(sock, inbuf, INBUF_SIZE)) <= 0)
                {
                    goto done;  /* socket closed or socket read error */
                }
                keeppos = 0;
                for (i = 0; i < read_count; i++)
                {
                    ch = inbuf[i];
/*AROS*/
  if ((atoi(ch) == 251) || (atoi(ch) == 252))     /* WILL or WONT */
      y = (254);                          /* -> DONT */
  if ((atoi(ch) == 251) || (atoi(ch)  == 252))     /* WILL or WONT */
      y = 254;                          /* -> DONT */
int vv;
vv = itoa(y);
write(sock,vv,1);


                    if (convert_nl)
                    {
                        if ((prev_ch == CR) && (ch != LF))
                            echo(LF);
                        if ((prev_ch == LF) && (ch != CR))
                            echo(CR);
                    }
                    prev_ch = (((prev_ch == CR) && (ch == LF)) ||
                               ((prev_ch == LF) && (ch == CR))) ? 0 : ch;
                    echo(ch);
                    keepbuf[keeppos] = ch;
                    keeppos++;
                    if ((ch == CR) || (ch == LF))
                        keeppos = 0;
                }
            }
            /* Check for local input on stdin */
            if (FD_ISSET(STDIN_FILENO, &fdset))
            {
                if ((read_count = read(STDIN_FILENO, inbuf, INBUF_SIZE)) <= 0)
                {
                    goto done; /* stdin read error */
                }
                for (i = 0; i < read_count; i++)
                {
                    ch = inbuf[i];
                    if (ch == CTRL_C)     /* Ctrl - C quits */
                    {
                        goto done;
                    }
                    if (ch != ESC)        /* Esc is not printed - it forces a send  */
                    {
                        if ((ch == DEL) || (ch == BS))
                        {                 /* Delete or Backspace key. Move back one */
                            if (pos)      /* char and re-echo the whole line so far */
                            {
                                pos--;
                                echo(CR);
                                reecho_str(keepbuf, keeppos);
                                reecho_str(outbuf, pos);
                            }
                        }
                        else              /* Normal char - echo and add to outbuf   */
                        {
                            echo(ch);
                            outbuf[pos] = ch;
                            pos ++;
                            if ((ch == CR) && (pos < OUTBUF_SIZE))
                            {
                                echo(LF);
                                outbuf[pos] = LF;
                                pos ++;
                            }
                        }
                    }
                    /* If outbuf is full, or we have an Esc or newline, send stuff */
                    if (pos && ((pos == OUTBUF_SIZE) || (ch == ESC) || (ch == CR)))
                    {
                        send(sock, outbuf, pos, 0);
                        /*
                         * If we didn't have space in outbuf to complete the CR/LF
                         * combination, send the missing char in a one-byte packet
                         */
                        if ((pos == OUTBUF_SIZE) && (outbuf[pos-1] == CR))
                        {
                            echo(LF);
                            outbuf[0] = LF;
                            send(sock, outbuf, 1, 0);
                        }
                        pos = 0;
                    }
                }
            }
        }
    }

    /*
     * Step 5: Clean up, (and error handling)
     */
done:
#ifdef _DEBUG_TTEL
    err_printf ("DEBUG: Session complete. Cleaning up.\r\n");
#endif

    retval = 0;
err:
    close(sock);
err_nosock:
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &save_termattr);
err_notty:
    return (retval);

err_usage:
    err_printf(
        "\nUsage:\n\nttel [ -x ] [ -l ] [ -r ] [ -h <ip> ] -p <port>\n\n"
        "-p <port> - port to connect to\n"
        "-h <ip> - host ip address, (default 127.0.0.1)\n"
        "-x - display hex characters as well as ascii\n"
        "-n - don't change remote newlines to CR/LF\n\n"
        "When connected, Ctrl-C ends the session.\n"
        "Esc sends current input to the server.\n"
        "(Input is normally sent line by line.)\n\n"
        );
    goto err_notty;
}
