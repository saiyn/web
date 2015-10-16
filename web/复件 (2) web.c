#include "common_head.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "lwiplib.h"
#include "fs.h"
#include "enet_fs.h"
#include "httpd_structs.h"
#include "lwip/api.h"

#include "web.h"

#include "System_Task.h"

uint8 g_MACArray[6];

#define HTTPD_BUF_SIZE   2048
#define HTTPD_SEND_BUF_SIZE   1024
#define JSON_BUF_SIZE  2048
//#define DEFAULT_FILE_NAME  "/GeneralSettings.htm"

#define DEFAULT_FILE_NAME  "/home.htm"

#define SUB_MODE  \
"\"submodel-items\":[{\"sub-name\":\"Mini InRoom\", \"sub-id\":\"1\"}," \
"{\"sub-name\":\"Mini FlexSub\", \"sub-id\":\"2\"}],"

#define AUDIO_LEVEL \
	"\"auto-off-items\":[{\"name\":\"5\", \"id\":\"1\"}," \
	"{\"name\":\"15\", \"id\":\"2\"}],"

#define POWER_ROUTING \
	"\"power-on-router-items\":[{\"name\":\"Manual\", \"id\":\"1\"}," \
	"{\"name\":\"Sense\", \"id\":\"2\"}],"
	
#define INPUT_SOURCE \
	"\"input-source-items\":[{\"name\":\"XLR\", \"id\":\"1\"}," \
	"{\"name\":\"RCA\", \"id\":\"2\"}],"
	
#define EQ_PRESET  \
	"\"preset-items\":[{\"preset-name\":\"REFERENCE\", \"preset-id\":\"1\"}," \
	"{\"preset-name\":\"CINEMA\", \"preset-id\":\"2\"}],"

#define CGI_ARGC_MAX   4

const CGI_t g_pCGI;
int g_NumCGI;

static int Cgi_setup_page_handler(http_state_t *,int,char *[]);
static int Cgi_home_page_handler(http_state_t *,int,char *[]);
static int Cgi_rooeq_page_handler(http_state_t *,int,char *[]);

CGI_Page_t g_Page_CGI[] ={
	{"page=setup", Cgi_setup_page_handler},
	{"page=home",  Cgi_home_page_handler},
	{"page=roomEQ", Cgi_rooeq_page_handler}
};

#define CGI_PAGE_NUM  (sizeof(g_Page_CGI)/sizeof(g_Page_CGI[0]))

static char httpd_buf[HTTPD_BUF_SIZE];
static char httpd_send_buf[HTTPD_SEND_BUF_SIZE];
static char json_output_buf[JSON_BUF_SIZE];
static char *pjson = json_output_buf;

#define JSON_BUF_OVERFLOW(x)  (pjson + x < &json_output_buf[JSON_BUF_SIZE - 1])


#define JSON_OUTPUT_BEGIN()  { \
	pjson = json_output_buf; \
	memset(json_output_buf, 0, sizeof(json_output_buf)); \
	strcat(pjson, "{"); \
	pjson++; \
}

#define JSON_OUTPUT_END() { \
	WEB_ASSERT(JSON_BUF_OVERFLOW(1)); \
	strcat(pjson, "}"); \
}

static char jsbuf[512] = {0};

static void json_output_print(const char *fmt, ...)
{
	  va_list ap;
	  
	  memset(jsbuf, 0, sizeof(jsbuf));
	  va_start(ap, fmt);
	  vsprintf(jsbuf, fmt, ap);
    va_end(ap);

    WEB_ASSERT(JSON_BUF_OVERFLOW(strlen(jsbuf))); 

    strcat(pjson, jsbuf);
    pjson += strlen(jsbuf);

}



/** Allocate a struct http_state. */
static http_state_t*
http_state_alloc(void)
{
  http_state_t *ret = NULL;

  ret = (http_state_t *)mem_malloc(sizeof(http_state_t));

  if (ret != NULL) {
    /* Initialize the structure. */
    memset(ret, 0, sizeof(http_state_t));
    /* Indicate that the headers are not yet valid */
    ret->hdr_index = 0;
  }
  return ret;
}

