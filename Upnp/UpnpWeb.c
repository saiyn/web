/***************************************************************************** 
* UpnpWeb.c - HTTP upnp server.
*
*
* portions Copyright (c) 2006 by Michael Vysotsky.
*
******************************************************************************/
#include "common_head.h"
#include "UpnpWeb.h"
#include "httpd_upnp.h"
#include "Ssdp.h"
#include "typedef.h"

#if LWIP_SSDP

/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/
#define UPNP_WEB_THREAD_PRIO      5
#define UPNP_WEB_THREAD_STACK_SIZE      (1*1024)

#define UPNP_FRIENDLY_NAME        "Triad"
#define UPNP_MANUFACTURER         "Triad"
#define UPNP_MANUFACTURER_URL     "http://www.st.com"
#define UPNP_MODEL_DECRIPTION     "Group speaker"
#define UPNP_MODEL_NAME           "Xeo II"
#define UPNP_MODEL_NUMBER         "1.0"
#define UPNP_MODEL_URL            "http://www.hansong-china.com"
#define UPNP_UPC                  ""
#define UPNP_URL_BASE             ""
/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/

const char *device_xml_prefix =
	"<?xml version=\"1.0\"?>\n"
	"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
	"<specVersion>\n"
	"<major>1</major>\n"
	"<minor>0</minor>\n"
	"</specVersion>\n";
const char *device_xml_postfix =
	"<serviceList>"
        "<service>"
        "<serviceType>urn:schemas-upnp-org:service:ManagementBasic:1"
	"</serviceType>\n"
	"<serviceId>urn:upnp-org:serviceId:1</serviceId>\n"
	"<SCPDURL>/" UPNP_DEVICE_XML_FILE "</SCPDURL>\n"
        "<controlURL></controlURL>\n"
        "<eventSubURL></eventSubURL>\n"
        "</service></serviceList>";
const char *device_xml_midfix = 
        "<device>\n"
        "<deviceType>urn:schemas-upnp-org:device:Management:1</deviceType>\n";


/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/

static void           xml_add_tagged_data(char *buf, const char *tag, const char *data);
static void           xml_add_tagged_url_data(char *buf, const char *tag, const char *data);
static size_t         format_device_xml(char *buf);
static void           xml_data_encode(char *buf, const char *data, int len);
static void           upnp_server_netconn_thread(void *arg);


void  UpnpWebInit(void)
{
  sys_thread_new("UPNP", upnp_server_netconn_thread, NULL, UPNP_WEB_THREAD_STACK_SIZE, UPNP_WEB_THREAD_PRIO);
}

/*
 *    UPNP WEB server
 */
static char send_buf[1024];
const char *http_header = 
  "%s"
  "Connection: close\r\n"
  "Content-Type: text/xml;charset=utf-8\r\n"
  "Content-Length: %d\r\n"
  "Accept-Ranges: none\r\n\r\n";
static void upnp_server_serve(struct netconn *conn) 
{
  struct netbuf *inbuf;
  char* buf, *temp;
  u16_t  buflen;
  size_t temp_len=0;
  size_t len_xml;
  char * xml_buf;
  
  /* Read the data from the port, blocking if nothing yet there. 
   We assume the request (the part we care about) is in one netbuf */
  netconn_recv(conn,&inbuf);
  
  if (inbuf != NULL && netconn_err(conn) == ERR_OK ){
      netbuf_data(inbuf, (void**)&buf, &buflen);
      LWIP_DEBUGF(HTTPD_DEBUG &0x00, ("\n------------------------------------------------------\n"));
      LWIP_DEBUGF(HTTPD_DEBUG&0x00, ("Request:\n%s\n", buf));                    
      
      if(HttpdReadHdrAnalyze(&buf,&temp,&temp_len)!=HTTPREAD_HDR_TYPE_GET){
        netconn_close(conn);
        netbuf_delete(inbuf);
        return;
      } else 
        if(!strncmp(temp,UPNP_DEVICE_XML_FILE,temp_len)){
          vPortEnterCritical();
          len_xml = format_device_xml(NULL);
          #if 0
            xml_buf = (char *)mem_malloc(len_xml +1);
            memset(xml_buf, 0, len_xml+1);
          #else
            xml_buf = send_buf;
            memset(xml_buf, 0, len_xml+1);
          #endif
          vPortExitCritical();          
          if(!xml_buf)
          {
              netconn_close(conn);
              netbuf_delete(inbuf);
              return;
          }
          vPortEnterCritical();          
          sprintf(xml_buf,http_header,HttpdResponseHdr(),len_xml);
          vPortExitCritical();          
          netconn_write(conn,(const uint8_t *)xml_buf,strlen(xml_buf), NETCONN_COPY);
          vPortEnterCritical();
          format_device_xml(xml_buf);
          vPortExitCritical();          
          netconn_write(conn,(const uint8_t *)xml_buf,strlen(xml_buf), NETCONN_COPY);
        }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);
  
  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}
