#include "common_head.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Display_Task.h"
#include "OSAL_Console.h"
#include "System_Task.h"
#include "Lcd_driver.h"
#include "keyboard.h"

#include "lwip/timers.h"

#include "lwip/ip_addr.h"


extern ip_addr_t *get_cur_ip(void);

static void display_welcome_handler(sys_state_t *sys);

#define IS_CODE_PASS  ((gCode[0] == '<' && gCode[1] == '>' && gCode[2] == '<') ? 1:0)

#define IS_CODE_ERR   (gCode[2] == 0? 0:1)

#define IS_RESET_IP_CODE ((gCode[0] == '<' && gCode[1] == '<' && gCode[2] == '<')? 1:0)

#define LCD_LINE_LEN  20

xQueueHandle gDisQueue;

menu_t *gMenuHead;

menu_t *cur_menu;

menu_t *last_menu;

const static char * const src_name[SRC_NUM]={
	"XLR ", "RCA ", "Speak"
};

const static char * const pre_name[PRESET_NUM]={
	"Normal", "Cinema", "Night", "User1", "User2", "User3"
};

const static char * const bright_name[BRI_NUM]={
	"Low", "Medium", "High"
};

const static char * const bright_name_center[BRI_NUM]={
	" Low", "Medium", " High"
};

const char * const sub_name[SUB_MODEL_NUM]={
 "   Mini InRoom", "  Mini InCeiling", "   Mini FlexSub",
 "  Bronze InRoom", "   Bronze InWall",
 " Bronze InCeiling", " Bronze SlimSub", "   Silver InRoom",
 "   Silver InWall", "  Silver FlexSub", "   Gold InRoom",
 "  Platinum InRoom", "       Flat"
};

const static char * const delay_name[DELAY_ROUTER_MAX]={
	"Sub", "Line", "Off"
};

const static char * const power_on_name[POWER_ON_MAX]={
	"Manua", "Sense", "Trigg", "IR/IP"
};

const static char * const power_on_name_center[POWER_ON_MAX]={
	" Manual", " Sense", "Trigger", " IR/IP"
};

const static char *const sense_theshold[SENSE_THRESHOLD_NUM]={
	"03mV", "06mV", "09mV", "12mV", "15mV" 
};

const static char *const sense_timeout[SE_TIMEOUT_NUM]={
	"05", "10", "20", "30", "60"
};

static menu_context_t menu_context[MENU_NUM] ={
0
};	

static menu_t *menu_list_add_next(menu_t **head, menu_id_t id)
{
	menu_t *menu;
	menu_t *p = *head;
	
	menu = (menu_t *)pvPortMalloc(sizeof(menu_t));
	
	OSAL_ASSERT(menu != NULL);
	
	menu->id = id;
	menu->context = &menu_context[id];
	menu->child = NULL;
	menu->parent = NULL;
	menu->next = NULL;
	menu->prev = NULL;
	
	if(*head){
		while(p->next){
			p = p->next;
		}
		
		p->next = menu;
		menu->prev = p;
		
	}else{
		*head = menu;
	}
	
	/*reach the list end*/
	if(id == MENU_NODE - 1){
		menu->next = (*head)->next->next->next;
		(*head)->next->next->next->prev = menu;
	}
	
	return menu;
}

static void menu_list_add_child_and_next(menu_t *parent, menu_id_t id)
{
	menu_t *menu;
	menu_t *p = parent;
	
	menu = (menu_t *)pvPortMalloc(sizeof(menu_t));
	
	OSAL_ASSERT(menu != NULL);
	
	menu->id = id;
	menu->context = &menu_context[id];
	menu->child = NULL;
	menu->parent = NULL;
	menu->next = NULL;
	menu->prev = NULL;
	
	if(id == MENU_NODE){
		p->child = menu;
		menu->parent = p;
	}else{
		p = p->child;
		
		while(p->next){
			p = p->next;
		}
		
		p->next = menu;
		menu->prev = p;
	}
	
	if(id == MENU_NUM - 1){
		menu->next = parent->child;
		parent->child->prev = menu;
	}
}




