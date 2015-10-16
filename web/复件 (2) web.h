#ifndef _WEB_H_
#define _WEB_H_






#define WEB_ASSERT(x) \
	if(!(x)){\
		s_printf("(%s) has assert failed at %s\r\n",#x, __FUNCTION__);\
		while(1);\
	}




#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

#define ETHERNET_INT_PRIORITY   0xC0



/* The maximum length of the string comprising the tag name */
#ifndef LWIP_HTTPD_MAX_TAG_NAME_LEN
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 8
#endif

/* The maximum length of string that can be returned to replace any given tag */
#ifndef LWIP_HTTPD_MAX_TAG_INSERT_LEN
#define LWIP_HTTPD_MAX_TAG_INSERT_LEN 192
#endif

#ifndef LWIP_HTTPD_MAX_CGI_PARAMETERS
#define LWIP_HTTPD_MAX_CGI_PARAMETERS 16
#endif

#ifndef NUM_FILE_HDR_STRINGS
#define NUM_FILE_HDR_STRINGS 3
#endif


enum tag_check_state {
  TAG_NONE,       /* Not processing an SSI tag */
  TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
  TAG_FOUND,      /* Tag name being read, looking for lead-out start */
  TAG_LEADOUT,    /* Tag lead out "-->" being processed */
  TAG_SENDING     /* Sending tag replacement string */
};


typedef struct {
	struct fs_file *handle;
	char *file;
	
	uint32_t left;       /* Number of unsent bytes in buf. */
  uint8_t retries;
	
/*LWIP_HTTPD_SSI || LWIP_HTTPD_DYNAMIC_HEADERS*/
	char *buf;        /* File read buffer. */
  int buf_len;      /* Size of file read buffer, buf. */
	const char *parsed;     /* Pointer to the first unparsed byte in buf. */
	const char *tag_started;/* Poitner to the first opening '<' of the tag. */
	const char *tag_end;    /* Pointer to char after the closing '>' of the tag. */
  uint32_t parse_left; /* Number of unparsed bytes in buf. */
  uint16_t tag_index;   /* Counter used by tag parsing state machine */
  uint16_t tag_insert_len; /* Length of insert in string tag_insert */
	uint16_t tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
	uint8_t tag_check;   /* true if we are processing a .shtml file else false */
  uint8_t tag_name_len; /* Length of the tag name in string tag_name */
  char tag_name[LWIP_HTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
  char tag_insert[LWIP_HTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
  enum tag_check_state tag_state; /* State of the tag processor */
	
	/*LWIP_HTTPD_CGI*/
	char *params[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
  char *param_vals[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
	
	/*LWIP_HTTPD_DYNAMIC_HEADERS*/
	const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
  uint16_t hdr_pos;     /* The position of the first unsent header byte in the
                        current string */
  uint16_t hdr_index;   /* The index of the hdr string currently being sent. */
	
	/*LWIP_HTTPD_SUPPORT_POST*/
	uint32_t post_content_len_left;
	uint32_t unrecved_bytes;
  struct tcp_pcb *pcb;
  uint8_t no_auto_wnd;
	
}http_state_t;


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




