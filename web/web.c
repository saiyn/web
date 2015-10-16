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
#include "lwip/ip_addr.h"
#include "web.h"

#include "System_Task.h"
#include "Display_Task.h"

#include "OSAL_Detect.h"

uint8 g_MACArray[6];

#define SUB_MODE  \
"\"submodel-items\":[{\"sub-name\":\"Mini InRoom\", \"sub-id\":\"1\"}," \
"{\"sub-name\":\"Mini InCeiling\", \"sub-id\":\"2\"}," \
"{\"sub-name\":\"Mini FlexSub\", \"sub-id\":\"3\"}," \
"{\"sub-name\":\"Bronze InRoom\", \"sub-id\":\"4\"}," \
"{\"sub-name\":\"Bronze InWall\", \"sub-id\":\"5\"}," \
"{\"sub-name\":\"Bronze InCeiling\", \"sub-id\":\"6\"}," \
"{\"sub-name\":\"Bronze SlimSub\", \"sub-id\":\"7\"}," \
"{\"sub-name\":\"Silver InRoom\", \"sub-id\":\"8\"}," \
"{\"sub-name\":\"Silver InWall\", \"sub-id\":\"9\"}," \
"{\"sub-name\":\"Silver FlexSub\", \"sub-id\":\"10\"}," \
"{\"sub-name\":\"Gold InRoom\", \"sub-id\":\"11\"}," \
"{\"sub-name\":\"Platinum InRoom\", \"sub-id\":\"12\"}," \
"{\"sub-name\":\"Flat\", \"sub-id\":\"13\"}" \
"],"

#define AUDIO_LEVEL \
"\"auto-off-time-items\":[{\"name\":\"5\", \"id\":\"1\"}," \
"{\"name\":\"10\", \"id\":\"2\"}," \
"{\"name\":\"20\", \"id\":\"3\"}," \
"{\"name\":\"30\", \"id\":\"4\"}," \
"{\"name\":\"60\", \"id\":\"5\"}" \
"],"

#define POWER_ROUTING \
"\"power-on-router-items\":[{\"name\":\"Manual\", \"id\":\"1\"}," \
"{\"name\":\"Sense\", \"id\":\"2\"}," \
"{\"name\":\"Trigger\", \"id\":\"3\"}," \
"{\"name\":\"IR/IP\", \"id\":\"4\"}" \
"],"
	
#define INPUT_SOURCE \
"\"input-source-items\":[{\"name\":\"XLR\", \"id\":\"1\"}," \
"{\"name\":\"RCA\", \"id\":\"2\"}," \
"{\"name\":\"Speak\", \"id\":\"3\"}" \
"],"
	
#define EQ_PRESET  \
"\"preset-items\":[{\"preset-name\":\"Normal\", \"preset-id\":\"1\"}," \
"{\"preset-name\":\"Cinema\", \"preset-id\":\"2\"}," \
"{\"preset-name\":\"Night\", \"preset-id\":\"3\"}," \
"{\"preset-name\":\"User1\", \"preset-id\":\"4\"}," \
"{\"preset-name\":\"User2\", \"preset-id\":\"5\"}," \
"{\"preset-name\":\"User3\", \"preset-id\":\"6\"}" \
"],"

#define SUB_LOWPASS_SLOPE \
"\"sub-lp-slop-items\":[{\"name\":\"12\", \"id\":\"1\"}," \
"{\"name\":\"24\", \"id\":\"2\"}," \
"{\"name\":\"48\", \"id\":\"3\"}" \
"],"

#define SUB_HIGHPASS_SLOPE \
"\"sub-hp-slop-items\":[{\"name\":\"12\", \"id\":\"1\"}," \
"{\"name\":\"24\", \"id\":\"2\"}," \
"{\"name\":\"48\", \"id\":\"3\"}" \
"],"

#define LINEOUT_HIGHPASS_SLOPE \
"\"lineout-hp-slop-items\":[{\"name\":\"12\", \"id\":\"1\"}," \
"{\"name\":\"24\", \"id\":\"2\"}," \
"{\"name\":\"48\", \"id\":\"3\"}" \
"],"

#define ROUTER_DELAY \
"\"route-delay-type-items\":[{\"name\":\"Sub\", \"id\":\"1\"}," \
"{\"name\":\"Line Output\", \"id\":\"2\"}," \
"{\"name\":\"OFF\", \"id\":\"3\"}" \
"],"

#define PHRASE_ADJ \
"\"phase-adjustment-items\":[{\"name\":\"0\", \"id\":\"1\"}," \
"{\"name\":\"180\", \"id\":\"2\"}" \
"],"

#define EQ_NUM \
"\"edit-eq-items\":[{\"name\":\"Normal\", \"id\":\"1\"}," \
"{\"name\":\"Cinema\", \"id\":\"2\"}," \
"{\"name\":\"Night\", \"id\":\"3\"}," \
"],"


#define CUSTOMER_EQ_NUM \
"\"edit-eq-items\":[{\"name\":\"User1\", \"id\": \"4\"}," \
"{\"name\":\"User2\", \"id\": \"5\"}," \
"{\"name\":\"User3\", \"id\": \"6\"}" \
"],"


extern ip_addr_t *get_cur_ip(void);


#define EQ_FILTER_BEGIN  "\"filter-items\":["
#define EQ_FILTER_END   "],"


#define CGI_ARGC_MAX   14

#define JSON_BUF_SIZE  2048

const CGI_t g_pCGI;
int g_NumCGI;

static int Cgi_setup_page_handler(http_state_t *,int,char *[]);
static int Cgi_home_page_handler(http_state_t *,int,char *[]);
static int Cgi_roomeq_page_handler(http_state_t *,int,char *[]);
static int Cgi_preset_page_handler(http_state_t *,int,char *[]);
static int Cgi_ipconfig_page_handler(http_state_t *,int,char *[]);
static int Cgi_about_page_handler(http_state_t *,int,char *[]);
static int Cgi_config_handler(http_state_t *,int,char *[]);