void display_menu_list_create(menu_t **head)
{
	uint8 j;
	menu_t *tail;
	
	for(j = 0; j < MENU_NODE; j++){
		tail = menu_list_add_next(head, (menu_id_t)j);
	}
	
	//SYS_TRACE("the first list end menu is[%d]\r\n", tail->id);
	
	/*welcome and factory menu just show once*/
	(*head)->next->next->next->prev = tail;
	 
	
	for(j = MENU_NODE; j < MENU_NUM; j++){
		menu_list_add_child_and_next(tail, (menu_id_t)j);
	}
	
}


void display_menu_jump(menu_id_t to)
{
	menu_t *found = gMenuHead;
	menu_t *begin = gMenuHead;
	
	while(found->id != INSTALL_MENU){
		if(found->id == to){
			cur_menu = found;
			return;
		}else{
			found = found->next;
		}
	}
	
	begin = found->child;
	found = found->child;
	
	OSAL_ASSERT(begin != NULL);
	
	while(found->next != begin){
     if(found->id == to){
			 cur_menu = found;
			 break;
		 }else{
			 found = found->next;
		 }
	}
	
}


static void dis_print(uint8 line, char *fmt, ...)
{
	 uint8 j;
	 char buf[LCD_LINE_LEN + 1] = {0};
   va_list ap;

   va_start(ap, fmt);
   vsprintf(buf, fmt, ap);
   va_end(ap);

   lcd_write_cmd(line);
   bsp_delay_us(47);
   for(j = 0; j < LCD_LINE_LEN; j++){
		 if(buf[j] == '<'){
			 lcd_write_data(0);
		 }else if(buf[j] == '>'){
			 lcd_write_data(6);
		 }else if(buf[j] == '@'){
			 lcd_write_data(4);
		 }else if(buf[j] == 0){
			 lcd_write_data(' ');
		 }else if(buf[j] == '^'){
			 lcd_write_data(7);
		 }else{
			 lcd_write_data(buf[j]);
		 }
		 
		 bsp_delay_us(47);
	 }
   
}

static void dis_print_buf(uint8 line, uint8 *buf)
{
	 uint8 j;
	
	 lcd_write_cmd(line);
   bsp_delay_us(47);
   for(j = 0; j < LCD_LINE_LEN; j++){
		 lcd_write_data(buf[j]);
		 bsp_delay_us(47);
	 }
}

static void dis_clean(void)
{
//	lcd_write_cmd(0x01);
//	bsp_delay_ms(2);
}

static void dis_welcome_menu(sys_state_t *sys)
{
	 dis_clean();
	 dis_print(LCD_LINE_1, " TRIAD SPEAKERS INC");
	 OSAL_ASSERT(sys->sub_model < SUB_MODEL_NUM);
	 dis_print(LCD_LINE_2, "  %s", sub_name[sys->sub_model]);
	
	 //sys_timeout(5000, welcome_dis_timeout_handler, (void *)sys);
}

static void dis_volume_menu(sys_state_t *sys)
{
	 uint8 vol,j;
	 uint8 index1, index2;
	 uint8 temp[20] = {0};
	

   /*when protection event occur, we should not overwrite the protection display*/
   if(sys->protect_bitmap != 0) return;

	 dis_clean(); 
	 if(sys->is_mute == true){
		    dis_print(LCD_LINE_1, "Volume       < Mute>");
		    dis_print(LCD_LINE_2, "                    ");
	 }else{
			if(sys->master_vol >= 0){
				dis_print(LCD_LINE_1, "Volume       <+%02ddB>", sys->master_vol);
			}else{
				dis_print(LCD_LINE_1, "Volume       <%03ddB>", sys->master_vol);
			}
			
			vol = sys->master_vol - VOLUME_VALUE_MIN(sys->input_gain);
			index1 = vol / 4;
			index2 = vol % 4;
			for(j = 0; j < index1; j++){
				temp[j] = 5;
			}
			
			for(j = index1; j < 20; j++){
				temp[j] = ' ';
			}
			
			if(index2){
			    temp[index1] = index2;
			}
			
			dis_print_buf(LCD_LINE_2,temp);
			
   }
	 
}