/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_free(http_state_t*hs)
{
  if (hs != NULL) {
    if(hs->handle) {
      fs_close(hs->handle);
      hs->handle = NULL;
    }
    mem_free(hs);
  }
}

static void http_post_rxdata(http_state_t *hs, char *data, uint16_t len)
{
	
	
}

static void do_httpd_send(struct netconn *conn, http_state_t *hs)
{
	uint32_t len;
	
	 if(!hs->tag_check)
	 {
		 while(hs->left){
			 if(hs->left > TCP_MSS){
				 len = TCP_MSS - 24;
			 }
			 else{
				 len = hs->left;
			 }
			 
			 /*we are not processing an sHTML file so no tag checking is necessary*/
		    netconn_write(conn, (void *)hs->file, len, NETCONN_COPY);
		   /*there is no restrication on the lenght of the data, the stack core will handle that*/
		   hs->left -= len;
			 hs->file += len;
		 }
		 
	 }
	 else
	 {
		  /**/
	 }
}

static void http_send_data(struct netconn *conn, http_state_t *hs)
{
	  err_t err = ERR_OK;
	  char *psend = httpd_send_buf;
	  uint16_t len = 0;
	
	  memset(httpd_send_buf, 0, sizeof(httpd_send_buf));
	  if(hs->hdr_index < NUM_FILE_HDR_STRINGS)
		{
			 for(;hs->hdr_index < NUM_FILE_HDR_STRINGS; hs->hdr_index++)
			{
				   strcat(psend, hs->hdrs[hs->hdr_index]);
				   len += strlen(hs->hdrs[hs->hdr_index]);
			}
			
			SYS_TRACE("send http header %s\r\n", psend);
			netconn_write(conn, (void *)psend, len, NETCONN_NOCOPY);
		}
		else{
			SYS_TRACE("waring!! will not send http header index=%d\r\n", hs->hdr_index);
		}
		
		/*have we run out of file data to send? if so, we need to read the next block from the file*/
		if(hs->left == 0)
		{
			 int count;
			 hs->buf = httpd_send_buf;
			 hs->buf_len = HTTPD_SEND_BUF_SIZE;
			 memset(httpd_send_buf, 0, sizeof(httpd_send_buf));
			 if(hs->handle != NULL)
			 {
				   SYS_TRACE("will send total %d bytes data\r\n", hs->handle->len);
				 
				  do{
						  count = fs_read(hs->handle, hs->buf, HTTPD_SEND_BUF_SIZE);
						  if(count < 0)
							{
								 /*we reach the end of the file so this request is done */
								 //SYS_TRACE("reach the end of the file\r\n");
								 fs_close(hs->handle);
								 hs->handle = NULL;
								 return;
							}
							
							/*set up to send the block of data we just read*/
							hs->file = hs->buf;
							hs->left = count;
//							hs->parse_left = count;
//							hs->parsed = hs->buf;
							
							//SYS_TRACE("will send %d bytes data\r\n", count);
							do_httpd_send(conn, hs);
					}while(1);
			 }
			
		}
		else
		{
			  do_httpd_send(conn, hs);
		}
}