static void sub_select_handler(sys_state_t *sys, char *arg);
static void master_volume_control(sys_state_t *sys, char *arg);
static void mute_state_control(sys_state_t *sys, char *arg);
static void preset_selcet_handler(sys_state_t *sys, char *arg);
static void src_select_handler(sys_state_t *sys, char *arg);
static void power_on_method_select(sys_state_t *sys, char *arg);
static void sense_level_select_handler(sys_state_t *sys, char *arg);
static void sense_timeout_select_handler(sys_state_t *sys, char *arg);
static void sub_lp_change_handler(sys_state_t *sys, char *arg);
static void sub_lp_slope_select_handler(sys_state_t *sys, char *arg);
static void sub_hp_change_handler(sys_state_t *sys, char *arg);
static void sub_hp_slope_select(sys_state_t *sys, char *arg);
static void input_gain_change_handler(sys_state_t *sys, char *arg);
static void lineout_hp_change_handler(sys_state_t *sys, char *arg);
static void lineout_slope_select_handler(sys_state_t *sys, char *arg);
static void delay_time_change_handler(sys_state_t *sys, char *arg);
static void delay_switch_handler(sys_state_t *sys, char *arg);
static void phase_adj_handler(sys_state_t *sys, char *arg);
static void temp_gain_adj_handler(sys_state_t *sys, char *arg);
static void temp_freq_adj_handler(sys_state_t *sys, char *arg);
static void temp_width_adj_handler(sys_state_t *sys, char *arg);
static void req_preset_select_handler(sys_state_t *sys, char *arg);
static void req_filter_select_handler(sys_state_t *sys, char *arg);
static void req_on_off_control(sys_state_t *sys, char *arg);
static void req_save_handler(sys_state_t *sys, char *a, char *b, char *g);
static void preset_vol_change_handler(sys_state_t *sys, char *arg);
static void preset_gain1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_freq1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_width1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_gain2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_freq2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_width2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g);
static void preset_customer_eq_select_handler(sys_state_t *sys, char *arg);

CGI_Page_t g_Page_CGI[] ={
	{"page=setup", Cgi_setup_page_handler},
	{"page=home",  Cgi_home_page_handler},
	{"page=roomEQ", Cgi_roomeq_page_handler},
  {"page=preset", Cgi_preset_page_handler},
  {"page=ipConfig", Cgi_ipconfig_page_handler},
  {"page=about", Cgi_about_page_handler},
  {"page=config", Cgi_config_handler}
};

#define CGI_PAGE_NUM  (sizeof(g_Page_CGI)/sizeof(g_Page_CGI[0]))


static char httpd_recv_buf[HTTPD_RECV_BUF_SIZE];
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


/** Allocate a struct http_state. */
static http_state_t*
http_state_alloc(void)
{
  http_state_t *ret = NULL;

  ret = (http_state_t *)mem_malloc(sizeof(http_state_t));

	OSAL_ASSERT(ret != NULL);
	
  if (ret != NULL) {
    /* Initialize the structure. */
    memset(ret, 0, sizeof(http_state_t));
  }
  return ret;
}


/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_free(http_state_t *hs)
{
  if (hs != NULL) {
    if(hs->handle) {
      fs_close(hs->handle );
      hs->handle = NULL;
    }
    mem_free(hs);
  }
}

static void do_httpd_send(struct netconn *conn, http_state_t *hs)
{
	uint32_t len;
	
	while(hs->left){
	if(hs->left > TCP_MSS){
		len = TCP_MSS - 24;
	}else{
		len = hs->left;
	 } 
  /*we are not processing an sHTML file so no tag checking is necessary*/
	netconn_write(conn, (void *)hs->file, len, NETCONN_COPY);
  /*there is no restrication on the lenght of the data, the stack core will handle that*/
	 hs->left -= len;
   hs->file += len;
}

}


static void http_send_data(struct netconn *conn, http_state_t *hs)
{
	  char *psend = httpd_send_buf;
	  uint16_t len = 0;
	
	  memset(httpd_send_buf, 0, sizeof(httpd_send_buf));
	  if(hs->hdr_index < NUM_FILE_HDR_STRINGS){
			 for(;hs->hdr_index < NUM_FILE_HDR_STRINGS; hs->hdr_index++){
				   strcat(psend, hs->hdrs[hs->hdr_index]);
				   len += strlen(hs->hdrs[hs->hdr_index]);
			}
			netconn_write(conn, (void *)psend, len, NETCONN_COPY);
		}
		else{
			SYS_TRACE("waring!! will not send http header index=%d\r\n", hs->hdr_index);
		}
		
		/*have we run out of file data to send? if so, we need to read the next block from the file*/
		if(hs->left == 0){
			 int count;
			 hs->buf = httpd_send_buf;
			 memset(httpd_send_buf, 0, sizeof(httpd_send_buf));
			 if(hs->handle != NULL){ 
				  do{
						  count = fs_read(hs->handle, hs->buf, HTTPD_SEND_BUF_SIZE);
						  if(count < 0){
								 /*we reach the end of the file so this request is done */
								 fs_close(hs->handle);
								 hs->handle = NULL;
								 return;
							}
							/*set up to send the block of data we just read*/
							hs->file = hs->buf;
							hs->left = count;
							do_httpd_send(conn, hs);
					}while(1);
			 }
			
		}else{
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
	
	 if(uri == NULL){
		  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
		  hs->hdrs[2] = g_psHTTPHeaderStrings[DEFAULT_404_HTML];
		 
		  /*set up to send the first header string*/
		  hs->hdr_index = 0;
		  //SYS_TRACE("fill NOT FOUND HEADER\r\n");
		  return;
	 }else{
		   if(strstr(uri, "404")){
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_FOUND];
			 }else if(strstr(uri, "400")){
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_BAD_REQUEST];
			 }else if(strstr(uri, "501")){
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_NOT_IMPL];
			 }else{
				  hs->hdrs[0] = g_psHTTPHeaderStrings[HTTP_HDR_OK];
			 } 
			 
			 /*check if the URI*/
			 pvar = strchr(uri, '?');
			 if(pvar){
				  *pvar = 0;
			 }			 
			 /*Get a pointer to the file extension*/
			 pext = NULL;
			 pwork = strchr(uri, '.');
			 while(pwork){
				  pext = pwork + 1;
				  pwork = strchr(pext, '.');
			 }		 
			 for(loop = 0; (loop < NUM_HTTP_HEADERS) && pext; loop++){
				  if(strcmp(g_psHTTPHeaders[loop].extension, pext) ==0){
						 hs->hdrs[2] = g_psHTTPHeaderStrings[g_psHTTPHeaders[loop].headerIndex];
						 break;
					}
			 }		 
			 /*Reinstate the parameter marker if there was one in the original URI*/
			 if(pvar){
				  *pvar = '?';
			 }
	 }
	 
	 /*Does the URL passed have any file extension? if not, we assume it is a special-case URL used 
	 for control state notification and we do not send any HTTP headers with the reponse*/
	 if(!pext){
		  /*force the header index to a value indicating that all headers have already been sent*/
		  hs->hdr_index = NUM_FILE_HDR_STRINGS;
	 }else{	 
		  /*did we find a matching extension?*/
		  if(loop == NUM_HTTP_HEADERS){
				 /*No - use the default, plain text file type*/
				 hs->hdrs[2] = g_psHTTPHeaderStrings[HTTP_HDR_DEFAULT_TYPE];
			}
			/*set up to send the first header string*/
		  hs->hdr_index = 0;
	 }
}