static void dis_source_menu(sys_state_t *sys)
{
	dis_clean(); 
  dis_print(LCD_LINE_1, " Input Source Select");
	OSAL_ASSERT(sys->src < SRC_NUM);
	dis_print(LCD_LINE_2, "%-6s<%5s >%6s", src_name[(sys->src == 0? (SRC_NUM - 1):(sys->src - 1))], src_name[sys->src], src_name[((sys->src + 1) % SRC_NUM)]);
}


static void dis_preset_menu(sys_state_t *sys)
{
	dis_clean();  
	dis_print(LCD_LINE_1, " Preset Selection");
	OSAL_ASSERT(sys->preset < PRESET_NUM);
	dis_print(LCD_LINE_2, "%-6s<%6s>%6s", pre_name[(sys->preset == 0? (PRESET_NUM - 1): (sys->preset - 1))], pre_name[sys->preset], pre_name[((sys->preset + 1) % PRESET_NUM)]);
}

static void dis_brightless_menu(sys_state_t *sys)
{
	dis_clean(); 
	dis_print(LCD_LINE_1, " Display Brightness");
	OSAL_ASSERT(sys->bright < BRI_NUM);
	dis_print(LCD_LINE_2, "%-6s<%-6s>%6s", bright_name[(sys->bright == 0? (BRI_NUM - 1):(sys->bright - 1))], bright_name_center[sys->bright], bright_name[((sys->bright + 1) % BRI_NUM)]);
}

static void dis_menu_flash(void *arg)
{
	static uint8 cnt = 0;

  if(cnt % 2){
      dis_clean();  
			dis_print(LCD_LINE_1, "  Installer Menu");
			dis_print(LCD_LINE_2, "  Enter Code ***");
	}else{
		  dis_clean();  
			dis_print(LCD_LINE_1, "  Installer Menu");
			dis_print(LCD_LINE_2, "                ");
	}
	
	cnt++;
	if(cnt == 4){
		cnt = 0;
	}else{
		sys_timeout(500, dis_menu_flash, NULL);
	}
	
	/*when we flash the menu, we must keep clear the code buf*/
	CLEAR_CODE();
}

static void dis_install_menu(sys_state_t *sys)
{
	
	if(IS_CODE_PASS){
		CLEAR_CODE();
		sys_send_msg(CODE_PASS_MSG, 0);
	}
	else{
		if(IS_CODE_ERR){
			CLEAR_CODE();
			sys_untimeout(dis_menu_flash, NULL);
	    sys_timeout(500, dis_menu_flash, NULL);
		}else{
			dis_clean();  
			dis_print(LCD_LINE_1, "  Installer Menu");
			dis_print(LCD_LINE_2, "  Enter Code ***");
		}
	}
}

static void dis_ip_address_menu(sys_state_t *sys)
{
	  if(IS_RESET_IP_CODE){
			CLEAR_CODE();
			dis_clean();  
			dis_print(LCD_LINE_1, "     IP Address");
			dis_print(LCD_LINE_2, "  set IP to static  ");  
			sys_ip_set_to_default(sys);
		}else{
			dis_clean();  
			dis_print(LCD_LINE_1, "     IP Address");
			dis_print(LCD_LINE_2, "    %s", ipaddr_ntoa(get_cur_ip()));  
		}
}

void update_ip_menu(void)
{
	 if(cur_menu->id == IP_ADDRESS_MENU){
		 dis_send_msg(DIS_UPDATE, MENU_NUM);
	 }
}


