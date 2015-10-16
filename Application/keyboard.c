#include "common_head.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "keyboard.h"
#include "Display_Task.h"
#include "System_Task.h"
#include "OSAL_RingBuf.h"
#include "IR_driver.h"
#include "OSAL_Detect.h"

#include "lwip/timers.h"

#define NUM_OF_KEY 7

#define DATABASE_UPDATE_TIMEOUT  200

#define IS_LEFT_KEY_DOWN   (GPIO_ReadSinglePin(LEFT_KEY_PORT, LEFT_KEY_PIN) == 0? 0:1)
#define IS_RIGHT_KEY_DOWN  (GPIO_ReadSinglePin(RIGHT_KEY_PORT, RIGHT_KEY_PIN) == 0? 0:1)
#define IS_UP_KEY_DOWN  (GPIO_ReadSinglePin(UP_KEY_PORT, UP_KEY_PIN) == 0? 0:1)


static void key_down_handler(sys_state_t *sys);
static void vol_menu_timeout_handler(void *arg);

typedef struct
{
	uint8 (*pkeyread)(UINT32 GPIOx, UINT8 GPIO_Pin);
	uint32 port;
	uint8 pin;
	key_value_t shortpress;
	key_value_t longpress;
  uint32 longpresstime;
	bool repeat_flag;
	bool key_locked_flag;
	uint32 key_timer;
}key_control_t;

key_control_t keylist[NUM_OF_KEY] =
{
	    {GPIO_ReadSinglePin,POWER_KEY_PORT, POWER_KEY_PIN, KEY_POWER, KEY_POWER_LONG,5000,false,false,0},
      {GPIO_ReadSinglePin,ENTER_KEY_PORT, ENTER_KEY_PIN, KEY_ENTER, KEY_ENTER_LONG,5000,false,false,0},
      {GPIO_ReadSinglePin,BACK_KEY_PORT, BACK_KEY_PIN, KEY_BACK, KEY_BACK_LONG,5000,false,false,0},
      {GPIO_ReadSinglePin,LEFT_KEY_PORT, LEFT_KEY_PIN, KEY_LEFT, KEY_LEFT_LONG,1000,false,false,0},
      {GPIO_ReadSinglePin,RIGHT_KEY_PORT, RIGHT_KEY_PIN, KEY_RIGHT, KEY_RIGHT_LONG,1000,false,false,0},
      {GPIO_ReadSinglePin,UP_KEY_PORT, UP_KEY_PIN, KEY_UP, KEY_UP_LONG,5000,false,false,0},
      {GPIO_ReadSinglePin,DOWN_KEY_PORT, DOWN_KEY_PIN, KEY_DOWN, KEY_DOWN_LONG,5000,false,false,0},
};

char gCode[CODE_NUM];


static void key_back_handler(sys_state_t *sys)
{
	 if(cur_menu != gMenuHead){
		 cur_menu = gMenuHead;
		 dis_send_msg(DIS_UPDATE, MENU_NUM);
	 }
}