static err_t http_init_file(http_state_t *hs, struct fs_file *file, const char *uri)
{
	 if(file != NULL){
		  hs->handle = file;
		  hs->file = NULL;
		  hs->left = 0;
	 }
	 get_http_headers(hs, (char *)uri);  
	 return ERR_OK;
}

static err_t http_find_file(http_state_t *hs, char *uri)
{
	struct fs_file *file = NULL;
	char *params;
	
	/*Have we been asked for the default root files*/
	if((uri[0] == '/') && (uri[1] == 0)){
		file = fs_open(DEFAULT_FILE_NAME);
		uri = DEFAULT_FILE_NAME;
		  
		if(file == NULL){
				SYS_TRACE("open index page file fail\r\n");
	    }
			else{
				// SYS_TRACE("open index page file success\r\n");
			}
	 }
	 else{
		  /*No - we've been asked for a specific file*/
		 /*First, isolate the base URI*/
		 params = (char *)strchr(uri, '?');
		 if(params != NULL){
			  *params = 0;
			   params++;
			   SYS_TRACE("CGI=%s---uri=%s\r\n", params, uri);
			   http_cgi_handler(hs, uri, params);
		 }else{
			 file = fs_open(uri);
		   if(file == NULL){
				 SYS_TRACE("Try open %s fail\r\n", uri);
				 uri = NULL;
			 }else{
				// SYS_TRACE("Try open %s success\r\n", uri);
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
	if(crlf != NULL){
		 int is_post = 0;
		 char *sp1, *sp2;
		 uint16_t left_len, uri_len;
		
		 if(strncmp(data, "GET ", strlen("GET ")) ==0){
			  sp1 = data + 3;
		 }
		 else if(strncmp(data, "POST ", strlen("POST ")) ==0){
			  is_post = 1;
			  sp1 = data + 4;
		 }else{
			   return ERR_ARG;
		 }
		 
		 /*if we come here, method is OK, parse URI*/
		 left_len = len - ((sp1 + 1) - data);
		 sp2 = strstr(sp1 + 1, " ");
		 if(sp2 != NULL){
				uri_len = sp2 - (sp1 + 1);
			  if((sp2 != 0) && (sp2 > sp1)){
					 char *uri = sp1 + 1;
					 /*null-terminate the METHOD*/
					 *sp1 = 0;
					 uri[uri_len] = 0;					
					 if(is_post){
						  //TBD
					 }else{
						   return http_find_file(hs,uri);
					 }
				}
		 }
		 else{
			   SYS_TRACE("invalid URI\r\n");
		 }
		 
	 (void)left_len;
	}
	
	
	
	 return ERR_ARG;
}

static err_t do_process(struct netconn *conn, http_state_t *hs, char *data, uint16_t len)
{
	err_t parsed = ERR_ISCONN;  
	
	/*first check whether we are in post context*/
	 if(hs->post_content_len_left > 0){
		 //TBD
	 }else{
		 if(hs->handle == NULL){
				 parsed = http_parse_request(hs, data, len);
			}else{
				 SYS_TRACE("http_recv: already sending data\r\n");
			}
			if(parsed == ERR_OK)
			{
				 if(hs->post_content_len_left == 0){
					   http_send_data(conn,hs);
					   parsed = ERR_CLSD;
				 }
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
		
		  /*recover from v0.4.4*/
		  sys_ip_power_on_handler(gSystem_t);
		
		  memset(httpd_recv_buf, 0, sizeof(httpd_recv_buf));
		  pdata = httpd_recv_buf;
		  netbuf_first(inbuf);
		  do{			  
				  netbuf_data(inbuf, (void **)&pdata, &len);
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
	SYS_TRACE("web task running\r\n");
	
	while(1){
		err = netconn_accept(conn, &newconn);
		if(err != ERR_OK) {
		  SYS_TRACE("netconn_accept =%d\r\n",err);
			continue;
    }
		
		hs = http_state_alloc();
		if(hs == NULL){
			  http_state_free(hs);
			  netconn_delete(newconn);
			  continue;
		}

		httpd_process(newconn,hs);
		http_state_free(hs); 
		netconn_delete(newconn);
		vTaskDelay(10/portTICK_RATE_MS); 
	}
}


void web_task(void *parameter)
{
	uint8 j ;
	
	fs_init();
	
	if(gSystem_t->mac[0] == 0xff && gSystem_t->mac[1] == 0xff && gSystem_t->mac[2] == 0xff
		&& gSystem_t->mac[3] == 0xff && gSystem_t->mac[4] == 0xff && gSystem_t->mac[5] == 0xff){
	
		g_MACArray[0] = 0x00;
		g_MACArray[1] = 0x60;
		g_MACArray[2] = 0x6e;
		g_MACArray[3] = 0x11;
		g_MACArray[4] = 0x22;
		g_MACArray[5] = 0x33;
	}else{
		for(j = 0; j < 6; j++){
			g_MACArray[j] = gSystem_t->mac[j];
		}
	}
	
	
	lwIPInit(gSysClock, g_MACArray, gSystem_t->ip, gSystem_t->netmask, gSystem_t->gateway, gSystem_t->ip_mode);
	//lwIPInit(gSysClock, g_MACArray, gSystem_t->ip, gSystem_t->netmask, gSystem_t->gateway, 1);

	IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
	
	  //
    // PF1/PK4/PK6 are used for Ethernet LEDs.
    //
    GPIOPinConfigure(GPIO_PK4_EN0LED0);
    GPIOPinConfigure(GPIO_PK6_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_4);
    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_6);
	
	//sys_timeout(1000, sys_database_update, NULL);
	web_server();
}


static void do_setup_cgi_page(int argv, char *argc[])
{
	 if(argv < 4) return;
	
	 if(strcmp(argc[2]+5, "sub-lp-freq") == 0){
		 sub_lp_change_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "cur-sub-lp-freq") == 0){
		 sub_lp_slope_select_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "sub-hp-freq") == 0){
		 sub_hp_change_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "cur-sub-hp-slop") == 0){
		 sub_hp_slope_select(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "input-gain-level") == 0){
		 input_gain_change_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "lineout-hp-freq") == 0){
		 lineout_hp_change_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "cur-lineout-hp-slop") == 0){
		 lineout_slope_select_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "route-delay-time") == 0){
		 delay_time_change_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "cur-route-delay-type") == 0){
		 delay_switch_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "cur-phase-adjustment") == 0){
		 phase_adj_handler(gSystem_t, argc[3]+6);
	 }else if(strcmp(argc[2]+5, "reset") == 0){
		 sys_factory_reset(gSystem_t);
	 }
}