static void dis_sub_model_menu(sys_state_t *sys)
{
	 dis_clean();  
   dis_print(LCD_LINE_1, "Current Sub Model EQ");
	 dis_print(LCD_LINE_2, "<%-18s>", sub_name[sys->sub_model]);
}

static void dis_input_gain_menu(sys_state_t *sys)
{
	 dis_clean();  
	 dis_print(LCD_LINE_1, "  Input Gain Level");
	 if(sys->input_gain == 10){
		  dis_print(LCD_LINE_2, "+09dB< +10dB  >     ");
	 }else if(sys->input_gain > 1){
			dis_print(LCD_LINE_2, "+%02ddB< +%02ddB  >+%02ddB", sys->input_gain - 1, sys->input_gain, sys->input_gain + 1);
	 }else if(sys->input_gain == 1){
		  dis_print(LCD_LINE_2, " 00dB< +01dB  >+02dB");
	 }else if(sys->input_gain == 0){
		  dis_print(LCD_LINE_2, "-01dB<  00dB  >+01dB");
	 }else if(sys->input_gain == -1){
		  dis_print(LCD_LINE_2, "-02dB< -01dB  > 00dB");
	 }else if(sys->input_gain == 10){
		  dis_print(LCD_LINE_2, "+09dB< +10dB  >     ");
	 }else if(sys->input_gain == -10){
		  dis_print(LCD_LINE_2, "     < -10dB  > -9dB");
	 }else{
		  dis_print(LCD_LINE_2, "%03ddB< %03ddB  >%03ddB", sys->input_gain -1, sys->input_gain, sys->input_gain + 1);
	 }
	 
}

static void dis_phrase_menu(sys_state_t *sys)
{
	dis_clean();    
	dis_print(LCD_LINE_1, "       Phase");
	if(sys->sub[sys->sub_model].phrase){
		dis_print(LCD_LINE_2, "00   <  180   >   00");
	}else{
		dis_print(LCD_LINE_2, "180  <   00   >  180");
	}
}

static void dis_sub_lowpass_menu(sys_state_t *sys)
{
	dis_clean(); 
  dis_print(LCD_LINE_1, " Sub Lowpass Filter");  
	if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_DEFAULT){
		dis_print(LCD_LINE_2, "300Hz<  LFE   > 30Hz");
	}else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_MIN){
		dis_print(LCD_LINE_2, "LFE  <  30Hz  > 31Hz");
	}else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_MAX){
		dis_print(LCD_LINE_2, "290Hz< 300Hz  >  LFE");
	}else if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->sub[sys->sub_model].lowpass - 1, 
		    sys->sub[sys->sub_model].lowpass, sys->sub[sys->sub_model].lowpass + 1);
	}else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", SUB_LOWPASS_NODE1 - 1, SUB_LOWPASS_NODE1, SUB_LOWPASS_NODE1 + 5);
	}else if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_NODE2){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->sub[sys->sub_model].lowpass - 5, 
		   sys->sub[sys->sub_model].lowpass, sys->sub[sys->sub_model].lowpass + 5);
	}else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_NODE2){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", SUB_LOWPASS_NODE2 - 5, SUB_LOWPASS_NODE2, SUB_LOWPASS_NODE2 + 10);
	}else if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_MAX){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->sub[sys->sub_model].lowpass - 10, 
		  sys->sub[sys->sub_model].lowpass, sys->sub[sys->sub_model].lowpass + 10);
	}
	
}

