/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#ifndef KERN_AMIGA_LOG_H
#define KERN_AMIGA_LOG_H

#define LOG_TASK_NAME "NETTRACE"
#define LOG_TASK_PRI 4
#define LOG_BUFS 4
#define LOG_BUF_LEN 128
#define TOCONS	0x01
#define TOTTY	0x02
#define TOLOG	0x04
#define END_LOG -1

/*
 * Configuration structure
 */ 
struct log_cnf {
  u_long log_bufs;
  u_long log_buf_len;
}; 
extern struct log_cnf log_cnf;

/*
 * These are options to config log
 */
#define LOG_CLOSE 0xff000000
#define LOG_CONFILE 0xfe000000
#define LOG_LOGFILE 0xfd000000
#define LOG_PORTOPEN 0xfc000000
#define LOG_PORTCLOSE 0xfb000000
#define LOG_CONGIF 0xff000000

extern struct Task *Nettrace_Task;
extern struct Process *logProc;
extern BOOL log_init(void);
extern void log_deinit(void);
extern struct log_msg *GetLogMsg(struct MsgPort *);

extern struct MsgPort *logReplyPort;
extern struct MsgPort *logPort;

struct log_msg {
  struct Message msg;		/* Standard Exec message */
  ULONG level;			/* Level of log message */
  UBYTE * string;		/* Pointer to string */
  ULONG chars;			/* Length of string */
  ULONG time;			/* Logging time */
};

extern struct log_msg *log_message;
extern STRPTR consolename, logfilename;
extern struct log_cnf log_cnf;

/* extern void stuffchar(...);*/

#endif /* KERN_AMIGA_LOG_H */