static void _load_setup_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"sub-mode\":\"%s\",", sub_name[sys->sub_model]);
	 json_output_print("\"ip-address\":\"%s\",", ipaddr_ntoa(get_cur_ip()));
	 json_output_print("\"master-volume\":\"%d\",", sys->master_vol);
	 
	
	 json_output_print("\"sub-lp-freq\":\"%d\",", sys->sub[sys->sub_model].lowpass);
	 json_output_print("%s", SUB_LOWPASS_SLOPE);
	 json_output_print("\"cur-sub-lp-freq\":\"%d\",", sys->sub[sys->sub_model].lowpass_slope);
	
	 json_output_print("\"sub-hp-freq\":\"%d\",", sys->sub[sys->sub_model].highpass);
	 json_output_print("%s", SUB_HIGHPASS_SLOPE);
	 json_output_print("\"cur-sub-hp-slop\":\"%d\",", sys->sub[sys->sub_model].highpass_slope);
	
	 json_output_print("\"input-gain-level\":\"%d\",", sys->input_gain);
	
	 json_output_print("\"lineout-hp-freq\":\"%d\",", sys->lineout_highpass);
	 json_output_print("%s", LINEOUT_HIGHPASS_SLOPE);
	 json_output_print("\"cur-lineout-hp-slop\":\"%d\",", sys->lineout_highpass_slope);
	
	 
	 json_output_print("\"route-delay-time\":\"%d\",", (sys->d_router == DELAY_SUB ? (sys->sub[sys->sub_model].delay):(sys->line_delay_time)));
	 json_output_print("%s", ROUTER_DELAY);
	 json_output_print("\"cur-route-delay-type\":\"%d\",", sys->d_router + 1);
	 
	 json_output_print("%s", PHRASE_ADJ);
	 json_output_print("\"cur-phase-adjustment\":\"%d\"", sys->sub[sys->sub_model].phrase + 1);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);
}





static int Cgi_setup_page_handler(http_state_t *hs, int argv, char *argc[])
{
	SYS_TRACE("in Cgi_setup_page_handler\r\n");
	
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_setup_cgi_page(argv,argc);
		 }
		   _load_setup_page_json_file(gSystem_t, hs);
	}		
	
	return 0;
}

static void do_home_cgi_page(int argv, char *argc[])
{
	if(argv < 4) return;

	//SYS_TRACE("%s----%s\r\n", argc[2], argc[3]);
	
	if(strcmp(argc[2]+5, "current-sub") ==0){
		sub_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "master-volume") == 0){
		master_volume_control(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-preset-item") == 0){
		preset_selcet_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-source-index") == 0){
		src_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-power-on-router") == 0){
		power_on_method_select(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "audio-level") == 0){
		sense_level_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-auto-off-time") == 0){
		sense_timeout_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "mute-state") == 0){
		mute_state_control(gSystem_t, argc[3]+6);
	}
	
	dis_send_msg(DIS_UPDATE, MENU_NUM);
}



static void _load_home_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"dsp-name\":\"%s\",", "Traid");
	 json_output_print("%s", SUB_MODE);
	 json_output_print("\"current-sub\":\"%d\",", sys->sub_model + 1);
	 json_output_print("\"master-volume\":\"%d\",", sys->master_vol);
	 json_output_print("\"mute-state\":\"%d\",", (sys->is_mute == true?(1):(0)));
	 json_output_print("%s", AUDIO_LEVEL);
	 json_output_print("\"audio-level\":\"%d\",", (sys->sense_thres + 1)*3);
	 json_output_print("\"cur-auto-off-time\":\"%d\",", sys->se_timeout + 1);
	 json_output_print("%s", POWER_ROUTING);
	 json_output_print("\"cur-power-on-router\":\"%d\",", sys->method + 1);
	 json_output_print("%s", INPUT_SOURCE);
	 json_output_print("\"cur-source-index\":\"%d\",", sys->src + 1);
	 json_output_print("%s", EQ_PRESET);
	 json_output_print("\"cur-preset-item\":\"%d\"", sys->preset + 1);
	 
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
		 _load_home_page_json_file(gSystem_t,hs);
	}		
	return 0;
}