static void dis_sub_highpass_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, " Sub Highpass Filter");  
	 if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_OFF){
		dis_print(LCD_LINE_2, "100Hz<  OFF   > 20Hz");
	}else if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_MIN){
		dis_print(LCD_LINE_2, "OFF  <  20Hz  > 21Hz");
	}else if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_MAX){
		dis_print(LCD_LINE_2, "90Hz < 100Hz  >  OFF");
	}else if(sys->sub[sys->sub_model].highpass < SUB_HIGHPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->sub[sys->sub_model].highpass - 1, 
		    sys->sub[sys->sub_model].highpass, sys->sub[sys->sub_model].highpass + 1);
	}else if(sys->sub[sys->sub_model].highpass == SUB_LOWPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", SUB_HIGHPASS_NODE1 - 1, SUB_HIGHPASS_NODE1, SUB_HIGHPASS_NODE1 + 5);
	}else if(sys->sub[sys->sub_model].highpass < SUB_HIGHPASS_MAX){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->sub[sys->sub_model].highpass - 5, 
		  sys->sub[sys->sub_model].highpass, sys->sub[sys->sub_model].highpass + 5);
	}
}

static void dis_lineout_highpass_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Line Output Highpass"); 
	 if(sys->lineout_highpass == LINEOUT_HIGHPASS_DEFAULT){
		dis_print(LCD_LINE_2, "300Hz<  Thru  > 30Hz");
	}else if(sys->lineout_highpass == LINEOUT_HIGHPASS_MIN){
		dis_print(LCD_LINE_2, "Thru <  30Hz  > 31Hz");
	}else if(sys->lineout_highpass == LINEOUT_HIGHPASS_MAX){
		dis_print(LCD_LINE_2, "290Hz< 300Hz  > Thru");
	}else if(sys->lineout_highpass < LINEOUT_HIGHPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->lineout_highpass - 1, 
		    sys->lineout_highpass, sys->lineout_highpass + 1);
	}else if(sys->lineout_highpass == LINEOUT_HIGHPASS_NODE1){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", LINEOUT_HIGHPASS_NODE1 - 1, LINEOUT_HIGHPASS_NODE1, LINEOUT_HIGHPASS_NODE1 + 5);
	}else if(sys->lineout_highpass < LINEOUT_HIGHPASS_NODE2){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->lineout_highpass - 5, 
		   sys->lineout_highpass, sys->lineout_highpass + 5);
	}else if(sys->lineout_highpass == LINEOUT_HIGHPASS_NODE2){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", LINEOUT_HIGHPASS_NODE2 - 5, LINEOUT_HIGHPASS_NODE2, LINEOUT_HIGHPASS_NODE2 + 10);
	}else if(sys->lineout_highpass < LINEOUT_HIGHPASS_MAX){
		dis_print(LCD_LINE_2, "%3dHz< %3dHz  >%3dHz", sys->lineout_highpass - 10, 
		  sys->lineout_highpass, sys->lineout_highpass + 10);
	}
}

static void dis_delay_router_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "   Route Delay to");  
	 OSAL_ASSERT(sys->d_router < DELAY_ROUTER_MAX);
	 dis_print(LCD_LINE_2, "%-5s<  %-6s>%5s", delay_name[(sys->d_router == 0? (DELAY_ROUTER_MAX - 1):(sys->d_router - 1))],
	      delay_name[sys->d_router], delay_name[(sys->d_router + 1)%DELAY_ROUTER_MAX]); 
}

static void dis_power_on_method_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "  Power On Method");  
	 OSAL_ASSERT(sys->method < POWER_ON_MAX);
	 dis_print(LCD_LINE_2, "%-5s<%-8s>%5s", power_on_name[(sys->method == 0? (POWER_ON_MAX - 1):(sys->method - 1))], 
	  power_on_name_center[sys->method], power_on_name[(sys->method + 1)%POWER_ON_MAX]); 
}

static void dis_reset_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Factory Reset Press@");  
	 if(sys->do_reset == true){
			dis_print(LCD_LINE_2, "No   <  Yes   >   No");  
	 }else{
		  dis_print(LCD_LINE_2, "Yes  <  No    >  Yes"); 
	 }
}

