/*
 *
 * req.h
 *
 */

#ifndef AMIGAOS_REQ_H
#define AMIGAOS_REQ_H

void req_init(char *title);
void req_name(int item, char *str);
void req_add(int item, char *str);
void req_default(int item, int nr);
void req_show(void);
int req_get(int item);
void req_exit(void);

#endif