static void do_roomeq_cgi_page(int argv, char *argc[])
{
	if(argv < 4) return;
	
	if(strcmp(argc[2]+5, "peq-gain") == 0){
		temp_gain_adj_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "peq-frequency") == 0){
		temp_freq_adj_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "peq-width") == 0){
		temp_width_adj_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-edit-eq-items") == 0){
		req_preset_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "cur-filter-id") == 0){
		req_filter_select_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "filter-eq-switch") == 0){
		req_on_off_control(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "filter-save") == 0){
		req_save_handler(gSystem_t, argc[3]+7, argc[4]+7, argc[5]+7);
	}
	
	dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void _load_roomeq_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 uint8 j;
	
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"peq-gain\":\"%d\",", gEqTemp.gain);
	 json_output_print("\"peq-frequency\":\"%d\",", gEqTemp.freq);
	 json_output_print("\"peq-width\":\"%d\",", gEqTemp.Q);
	
	 json_output_print("%s", EQ_NUM);
	 json_output_print("\"cur-edit-eq-items\":\"%d\",", sys->cur_room_eq + 1);
	
	
	 json_output_print("%s", EQ_FILTER_BEGIN);
	 for(j = 0; j < ROOM_EQ_FILTER_NUM; j++){
			json_output_print("{\"name\":\"Filter %d\", \"id\": \"%d\", \"eq-gain\": \"%d\", \"eq-frequency\": \"%d\", \"eq-width\":\"%d\"},",
       j+1, j+1, sys->eq->req.filter[j].gain, sys->eq->req.filter[j].freq, sys->eq->req.filter[j].Q);
	 }
	 json_output_print("%s", EQ_FILTER_END);
	 
	 json_output_print("\"cur-filter-id\":\"%d\",", sys->room_eq_index + 1);
	 json_output_print("\"filter-eq-switch\":\"%d\",", sys->eq_on_off);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf); 
}

static int Cgi_roomeq_page_handler(http_state_t *hs, int argv, char *argc[])
{
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_roomeq_cgi_page(argv,argc);
		 }
		 _load_roomeq_page_json_file(gSystem_t,hs);
	}

  return 0;  
}

static void _load_preset_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"volume\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].vol);
	 json_output_print("\"peq-gain\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[0].gain);
	 json_output_print("\"peq-frequency\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[0].freq);
	 json_output_print("\"peq-width\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[0].Q);
	 json_output_print("\"peq-gain2\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[1].gain);
	 json_output_print("\"peq-frequency2\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[1].freq);
	 json_output_print("\"peq-width2\":\"%d\",", sys->eq->peq[sys->cur_cust_eq].filter[1].Q);

	 json_output_print("%s", CUSTOMER_EQ_NUM);
	 json_output_print("\"current-peq\":\"%d\"", sys->cur_cust_eq + 1);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);  
}

static void do_preset_cgi_page(int argv, char *argc[])
{
	if(argv < 4) return;
	
	if(strcmp(argc[2]+5, "volume") == 0){
		preset_vol_change_handler(gSystem_t, argc[3]+6);
	}else if(strcmp(argc[2]+5, "peq-gain") == 0){
		preset_gain1_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "peq-frequency") == 0){
		preset_freq1_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "peq-width") == 0){
		preset_width1_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "peq-gain2") == 0){
		preset_gain2_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "peq-frequency2") == 0){
		preset_freq2_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "peq-width2") == 0){
		preset_width2_change_handler(gSystem_t, argc[3]+6, argc[4]+7, argc[5]+7, argc[6]+7);
	}else if(strcmp(argc[2]+5, "current-peq") == 0){
		preset_customer_eq_select_handler(gSystem_t, argc[3]+6);
	}
}


static int Cgi_preset_page_handler(http_state_t *hs,int argv, char *argc[])
{
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_preset_cgi_page(argv,argc);
		 }
		   _load_preset_page_json_file(gSystem_t, hs);
	}	  
	
	
	return 0;
}

static void _load_ipconfig_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"mac-address\":\"%02x.%02x.%02x.%02x.%02x.%02x\",", g_MACArray[0], g_MACArray[1], g_MACArray[2], g_MACArray[3], g_MACArray[4], g_MACArray[5]);
	 json_output_print("\"ip-address\":\"%d.%d.%d.%d\",", ((sys->ip >> 24) & 0xff), ((sys->ip >> 16) & 0xff), ((sys->ip >> 8) & 0xff), (sys->ip & 0xff));
	 json_output_print("\"gateway\":\"%d.%d.%d.%d\",", ((sys->gateway >> 24) & 0xff), ((sys->gateway >> 16) & 0xff), ((sys->gateway >> 8) & 0xff), (sys->gateway & 0xff));
	 json_output_print("\"sub-mask\":\"%d.%d.%d.%d\",", ((sys->netmask >> 24) & 0xff), ((sys->netmask >> 16) & 0xff), ((sys->netmask >> 8) & 0xff), (sys->netmask & 0xff));
	 json_output_print("\"host-name\":\"%s\",", "RackAmp700DSP");
	 json_output_print("\"primary-dns\":\"%s\",", "10.10.1.3");
	 json_output_print("\"secondary-dns\":\"%s\",", "221.6.4.67");
	 json_output_print("\"dhcp-enable\":\"%d\"", sys->ip_mode == 1? (1):(0));
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);  
}


#include "netconf.h"