static void dis_delay_time_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Delay in ms or (ft)");
	 if(sys->d_router == DELAY_SUB){
     dis_print(LCD_LINE_2, "%04.1f <  %04.1f  > %04.1f", (float)((sys->sub[sys->sub_model].delay == 0? (SUB_DELAY_MAX):(sys->sub[sys->sub_model].delay - 5))/10.0),
	     (float)(sys->sub[sys->sub_model].delay / 10.0), (float)((sys->sub[sys->sub_model].delay == SUB_DELAY_MAX ?(0):(sys->sub[sys->sub_model].delay + 5))/10.0) ); 
	 }else if(sys->d_router == DELAY_LINEOUT){
		 dis_print(LCD_LINE_2, "%04.1f <  %04.1f  > %04.1f", (float)((sys->line_delay_time == 0? (SUB_DELAY_MAX):(sys->line_delay_time - 5))/10.0),
	     (float)(sys->line_delay_time / 10.0), (float)((sys->line_delay_time == SUB_DELAY_MAX ?(0):(sys->line_delay_time + 5))/10.0) );
	 }
}

static void dis_sense_threshold_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Sense On Threshold");
	 OSAL_ASSERT(sys->sense_thres < SENSE_THRESHOLD_NUM);
	 dis_print(LCD_LINE_2, "%s <  %s  > %s", sense_theshold[(sys->sense_thres == 0? (SENSE_THRESHOLD_NUM - 1):(sys->sense_thres - 1))],
	   sense_theshold[sys->sense_thres], sense_theshold[(sys->sense_thres + 1) % SENSE_THRESHOLD_NUM]);
}

static void dis_sense_off_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Sense Off in Minutes");
	 dis_print(LCD_LINE_2, "%s   <   %s   >   %s", sense_timeout[(sys->se_timeout == 0?(SE_TIMEOUT_NUM - 1):(sys->se_timeout - 1))],
      	sense_timeout[sys->se_timeout], sense_timeout[(sys->se_timeout + 1) % SE_TIMEOUT_NUM]); 
}

static void dis_reset_confirm_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Confirm Full Reset @");
	 if(sys->confirm_reset == true){		 
		dis_print(LCD_LINE_2, "No   <  Yes   >   No"); 
	 }else{
		dis_print(LCD_LINE_2, "Yes  <   No   >  Yes"); 
	 }
}

static void dis_factory_sub_select_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Sub Model EQ Press@");
	 dis_print(LCD_LINE_2, "<%-18s>", sub_name[sys->temp_sub_model]);
}

static void dis_factory_sub_confirm_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, "Confirm Model Press@");
	 dis_print(LCD_LINE_2, "^%-18s^", sub_name[sys->temp_sub_model]); 
}

static void dis_peq_state_menu(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, " Parametric Room EQ ");
	 if(sys->eq_on_off == 0){
			dis_print(LCD_LINE_2, "       ^ OFF^"); 
	 }else{
		  dis_print(LCD_LINE_2, "       ^ ON ^");
	 }
}

static void dis_protect_menu(menu_id_t id)
{
	 switch(id){
		 case DIS_OVER_TEMP:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
			 dis_print(LCD_LINE_2, " Over Temperature "); 
			 break;
		 case DIS_OVER_VOLTAGE_OR_SHORT:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
		   if(IS_SHORT_CIRCUIT){
				 dis_print(LCD_LINE_2, "  Short Circuit");
			 }else{
				 dis_print(LCD_LINE_2, "    Over Voltage ");
			 }
			 
			 break;
		 case DIS_OVER_CLIP:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
			 dis_print(LCD_LINE_2, "    Clipping ");
			 break;
		 case DIS_OVER_DC_ERROR:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
			 dis_print(LCD_LINE_2, "     DC Error ");
			 break;
		 case DIS_OVER_CURRENT:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
			 dis_print(LCD_LINE_2, "    Over Current ");
			 break;
		 case DIS_OVER_15V:
			 dis_clean(); 
			 dis_print(LCD_LINE_1, "   PROTECTION!!!!   ");
			 dis_print(LCD_LINE_2, "    Low 15v ");
			 break;
		 
		 default:
			 break;
	 }
}