/**
  * @brief  upnp web server thread 
  * @param arg: pointer on argument(not used here) 
  * @retval None
  */
static void upnp_server_netconn_thread(void *arg)
{ 
  struct netconn *conn, *newconn;
  err_t err;
  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);
  if (conn!= NULL){
    /* Bind to port 5200 with default IP address */
    err = netconn_bind(conn, NULL, UPNP_HTTP_PORT);
    
    if (err == ERR_OK){
      /* Put the connection into LISTEN state */
      netconn_listen(conn);
      while(1){
        /* accept any icoming connection */
        netconn_accept(conn,&newconn);
        if(newconn){
          /* serve connection */
          upnp_server_serve(newconn);
        }        
          /* delete connection */
          netconn_delete(newconn);				
        //vTaskDelay(100/portTICK_RATE_MS);
      }
    }
  }
}
/*
 * (UPNP_WPS_DEVICE_XML_FILE)
 */
static size_t format_device_xml(char *buf)
{
  #define ADD_XML_DATA(tag,data) {\
    if(buf)\
      xml_add_tagged_data(buf,tag,data);\
    else{\
      xml_add_tagged_data(helper,tag,data);\
      length += strlen(helper);\
      memset(helper,0,256);\
    }\
  }
  #define ADD_XML_URL(tag,url) {\
    if(buf)\
      xml_add_tagged_url_data(buf,tag,url);\
    else{\
      xml_add_tagged_url_data(helper,tag,url);\
      length += strlen(helper);\
      memset(helper,0,256);\
    }\
  }
  #define ADD_XML_CAT(text) {\
    if(buf)\
      strcat(buf,text);\
    else\
      length += strlen(text);\
  }
  static char *uuid_string=NULL;  
  const char *s;
  char *helper=NULL;
  size_t length= strlen(device_xml_prefix);
  if(buf)
    strcpy(buf, device_xml_prefix);
  else{
    helper = mem_malloc(256);
    if(helper==NULL)
      return 0;
    memset(helper,0,256);
  }
  uuid_string = mem_malloc(80);
  if(uuid_string==NULL){
    mem_free(helper);
    return 0;
  }
/*
 * Add required fields with default values if not configured. Add
 * optional and recommended fields only if configured.
 */
  if(strlen(UPNP_URL_BASE)){
    ADD_XML_URL("URLBase",UPNP_URL_BASE);
  }
  ADD_XML_CAT(device_xml_midfix);
  s = UPNP_FRIENDLY_NAME;
  s = ((s && *s) ? s : "Unknow Device");
  sprintf(uuid_string,"%s: \'%s\' (%s)",s,GetHostName(),htoa((struct in_addr *)Localhost()));
  ADD_XML_DATA("friendlyName", uuid_string);
  s = UPNP_MANUFACTURER;
  s = ((s && *s) ? s : "");
  ADD_XML_DATA("manufacturer", s);
  if (strlen(UPNP_MANUFACTURER_URL)){
    ADD_XML_DATA("manufacturerURL",UPNP_MANUFACTURER_URL);
  }
  if (strlen(UPNP_MODEL_DECRIPTION)){
    ADD_XML_DATA("modelDescription",UPNP_MODEL_DECRIPTION);
  }
  s = UPNP_MODEL_NAME;
  s = ((s && *s) ? s : "");
  ADD_XML_DATA("modelName", s);
  if (strlen(GetHostName())){
    ADD_XML_DATA( "modelNumber",GetHostName());
  }
  if (strlen(UPNP_MODEL_URL)){
    ADD_XML_DATA("modelURL", UPNP_MODEL_URL);
  }
  //memset(uuid_string,0,sizeof(uuid_string));
  //BSP_GetSerialNumber(uuid_string); 
  //ADD_XML_DATA("serialNumber",uuid_string);
  //uuid_bin2str(UUID_DEVICE, uuid_string, sizeof(uuid_string));
  s = uuid_string;