static void do_ipconfig_cgi_page(int argv, char *argc[])
{
	int temp[4];
	uint32 ip = 0,mask = 0,gate = 0;
	
  SYS_TRACE("hostname:%s\r\n", &argc[2][10]);
	
	SetHostName(&argc[2][10]);
	
	if(argv == 10){
		sscanf(argc[4], "ip-address=%d.%d.%d.%d", &temp[0], &temp[1],&temp[2],&temp[3]);
		ip = (uint32)((temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | (temp[3] << 0));
		sscanf(argc[5], "gateway=%d.%d.%d.%d", &temp[0], &temp[1],&temp[2],&temp[3]);
		gate = (uint32)((temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | (temp[3] << 0));
		sscanf(argc[6], "sub-mask=%d.%d.%d.%d", &temp[0], &temp[1],&temp[2],&temp[3]);
		mask = (uint32)((temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | (temp[3] << 0));
		
		lwIPNetworkConfigChange(ip, mask, gate, 0);
	
	  gSystem_t->ip_mode = 0;
		gSystem_t->ip = ip;
		gSystem_t->gateway = gate;
		gSystem_t->netmask = mask;
	}else{
		lwIPNetworkConfigChange(ip, mask, gate, 1);
	
	  gSystem_t->ip_mode = 1;
	}
	
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void _load_about_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"software-version\":\"%s\",", version);
	 json_output_print("\"build-date\":\"%s\",", __DATE__);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);   
}

static int Cgi_about_page_handler(http_state_t *hs, int argv, char *argc[])
{
	if(argv >= 2){
		_load_about_page_json_file(gSystem_t, hs);
	}	  
	

  return 0;  
}

static void do_config_cgi_page(int argv, char *argc[])
{
	uint8 j;
	char temp[6][3] = {0};
  int buf[6] = {0};
  char *p = &argc[3][6];
  uint8 len = strlen(p);
	
	SYS_TRACE("do_config_cgi_page=[%s]\r\n", p);
	
	//OSAL_ASSERT(len == 12);
  if(len < 12) return;

  for(j = 0; j < (len >> 1); j++){
	
		temp[j][0] = p[2*j];
		temp[j][1] = p[2*j + 1];
		
		sscanf(temp[j], "%x", &buf[j]);
	}
	
	for(j = 0; j < 6; j++){
		gSystem_t->mac[j] = buf[j];
		g_MACArray[j] = buf[j];
	}

	sys_send_msg(DATABASE_UPDATE_MSG, 0);
	SYS_TRACE("mac = [%02x.%02x.%02x.%02x.%02x.%02x]\r\n", buf[0], buf[1],buf[2],buf[3],buf[4], buf[5]);
	
}

static void _load_config_page_json_file(sys_state_t *sys, http_state_t *hs)
{
	 /*don't care the below content*/
	 JSON_OUTPUT_BEGIN();
	
	 json_output_print("\"software-version\":\"%s\",", version);
	 json_output_print("\"build-date\":\"%s\",", __DATE__);
	 
	 JSON_OUTPUT_END();
	
	 hs->file = json_output_buf;
	 hs->left = strlen(json_output_buf);  
	
}



static int Cgi_config_handler(http_state_t *hs,int argv,char *argc[])
{
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_config_cgi_page(argv,argc);
		 }
		   _load_config_page_json_file(gSystem_t, hs);
	}	  
	
	return 0;
}

static int Cgi_ipconfig_page_handler(http_state_t *hs, int argv,char *argc[])
{
	if(argv >= 2){
     if(strcmp(argc[1], "action=write") == 0){
			 do_ipconfig_cgi_page(argv,argc);
		 }
		   _load_ipconfig_page_json_file(gSystem_t, hs);
	}	  
	
	return 0;
}


static void preset_vol_change_handler(sys_state_t *sys, char *arg)
{
  int vol;

  vol = atoi(arg);
	
	if(vol <= VOLUME_VALUE_MAX(sys->input_gain) && vol >= VOLUME_VALUE_MIN(sys->input_gain)){
		sys->eq->peq[sys->cur_cust_eq].vol = vol;
	}
	
	if(sys->preset == sys->cur_cust_eq){
		 sys->master_vol = sys->eq->peq[sys->cur_cust_eq].vol;
		 sys_master_vol_set(sys);
		 //sys_eq_single_set(sys, sys->cur_cust_eq, 0);
	 }
	 
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

static void preset_gain1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	 int gain;
	 uint32 var_a = atoi(a);
	 uint32 var_b = atoi(b);
	 uint32 var_g = atoi(g);  
	
	 gain = atoi(arg);
	 
	 if(gain <= EQ_GAIN_MAX && gain >= EQ_GAIN_MIN){
		 sys->eq->peq[sys->cur_cust_eq].filter[0].gain = gain;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].g = var_g;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].a = var_a;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].b = var_b;
	 }
	 
	 if(sys->preset == sys->cur_cust_eq){
		 sys_peq_single_set(sys, sys->cur_cust_eq, 0, var_a, var_b, var_g);
	 }
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

static void preset_freq1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	uint16 freq;
	uint32 var_a = atoi(a);
	uint32 var_b = atoi(b);
	uint32 var_g = atoi(g);  
	
	freq = atoi(arg);
	
	if(freq <= EQ_FREQ_MAX && freq >= EQ_FREQ_MIN){
		 sys->eq->peq[sys->cur_cust_eq].filter[0].freq = freq;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].g = var_g;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].a = var_a;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].b = var_b;
	}
	
	if(sys->preset == sys->cur_cust_eq){
		 sys_peq_single_set(sys, sys->cur_cust_eq, 0, var_a, var_b, var_g);
	 }
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}


static void preset_width1_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	 uint8 q;
	 uint32 var_a = atoi(a);
	 uint32 var_b = atoi(b);
	 uint32 var_g = atoi(g);  
	
	 q = atoi(arg);
	
	 if(q <= EQ_Q_MAX && q >= EQ_Q_MIN){
		 sys->eq->peq[sys->cur_cust_eq].filter[0].Q = q;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].g = var_g;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].a = var_a;
		 sys->eq->peq[sys->cur_cust_eq].filter[0].b = var_b;
	 }
	 
	 if(sys->preset == sys->cur_cust_eq){
		 sys_peq_single_set(sys, sys->cur_cust_eq, 0, var_a, var_b, var_g);
	 }
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