static void dis_version_menu(void)
{
	dis_clean(); 
	dis_print(LCD_LINE_1, "  Firmware Version");
	dis_print(LCD_LINE_2, "      %s", version);
}

static void display_handler(sys_state_t *sys, menu_id_t id)
{
	 OSAL_ASSERT(sys != NULL);
	
	 if(id >= DIS_OVER_TEMP && id <= DIS_OVER_15V){
		 dis_protect_menu(id);
	 }
	
	 if(sys->status != STATUS_WORKING) return;
	
	 if(sys->protect_bitmap != 0){
		 if(id < MENU_NUM) return;
	 }
	
	 switch(id){
		 case WELCOM_MENU:
			 display_welcome_handler(sys);
			 break;
		 
		 case VOLUME_MENU:
			 dis_volume_menu(sys);
			 break;
		 
		 case SOURCE_MENU:
			 dis_source_menu(sys);
			 break;
		 
		 case PRESET_MENU:
			 dis_preset_menu(sys);
			 break;
		 
		 case BRIGHTLESS_MENU:
			 dis_brightless_menu(sys);
			 break;
		 
		 case INSTALL_MENU:
			 dis_install_menu(sys);
			 break;
		 
		 case IP_ADDRESS_MENU:
			 dis_ip_address_menu(sys);
			 break;
		 
		 case SUB_MODEL_MENU:
			 dis_sub_model_menu(sys);
			 break;
		 
		 case INPUT_GAIN_MENU:
			 dis_input_gain_menu(sys);
			 break;
		 
		 case PHRASE_MENU:
			 dis_phrase_menu(sys);
			 break;
		 
		 case SUB_LOWPASS_MENU:
			 dis_sub_lowpass_menu(sys);
			 break;
		 
		 case SUB_HIGHPASS_MENU:
			 dis_sub_highpass_menu(sys);
			 break;
		 
		 case LINEOUT_HIGHPASS_MENU:
			 dis_lineout_highpass_menu(sys);
			 break;
		 
		 case DELAY_ROUTER_MENU:
			 dis_delay_router_menu(sys);
			 break;
		 
		 case DELAY_TIME_MENU:
			 dis_delay_time_menu(sys);
			 break;
		 
		 case POWER_ON_METHOD_MENU:
			 dis_power_on_method_menu(sys);
			 break;
		 
		 case SENSE_THRESHOLD:
			 dis_sense_threshold_menu(sys);
			 break;
		 
		 case SENSE_OFF_TIME:
			 dis_sense_off_menu(sys);
			 break;
		 
		 case RESET_MENU:
			 dis_reset_menu(sys);
			 break;
		 
		 case RESET_CONFIRM_MENU:
			 dis_reset_confirm_menu(sys);
			 break;
		 
		 case FACTORY_SUB_SELECT_MENU:
			 dis_factory_sub_select_menu(sys);
			 break;
		 
		 case FACTOTY_SUB_CONFIRM_MENU:
			 dis_factory_sub_confirm_menu(sys);
		   break;
		 
//		 case DIS_OVER_TEMP:
//		 case DIS_OVER_VOLTAGE_OR_SHORT:
//		 case DIS_OVER_CLIP:
//		 case DIS_OVER_DC_ERROR:
//		 case DIS_OVER_CURRENT:
//		 case DIS_OVER_15V:
//			 dis_protect_menu(id);
//			 break;
		 
		 case DIS_PEQ_STATE:
			   dis_peq_state_menu(sys);
			  break; 
		 
		 case DIS_VERSION_INFOR:
			   dis_version_menu();
			 break;
		 
		 default:
			 break;
	 }
}

