#ifndef _SSDP_H
#define _SSDP_H

//#include "typedef.h"

int uuid_bin2str(char *str, size_t max_len);
void  SsdpInit(struct netif *net);
void  SsdpDown(void);
#endif