static void preset_gain2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	 int gain;
   uint32 var_a = atoi(a);
	 uint32 var_b = atoi(b);
	 uint32 var_g = atoi(g);   
	
	 gain = atoi(arg);
	 
	 if(gain <= EQ_GAIN_MAX && gain >= EQ_GAIN_MIN){
		 sys->eq->peq[sys->cur_cust_eq].filter[1].gain = gain;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].g = var_g;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].a = var_a;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].b = var_b;
	 }
	 
	 if(sys->preset == sys->cur_cust_eq){
		 sys_peq_single_set(sys, sys->cur_cust_eq, 1, var_a, var_b, var_g);
	 }
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

static void preset_freq2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	uint16 freq;
	uint32 var_a = atoi(a);
	uint32 var_b = atoi(b);
	uint32 var_g = atoi(g);
	
	freq = atoi(arg);
	
	if(freq <= EQ_FREQ_MAX && freq >= EQ_FREQ_MIN){
		sys->eq->peq[sys->cur_cust_eq].filter[1].freq = freq;
		sys->eq->peq[sys->cur_cust_eq].filter[1].g = var_g;
		sys->eq->peq[sys->cur_cust_eq].filter[1].a = var_a;
		sys->eq->peq[sys->cur_cust_eq].filter[1].b = var_b;
	}
	
	if(sys->preset == sys->cur_cust_eq){
	  sys_peq_single_set(sys, sys->cur_cust_eq, 1, var_a, var_b, var_g);
	 }
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}


static void preset_width2_change_handler(sys_state_t *sys, char *arg, char *a, char *b, char *g)
{
	 uint8 q;
	 uint32 var_a = atoi(a);
	 uint32 var_b = atoi(b);
	 uint32 var_g = atoi(g);
	
	 q = atoi(arg);
	
	 if(q <= EQ_Q_MAX && q >= EQ_Q_MIN){
		 sys->eq->peq[sys->cur_cust_eq].filter[1].Q = q;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].g = var_g;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].a = var_a;
		 sys->eq->peq[sys->cur_cust_eq].filter[1].b = var_b;
	 }
	 
	 if(sys->preset == sys->cur_cust_eq){
		 sys_peq_single_set(sys, sys->cur_cust_eq, 1, var_a, var_b, var_g);
	 }
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}


static void preset_customer_eq_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 preset;
	
	 preset = atoi(arg);
	
	 sys->cur_cust_eq = preset - 1;
}

static void temp_gain_adj_handler(sys_state_t *sys, char *arg)
{
	 int gain;
	
	 gain = atoi(arg);
	
	 if(gain <= EQ_GAIN_MAX && gain >= EQ_GAIN_MIN){
		 gEqTemp.gain = gain;
	 }
}

static void temp_freq_adj_handler(sys_state_t *sys, char *arg)
{
	 uint16 freq;
	
	 freq = atoi(arg);
	
	 if(freq <= EQ_FREQ_MAX && freq >= EQ_FREQ_MIN){
		 gEqTemp.freq = freq;
	 }
}


static void temp_width_adj_handler(sys_state_t *sys, char *arg)
{
	 uint8 q;
	
	 q = atoi(arg);
	
	 if(q <= EQ_Q_MAX && q>= EQ_Q_MIN){
		 gEqTemp.Q = q;
	 }
}

static void req_preset_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 preset;
	
	 preset = atoi(arg);
	
	 if(preset - 1 < PRE_USER1){
		 sys->cur_room_eq = preset - 1;
	 }
	 
	 if(sys->cur_room_eq == sys->preset){
		 sys_load_roomeq(sys);
	 }
}

static void req_filter_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 filter;
	
	 filter = atoi(arg);
	
	 if(filter - 1 < ROOM_EQ_FILTER_NUM){
		 sys->room_eq_index = filter - 1;
		 
		 gEqTemp.freq = sys->eq->req.filter[sys->room_eq_index].freq;
		 gEqTemp.gain = sys->eq->req.filter[sys->room_eq_index].gain;
		 gEqTemp.Q = sys->eq->req.filter[sys->room_eq_index].Q;
	 }
}