static void welcome_dis_timeout_handler(void *arg)
{
	 if(((sys_state_t *)arg)->is_in_factory){
		 display_menu_jump(FACTORY_SUB_SELECT_MENU);
	 }else{
		 display_menu_jump(VOLUME_MENU);
		 sys_timeout(DIS_TIMEOUT, dis_timeout_handler, NULL);
	 }
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void display_welcome_handler(sys_state_t *sys)
{
	 dis_clean(); 
	 dis_print(LCD_LINE_1, " TRIAD SPEAKERS INC ");
	 dis_print(LCD_LINE_2, " %s", sub_name[sys->sub_model]);
	
	 sys_untimeout(welcome_dis_timeout_handler, (void *)sys);
	 sys_timeout(5000, welcome_dis_timeout_handler, (void *)sys);
}

void cancle_welcome_timeout(sys_state_t *sys)
{
	 sys_untimeout(welcome_dis_timeout_handler, (void *)sys);
}

void display_init_menu(void)
{
	dis_clean(); 
	dis_print(LCD_LINE_1, "   Initializing...  "); 
}


void display_task(void *parameter)
{
	 dis_msg_t msg;
	
	 gDisQueue = xQueueCreate(20, sizeof(dis_msg_t));
	 OSAL_ASSERT(gDisQueue != NULL);
	
	 display_menu_list_create(&gMenuHead);
	 cur_menu = gMenuHead;
	 
	 while(1){
		 if(xQueueReceive(gDisQueue, &msg, portMAX_DELAY)){
			 switch(msg.msg){
				 case DIS_WELCOME:
					 display_welcome_handler(gSystem_t);
					 break;
				 
				 case DIS_UPDATE:
					   display_handler(gSystem_t, cur_menu->id);
					 break;
				 
				 case DIS_JUMP:
					   display_handler(gSystem_t, msg.id);
					 break;
				 
				 default:
					 break;
			 }
		 }
	 }
}

void dis_send_msg(dis_msg_type_t type, menu_id_t id)
{
	 dis_msg_t msg;

   msg.msg = type;
   msg.id = id;	
	 
	 xQueueSend(gDisQueue, &msg, portMAX_DELAY);
}

void dis_send_msg_isr(dis_msg_type_t type, menu_id_t id)
{
	 dis_msg_t msg;

   msg.msg = type;
	 msg.id = id;
	
	 xQueueSendToFrontFromISR(gDisQueue, &msg, NULL);
}


/*******************************For Test*********************************************/
int menu_list_create(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	display_menu_list_create(&gMenuHead);
	cur_menu = gMenuHead;
	return 0;
}

OSAL_CMD(list_create, 1, menu_list_create, "create menu list");


int menu_list_next(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	
	if(cur_menu->next){
		cur_menu = cur_menu->next;
	}else{
		SYS_TRACE("reach the list end\r\n");
		cur_menu = gMenuHead;
	}
	
	SYS_TRACE("will display [%d] menu\r\n", cur_menu->id);
	
	return 0;
}

OSAL_CMD(list_next, 1, menu_list_next, "nav next menu list");

int menu_list_prev(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	 
	if(cur_menu->prev){
		cur_menu = cur_menu->prev;
	}
	
	SYS_TRACE("will display [%d] menu\r\n", cur_menu->id);
	return 0;
}

OSAL_CMD(list_prev, 1, menu_list_prev, "nav prev menu list");


int menu_list_enter(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	if(cur_menu->child){
		cur_menu = cur_menu->child;
		SYS_TRACE("will display [%d] menu\r\n", cur_menu->id);
	}
	
	 
	return 0;
}

OSAL_CMD(list_enter, 1, menu_list_enter, "nav neter menu list");

int dis_test(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	  
	dis_send_msg(DIS_UPDATE, MENU_NUM);
	return 0;
}

OSAL_CMD(dis, 1, dis_test, "dis test");

/*******************************For Test*********************************************/










