#ifndef _WEB_H_
#define _WEB_H_

#define MACADDR_1		0x2c
#define MACADDR_2		0xd0
#define MACADDR_3		0x5a
#define MACADDR_4		0xfa
#define MACADDR_5		0x58
#define MACADDR_6		0x28


#define ETHERNET_INT_PRIORITY   0xC0

#ifndef NUM_FILE_HDR_STRINGS
#define NUM_FILE_HDR_STRINGS 3
#endif

#define DEFAULT_FILE_NAME  "/home.htm"

#define HTTPD_RECV_BUF_SIZE  1024

#define HTTPD_SEND_BUF_SIZE  1024

typedef struct{
	struct fs_file *handle;
	char *file;
	char *buf;
	uint32 left;       /* Number of unsent bytes in file. */
	
	
	/*LWIP_HTTPD_DYNAMIC_HEADERS*/
	const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
	uint16_t hdr_index;   /* The index of the hdr string currently being sent. */
	uint32 post_content_len_left;
}http_state_t;


#define WEB_ASSERT(x) \
	if(!(x)){\
		s_printf("(%s) has assert failed at %s\r\n",#x, __FUNCTION__);\
		while(1);\
	}



typedef const char *(*CGIHandler)(int index, int numparams, char *param[], char *value[]);

typedef struct
{
	const char *CGIName;
	CGIHandler  fnCGIHandler;
}CGI_t;

typedef int (*Cgi_Page_Handler)(http_state_t *,int argv, char *argc[]);

typedef struct{
	const char *PageName;
	Cgi_Page_Handler fnCGIPageHandler;
}CGI_Page_t;





void web_task(void *parameter);





#endif