static void get_http_headers(http_state_t *hs, char *uri)
{
	 uint8_t loop;
	 char *pwork;
	 char *pext;
	 char *pvar;
	
	 /*in all cases, the second header we send is the server identification so set it here*/
	 hs->hdrs[1] = g_psHTTPHeaderStrings[HTTP_HDR_SERVER];
	
	 if(uri == NULL)
	 {
		  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
		  hs->hdrs[2] = g_psHTTPHeaderStrings[DEFAULT_404_HTML];
		 
		  /*set up to send the first header string*/
		  hs->hdr_index = 0;
		  hs->hdr_pos = 0;
		  SYS_TRACE("fill NOT FOUND HEADER\r\n");
		  return;
	 }
	 else
	 {
		   if(strstr(uri, "404"))
			 {
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
			 }
			 else if(strstr(uri, "400"))
			 {
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_BAD_REQUEST];
			 }
			 else if(strstr(uri, "501"))
			 {
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_IMPL];
			 }
			 else
			 {
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_OK];
			 } 
			 
			 /*check if the URI*/
			 pvar = strchr(uri, '?');
			 if(pvar)
			 {
				  *pvar = 0;
			 }
			 
			 /*Get a pointer to the file extension*/
			 pext = NULL;
			 pwork = strchr(uri, '.');
			 while(pwork)
			 {
				  pext = pwork + 1;
				  pwork = strchr(pext, '.');
			 }
			 
			 
			 SYS_TRACE("uri=%s, ex=%s\r\n", uri,pext);
			 
			 for(loop = 0; (loop < NUM_HTTP_HEADERS) && pext; loop++)
			 {
				  if(strcmp(g_psHTTPHeaders[loop].extension, pext) ==0)
					{
						 hs->hdrs[2] = g_psHTTPHeaderStrings[g_psHTTPHeaders[loop].headerIndex];
						 SYS_TRACE("find content type = %s\r\n", hs->hdrs[2]);
						 break;
					}
			 }
			 
			 /*Reinstate the parameter marker if there was one in the original URI*/
			 if(pvar)
			 {
				  *pvar = '?';
			 }
	 }
	 
	 /*Does the URL passed have any file extension? if not, we assume it is a special-case URL used 
	 for control state notification and we do not send any HTTP headers with the reponse*/
	 if(!pext)
	 {
		  /*force the header index to a value indicating that all headers have already been sent*/
		  hs->hdr_index = NUM_FILE_HDR_STRINGS;
		  SYS_TRACE("it is a special-case URL\r\n");
	 }
	 else
	 {
		 
		 
		  SYS_TRACE("loop = %d\r\n", loop);
		  /*did we find a matching extension?*/
		  if(loop == NUM_HTTP_HEADERS)
			{
				 /*No - use the default, plain text file type*/
				 hs->hdrs[2] = g_psHTTPHeaderStrings[HTTP_HDR_DEFAULT_TYPE];
				 SYS_TRACE("didn't find content type set to default\r\n");
			}
			
			/*set up to send the first header string*/
		  hs->hdr_index = 0;
		  hs->hdr_pos = 0;
	 }
}

static err_t http_init_file(http_state_t *hs, struct fs_file *file, const char *uri)
{
	 if(file != NULL)
	 {
		  hs->tag_index = 0;
		  hs->tag_state = TAG_NONE;
		  hs->parsed = file->data;
		  hs->parse_left = file->len;
		  hs->tag_end = file->data;		 
		  hs->handle = file;
		  hs->file = NULL;
		  hs->left = 0;
		  hs->retries = 0;
	 }
	 
	 get_http_headers(hs, (char *)uri);
	  
	 return ERR_OK;
}

static int _cgi_agr_parse(char *arg, char *argc[])
{
	 int index = 0;
	 char *p = NULL;
	
	 p = strtok(arg, "&");
	 while(p){
		 if(index < CGI_ARGC_MAX){
			  argc[index] = p;
		 }
		 else{
			 break;
		 }
		 p = strtok(NULL, "&");
		 index++;
	 }
	 
	 return index;
}

static void http_cgi_handler(http_state_t *hs, char *uri, char *arg)
{
	  int argv;
	  char *argc[CGI_ARGC_MAX] = {NULL};
    uint8_t index;

    argv = _cgi_agr_parse(arg,argc);
	  SYS_TRACE("cgi contains %d arg\r\n", argv);
	  
		for(index = 0; index < CGI_PAGE_NUM; index++){
			if(strcmp(g_Page_CGI[index].PageName, argc[0]) == 0){
				g_Page_CGI[index].fnCGIPageHandler(hs,argv,argc);
				break;
			}
		}
}