static void req_on_off_control(sys_state_t *sys, char *arg)
{
	 uint8 on_off;
	
	 on_off = atoi(arg);
	
	 if(on_off){
		 sys->eq_on_off = 1;
	 }else{
		 sys->eq_on_off = 0;
	 }
	 
	 sys_eq_state_set(sys);
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void req_save_handler(sys_state_t *sys, char *a, char *b, char *g)
{
	 uint32 var_a = atoi(a);
	 uint32 var_b = atoi(b);
	 uint32 var_g = atoi(g);
	
	 //OSAL_ASSERT(sys->cur_room_eq < PRE_USER1);
	 OSAL_ASSERT(sys->room_eq_index < ROOM_EQ_FILTER_NUM);
	
	 sys->eq->req.filter[sys->room_eq_index].freq = gEqTemp.freq;
	 sys->eq->req.filter[sys->room_eq_index].gain = gEqTemp.gain;
	 sys->eq->req.filter[sys->room_eq_index].Q = gEqTemp.Q;
	 sys->eq->req.filter[sys->room_eq_index].a = var_a;
	 sys->eq->req.filter[sys->room_eq_index].b = var_b;
	 sys->eq->req.filter[sys->room_eq_index].g = var_g;
	

	 sys_req_single_set(sys,sys->room_eq_index, var_a, var_b, var_g);
	 
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

static void sub_lp_change_handler(sys_state_t *sys, char *arg)
{
	uint16 freq;
	
	freq = atoi(arg);
	
	if(freq >= SUB_LOWPASS_MIN){
		sys->sub[sys->sub_model].lowpass = freq;
	}
	
	sys_sub_lowpass_set(sys);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_lp_slope_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 slope;
	
	 slope = atoi(arg);
	
	 sys->sub[sys->sub_model].lowpass_slope = slope;
	
	 sys_sub_lowpass_set(sys);
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_hp_change_handler(sys_state_t *sys, char *arg)
{
	 uint16 freq;
	
	 freq = atoi(arg);
	
	 if(freq >= SUB_HIGHPASS_MIN){
		 sys->sub[sys->sub_model].highpass = freq;
	 }
	 
	 sys_sub_highpass_set(sys);
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_hp_slope_select(sys_state_t *sys, char *arg)
{
	 uint8 slope;
	
	 slope = atoi(arg);
	
	 sys->sub[sys->sub_model].highpass_slope = slope;
	
	 sys_sub_highpass_set(sys);
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void input_gain_change_handler(sys_state_t *sys, char *arg)
{
	 int gain;
	
	 gain = atoi(arg);
	
	 if(gain <= INPUT_GAIN_MAX && gain >= INPUT_GAIN_MIN){
		 
		 sys->master_vol += (sys->input_gain - gain);
		 
		 sys->input_gain = gain;
	 
	   if(sys->master_vol < MASTER_VOL_VALUE_MIN){
		   sys->master_vol = MASTER_VOL_VALUE_MIN;
	   }else if(sys->master_vol > MASTER_VOL_VALUE_MAX){
		   sys->master_vol = MASTER_VOL_VALUE_MAX;
	   }
		 
		 sys_master_vol_set(sys);
		 
		 dis_send_msg(DIS_UPDATE, MENU_NUM);
		 sys_send_msg(DATABASE_UPDATE_MSG, 1);
	 }
	 
}

static void lineout_hp_change_handler(sys_state_t *sys, char *arg)
{
	 uint16 freq;
	
	 freq = atoi(arg);
	
	 if(freq >= LINEOUT_HIGHPASS_MIN){
		 sys->lineout_highpass = freq;
	 }
	 
	 sys_line_highpass_set(sys);
	 
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void lineout_slope_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 slope;
	
	 slope = atoi(arg);
	
	 sys->lineout_highpass_slope = slope;
	
	 sys_line_highpass_set(sys);
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void delay_time_change_handler(sys_state_t *sys, char *arg)
{
	  uint16 time;
	
	  time = atoi(arg);
	
	  if(time <= SUB_DELAY_MAX){
			if(sys->d_router == DELAY_SUB){
				sys->sub[sys->sub_model].delay = time;
				sys_send_msg(DATABASE_UPDATE_MSG, 1);
			}else{
				sys->line_delay_time = time;
				sys_send_msg(DATABASE_UPDATE_MSG, 0);
			}
		}
		
		sys_delay_time_set(sys);
}

static void delay_switch_handler(sys_state_t *sys, char *arg)
{
	 uint8 router;
	
	 router = atoi(arg);
	
	 OSAL_ASSERT(router > 0);
	
	 if(router < DELAY_ROUTER_MAX + 1){
		 sys->d_router = (delay_router_t)(router - 1);
	 }
	 
	sys_delay_switch_to(sys);
	 
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void phase_adj_handler(sys_state_t *sys, char *arg)
{
   uint8 phase;
	
	 phase = atoi(arg);
	
	 sys->sub[sys->sub_model].phrase = phase - 1;
	
	 sys_phrase_set(sys);
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 1);
}


static void sub_select_handler(sys_state_t *sys, char *arg)
{
	uint8 sub_index;
	
	sub_index = atoi(arg);
	if(sub_index <= SUB_MODEL_NUM){
		sys->sub_model = (sub_model_t)(sub_index > 0 ? (sub_index - 1):(0));
	}
	
	sys_sub_preset_load(sys);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}


static void master_volume_control(sys_state_t *sys, char *arg)
{
	int vol;
	
	vol = atoi(arg);
	if(vol <= VOLUME_VALUE_MAX(sys->input_gain) && vol >= VOLUME_VALUE_MIN(sys->input_gain)){
		sys->master_vol = vol;
	}
	
	if(sys->is_mute == true) sys->is_mute = false;
	
	sys_master_vol_set(sys);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void mute_state_control(sys_state_t *sys, char *arg)
{
	uint8 is_mute;
	
	is_mute = atoi(arg);
	
	if(is_mute){
		sys->is_mute = true;
	}else{
		sys->is_mute = false;
	}
	
	sys_master_vol_set(sys);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void preset_selcet_handler(sys_state_t *sys, char *arg)
{
	uint8 preset;
	
	preset = atoi(arg);
	if(preset <= PRESET_NUM){
		sys->preset = (preset_t)(preset > 0? (preset - 1):(0));
	}
	
	sys_load_peq(sys);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}


static void src_select_handler(sys_state_t *sys, char *arg)
{
	uint8 src;
	
	src = atoi(arg);
	
	if(src <= SRC_NUM){
		sys->src = (src_t)(src > 0 ? (src - 1):(0));
	}
	
	sys_input_select(sys->src);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}


static void power_on_method_select(sys_state_t *sys, char *arg)
{
	 uint8 method;
	
	 method = atoi(arg);
	 
	 if(method <= POWER_ON_MAX){
		 sys->method = (power_on_t)(method > 0 ? (method - 1):(0));
	 }
	 
	 if(sys->method == AUDIO){
		 update_detect_state(DETECT_AUDIO);
	 }else if(sys->method == TRIGGER){
		 sys->is_trigger_off_need_delay = true;
		 update_detect_state(DETECT_TRIGGER);
	 }
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void sense_level_select_handler(sys_state_t *sys, char *arg)
{
	uint8 level;
	
	level = atoi(arg);
	
	if(level <= SENSE_LEVEL_MAX && level >= SENSE_LEVEL_MIN){
		sys->sense_thres = (sense_threshold_t)(level/3 - 1);
	}
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}
	sys_audio_sense_thres_set(sys->sense_thres);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void sense_timeout_select_handler(sys_state_t *sys, char *arg)
{
	 uint8 timeout;
	
	 timeout = atoi(arg);
	
	 if(timeout <= SE_TIMEOUT_NUM){
		 sys->se_timeout = (sense_timeout_t)(timeout > 0? (timeout - 1):(0));
	 }
	 if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	 }
	
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}