static void key_enter_handler(sys_state_t *sys)
{
//	 if(cur_menu->child){
//		//cur_menu = cur_menu->child;
//	 }else{
		 if(cur_menu->id == RESET_CONFIRM_MENU && sys->confirm_reset == true){
			 sys_factory_reset(sys);
			 cur_menu = gMenuHead;
		 }else if(cur_menu->id == WELCOM_MENU){
			 cancle_welcome_timeout(sys);
			 if(sys->is_in_factory){
				  cur_menu = cur_menu->next;
				}else{
					display_menu_jump(VOLUME_MENU);
				}
		 }else if(cur_menu->id == FACTOTY_SUB_CONFIRM_MENU){
			  sys->sub_model = sys->temp_sub_model;
			  /*load sub eq*/
			  sys_sub_preset_load(sys);			 
			 
			  sys->is_in_factory = 0;
			  cur_menu = cur_menu->next;
		 }else{
			 key_down_handler(sys);
		 }
	// }
	
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void key_up_handler(sys_state_t *sys)
{
	// OSAL_ASSERT(cur_menu->prev != NULL);
	
	 if(cur_menu->prev == NULL) return;
	
	 if(cur_menu->id == RESET_MENU && sys->method != AUDIO){
				cur_menu = cur_menu->prev->prev->prev;
	 }else if(cur_menu->id == POWER_ON_METHOD_MENU && sys->d_router == DELAY_OFF){
			 cur_menu = cur_menu->prev->prev;
	 }else if(cur_menu->id == IP_ADDRESS_MENU){
		 cur_menu = cur_menu->prev->prev;
	 }else if(cur_menu->id != FACTORY_SUB_SELECT_MENU){
		 cur_menu = cur_menu->prev;
	 }
	 
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void key_down_handler(sys_state_t *sys)
{
   //OSAL_ASSERT(cur_menu->next != NULL);
	
	 if(cur_menu->next == NULL) return;
	
	 if(cur_menu->id == DELAY_ROUTER_MENU && sys->d_router == DELAY_OFF){
			 cur_menu = cur_menu->next->next;
	 }else if(cur_menu->id == POWER_ON_METHOD_MENU && sys->method != AUDIO){
			 cur_menu = cur_menu->next->next->next; 
	 }else if(cur_menu->id == RESET_MENU && sys->do_reset != true){
			 cur_menu = cur_menu->next->next;	 
	 }else if(cur_menu->id == WELCOM_MENU){
		 cancle_welcome_timeout(sys);
		 if(sys->is_in_factory){
				cur_menu = cur_menu->next;
			}else{
				cur_menu = cur_menu->next->next->next;
			}
	 }else if(cur_menu->id != FACTOTY_SUB_CONFIRM_MENU){
		 cur_menu = cur_menu->next;
	 }
	 
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void volume_decrease_handler(sys_state_t *sys)
{
	 if(sys->master_vol > VOLUME_VALUE_MIN(sys->input_gain)){
		 if(sys->is_mute == true){
			  sys->is_mute = false;
		 }else{
				sys->master_vol--;
		 }
	 }else{
		 sys->is_mute = true;
	 }
	 
	 sys_master_vol_set(sys);
	 
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	 
	// sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void volume_increase_handler(sys_state_t *sys)
{
	 if(sys->master_vol < VOLUME_VALUE_MAX(sys->input_gain)){
		 if(sys->is_mute == true){
			 sys->is_mute = false;
		 }else{
		    sys->master_vol++;
		 }
	 }
	 
	 sys_master_vol_set(sys);
	 
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	 //sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void source_next_handler(sys_state_t *sys)
{
	 sys->src = (src_t)((sys->src + 1) % SRC_NUM);
	
	 sys_input_select(sys->src);
	
	// sys_send_msg(DATABASE_UPDATE_MSG, 0);
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void source_prev_handler(sys_state_t *sys)
{
	 sys->src = (src_t)(sys->src == 0? (SRC_NUM - 1):(sys->src - 1));
	
	 sys_input_select(sys->src);
	
	// sys_send_msg(DATABASE_UPDATE_MSG, 0);
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void preset_prev_handler(sys_state_t *sys)
{
	sys->preset = (preset_t)(sys->preset == 0? (PRESET_NUM - 1):(sys->preset - 1)); 
	
	sys_load_peq(sys);
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void preset_next_handler(sys_state_t *sys)
{
	sys->preset = (preset_t)((sys->preset + 1) % PRESET_NUM);  
	
	sys_load_peq(sys);
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void bright_prev_handler(sys_state_t *sys)
{
	sys->bright = (lcd_bright_t)(sys->bright == 0? (BRI_NUM - 1):(sys->bright - 1));
	
	sys_lcd_bright_change(sys);
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void bright_next_handler(sys_state_t *sys)
{
	 sys->bright = (lcd_bright_t)((sys->bright + 1) % BRI_NUM);
	
	 sys_lcd_bright_change(sys);
	
	 sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void load_code(sys_state_t *sys, char code)
{
	 OSAL_ASSERT(sys->code_index < CODE_NUM);
	
	 gCode[sys->code_index] = code;
	
	 sys->code_index = (sys->code_index + 1) % CODE_NUM;
}

static void sub_model_prev_handler(sys_state_t *sys)
{
	 sys->sub_model = (sub_model_t)(sys->sub_model == 0? (SUB_MODEL_NUM - 1):(sys->sub_model - 1));
	
	 sys_sub_preset_load(sys);
	
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
}

static void sub_model_next_handler(sys_state_t *sys)
{
	 sys->sub_model = (sub_model_t)((sys->sub_model + 1) % SUB_MODEL_NUM); 

   sys_sub_preset_load(sys);
	
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
}

static void input_gain_decrease_handler(sys_state_t *sys)
{
	 if(sys->input_gain > INPUT_GAIN_MIN){
		 sys->input_gain--;
		 
		 sys->master_vol++;
	 
	   if(sys->master_vol < MASTER_VOL_VALUE_MIN){
		   sys->master_vol = MASTER_VOL_VALUE_MIN;
	   }else if(sys->master_vol > MASTER_VOL_VALUE_MAX){
			 sys->master_vol = MASTER_VOL_VALUE_MAX;
		 }
		 
		 sys_master_vol_set(sys);
		 
		 sys_untimeout(sys_database1_update_later, NULL);
	   sys_timeout(200, sys_database1_update_later, NULL);
	 }
	
}

static void input_gain_increase_handler(sys_state_t *sys)
{
	 if(sys->input_gain < INPUT_GAIN_MAX){
     sys->input_gain++;
		 
		 sys->master_vol--;
	 
	   if(sys->master_vol < MASTER_VOL_VALUE_MIN){
		   sys->master_vol = MASTER_VOL_VALUE_MIN;
	   }else if(sys->master_vol > MASTER_VOL_VALUE_MAX){
			 sys->master_vol = MASTER_VOL_VALUE_MAX;
		 }
		 sys_master_vol_set(sys);
		 
		 sys_untimeout(sys_database1_update_later, NULL);
	   sys_timeout(200, sys_database1_update_later, NULL);
	 }		 
	 
}

static void phrase_change(sys_state_t *sys)
{
	 if(sys->sub[sys->sub_model].phrase){
		 sys->sub[sys->sub_model].phrase = 0;
	 }else{
		 sys->sub[sys->sub_model].phrase = 1;
	 }
	 
	 sys_phrase_set(sys);
	 
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
}

static void sub_lowpass_increase(sys_state_t *sys)
{
	 if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_NODE1){
      sys->sub[sys->sub_model].lowpass++;
	 }else if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_NODE2){
		  sys->sub[sys->sub_model].lowpass += 5;
	 }else if(sys->sub[sys->sub_model].lowpass < SUB_LOWPASS_MAX){
		  sys->sub[sys->sub_model].lowpass += 10;
	 }else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_MAX){
		  sys->sub[sys->sub_model].lowpass = SUB_LOWPASS_DEFAULT;
	 }else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_DEFAULT){
		  sys->sub[sys->sub_model].lowpass = SUB_LOWPASS_MIN;
	 }
	 
	 sys_sub_lowpass_set(sys);
	 
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
	 //sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_lowpass_decrease(sys_state_t *sys)
{
	 if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_MIN){
		 sys->sub[sys->sub_model].lowpass = SUB_LOWPASS_DEFAULT;
	 }else if(sys->sub[sys->sub_model].lowpass <= SUB_LOWPASS_NODE1){
		 sys->sub[sys->sub_model].lowpass--;
	 }else if(sys->sub[sys->sub_model].lowpass <= SUB_LOWPASS_NODE2){
		 sys->sub[sys->sub_model].lowpass -= 5;
	 }else if(sys->sub[sys->sub_model].lowpass <= SUB_LOWPASS_MAX){
		 sys->sub[sys->sub_model].lowpass -= 10;
	 }else if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_DEFAULT){
		 sys->sub[sys->sub_model].lowpass = SUB_LOWPASS_MAX;
	 }
	 
	 sys_sub_lowpass_set(sys);
	 
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
	 //sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_highpass_decrease(sys_state_t *sys)
{
	 if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_MIN){
		 sys->sub[sys->sub_model].highpass = SUB_HIGHPASS_OFF;
	 }else if(sys->sub[sys->sub_model].highpass <= SUB_HIGHPASS_NODE1){
		 sys->sub[sys->sub_model].highpass--;
	 }else if(sys->sub[sys->sub_model].highpass <= SUB_HIGHPASS_MAX){
		 sys->sub[sys->sub_model].highpass -= 5;
	 }else if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_OFF){
		 sys->sub[sys->sub_model].highpass = SUB_HIGHPASS_MAX;
	 }
	 
	 sys_sub_highpass_set(sys);
	 
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
	// sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void sub_highpass_increase(sys_state_t *sys)
{
	 if(sys->sub[sys->sub_model].highpass < SUB_HIGHPASS_NODE1){
      sys->sub[sys->sub_model].highpass++;
	 }else if(sys->sub[sys->sub_model].highpass < SUB_HIGHPASS_MAX){
		  sys->sub[sys->sub_model].highpass += 5;
	 }else if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_MAX){
		  sys->sub[sys->sub_model].highpass = SUB_HIGHPASS_OFF;
	 }else if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_OFF){
		  sys->sub[sys->sub_model].highpass = SUB_HIGHPASS_MIN;
	 } 
	 
	 sys_sub_highpass_set(sys);
	 
	 sys_untimeout(sys_database1_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
	 //sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

static void lineout_highpass_decrease(sys_state_t *sys)
{
	 if(sys->lineout_highpass == LINEOUT_HIGHPASS_MIN){
		 sys->lineout_highpass = LINEOUT_HIGHPASS_DEFAULT;
	 }else if(sys->lineout_highpass <= LINEOUT_HIGHPASS_NODE1){
		 sys->lineout_highpass--;
	 }else if(sys->lineout_highpass <= LINEOUT_HIGHPASS_NODE2){
		 sys->lineout_highpass -= 5;
	 }else if(sys->lineout_highpass <= LINEOUT_HIGHPASS_MAX){
		 sys->lineout_highpass -= 10;
	 }else if(sys->lineout_highpass == LINEOUT_HIGHPASS_DEFAULT){
		 sys->lineout_highpass = LINEOUT_HIGHPASS_MAX; 
	 }
	 
	 sys_line_highpass_set(sys);
	 
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	// sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void lineout_highpass_increase(sys_state_t *sys)
{
	if(sys->lineout_highpass < LINEOUT_HIGHPASS_NODE1){
      sys->lineout_highpass++;
	 }else if(sys->lineout_highpass < LINEOUT_HIGHPASS_NODE2){
		  sys->lineout_highpass += 5;
	 }else if(sys->lineout_highpass < LINEOUT_HIGHPASS_MAX){
		  sys->lineout_highpass += 10;
	 }else if(sys->lineout_highpass == LINEOUT_HIGHPASS_MAX){
		  sys->lineout_highpass = LINEOUT_HIGHPASS_DEFAULT;
	 }else if(sys->lineout_highpass == LINEOUT_HIGHPASS_DEFAULT){
		  sys->lineout_highpass = LINEOUT_HIGHPASS_MIN;
	 }  
	 
	 sys_line_highpass_set(sys);
	 
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	// sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void delay_switch_to_prev(sys_state_t *sys)
{
	 sys->d_router = (delay_router_t)(sys->d_router == 0? (DELAY_ROUTER_MAX - 1):(sys->d_router - 1));
	
	 sys_delay_switch_to(sys);
	
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void delay_switch_to_next(sys_state_t *sys)
{
	 sys->d_router = (delay_router_t)((sys->d_router + 1) % DELAY_ROUTER_MAX);
	
	 sys_delay_switch_to(sys);
	
	 sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void power_on_method_switch_to_prev(sys_state_t *sys)
{
	sys->method = (power_on_t)(sys->method == 0? (POWER_ON_MAX - 1):(sys->method - 1));
	
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}else if(sys->method == TRIGGER){
		sys->is_trigger_off_need_delay = true;
		update_detect_state(DETECT_TRIGGER);
	}
	
   sys_untimeout(sys_database0_update_later, NULL);
	 sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void power_on_method_switch_to_next(sys_state_t *sys)
{
	sys->method = (power_on_t)((sys->method + 1) % POWER_ON_MAX);
	
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}else if(sys->method == TRIGGER){
		sys->is_trigger_off_need_delay = true;
		update_detect_state(DETECT_TRIGGER);
	}
	
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	
}

static void delay_time_decrease(sys_state_t *sys)
{
	 if(sys->d_router == DELAY_SUB){
      sys->sub[sys->sub_model].delay = sys->sub[sys->sub_model].delay == 0? (SUB_DELAY_MAX):(sys->sub[sys->sub_model].delay - 5);
		  sys_untimeout(sys_database1_update_later, NULL);
	    sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
	 }else if(sys->d_router == DELAY_LINEOUT){
		  sys->line_delay_time = sys->line_delay_time == 0? (SUB_DELAY_MAX):(sys->line_delay_time - 5);
		  sys_untimeout(sys_database0_update_later, NULL);
	    sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
	 }
	 
	 sys_delay_time_set(sys);
	 
	 
}

static void delay_time_increase(sys_state_t *sys)
{
	  if(sys->d_router == DELAY_SUB){
			sys->sub[sys->sub_model].delay = sys->sub[sys->sub_model].delay == SUB_DELAY_MAX ? (0):(sys->sub[sys->sub_model].delay + 5);
			sys_untimeout(sys_database1_update_later, NULL);
	    sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database1_update_later, NULL);
		}else if(sys->d_router == DELAY_LINEOUT){
			sys->line_delay_time = sys->line_delay_time == SUB_DELAY_MAX ? (0):(sys->line_delay_time + 5);
			sys_untimeout(sys_database0_update_later, NULL);
	    sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
		}
		
		sys_delay_time_set(sys);
}

static void change_reset_state(sys_state_t *sys)
{
	 sys->do_reset = sys->do_reset == true? false:true; 
}

static void change_reset_confirm_state(sys_state_t *sys)
{
	 sys->confirm_reset = sys->confirm_reset == true? false:true;
}

static void sense_threshold_prev(sys_state_t *sys)
{
	 sys->sense_thres = (sense_threshold_t)(sys->sense_thres == 0?(SENSE_THRESHOLD_NUM - 1):(sys->sense_thres - 1));
	
	 if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}
	
  sys_audio_sense_thres_set(sys->sense_thres);
	
	sys_untimeout(sys_database0_update_later, NULL);
  sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void sense_threshold_next(sys_state_t *sys)
{
	sys->sense_thres = (sense_threshold_t)((sys->sense_thres + 1) % SENSE_THRESHOLD_NUM);
	
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}
	
	 sys_audio_sense_thres_set(sys->sense_thres);
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void sense_off_prev(sys_state_t *sys)
{
	 sys->se_timeout = (sense_timeout_t)(sys->se_timeout == 0?(SE_TIMEOUT_NUM - 1):(sys->se_timeout - 1));
	
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void sense_off_next(sys_state_t *sys)
{
	 sys->se_timeout = (sense_timeout_t)((sys->se_timeout + 1) % SE_TIMEOUT_NUM);
	
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}
	
	sys_untimeout(sys_database0_update_later, NULL);
	sys_timeout(DATABASE_UPDATE_TIMEOUT, sys_database0_update_later, NULL);
}

static void factory_sub_select_next(sys_state_t *sys)
{
	 sys->temp_sub_model = (sub_model_t)((sys->temp_sub_model + 1) % SUB_MODEL_NUM);  
}

static void factory_sub_select_prev(sys_state_t *sys)
{
	 sys->temp_sub_model = (sub_model_t)(sys->temp_sub_model == 0? (SUB_MODEL_NUM - 1):(sys->temp_sub_model - 1));
}

static void key_left_handler(sys_state_t *sys)
{
	
	CHECK_AND_CLEAR_CODE();
	
	switch(cur_menu->id){
		case VOLUME_MENU:
			volume_decrease_handler(sys);
			break;
		
		case SOURCE_MENU:
			source_prev_handler(sys);
			break;
		
		case PRESET_MENU:
			preset_prev_handler(sys);
			break;
		
		case BRIGHTLESS_MENU:
			bright_prev_handler(sys);
			break;
		
		case IP_ADDRESS_MENU:
		case INSTALL_MENU:
			load_code(sys, '<');
			break;
		
		case SUB_MODEL_MENU:
			sub_model_prev_handler(sys);
			break;
		
		case INPUT_GAIN_MENU:
			input_gain_decrease_handler(sys);
			break;
		
		case PHRASE_MENU:
			phrase_change(sys);
			break;
		
		case SUB_LOWPASS_MENU:
			sub_lowpass_decrease(sys);
			break;
		
		case SUB_HIGHPASS_MENU:
			sub_highpass_decrease(sys);
			break;
		
		case LINEOUT_HIGHPASS_MENU:
			lineout_highpass_decrease(sys);
			break;
		
		case DELAY_ROUTER_MENU:
			delay_switch_to_prev(sys);
			break;
		
		case POWER_ON_METHOD_MENU:
			power_on_method_switch_to_prev(sys);
			break;
		
		case DELAY_TIME_MENU:
			delay_time_decrease(sys);
			break;
		
		case RESET_MENU:
			change_reset_state(sys);
			break;
		
		case RESET_CONFIRM_MENU:
			change_reset_confirm_state(sys);
			break;
		
		case SENSE_THRESHOLD:
			sense_threshold_prev(sys);
			break;
		
		case SENSE_OFF_TIME:
			sense_off_prev(sys);
			break;
		
		case FACTORY_SUB_SELECT_MENU:
			factory_sub_select_prev(sys);
		  break;
		
		default:
			break;
	}
	
  dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void key_right_handler(sys_state_t *sys)
{
  CHECK_AND_CLEAR_CODE();
	
	switch(cur_menu->id){
		case VOLUME_MENU:
			volume_increase_handler(sys);
			break;
		
		case SOURCE_MENU:
			source_next_handler(sys);
			break;
		
		case PRESET_MENU:
			preset_next_handler(sys);
			break;
		
		case BRIGHTLESS_MENU:
			bright_next_handler(sys);
			break;
		
		case IP_ADDRESS_MENU:
		case INSTALL_MENU:
			load_code(sys, '>');
			break;
		
		case SUB_MODEL_MENU:
			sub_model_next_handler(sys);
			break;
		
		case INPUT_GAIN_MENU:
			input_gain_increase_handler(sys);
			break;
		
		case PHRASE_MENU:
			phrase_change(sys);
			break;
		
		case SUB_LOWPASS_MENU:
			sub_lowpass_increase(sys);
			break;
		
		case SUB_HIGHPASS_MENU:
			sub_highpass_increase(sys);
			break;
		
		case LINEOUT_HIGHPASS_MENU:
			lineout_highpass_increase(sys);
			break;
		
		case DELAY_ROUTER_MENU:
			delay_switch_to_next(sys);
			break;
		
		case POWER_ON_METHOD_MENU:
			power_on_method_switch_to_next(sys);
			break;
		
		case DELAY_TIME_MENU:
			delay_time_increase(sys);
			break;
		
		case RESET_MENU:
			change_reset_state(sys);
			break;
		
		case RESET_CONFIRM_MENU:
			change_reset_confirm_state(sys);
			break;
		
		case SENSE_THRESHOLD:
			sense_threshold_next(sys);
			break;
		
		case SENSE_OFF_TIME:
			sense_off_next(sys);
			break;
		
		case FACTORY_SUB_SELECT_MENU:
			factory_sub_select_next(sys);
			break;
		
		default:
			break;
	}
	
  dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void show_version_infor(void)
{
	dis_send_msg(DIS_JUMP, DIS_VERSION_INFOR);
	sys_untimeout(vol_menu_timeout_handler, NULL);
	sys_timeout(VOL_MENU_3S_TIMEOUT, vol_menu_timeout_handler, NULL);
}

void keyboard_msg_handler(sys_state_t *sys, key_value_t key)
{
	if(sys->status != STATUS_WORKING){
		SYS_TRACE("get invalid key %d\r\n", key);
		return;
	}
	
	if(sys->is_dis_timeout == true){
		sys_lcd_bright_change(gSystem_t);
		sys->is_dis_timeout = false;
		sys_untimeout(dis_timeout_handler, NULL);
	  sys_timeout(DIS_TIMEOUT, dis_timeout_handler, NULL);
		return;
	}
	
	if(sys->protect_bitmap != 0){
		SYS_TRACE("in protection\r\n");
		return;
	}
	
	 switch(key){
		 case KEY_UP:
			 key_up_handler(sys);
			 break;
		 
		 case KEY_DOWN:
		   key_down_handler(sys);
			 break;
		 
		 case KEY_LEFT:
		   key_left_handler(sys);
			 break;
		 
		 case KEY_RIGHT:
		   key_right_handler(sys);
			 break;
		 
		 case KEY_ENTER:
		   key_enter_handler(sys);
			 break;
		 
		 case KEY_BACK:
		   key_back_handler(sys);
			 break;
		 
		 case KEY_LEFT_LONG:
			 if(IS_RIGHT_KEY_DOWN){
				 vTaskDelay(3000/portTICK_RATE_MS);
				 if(IS_RIGHT_KEY_DOWN){
					 SYS_TRACE("WILL JUMP TO IP_ADDRESS_MENU");
					 display_menu_jump(IP_ADDRESS_MENU);
					 dis_send_msg(DIS_UPDATE, MENU_NUM);
				 }
			 }else{
					while(IS_LEFT_KEY_DOWN){
						key_left_handler(sys);
						vTaskDelay(100/portTICK_RATE_MS);
				}
			}
			 break;
				
		 case KEY_RIGHT_LONG:
			 if(IS_LEFT_KEY_DOWN){
				 vTaskDelay(3000/portTICK_RATE_MS);
				 if(IS_LEFT_KEY_DOWN){
					 SYS_TRACE("WILL JUMP TO IP_ADDRESS_MENU");
					 display_menu_jump(IP_ADDRESS_MENU);
					 dis_send_msg(DIS_UPDATE, MENU_NUM);
				 }
			 }else{
			    while(IS_RIGHT_KEY_DOWN){
						key_right_handler(sys);
						vTaskDelay(100/portTICK_RATE_MS);
			 }
		 }
			 break;
		 
		 case KEY_DOWN_LONG:
			 if(IS_UP_KEY_DOWN){
				 show_version_infor();
			 }
			 break;
		 
		 default:
			 break;
	 }
	
	sys_untimeout(dis_timeout_handler, NULL);
	 
	sys_timeout(DIS_TIMEOUT, dis_timeout_handler, NULL);
}

static void vol_menu_timeout_handler(void *arg)
{
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

void dis_timeout_handler(void *arg)
{
	if(cur_menu->id < IP_ADDRESS_MENU && cur_menu->id >= VOLUME_MENU){
		gSystem_t->is_dis_timeout = true;
		bsp_lcd_bright_control(0);
		display_menu_jump(VOLUME_MENU);
		dis_send_msg(DIS_UPDATE, MENU_NUM);
	}
}

static void vol_mute_handler(sys_state_t *sys)
{
	sys->is_mute = (sys->is_mute == true? false:true);
	
	sys_master_vol_set(sys);
	
	dis_send_msg(DIS_JUMP, VOLUME_MENU);
	sys_untimeout(vol_menu_timeout_handler, NULL);
	sys_timeout(VOL_MENU_3S_TIMEOUT, vol_menu_timeout_handler, NULL);
}

static void ir_on_handler(sys_state_t *sys)
{ 
	 if(sys->method == IR_IP && (sys->status != STATUS_WORKING && sys->status != STATUS_INITING)){
		 sys_power_on_handler(sys);
	 }
}

static void ir_off_handler(sys_state_t *sys)
{
	 if(sys->method == IR_IP && sys->status != STATUS_STANDBY){
		 sys_power_off_handler(sys);
	 }
}

static void ir_preset_select_handler(sys_state_t *sys, preset_t preset)
{
	 sys->preset = preset;
	
	 display_menu_jump(PRESET_MENU);
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
	 sys_load_peq(sys);
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

static void ir_peq_state_control(sys_state_t *sys, uint8 state)
{
	if(ON == state){
		if(sys->eq_on_off == 0){
			sys->eq_on_off = 1;
			sys_eq_state_set(sys);
		}
	}else{
		if(sys->eq_on_off == 1){
			sys->eq_on_off = 0;
			sys_eq_state_set(sys);
		}
	}
	
	dis_send_msg(DIS_JUMP, DIS_PEQ_STATE);
	sys_untimeout(vol_menu_timeout_handler, NULL);
	sys_timeout(VOL_MENU_3S_TIMEOUT, vol_menu_timeout_handler, NULL);
}

void ir_commond_handler(sys_state_t *sys, ir_commond_t cmd)
{
	if(sys == NULL) return;
	
	if(cmd != IR_OFF && cmd != IR_ON){
		if(sys->is_dis_timeout == true){
			sys_lcd_bright_change(gSystem_t);
			sys->is_dis_timeout = false;
			return;
		}
	
	}
	
	switch(cmd){
		case IR_VOL_UP:
			volume_increase_handler(sys);
		  dis_send_msg(DIS_JUMP, VOLUME_MENU);
		  sys_untimeout(vol_menu_timeout_handler, NULL);
		  sys_timeout(VOL_MENU_3S_TIMEOUT, vol_menu_timeout_handler, NULL);
			break;
		
		case IR_VOL_DOWN:
			volume_decrease_handler(sys);
		  dis_send_msg(DIS_JUMP, VOLUME_MENU);
		  sys_untimeout(vol_menu_timeout_handler, NULL);
		  sys_timeout(VOL_MENU_3S_TIMEOUT, vol_menu_timeout_handler, NULL);
			break;
		
		case IR_UP:
			key_up_handler(sys);
			break;
		
		case IR_LFET:
			key_left_handler(sys);
			break;
		
		case IR_RIGHT:
			key_right_handler(sys);
			break;
		
		case IR_DOWN:
			key_down_handler(sys);
			break;
		
		case IR_BACK:
			key_back_handler(sys);
			break;
		
		case IR_CENTER:
			key_enter_handler(sys);
			break;
		
		case IR_MUTE:
			vol_mute_handler(sys);
			break;
		
		case IR_ON:
			ir_on_handler(sys);
			break;
		
		case IR_OFF:
			ir_off_handler(sys);
			break;
		
		case IR_NORMAL:
		case IR_CINEMA:
		case IR_NIGHT:
		case IR_USER1:
		case IR_USER2:
		case IR_USER3:
			ir_preset_select_handler(sys, (preset_t)(PRE_NORMAL+(cmd - IR_NORMAL)));
			break;
		
		case IR_PQE_ON:
			ir_peq_state_control(sys, ON);
			break;
		
		case IR_PQE_OFF:
			ir_peq_state_control(sys, OFF);
			break;
		
		default:
			break;
	}
	
	if(cmd != IR_OFF){
		sys_untimeout(dis_timeout_handler, NULL);
		sys_timeout(DIS_TIMEOUT, dis_timeout_handler, NULL);
	}
}



void KeyScanLoop(void)
{
	 unsigned char index = 0;
	 unsigned int temp;
	
   for(index = 0; index < NUM_OF_KEY; index++)
	{
		 if(keylist[index].pkeyread(keylist[index].port, keylist[index].pin) == 1)
		 {
			  if((keylist[index].key_locked_flag == false) || (keylist[index].repeat_flag == true))
				{
					    if(keylist[index].key_locked_flag)
							{
								  temp = 200;
							}
							else
							{
								   temp = keylist[index].longpresstime;
							}
							
							if(xTaskGetTickCountFromISR() - keylist[index].key_timer > temp)
							{
								  if(index == 0){
								     sys_send_msg_isr(POWER_KEY_MSG, (uint8)keylist[index].longpress);
									}else{
										 sys_send_msg_isr(KEYBOARD_MSG, (uint8)keylist[index].longpress);
									}
								  keylist[index].key_locked_flag = true;
								  keylist[index].key_timer = xTaskGetTickCountFromISR();
							}
				}
				else if(keylist[index].key_locked_flag == true)
				{
					  keylist[index].key_timer = xTaskGetTickCountFromISR();
				}
		 }
		 else 
		 {
			    if(xTaskGetTickCountFromISR() - keylist[index].key_timer > 80)
					{
						  if(index == 0)
                   sys_send_msg_isr(POWER_KEY_MSG, (uint8)keylist[0].shortpress);   	
              else							
						       sys_send_msg_isr(KEYBOARD_MSG, (uint8)keylist[index].shortpress);
					}
					
					keylist[index].key_locked_flag = false;
					keylist[index].key_timer = xTaskGetTickCountFromISR();
		 }
		 
	 }
}