static err_t http_find_file(http_state_t *hs, char *uri)
{
	 struct fs_file *file = NULL;
	 char *params;
	 int i;
	 int count;
	
	 /*By default, assume we will not be processing server-side-includes tags*/
	 hs->tag_check = false;
	 /*Have we been asked for the default root files*/
	 if((uri[0] == '/') && (uri[1] == 0))
	 {
		  file = fs_open(DEFAULT_FILE_NAME);
		  uri = DEFAULT_FILE_NAME;
		  
		  if(file == NULL)
	    {
		    /*None of the default filenames exist so send back a 404 page*/
		    //file = http_get_404_file(&uri);
				SYS_TRACE("open index page file fail\r\n");
	    }
			else
			{
				 SYS_TRACE("open index page file success\r\n");
			}
	 }
	 else
	 {
		  /*No - we've been asked for a specific file*/
		 /*First, isolate the base URI*/
		 params = (char *)strchr(uri, '?');
		 if(params != NULL)
		 {
			  *params = 0;
			   params++;
			   SYS_TRACE("CGI=%s---uri=%s\r\n", params, uri);
			   http_cgi_handler(hs, uri, params);
		 }
		 else{
			 file = fs_open(uri);
		   if(file == NULL)
		   {
				 SYS_TRACE("Try open %s fail\r\n", uri);
				 uri = NULL;
			 }
			 else
			 {
				 SYS_TRACE("Try open %s success\r\n", uri);
			 }
		 }
		 
	 }
	 
   return http_init_file(hs,file,uri);	 
	 
}

err_t http_parse_request(http_state_t *hs, char *data, uint16_t len)
{
	char *crlf;

	/*wait for CRLF before parsing anything*/
  crlf = 	strstr(data, "\r\n");
	if(crlf != NULL)
	{
		 int is_post = 0;
		 char *sp1, *sp2;
		 uint16_t left_len, uri_len;
		
		 if(strncmp(data, "GET ", strlen("GET ")) ==0)
		 {
			  sp1 = data + 3;
		 }
		 else if(strncmp(data, "POST ", strlen("POST ")) ==0)
		 {
			  is_post = 1;
			  sp1 = data + 4;
		 }
		 else
		 {
			   return ERR_ARG;
		 }
		 
		 /*if we come here, method is OK, parse URI*/
		 left_len = len - ((sp1 + 1) - data);
		 sp2 = strstr(sp1 + 1, " ");
		 if(sp2 != NULL)
		 {
				uri_len = sp2 - (sp1 + 1);
			  if((sp2 != 0) && (sp2 > sp1))
				{
					 char *uri = sp1 + 1;
					 /*null-terminate the METHOD*/
					 *sp1 = 0;
					 uri[uri_len] = 0;
					 SYS_TRACE("Received \"%s\" request for URI: \"%s\"\r\n", data, uri);
					
					 if(is_post)
					 {
						  
					 }
					 else
					 {
						   return http_find_file(hs,uri);
					 }
				}
		 }
		 else
		 {
			   SYS_TRACE("invalid URI\r\n");
		 }
	}
	 
   return ERR_ARG;
}

static err_t do_process(struct netconn *conn, http_state_t *hs, char *data, uint16_t len)
{
	 err_t parsed = ERR_ISCONN;
	
	 /*first check whether we are in post context*/
	 if(hs->post_content_len_left > 0)
	 {
		 /*reset idle counter when POST data is received*/
		 hs->retries = 0;
		 /*this is data for a POST, pass the complete data to the application*/
		 http_post_rxdata(hs, data, len);
		 
		 if(hs->post_content_len_left == 0)
		 {
			  /*all data received, send reponse or close connection*/
			  http_send_data(conn,hs);
			  parsed = ERR_CLSD;
		 }

	 }
	 else
	 {
		  if(hs->handle == NULL)
			{
				 parsed = http_parse_request(hs, data, len);
			}
			else
			{
				 SYS_TRACE("http_recv: already sending data\r\n");
			}
			
			if(parsed == ERR_OK)
			{
				 SYS_TRACE("http request parse done, try to send reponse\r\n");
				
				 if(hs->post_content_len_left == 0)
				 {
					   http_send_data(conn,hs);
					 
					   parsed = ERR_CLSD;
				 }
			}
			else if(parsed == ERR_ARG)
			{
				 
			}
	 }
	 
	 return parsed;
}

