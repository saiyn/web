#include "common_head.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSAL_Console.h"
#include "serial.h"
#include "System_Task.h"
#include "keyboard.h"
#include "Display_Task.h"
#include "Lcd_driver.h"
#include "EPRom_driver.h"
#include "OSAL_Utility.h"
#include "OSAL_RingBuf.h"
#include "IR_driver.h"
#include "keyboard.h"
#include "lwiplib.h"
#include "cs4251x.h"
#include "cs485xx.h"
#include "njw1195_driver.h"
#include "OSAL_Detect.h"
#include "dsp_app.h"

xQueueHandle gSysQueue;

sys_state_t gSystem_last;
sys_state_t *gSystem_t;

ir_commond_t gIrOutputCmd;

b_eq_t gEqTemp ={
	100,
	10,
	0,
  0x7ed21bc1,
  0x7ffd2e94,
	0x20000000
};

const static uint32 sub_eq_array_a[SUB_MODEL_NUM][8] = {
	{0x7fec5f39, 0x7f95a294, 0x7d112fd3, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
  {0x7fec5f39, 0x7f8f57f7, 0x7fa2595b, 0x7e2d2780, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
  {0x7eb19a41, 0x7f1de69a, 0x7fe82d7b, 0x7f98b71b, 0x7f6c9c0a, 0x7ed59d55, 0x7f6af038, 0x7f6af038},
	{0x7e889c1b, 0x7fed6345, 0x7fe2046c, 0x7dd7c2ca, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038}, 
	{0x7f53c579, 0x7fd334c5, 0x7fe2046c, 0x7dd7c2ca, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
	{0x7f53c579, 0x7fd334c5, 0x7fe2046c, 0x7dd7c2ca, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
  {0x7fd334c5, 0x7f5aa715, 0x7eaec8d5, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
	{0x7fdaad60, 0x7f62174c, 0x7fb8dc55, 0x7f6af038, 0x7fbb4916, 0x7f79a6b5, 0x7fd9fd36, 0x7f6af038},//
	{0x7fdaad60, 0x7f62174c, 0x7fc1e796, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038},
	{0x7fdaad60, 0x7f606d7a, 0x7fa0e842, 0x7efc9363, 0x7f9e6ab9, 0x7ec258d5, 0x7f581062, 0x7f6af038},
  {0x7ff24d8f, 0x7fb2edfe, 0x7eaa2a0a, 0x7e56605e, 0x7feebb34, 0x7f6af038, 0x7f6af038, 0x7f6af038},
  {0x7ff41aef, 0x7fb2edfe, 0x7f15379f, 0x7f6af038, 0x7fdf2239, 0x7f9df548, 0x7da860dc, 0x7be236c1},
  {0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038, 0x7f6af038}
};

const static uint32 sub_eq_array_b[SUB_MODEL_NUM][8] = {
  {0x7fffb480, 0x7ffefbf4, 0x7ffb9bed, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94}, 
  {0x7fffb480, 0x7ffefbf4, 0x7ffe6d58, 0x7fee7818, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94},
  {0x7ffdf7e8, 0x7ff12755, 0x7fffb480, 0x7fff25e5, 0x7ffe9fad, 0x7ff1c758, 0x7ffd2e94, 0x7ffd2e94},
	{0x7ffe9fad, 0x7fffd60e, 0x7fffd60e, 0x7ffb3fa6, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94}, 
  {0x7ffed202, 0x7fffbce4, 0x7fffd60e, 0x7ffb3fa6, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94},
	{0x7ffed202, 0x7fffbce4, 0x7fffd60e, 0x7ffb3fa6, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94},
	{0x7fffbce4, 0x7ffe9fad, 0x7ffbf833, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94}, 
  {0x7fffd60e, 0x7ffe6d58, 0x7fff7164, 0x7ffd2e94, 0x7ffbf833, 0x7ffefbf4, 0x7ffff79c, 0x7ffd2e94},//
	{0x7fffd60e, 0x7ffe6d58, 0x7fffac1d, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94},
	{0x7fffd60e, 0x7ffdf7e8, 0x7ff8d2e5, 0x7ff266ba, 0x7fff0cba, 0x7ffe32a0, 0x7ffbf833, 0x7ffd2e94},
  {0x7fffd60e, 0x7fff25e5, 0x7fff25e5, 0x7ffd2e94, 0x7fffc547, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94},
  {0x7fffe6d5, 0x7fff25e5, 0x7fff25e5, 0x7ffd2e94, 0x7fffde72, 0x7fff92f2, 0x7ffd2e94, 0x7fee7818},
  {0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94, 0x7ffd2e94}
};


const static uint32 sub_eq_array_g[SUB_MODEL_NUM][8] = {
	{0x38e7a95c, 0x1aecb535, 0x1430ccbc, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000}, 
  {0x3fd92fb1, 0x196b228d, 0x196b228d, 0x196b228d, 0x20000000, 0x20000000, 0x20000000, 0x20000000},
  {0x0809bd83, 0x0809bd83, 0x2d3382e4, 0x196b228d, 0x16a77e5e, 0x0e4b3c3e, 0x20000000, 0x20000000},
	{0x11feb28d, 0x32b77186, 0x196b228d, 0x16a77e5e, 0x20000000, 0x20000000, 0x20000000, 0x20000000}, 
  {0x11feb28d, 0x47a39a38, 0x196b228d, 0x16a77e5e, 0x20000000, 0x20000000, 0x20000000, 0x20000000},
  {0x11feb28d, 0x47a39a38, 0x196b228d, 0x16a77e5e, 0x20000000, 0x20000000, 0x20000000, 0x20000000},
	{0x38e7a95c, 0x1430ccbc, 0x16a77e5e, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000},
	{0x38e7a95c, 0x16a77e5e, 0x1e35bf6a, 0x20000000, 0x196b228d, 0x1c8520d1, 0x1430ccbc, 0x20000000}, //
  {0x38e7a95c, 0x16a77e5e, 0x1aecb535, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000},
	{0x38e7a95c, 0x0a1e8a2e, 0x38e7a95c, 0x0809bd83, 0x16a77e5e, 0x1009b951, 0x16a77e5e, 0x20000000},
  {0x38e7a95c, 0x16a77e5e, 0x16a77e5e, 0x16a77e5e, 0x2d3382e4, 0x20000000, 0x20000000, 0x20000000},
  {0x43a1b3ae, 0x16a77e5e, 0x16a77e5e, 0x20000000, 0x28491d14, 0x196b228d, 0x1009b951, 0x16a77e5e},
	{0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000, 0x20000000}
};

const static uint8 sub_highpass_array[SUB_MODEL_NUM] ={
33, 33, 30, 33, 27, 27, 24, 20, 20, 20, 23, 20, SUB_HIGHPASS_OFF
};


const static char * const protect_name[]={
	"AUDIO",
	"TRIGGER",
	"OVER_TEMP",
	"OVER_VOLTAGE_AND_SHORT_CIRCIUT",
	"OVER_CLIP",
	"DC_ERROR",
	"OVER_CURRENT",
	"15V"
};

static void sys_database_save(sys_state_t *sys, uint8 type);
static int sys_audio_config_load(void);
static void sys_bled_flash(void *arg);
static void power_on_timeout(void *arg);

typedef int (*pInitFunc)(void);

static pInitFunc sys_init_array[] ={
	  dsp_boot_init,
	  sys_audio_config_load,
};	

#define SYS_INIT_FUNC_NUM (sizeof(sys_init_array)/sizeof(sys_init_array[0]))

static void power_on_routine(void)
{
	uint8 j;
	
  bsp_delay_ms(3000);
	
	for(j = 0; j < SYS_INIT_FUNC_NUM; j++){
		sys_init_array[j]();
	}
	
//	/*init PCM1804 must after DSP boot*/
//	PCM1804_RST(0);
//	bsp_delay_ms(100);
//	PCM1804_RST(1);
//	
//	RCA_OUT_DSP();
//	BAN_OUT_DSP();
}

static void sys_update_protect_state(sys_state_t *sys)
{
	uint8 index;
	
	for(index = DETECT_OVER_TEMP; index < NUM_OF_DETECT; index++){
		if(sys->protect_bitmap & (1 << index)){
			update_detect_state((detect_event_t)index);
		}
	}
}

static void fix_hardware_bug(void *arg)
{
	 SYS_TRACE("in fix_hardware_bug\r\n");
	/*hardwrae will be in protection mode when power on*/
//	if(gSystem_t->protect_bitmap & ((1 << DETECT_OVER_TEMP) | (1 << DETECT_OVER_VOLTAGE_OR_SHORT) 
//		| (1 << DETECT_OVER_CURRENT) | (1 << DETECT_OVER_DC_ERROR))){
	if(IS_OVER_CURRENT | IS_OVER_VOLTAGE | IS_OVER_TEMP | IS_DC_ERROR){
			
			sys_timeout(500, fix_hardware_bug, NULL);	
		}else{
			
			sys_untimeout(power_on_timeout, NULL);
			/*init PCM1804 must after DSP boot*/
			PCM1804_RST(0);
			bsp_delay_ms(100);
			PCM1804_RST(1);
			
			AMP_ENABLE();
			
			bsp_delay_ms(500);
			AMP_MUTE_OFF();
			
			bsp_delay_ms(100);
			RCA_OUT_DSP();
			BAN_OUT_DSP();
			RCA_OUT_MUTE_OFF();
			BAN_OUT_MUTE_OFF();
			OUTPUT_M_MUTE_OFF();
			
			SYS_TRACE("out of fix_hardware_bug\r\n");
			gSystem_t->is_out_of_fix_hardware = true;
			
			/*update protection event detect*/
	    update_all_protect_state();
	    gSystem_t->protect_bitmap = 0;
	    gSystem_t->is_in_protection = false;
	    /*when set the new state, the protection detection will be on the run*/
	    gSystem_t->status = STATUS_WORKING;   
			dis_send_msg(DIS_WELCOME, MENU_NUM);
		}
}

static void fix_power_bug(void *arg)
{
	
  if(IS_POWER_READY){
		SYS_TRACE("power ready now\r\n");
		bsp_delay_ms(100);
		CS4251X_Init();
		bsp_delay_ms(5);
		sys_input_init();
		
		bsp_delay_ms(100);
		sys_master_vol_set(gSystem_t);
		bsp_delay_ms(100);
		/*since the hardwrare will be in protection mode, so unmute the AMP someime later*/
		sys_untimeout(fix_hardware_bug, NULL);
		sys_timeout(500, fix_hardware_bug, NULL);	
		
	}else{
		SYS_TRACE("wait for power ready...\r\n");
		sys_timeout(500, fix_power_bug, NULL);	
	}
		

}

static void power_on_timeout(void *arg)
{
	if(IS_OVER_CURRENT){
		dis_send_msg(DIS_JUMP, DIS_OVER_CURRENT);
	}else if(IS_OVER_VOLTAGE){
		dis_send_msg(DIS_JUMP, DIS_OVER_VOLTAGE_OR_SHORT);
	}else if(IS_OVER_TEMP){
		dis_send_msg(DIS_JUMP, DIS_OVER_TEMP);
	}else if(IS_DC_ERROR){
		dis_send_msg(DIS_JUMP, DIS_OVER_DC_ERROR);
	}
	
}

static void fix_power_off_bug(void *arg)
{
	SYS_TRACE("in fix_power_off_bug\r\n");
	
	OUTPUT_M_MUTE_OFF();
}

void sys_power_on_handler(sys_state_t *sys)
{
	sys_untimeout(fix_power_off_bug, NULL);
	
	/*first do hardware power on*/
	AC_STANDBY(1);
	DC5V_ON();
	BLED_SET(1);
	
	/*add for fix repeat power on bug from v0.4.3*/
	sys->status = STATUS_INITING;
	
	OUTPUT_M_MUTE_ON();
	
	sys_untimeout(sys_bled_flash, NULL);
	
	bsp_delay_ms(100);
	lcd_init();

  sys_lcd_bright_change(sys);
	sys->is_dis_timeout = false;
	//OUTPUT_M_MUTE_OFF();
	/*do system power on routine*/
	power_on_routine();
		
	if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	}else if(sys->method == TRIGGER){
		update_detect_state(DETECT_TRIGGER);
	}
	
//	/*update protection event detect*/
//	update_all_protect_state();
//	sys->protect_bitmap = 0;
//	sys->is_in_protection = false;
//	/*when set the new state, the protection detection will be on the run*/
//	sys->status = STATUS_WORKING;
	sys->is_out_of_fix_hardware = false;

	sys_untimeout(power_on_timeout, NULL);
	sys_timeout(1000*30, power_on_timeout, NULL);
	
	sys_untimeout(fix_power_bug, NULL);
  sys_timeout(500, fix_power_bug, NULL);	
	
}



void sys_power_off_handler(sys_state_t *sys)
{
	bsp_lcd_bright_control(0);
	OUTPUT_M_MUTE_ON();
	
  RCA_OUT_BYPASS();
	BAN_OUT_BYPASS();
	RCA_OUT_MUTE_ON();
	BAN_OUT_MUTE_ON();
	AMP_MUTE_ON();
 
	bsp_delay_ms(100);
	
	AMP_DISENABLE();
	
	njw1195_channel_mute();
	bsp_delay_ms(500);
	DC5V_OFF();
	AC_STANDBY(0);
	RLED_SET(1);

	bsp_delay_ms(500);
	gSystem_t->is_trigger_off_need_delay = false;
	sys_untimeout(dis_timeout_handler, NULL);
	
	sys->status = STATUS_STANDBY;
	
	sys_timeout(1000*10, fix_power_off_bug, NULL);	
}

static void sys_shutdown_handler(sys_state_t *sys)
{
	if(sys->status == STATUS_SHUTDOWN) return;
	
	SYS_TRACE("will shut down\r\n");
	
	njw1195_channel_mute();
	RCA_OUT_BYPASS();
	BAN_OUT_BYPASS();
	eeprom_write(SYS_LOW_POWER_FLAG_ADDRESS, (uint8 *)"LOWPOWER", strlen("LOWPOWER"));
	bsp_delay_ms(100);

	sys->status = STATUS_SHUTDOWN;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
	WatchdogReloadSet(WATCHDOG0_BASE, 100);
	WatchdogResetEnable(WATCHDOG0_BASE);
	WatchdogEnable(WATCHDOG0_BASE);
}

static void do_wake_up(void *arg)
{
	  SYS_TRACE("do_wake_up\r\n");
	  /*first disable power key intrrupt*/
	  IntDisable(INT_KEY_POWER);
		sys_power_on_handler(gSystem_t);
}

void sys_wake_up_handler(void)
{
	uint32 status;
	
	status = GPIOIntStatus(POWER_KEY_PORT, 1);
	GPIOIntClear(POWER_KEY_PORT, status);
	

	sys_untimeout(do_wake_up, NULL);
  sys_timeout(500, do_wake_up, NULL);	
	
}

static void sys_bled_flash(void *arg)
{
	static uint8 j = 1;
	
	if(j){
		LED_OFF();
		j = 0;
	}else{
		BLED_SET(1);
		j = 1;
	}
	
	if(gSystem_t->status == STATUS_STANDBY){
		sys_timeout(500, sys_bled_flash, NULL);
	}else if(gSystem_t->status == STATUS_WORKING || gSystem_t->status == STATUS_INITING){
		BLED_SET(1);
	}
}

static void sys_Rled_flash(void *arg)
{
	static uint8 j = 1;
	
	if(j){
		LED_OFF();
		j = 0;
	}else{
		RLED_SET(1);
		j = 1;
	}
	
	if(gSystem_t->is_in_protection == true && (gSystem_t->status == STATUS_WORKING || gSystem_t->status == STATUS_INITING)){
		sys_timeout(500, sys_Rled_flash, NULL);
	}else{
		if(gSystem_t->status == STATUS_WORKING || gSystem_t->status == STATUS_INITING){
			BLED_SET(1);
		}else if(gSystem_t->status == STATUS_STANDBY){
			RLED_SET(1);
		}else if(gSystem_t->status == STATUS_SHUTDOWN){
			LED_OFF();
		}
	}
}


static void power_key_msg_handler(sys_state_t *sys, key_value_t key)
{
	SYS_TRACE("get power key down msg, state=%d\r\n", sys->status);
	
	if(KEY_POWER == key){
		if(sys->status != STATUS_WORKING && sys->status != STATUS_INITING){
			if(sys->method == AUDIO || sys->method == TRIGGER){
				sys->is_trigger_off_need_delay = true;
			}
			sys_power_on_handler(sys);
		}else{
			if(sys->method == AUDIO || sys->method == TRIGGER){
				sys->is_auto_power_on_enable = false;
				sys_timeout(500, sys_bled_flash, NULL);
			}
			sys_power_off_handler(sys);
		}
	}else if(KEY_POWER_LONG == key){
		sys_shutdown_handler(sys);
	}
}

static void sys_code_pass_handler(sys_state_t *sys)
{
	 if(cur_menu->child){
		cur_menu = cur_menu->child;
	 }
	 
	 dis_send_msg(DIS_UPDATE, MENU_NUM);
}

static void sys_msg_handler(sys_msg_t *msg)
{
  OSAL_ASSERT(msg != NULL);
	
	switch(msg->msg){
		case KEYBOARD_MSG:
			keyboard_msg_handler(gSystem_t, (key_value_t)msg->data);
		break;
		
		case POWER_KEY_MSG:
			power_key_msg_handler(gSystem_t, (key_value_t)msg->data);
			break;
		
		case DATABASE_UPDATE_MSG:
			sys_database_save(gSystem_t, msg->data);
			break;
		
		case CODE_PASS_MSG:
			sys_code_pass_handler(gSystem_t);
			break;
		
		default:
			break;
		
	}
}

static void sys_nv_set_default(sys_state_t *sys, uint8 type)
{
  uint8 i,j; 
	
	if(type == 0){
	 sys->ip = (uint32)((192 << 24) | (168 << 16) | (1 << 8) | (222 << 0));
	 sys->netmask = (uint32)((255 << 24) | (255 << 16) | (255 << 8) | (0 << 0));
	 sys->gateway = (uint32)((192 << 24) | (168 << 16) | (1 << 8) | (1 << 0));
	 sys->ip_mode = 1;//IPADDR_USE_DHCP
	 sys->method = POWER_ON_METHOD_DEFAULT; 
	 sys->bright = LCD_BRIGHTNESS_DEFAULT;
	 sys->input_gain = 0;
	 sys->lineout_highpass = LINEOUT_HIGHPASS_DEFAULT;
	 sys->lineout_highpass_slope = 1;
	 sys->line_delay_time = 0;
	 sys->master_vol = 0;
	 sys->src = DEFAULTT_SOURCE;
	 sys->d_router = DSP_DELAY_DEFAULT_ROUTER;
	 sys->preset = DSP_DEFAULT_PRESET;
	 sys->sub_model = SUB_DEFAULT_MODE;
	 sys->sense_thres = SENSE_LEVEL_DEFAULT;
	 sys->se_timeout = SE_TIMEOUT_20;
	 sys->eq_on_off = 0;
	 sys->is_out_of_fix_hardware = false;
	}else if(type == 1){
	
	 for(j = 0; j < SUB_MODEL_NUM; j++){
		 sys->sub[j].delay = 0;
		 sys->sub[j].highpass = sub_highpass_array[j];
		 sys->sub[j].lowpass = SUB_LOWPASS_DEFAULT;
		 sys->sub[j].lowpass_slope = 1;
		 sys->sub[j].highpass_slope = 1;
		 sys->sub[j].phrase = 0;	 
	 }
	 
 }else if(type == 2){
	 for(j = 0; j < PRESET_NUM; j++){
		 if(j == PRE_CINEMA){
			 //for(i = 0; i < PRESET_EQ_FILTER_NUM; i++){
				sys->eq->peq[j].filter[0].freq = 35;
				sys->eq->peq[j].filter[0].gain = 4;
				sys->eq->peq[j].filter[0].Q = 13;
				sys->eq->peq[j].filter[0].a = 0x7faf1886;
				sys->eq->peq[j].filter[0].b = 0x7fffac1d;
				sys->eq->peq[j].filter[0].g = 0x32b77186;
			 
			  sys->eq->peq[j].filter[0].freq = 100;
				sys->eq->peq[j].filter[0].gain = 3;
				sys->eq->peq[j].filter[0].Q = 0;
				sys->eq->peq[j].filter[0].a = 0x7ed21bc1;
				sys->eq->peq[j].filter[0].b = 0x7ffd2e94;
				sys->eq->peq[j].filter[0].g = 0x20000000;
			//}
			 
		 }else if(j == PRE_NIGHT){
			 //for(i = 0; i < PRESET_EQ_FILTER_NUM; i++){
				sys->eq->peq[j].filter[0].freq = 30;
				sys->eq->peq[j].filter[0].gain = -6;
				sys->eq->peq[j].filter[0].Q = 40;
				sys->eq->peq[j].filter[0].a = 0x7faf1886;
				sys->eq->peq[j].filter[0].b = 0x7fffbce4;
				sys->eq->peq[j].filter[0].g = 0x1009b951;
			// }
			 
			  sys->eq->peq[j].filter[0].freq = 100;
				sys->eq->peq[j].filter[0].gain = 3;
				sys->eq->peq[j].filter[0].Q = 0;
				sys->eq->peq[j].filter[0].a = 0x7ed21bc1;
				sys->eq->peq[j].filter[0].b = 0x7ffd2e94;
				sys->eq->peq[j].filter[0].g = 0x20000000;
	 
		 }else{
			for(i = 0; i < PRESET_EQ_FILTER_NUM; i++){
			 
				sys->eq->peq[j].filter[i].freq = EQ_FREQ_DEFAULT;
				sys->eq->peq[j].filter[i].gain = 0;
				sys->eq->peq[j].filter[i].Q = EQ_Q_MIN;
				sys->eq->peq[j].filter[i].a = 0x7ed21bc1;
				sys->eq->peq[j].filter[i].b = 0x7ffd2e94;
				sys->eq->peq[j].filter[i].g = 0x20000000;
			}
		 
			sys->eq->peq[j].vol = 0;
		 
	   }
	 }
	 
	 for(i = 0; i < ROOM_EQ_FILTER_NUM; i++){
		  sys->eq->req.filter[i].freq = EQ_FREQ_DEFAULT;
		  sys->eq->req.filter[i].gain = 0;
			sys->eq->req.filter[i].Q = EQ_Q_MIN;
			sys->eq->req.filter[i].a = 0x7ed21bc1;
			sys->eq->req.filter[i].b = 0x7ffd2e94;
			sys->eq->req.filter[i].g = 0x20000000;
	 }

 }
 
	 sys_database_save(sys, type);

}


void sys_ip_set_to_default(sys_state_t *sys)
{
	uint32 ip = (uint32)((192 << 24) | (168 << 16) | (1 << 8) | (222 << 0));
	uint32 netmask = (uint32)((255 << 24) | (255 << 16) | (255 << 8) | (0 << 0));
	uint32 gateway = (uint32)((192 << 24) | (168 << 16) | (1 << 8) | (1 << 0));
	
	lwIPNetworkConfigChange(ip, netmask, gateway, 0);
	
	sys->ip_mode = 0;
	sys->ip = ip;
	sys->netmask = netmask;
	sys->gateway = gateway;
	
	dis_send_msg(DIS_UPDATE, MENU_NUM);
	
	sys_send_msg(DATABASE_UPDATE_MSG, 0);
}


static void sys_print_sys_nv(sys_state_t *sys)
{
	 SYS_TRACE("*******************************");
	 SYS_TRACE("method = %d\r\n", sys->method);
	 SYS_TRACE("bright = %d\r\n", sys->bright);
	 SYS_TRACE("vol = %d\r\n", sys->master_vol);
}

static void sys_runtime_parameter_init(sys_state_t *sys)
{
	 sys->cur_room_eq = PRE_NORMAL;
	 sys->room_eq_index = 0;
	 sys->cur_cust_eq = PRE_USER1;
	 sys->temp_sub_model = SUB_DEFAULT_MODE;
	 sys->is_trigger_off_need_delay = false;
	 sys->is_auto_power_on_enable = true;
	 sys->confirm_reset = false;
	 sys->do_reset = false;
	 sys->is_in_protection = false;
	 sys->is_short_circuit = false;
	 sys->protect_bitmap = 0;
	 sys->is_dis_timeout = false;
}

void sys_factory_reset(sys_state_t *sys)
{
	 SYS_TRACE("will do factory reset\r\n");
	
	 sys_nv_set_default(sys, 0);
	 sys_nv_set_default(sys, 1);
	 sys_nv_set_default(sys, 2);
	 sys_runtime_parameter_init(sys);
	
	 lwIPNetworkConfigChange(0, 0, 0, 1);
	
	 sys_lcd_bright_change(sys);
	 njw1195_input_select(NJW_RCA);
	
	 sys_sub_preset_load(gSystem_t);
	 sys_master_vol_set(gSystem_t);
	 sys_load_peq(gSystem_t);
	 sys_eq_state_set(gSystem_t);
	 sys_line_highpass_set(gSystem_t);
	
	 if(sys->method == AUDIO){
		update_detect_state(DETECT_AUDIO);
	 }else if(sys->method == TRIGGER){
		update_detect_state(DETECT_TRIGGER);
	 }
	
	 sys->is_in_factory = 1;
	 display_menu_jump(WELCOM_MENU);
	 dis_send_msg(DIS_WELCOME, MENU_NUM);
}

void sys_database_init(void)
{
	int retval;
	uint32 crc = 0;
	uint8 *p = pvPortMalloc(sizeof(sys_state_t));
	OSAL_ASSERT(p != NULL);
	memset(p, 0, sizeof(sys_state_t));
	
  SYS_TRACE("will load %d bytes sys nv data from epprom\r\n", SIZEOF_SYS_NV);
	
	vPortEnterCritical();
	retval = eeprom_read(SYS_NV_ADDRESS, p, SIZEOF_SYS_NV);
	vPortExitCritical();
	OSAL_ASSERT(retval > 0);
	
	crc = calc_crc32(crc, p, SIZEOF_SYS_NV - 12);
	
	gSystem_t = (sys_state_t *)p;
	
	if(crc != gSystem_t->sys_crc32){
		SYS_TRACE("will set sys nv to default crc=%x, sys_crc=%x\r\n", crc, gSystem_t->sys_crc32);
		sys_nv_set_default(gSystem_t, 0);
		gSystem_t->is_in_factory = 1;
	}else{
		SYS_TRACE("sys nv data load succuess\r\n");
	}
/***************sub nv data*********************/	
  crc = 0;
	p = pvPortMalloc(SIZEOF_SUB_NV);
	OSAL_ASSERT(p != NULL);
	memset(p, 0, SIZEOF_SUB_NV);
	
	SYS_TRACE("will load %d bytes sub nv data from epprom\r\n", SIZEOF_SUB_NV);
	
	vPortEnterCritical();
	retval = eeprom_read(SYS_NV_ADDRESS_SUB, p, SIZEOF_SUB_NV);
	vPortExitCritical();
	OSAL_ASSERT(retval > 0);
	
	crc = calc_crc32(crc, p, SIZEOF_SUB_NV);
	
	gSystem_t->sub = (sub_state_t *)p;
	
	if(crc != gSystem_t->sub_crc32){
		SYS_TRACE("will set sub nv to default crc=%x, sys_crc=%x\r\n", crc, gSystem_t->sub_crc32);
		sys_nv_set_default(gSystem_t, 1);
		gSystem_t->is_in_factory = 1;
	}else{
		SYS_TRACE("sub nv data load succuess\r\n");
	}
	
/***************EQ nv data*************************************/
	crc = 0;
	p = pvPortMalloc(SIZEOF_EQ_NV);
	OSAL_ASSERT(p != NULL);
	memset(p, 0, SIZEOF_EQ_NV);
	
	SYS_TRACE("will load %d bytes eq nv data from epprom\r\n", SIZEOF_EQ_NV);
	
	vPortEnterCritical();
	retval = eeprom_read(SYS_NV_ADDRESS_EQ, p, SIZEOF_EQ_NV);
	vPortExitCritical();
	OSAL_ASSERT(retval > 0);
	
	crc = calc_crc32(crc, p, SIZEOF_EQ_NV);
	
	gSystem_t->eq = (EQ_t *)p;
	
	if(crc != gSystem_t->eq_crc32){
		SYS_TRACE("will set eq nv to default crc=%x, sys_crc=%x\r\n", crc, gSystem_t->eq_crc32);
		sys_nv_set_default(gSystem_t, 2);
		gSystem_t->is_in_factory = 1;
	}else{
		SYS_TRACE("eq nv data load succuess\r\n");
	}
	
	
	/*init sys runtime parameter*/
	sys_runtime_parameter_init(gSystem_t);
	
	sys_audio_sense_thres_set(gSystem_t->sense_thres);

  memcpy(&gSystem_last, gSystem_t, sizeof(sys_state_t));
}

static void sys_database_save(sys_state_t *sys, uint8 type)
{
	 uint32 crc = 0;
	 int retval;
	
	 if(type == 1){
		 crc = calc_crc32(crc, sys->sub, SIZEOF_SUB_NV);
		 
		 SYS_TRACE("will save sub crc=%x\r\n", crc);	
		 
		 sys->sub_crc32 = crc;
		 
		 vPortEnterCritical();
	   retval = eeprom_write(SYS_NV_ADDRESS_SUB, (uint8 *)sys->sub, SIZEOF_SUB_NV);
	   vPortExitCritical();
	   OSAL_ASSERT(retval > 0);
	 }else if(type == 2){
		 crc = calc_crc32(crc, sys->eq, SIZEOF_EQ_NV);
		 
		 SYS_TRACE("will save sub crc=%x\r\n", crc);	
		 
		 sys->eq_crc32 = crc;
		 
		 vPortEnterCritical();
	   retval = eeprom_write(SYS_NV_ADDRESS_EQ, (uint8 *)sys->eq, SIZEOF_EQ_NV);
	   vPortExitCritical();
	   OSAL_ASSERT(retval > 0);
	 }

	 crc = 0;
	 
	 crc = calc_crc32(crc, sys, SIZEOF_SYS_NV - 12);
	
   SYS_TRACE("will save sys crc=%x\r\n", crc);	
	  
	 sys->sys_crc32 = crc;
	
	 vPortEnterCritical();
	 retval = eeprom_write(SYS_NV_ADDRESS, (uint8 *)sys, SIZEOF_SYS_NV);
	 vPortExitCritical();
	 OSAL_ASSERT(retval > 0);
}



void sys_lcd_bright_change(sys_state_t *cur)
{
	if(cur->bright == BRI_LOW){
		bsp_lcd_bright_control(2);
	}else if(cur->bright == BRI_MEDIUM){
		bsp_lcd_bright_control(30);
	}else if(cur->bright == BRI_HIGH){
		bsp_lcd_bright_control(95);
	}
}


static void sys_poll_handler(void)
{
   ir_commond_t cmd;
	
	 if(ir_get_commond(IR_FRONT,&cmd)){
		 SYS_TRACE("get front ir commond[%2x]\r\n", cmd);
		 ir_commond_handler(gSystem_t, cmd);
//		 vPortEnterCritical();
//		 IrOutHandler(ir_commond[cmd]);
//		 vPortExitCritical();
	 }
	 
	 if(ir_get_commond(IR_BACK_IN,&cmd)){
		 SYS_TRACE("get IR_BACK_IN ir commond[%2x]\r\n", cmd);
		 ir_commond_handler(gSystem_t, cmd);
//		 vPortEnterCritical();
//		 IrOutHandler(ir_commond[cmd]);
//		 vPortExitCritical();
	 }
	 
	 DetectGpioPoll();
}

void system_task(void *parameter)
{
	sys_msg_t msg;
	gSysQueue = xQueueCreate(20, sizeof(sys_msg_t));
	
	OSAL_ASSERT(gSysQueue != NULL);
	
	sys_database_init();
	SYS_TRACE("sip=%x, smask=%x, sgate=%x\r\n", gSystem_t->ip, gSystem_t->netmask, gSystem_t->gateway);
	
	
	while(1){
		static uint8 is_first = 1;
		
		if(is_first){
	   	vTaskDelay(1000/portTICK_RATE_MS);
			is_first = 0;
		}else{
			vTaskDelay(5/portTICK_RATE_MS);
		}
		
		if(xQueueReceive(gSysQueue, &msg, 0)){
			sys_msg_handler(&msg);
		}
		
		sys_poll_handler();

	}

}


void sys_send_msg_isr(sys_msg_type_t type, uint8 data)
{
	 sys_msg_t msg;

   msg.msg = type;
	 msg.data = data;
	
	 xQueueSendToFrontFromISR(gSysQueue, &msg, NULL);
}

void sys_send_msg(sys_msg_type_t type, uint8 data)
{
	 sys_msg_t msg;

   msg.msg = type;
	 msg.data = data;  
	
	 xQueueSend(gSysQueue, &msg, portMAX_DELAY);
}

static int sys_audio_config_load(void)
{
//	int j;
//	int vol = gSystem_t->master_vol;
	
  sys_sub_preset_load(gSystem_t);
	sys_eq_state_set(gSystem_t);
	sys_load_peq(gSystem_t);
	sys_line_highpass_set(gSystem_t);
	
//	for(j = -40; j <= vol; j++){
//		gSystem_t->master_vol = j;
//		sys_master_vol_set(gSystem_t);
//	}
	
	
//	AMP_ENABLE(); 
//	AMP_MUTE_OFF();
	return 0;
}

int sys_input_init(void)
{
	njw1194_channel_t cha;
	uint8 vol;
	
	bsp_delay_ms(3000);
	
	if(gSystem_t->src == BALANCE){
		cha = NJW_BALANCE;
	}else if(gSystem_t->src == RCA){
		cha = NJW_RCA;
	}else if(gSystem_t->src == SPEAHER){
		cha = NJW_SPEAK;
	}
	
	njw1195_input_select(cha);
	
	vol = NJW1195_0DB_GAIN - (VOLUME_GAIN_IN_1195_DEFAULT * 2);
	njw1195_volume_set(vol, SUB_CHANNEL);
	njw1195_volume_set(NJW1195_4_5DB_GAIN, LINEOUT_CHANNEL);
	
	SYS_TRACE("sys_input_init done[%d]\r\n", cha);
	
	return 0;
}

void sys_input_select(src_t src)
{
	njw1194_channel_t cha;
	int vol = gSystem_t->master_vol;
	int j;
	
  if(src == BALANCE){
		cha = NJW_BALANCE;
	}else if(src == RCA){
		cha = NJW_RCA;
	}else if(src == SPEAHER){
		cha = NJW_SPEAK;
	}
	
  njw1195_volume_set(0xff, LINEOUT_CHANNEL);
	bsp_delay_ms(100);
	njw1195_input_select(cha);
	bsp_delay_ms(100);
  njw1195_volume_set(NJW1195_4_5DB_GAIN, LINEOUT_CHANNEL);
}


static void audio_invalid_timeout(void *arg)
{
	 uint32 timeout;
	 sys_state_t *sys = (sys_state_t *)arg;
	
	 if(sys->method != AUDIO) return;
	
	 if(cur_menu->id <= RESET_CONFIRM_MENU && cur_menu->id >= IP_ADDRESS_MENU){
		 SYS_TRACE("audio_invalid_timeout, in install menu, will not power off");
     update_detect_state(DETECT_AUDIO);		 
		 return;
	 }
	
	
	 if(!IS_AUDIO_VALID){
		 SYS_TRACE("audio_invalid_timeout, will power off\r\n");
		 sys->is_auto_power_on_enable = true;
		 sys_power_off_handler(sys);
	 }
}

void sys_audio_valid_handler(uint8 index)
{
  SYS_TRACE("audio valid\r\n");
	
	if(gSystem_t->method != AUDIO) return;
	
	if((gSystem_t->status != STATUS_WORKING && gSystem_t->status != STATUS_INITING) && (gSystem_t->is_auto_power_on_enable == true)){
		SYS_TRACE("will power on\r\n");
		sys_power_on_handler(gSystem_t);
	}else{
		SYS_TRACE("will not power on [%d][%d]\r\n", gSystem_t->status, gSystem_t->is_auto_power_on_enable);
	}
	
	sys_untimeout(audio_invalid_timeout, (void *)gSystem_t);
}


void sys_audio_invalid_handler(uint8 index)
{
	uint32 timeout;
	uint8 temp[SE_TIMEOUT_NUM] = {5, 10, 20, 30, 60};

  SYS_TRACE("audio invalid, timeout is[%d]\r\n", gSystem_t->se_timeout);
 
  if(gSystem_t->method != AUDIO) return;
	
    OSAL_ASSERT(gSystem_t->se_timeout < SE_TIMEOUT_NUM);
	
	  timeout = temp[gSystem_t->se_timeout]*1000*60;
	
    sys_untimeout(audio_invalid_timeout, (void *)gSystem_t);
	  sys_timeout(timeout, audio_invalid_timeout, (void *)gSystem_t);
		
}


static void trigger_invalid_timeout(void *arg)
{
	sys_state_t *sys = (sys_state_t *)arg;
	
	SYS_TRACE("trigger_invalid_timeout, method=%d\r\n", gSystem_t->method);
	
	if(gSystem_t->method != TRIGGER) return;
	
	if(cur_menu->id <= RESET_CONFIRM_MENU && cur_menu->id >= IP_ADDRESS_MENU){
		 SYS_TRACE("trigger_invalid_timeout, in install menu, will not power off\r\n");
     update_detect_state(DETECT_TRIGGER);		 
		 return;
	 }
	
	if(!IS_TRIGGER_VALID){
		 sys_power_off_handler(sys);
	 }else{
		 SYS_TRACE("trigger_invalid_timeout, IS_TRIGGER_VALID\r\n");
	 }
}

void sys_trigger_valid_handler(uint8 index)
{
	SYS_TRACE("Trigger detect\r\n");
	
  if(gSystem_t->method != TRIGGER) return;
	
	if((gSystem_t->status != STATUS_WORKING && gSystem_t->status != STATUS_INITING) && (gSystem_t->is_auto_power_on_enable == true)){
		
		gSystem_t->is_trigger_off_need_delay = false;
		sys_power_on_handler(gSystem_t);
	}
	sys_untimeout(trigger_invalid_timeout, (void *)gSystem_t);
}



void sys_trigger_invalid_handler(uint8 index)
{
  
	SYS_TRACE("No Trigger detect, method=%d\r\n", gSystem_t->method);
	
	if(gSystem_t->method != TRIGGER) return;
	
	if(gSystem_t->is_trigger_off_need_delay){
		sys_untimeout(trigger_invalid_timeout, (void *)gSystem_t);
		sys_timeout(1000*60, trigger_invalid_timeout, (void *)gSystem_t);
	}else{
		sys_power_off_handler(gSystem_t);
	}
}



void sys_master_vol_set(sys_state_t *sys)
{
	int gain;
	int dsp_gain;
	int njw_gain;
	uint8 vol;
	
	if(sys->is_mute){
		dsp_master_vol_set(MASTER_VOL_MUTE_REG_VALUE);
	}else{
		gain = VOLUME_OFFSET_BETWEEN_VALUE_AND_GAIN(sys->input_gain) + sys->master_vol;
	
		dsp_gain = VOLUME_GAIN_IN_DSP(gain);
		SYS_TRACE("set dsp_gain to %d\r\n", dsp_gain);
		
	
		if(dsp_gain > VOLUME_GAIN_IN_DSP_MAX){
			njw_gain = dsp_gain - VOLUME_GAIN_IN_DSP_MAX + VOLUME_GAIN_IN_1195_DEFAULT;
			SYS_TRACE("set njw_gain to %d\r\n", njw_gain);
			vol = NJW1195_0DB_GAIN - (njw_gain * 2);
		  SYS_TRACE("njw1195_volume_set to %d\r\n", vol);
		  njw1195_volume_set(vol ,SUB_CHANNEL);
			
			dsp_gain = VOLUME_GAIN_IN_DSP_MAX;
		}else{
			vol = NJW1195_0DB_GAIN - (VOLUME_GAIN_IN_1195_DEFAULT * 2);
			njw1195_volume_set(vol ,SUB_CHANNEL);
		}
	
		dsp_master_vol_set(dsp_gain);
	  
	}
	
}


void sys_audio_sense_thres_set(sense_threshold_t thrs)
{
	uint16 array[SENSE_THRESHOLD_NUM] = {271, 260, 249, 236, 225};
	
	OSAL_ASSERT(thrs < SENSE_THRESHOLD_NUM);
	
	bsp_pwm_for_sense_set(array[thrs], 400);
}



static uint8 _get_sub_lp_freq_index(uint16 freq)
{
  uint8 index = 0;
	
	if(freq < SUB_LOWPASS_MIN){
		
	}else if(freq < SUB_LOWPASS_NODE1){
		index = (freq - SUB_LOWPASS_MIN + SUB_LP_OFFSET_OF_FILTER) / SUB_LP_NODE1_STEP;
	}else if(freq < SUB_LOWPASS_NODE2){
#define NODE1_OFFSET ((SUB_LOWPASS_NODE1 - SUB_LOWPASS_MIN + SUB_LP_OFFSET_OF_FILTER) / SUB_LP_NODE1_STEP)
		index = (freq - SUB_LOWPASS_NODE1) / SUB_LP_NODE2_STEP + NODE1_OFFSET;
	}else if(freq <= SUB_LOWPASS_MAX){
#define NODE2_OFFSET ((SUB_LOWPASS_NODE2 - SUB_LOWPASS_NODE1) / SUB_LP_NODE2_STEP + ((SUB_LOWPASS_NODE1 - SUB_LOWPASS_MIN + SUB_LP_OFFSET_OF_FILTER) / SUB_LP_NODE1_STEP))
   	index = (freq - SUB_LOWPASS_NODE2) / SUB_LP_NODE3_STEP + NODE2_OFFSET;	
	}
	
	return index;
}

void sys_sub_lowpass_set(sys_state_t *sys)
{
	uint8 index;
	
	if(sys->sub[sys->sub_model].lowpass == SUB_LOWPASS_DEFAULT){
		dsp_sub_lp_bypass();
	}else{
		index = _get_sub_lp_freq_index(sys->sub[sys->sub_model].lowpass);
		SYS_TRACE("sys_sub_lowpass_set index=[%d]\r\n", index);
	
		switch(sys->sub[sys->sub_model].lowpass_slope){
			case FILTER_SLOPE_12DB:
				dsp_subLpFre_set(FILTER_SLOPE_12DB, index, SUB_LP12DB_REG_NUM, 0, 0);
			break;
		
			case FILTER_SLOPE_24DB:
				dsp_subLpFre_set(FILTER_SLOPE_24DB, index, SUB_LP24DB_REG_NUM, 0, 0);
			break;
		
			case FILTER_SLOPE_48DB:
				dsp_subLpFre_set(FILTER_SLOPE_48DB, index, SUB_LP48DB_REG_NUM, 0, 0);
			break;
	}
	
}
}

static uint8 _get_sub_hp_freq_index(uint16 freq)
{
	uint8 index = 0;
	
	if(freq < SUB_HIGHPASS_MIN){
	}else if(freq < SUB_HIGHPASS_NODE1){
		index = (freq - SUB_HIGHPASS_MIN) / SUB_HP_NODE1_STEP;
	}else if(freq <=  SUB_HIGHPASS_MAX){
#define HP_NODE1_OFFSET ((SUB_HIGHPASS_NODE1 - SUB_HIGHPASS_MIN) / SUB_HP_NODE1_STEP)				
	  index = (freq - SUB_HIGHPASS_NODE1) / SUB_HP_NODE2_STEP + HP_NODE1_OFFSET;
	}
			
  return index;
}

void sys_sub_highpass_set(sys_state_t *sys)
{
	uint8 index = 0;
	
	if(sys->sub[sys->sub_model].highpass == SUB_HIGHPASS_OFF){
		dsp_sub_hp_bypass();
		SYS_TRACE("dsp_sub_hp_bypass\r\n");
	}else{
		
		index = _get_sub_hp_freq_index(sys->sub[sys->sub_model].highpass);
	
		switch(sys->sub[sys->sub_model].highpass_slope){
			case FILTER_SLOPE_12DB:
				dsp_subHpFreq_set(FILTER_SLOPE_12DB, index, SUB_LP12DB_REG_NUM, 0x30);
			break;
		
			case FILTER_SLOPE_24DB:
				dsp_subHpFreq_set(FILTER_SLOPE_24DB, index, SUB_LP24DB_REG_NUM, 0x30);
			break;
		
			case FILTER_SLOPE_48DB:
				dsp_subHpFreq_set(FILTER_SLOPE_48DB, index, SUB_LP48DB_REG_NUM, 0x30);
			break;
	}
	
}
}


void sys_line_highpass_set(sys_state_t *sys)
{
	uint8 index;
	
	if(sys->lineout_highpass == LINEOUT_HIGHPASS_DEFAULT){
		 dsp_lineout_hp_bypass();
	}else{
	
		index = _get_sub_lp_freq_index(sys->lineout_highpass);
	
		switch(sys->lineout_highpass_slope){
			case FILTER_SLOPE_12DB:
				dsp_subLpFre_set(FILTER_SLOPE_12DB, index, SUB_LP12DB_REG_NUM, -0x20, -0x30);
			break;
		
			case FILTER_SLOPE_24DB:
				dsp_subLpFre_set(FILTER_SLOPE_24DB, index, SUB_LP24DB_REG_NUM, -0x20, -0x30);
			break;
		
			case FILTER_SLOPE_48DB:
				dsp_subLpFre_set(FILTER_SLOPE_48DB, index, SUB_LP48DB_REG_NUM, -0x20, -0x30);
			break;
	}
	
}
}

void sys_delay_time_set(sys_state_t *sys)
{
	if(sys->d_router == DELAY_SUB){
		dsp_delay_time_set(sys->sub[sys->sub_model].delay);
	}
	else if(sys->d_router == DELAY_LINEOUT){
		dsp_delay_time_set(sys->line_delay_time);
	}
}

void sys_delay_switch_to(sys_state_t *sys)
{
  if(sys->d_router == DELAY_SUB){
		dsp_delay_switch_to_sub();
		SYS_TRACE("delay switch to sub\r\n");
	}
	else if(sys->d_router == DELAY_LINEOUT){
		dsp_delay_switch_to_line();
		SYS_TRACE("delay switch to lineout\r\n");
	}else{
    dsp_delay_switch_off();
		SYS_TRACE("delay switch to off\r\n");
	}		
}


void sys_phrase_set(sys_state_t *sys)
{
	if(sys->sub[sys->sub_model].phrase == 1){
		dsp_phrase_on();
	}else{
		dsp_phrase_off();
	}
}


void sys_load_roomeq(sys_state_t *sys)
{
	uint8 j;
	
	for(j = 0; j < ROOM_EQ_FILTER_NUM; j++){
		dsp_eq_set_freq(sys->eq->req.filter[j].a, j, TYPE_ROOM_EQ);
		dsp_eq_set_width(sys->eq->req.filter[j].b, j, TYPE_ROOM_EQ);
		dsp_eq_set_gain(sys->eq->req.filter[j].g, j, TYPE_ROOM_EQ);
	}
	
	SYS_TRACE("sys_load_roomeq\r\n");
}

void sys_peq_single_set(sys_state_t *sys, uint8 preset, uint8 index, uint32 a, uint32 b, uint32 g)
{

		dsp_eq_set_freq(a, index, TYPE_PRESET_EQ);
		dsp_eq_set_width(b, index, TYPE_PRESET_EQ);
		dsp_eq_set_gain(g, index, TYPE_PRESET_EQ);

	
	  SYS_TRACE("sys_peq_single_set\r\n");
}

void sys_req_single_set(sys_state_t *sys, uint8 index, uint32 a, uint32 b, uint32 g)
{
	  dsp_eq_set_freq(a, index, TYPE_ROOM_EQ);
		dsp_eq_set_width(b, index, TYPE_ROOM_EQ);
		dsp_eq_set_gain(g, index, TYPE_ROOM_EQ);
}



void sys_load_peq(sys_state_t *sys)
{
  uint8 j;

  OSAL_ASSERT(sys->preset < PRESET_NUM);	
	
	for(j = 0; j < PRESET_EQ_FILTER_NUM; j++){
		dsp_eq_set_freq(sys->eq->peq[sys->preset].filter[j].a, j, TYPE_PRESET_EQ);
		dsp_eq_set_width(sys->eq->peq[sys->preset].filter[j].b, j, TYPE_PRESET_EQ);
		dsp_eq_set_gain(sys->eq->peq[sys->preset].filter[j].g, j, TYPE_PRESET_EQ);
	}
}

void sys_eq_state_set(sys_state_t *sys)
{
	if(sys->eq_on_off == 0){
		dsp_eq_off();
	}else{
		dsp_eq_on();
		sys_load_roomeq(sys);
	}
}

static void sys_sub_eq_set(sys_state_t *sys)
{
	uint8 j;
	
	SYS_TRACE("sys_sub_eq_set:%d\r\n", sys->sub_model);
	
	for(j = 0; j < SUB_MODE_EQ_FILTER_NUM; j++){
		dsp_eq_set_freq(sub_eq_array_a[sys->sub_model][j], j, TYPE_SUB_EQ);
		dsp_eq_set_width(sub_eq_array_b[sys->sub_model][j], j, TYPE_SUB_EQ);
		dsp_eq_set_gain(sub_eq_array_g[sys->sub_model][j], j, TYPE_SUB_EQ);
	}
	
}

void sys_sub_preset_load(sys_state_t *sys)
{
	OSAL_ASSERT(sys->sub_model < SUB_MODEL_NUM);
	
	sys_sub_lowpass_set(sys);
  sys_sub_highpass_set(sys);
  sys_phrase_set(sys);
	sys_delay_switch_to(sys);
	sys_delay_time_set(sys);
	sys_sub_eq_set(sys);
}

static void protect_recover_later(void *arg)
{
	 uint8 index = *((uint8 *)arg);
	
	 AMP_ENABLE();
	 AMP_MUTE_OFF();
	
	 vTaskDelay(100/portTICK_RATE_MS);
	
   if(index == DETECT_OVER_VOLTAGE_OR_SHORT && IS_SHORT_CIRCUIT){
		 return;
	 }
	
	 if(index == DETECT_OVER_CURRENT && IS_OVER_CURRENT) {
		 return;
	 }

	 gSystem_t->is_in_protection = false;
			 
	 if(gSystem_t->status == STATUS_WORKING  || gSystem_t->status == STATUS_INITING){
			BLED_SET(1);
		}else if(gSystem_t->status == STATUS_STANDBY){
			RLED_SET(1);
		}else if(gSystem_t->status == STATUS_SHUTDOWN){
			LED_OFF();
		}
	
	  dis_send_msg(DIS_UPDATE, MENU_NUM);
		 
	 
}

void sys_protect_valid_handler(uint8 index)
{
	SYS_TRACE("sys_protect_valid_handler[%s]-[%d]\r\n", protect_name[index], (index - 2 + DIS_OVER_TEMP));
	
	/*since more than one protection event may happen at the same time, we should record these*/
	gSystem_t->protect_bitmap |= (1 << index);
	
	if(gSystem_t->status != STATUS_WORKING && gSystem_t->status != STATUS_INITING){
		/*when protection event happen in standby or shutdown mode, we need do noting but just record the event*/
		return;
	}
	
	/*when short circuit hanppen then must case over current, so don't care it and clear it from gSystem_t->protect_bitmap*/
	if(gSystem_t->is_short_circuit == true && index == DETECT_OVER_CURRENT){
		gSystem_t->protect_bitmap &= ~(1 << index);
		return;
	}
	
	
//	/*before out of fix hardware bug, we don't indicate protection events*/
//	if(gSystem_t->is_out_of_fix_hardware == false) return;
	
	
	
	/*when clip happen, no need mute but just flash the LED*/
	if(index != DETECT_OVER_CLIP){
		AMP_DISENABLE();
		AMP_MUTE_ON();
	}
	
	if(gSystem_t->is_in_protection == false){
	  gSystem_t->is_in_protection = true;
	  sys_Rled_flash(NULL);
	}
	
	/*since the over voltage and short circuit use the same gpio to indicate event, we need do */
	if(index == DETECT_OVER_VOLTAGE_OR_SHORT){
		vTaskDelay(5000/portTICK_RATE_MS);
		if(IS_SHORT_CIRCUIT){
			gSystem_t->is_short_circuit = true;
			SYS_TRACE("is_short_circuit\r\n");
			sys_untimeout(protect_recover_later, (void *)&index);
		}
	}
	
	if(index == DETECT_OVER_CURRENT){
		sys_untimeout(protect_recover_later, (void *)&index);
	}
	
	dis_send_msg(DIS_JUMP, (menu_id_t)(index - 2 + DIS_OVER_TEMP));
}



void sys_protect_invalid_handler(uint8 index)
{
	SYS_TRACE("sys_protect_invalid_handler[%s]\r\n", protect_name[index]);
	
	if(gSystem_t->protect_bitmap == 0){
		/*since no protection event happened, we need do nothing*/
		return;
	}else{

		gSystem_t->protect_bitmap &= ~(1 << index);
	}
	
	/*this maybe should remove since v0.3.8*/
	if(gSystem_t->status != STATUS_WORKING && gSystem_t->status != STATUS_INITING) return;
	
	/*when short circuit happen we don't care about over current*/
	if(gSystem_t->is_short_circuit == true && index == DETECT_OVER_CURRENT) return;
	
	/*special handler when short circuit occur*/
	if(index == DETECT_OVER_VOLTAGE_OR_SHORT && gSystem_t->is_short_circuit == true){
		
		/*it is complicated to handler this, we need take consideration of more than one protetion events occur at the same time and power on event*/
		if(gSystem_t->protect_bitmap == 0 && gSystem_t->is_out_of_fix_hardware == true){
			sys_timeout(5*1000, protect_recover_later, (void *)&index);
		}
		
		return;
	}
	
	/*special handler when over current occur*/
	if(index == DETECT_OVER_CURRENT){
		
		/*it is complicated to handler this, we need take consideration of more than one protetion events occur at the same time and power on event*/
		if(gSystem_t->protect_bitmap == 0 && gSystem_t->is_out_of_fix_hardware == true){
			sys_timeout(5*1000, protect_recover_later, (void *)&index);
		}
		
		return;
	}
		
	
	if(gSystem_t->protect_bitmap == 0){
	   gSystem_t->is_in_protection = false;
	   AMP_ENABLE();
	   AMP_MUTE_OFF();
	
	  if(gSystem_t->status == STATUS_WORKING || gSystem_t->status ==STATUS_INITING){
			BLED_SET(1);
		}else if(gSystem_t->status == STATUS_STANDBY){
			RLED_SET(1);
		}else if(gSystem_t->status == STATUS_SHUTDOWN){
			LED_OFF();
		}
	
	  dis_send_msg(DIS_UPDATE, MENU_NUM);
	}
}

void sys_database0_update_later(void *arg)
{
	 sys_send_msg(DATABASE_UPDATE_MSG, 0);
}

void sys_database1_update_later(void *arg)
{
	 sys_send_msg(DATABASE_UPDATE_MSG, 1);
}

void sys_database2_update_later(void *arg)
{
	 sys_send_msg(DATABASE_UPDATE_MSG, 2);
}

void sys_15v_valid_handler(uint8 index)
{
	SYS_TRACE("sys_15v_valid_handler\r\n");
	
	
}

void sys_15v_invalid_handler(uint8 index)
{
	SYS_TRACE("sys_15v_invalid_handler\r\n"); 
	
	
}


void sys_low_power_handler(void)
{
	uint8 buf[10] = {0};
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	
	/*eeprom releated gpio init*/
  GPIOPinTypeGPIOOutput(EPROM_SDA_PORT, EPROM_SDA_PIN);
	GPIOPinTypeGPIOOutput(EPROM_SCL_PORT, EPROM_SCL_PIN);
	
	eeprom_read(SYS_LOW_POWER_FLAG_ADDRESS, buf, strlen("LOWPOWER"));

  if(strcmp((char *)buf, "LOWPOWER") == 0){
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
		GPIOPinTypeGPIOInput(POWER_KEY_PORT, POWER_KEY_PIN);
	  GPIOIntTypeSet(POWER_KEY_PORT, POWER_KEY_PIN, GPIO_RISING_EDGE);
		IntEnable(INT_KEY_POWER);
	  GPIOIntEnable(POWER_KEY_PORT, POWER_KEY_PIN);
		
		eeprom_write(SYS_LOW_POWER_FLAG_ADDRESS, (uint8 *)"12345678", 8);
		
		bsp_delay_ms(100);
	
		
		SysCtlDeepSleep();
	}
}

void sys_ip_power_on_handler(sys_state_t *sys)
{
	 if(sys->status == STATUS_STANDBY && sys->method == IR_IP){
		 sys_power_on_handler(sys);
	 }
}


int lcd_test(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	 
	lcd_test_for_nad();
	
	SYS_TRACE("\r\n");
	return 0;
}

OSAL_CMD(lcd, 1, lcd_test, "nav prev menu list");