/* Need "uuid:" prefix, thus we can't use xml_add_tagged_data()
 * easily...
 */
  ADD_XML_CAT("<UDN>uuid:");
  if(buf){
    xml_data_encode(buf, s, strlen(s));  
  }else{
    xml_data_encode(helper, s, strlen(s));
    length +=strlen(helper);
    memset(helper,0,256);
  }
  ADD_XML_CAT("</UDN>\n");
  
  if (strlen(UPNP_UPC)){
    ADD_XML_DATA("UPC",UPNP_UPC);     
  }
  ADD_XML_CAT(device_xml_postfix);
  ADD_XML_URL("presentationURL",htoa((struct in_addr *)Localhost()));
  ADD_XML_CAT("</device>\n</root>\n");
  mem_free(helper);
  mem_free(uuid_string);  
  return ((buf)? strlen(buf):length);
}
/* xml_add_tagged_data -- format tagged data as a new xml line.
 *
 * tag must not have any special chars.
 * data may have special chars, which are escaped.
 */
static void xml_add_tagged_data(char *buf, const char *tag, const char *data)
{
  sprintf(&buf[strlen(buf)], "<%s>", tag);
  xml_data_encode(buf, data, strlen(data));
  sprintf(&buf[strlen(buf)], "</%s>\n", tag);
}
static void xml_add_tagged_url_data(char *buf, const char *tag, const char *data)
{
  sprintf(&buf[strlen(buf)], "<%s>http://", tag);
  xml_data_encode(buf, data, strlen(data));
  sprintf(&buf[strlen(buf)], "</%s>\n", tag);
}
/* xml_data_encode -- format data for xml file, escaping special characters.
 *
 * Note that we assume we are using utf8 both as input and as output!
 * In utf8, characters may be classed as follows:
 *     0xxxxxxx(2) -- 1 byte ascii char
 *     11xxxxxx(2) -- 1st byte of multi-byte char w/ unicode value >= 0x80
 *         110xxxxx(2) -- 1st byte of 2 byte sequence (5 payload bits here)
 *         1110xxxx(2) -- 1st byte of 3 byte sequence (4 payload bits here)
 *         11110xxx(2) -- 1st byte of 4 byte sequence (3 payload bits here)
 *      10xxxxxx(2) -- extension byte (6 payload bits per byte)
 *      Some values implied by the above are however illegal because they
 *      do not represent unicode chars or are not the shortest encoding.
 * Actually, we can almost entirely ignore the above and just do
 * text processing same as for ascii text.
 *
 * XML is written with arbitrary unicode characters, except that five
 * characters have special meaning and so must be escaped where they
 * appear in payload data... which we do here.
 */
static void xml_data_encode(char *buf, const char *data, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    BYTE c = ((BYTE *) data)[i];
    if (c == '<') {
      strcat(buf, "&lt;");
      continue;
    }
    if (c == '>') {
      strcat(buf, "&gt;");
      continue;
    }
    if (c == '&') {
      strcat(buf, "&amp;");
      continue;
    }
    if (c == '\'') {
      strcat(buf, "&apos;");
      continue;
    }
    if (c == '"') {
      strcat(buf, "&quot;");
      continue;
    }
    /*
     * We could try to represent control characters using the
     * sequence: &#x; where x is replaced by a hex numeral, but not
     * clear why we would do this.
     */
    buf[strlen(buf)]=c;
  }
}
#endif