static void httpd_process(struct netconn *conn, http_state_t *hs)
{
	struct netbuf *inbuf;
	uint16_t len;
	char *pdata;
	err_t parsed = ERR_ISCONN;
	
	netconn_set_recvtimeout(conn, 1000);
	
	while(netconn_recv(conn, &inbuf) == ERR_OK)
	{
		  memset(httpd_buf, 0, sizeof(httpd_buf));
		  pdata = httpd_buf;
		  netbuf_first(inbuf);
		  do{
				  
				  netbuf_data(inbuf, (void **)&pdata, &len);
				  SYS_TRACE("Receive %d bytes data\r\n", len);
				  parsed = do_process(conn,hs, pdata, len);
				  if(ERR_CLSD == parsed){
						netbuf_delete(inbuf);
						return;
					}
			}while(netbuf_next(inbuf) >= 0);
			
			netbuf_delete(inbuf);
	}
	
}

static void web_server(void)
{
	struct netconn *conn, *newconn;
	http_state_t *hs = NULL;
	err_t err;
	
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 80);
	netconn_listen(conn);
	
	while(1)
	{
		err = netconn_accept(conn, &newconn);
		if(err != ERR_OK) 
		{
		 SYS_TRACE("netconn_accept =%d\r\n",err);
			continue;
    }
		SYS_TRACE("GET A NEW TCP CONNECT\r\n");
		
		hs = http_state_alloc();
		if(hs == NULL)
		{
			  netconn_delete(newconn);
			  continue;
		}

		httpd_process(newconn,hs);
		SYS_TRACE("HTTP Process done, will close the TCP\r\n");
		http_state_free(hs); 
		netconn_delete(newconn);
		vTaskDelay(10/portTICK_RATE_MS); 
	}
}

void web_task(void * pvParameter)
{
	 uint32_t ip,mask,gate;
	 uint32_t test;
	
	 ip = ((192 << 24) + (168 << 16) + (1 << 8) + 222);
	 mask = ((255 << 24) + (255 << 16) + (255 << 8));
	 gate = ((192 << 24) + (168 << 16) + (1 << 8) + (1 << 0));
	
	 fs_init();
	
	 lwIPInit(gSysClock, g_MACArray, ip, mask, gate, IPADDR_USE_STATIC);
	
	 IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
	
	 web_server();
}

static void do_setup_cgi_page(int argv, char *argc[])
{
	  
}

static void _load_setup_page_json_file(http_state_t *hs)
{
	  
}

static int Cgi_setup_page_handler(http_state_t *hs, int argv, char *argc[])
{
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_setup_cgi_page(argv,argc);
		 }
		 _load_setup_page_json_file(hs);
	}		
	
	return 0;
}

static void do_home_cgi_page(int argv, char *argc[])
{
	 if(argv < 4) return;
	
	 SYS_TRACE("%s----%s\r\n", argc[2], argc[3]);
}

static uint8_t sub_mode = 1;
static int volume = -1;

static void _load_home_page_json_file(http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"dsp-name\":\"%s\",", "Traid");
	 json_output_print("%s", SUB_MODE);
	 json_output_print("\"current-sub\":\"%d\",", sub_mode);
	 json_output_print("\"master-volume\":\"%d\",", volume);
	 json_output_print("%s", AUDIO_LEVEL);
	 json_output_print("\"audio-level\":\"%d\",", 1);
	 json_output_print("\"cur-auto-off-time\":\"%d\",", 1);
	 json_output_print("%s", POWER_ROUTING);
	 json_output_print("\"cur-power-on-router\":\"%d\",", 1);
	 json_output_print("%s", INPUT_SOURCE);
	 json_output_print("\"cur-source-index\":\"%d\",", 1);
	 json_output_print("%s", EQ_PRESET);
	 json_output_print("\"cur-preset-item\":\"%d\"", 1);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);
}

static int Cgi_home_page_handler(http_state_t *hs, int argv, char *argc[])
{
	 if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_home_cgi_page(argv,argc);
		 }
		 _load_home_page_json_file(hs);
	}		
	return 0;
}

static int Cgi_rooeq_page_handler(http_state_t *hs, int argv, char *argc[])
{
	

  return 0;  
}











