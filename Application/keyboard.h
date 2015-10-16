#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_


#define CHECK_AND_CLEAR_CODE() do{ \
	if(cur_menu->id != INSTALL_MENU && cur_menu->id != IP_ADDRESS_MENU){ \
		memset(gCode, 0, sizeof(gCode)); \
		sys->code_index = 0; \
	} \
}while(0)

#define CLEAR_CODE() do{ \
		memset(gCode, 0, sizeof(gCode)); \
		gSystem_t->code_index = 0; \
}while(0)

typedef enum{
	KEY_UP = 0,
	KEY_UP_LONG,
	KEY_DOWN,
	KEY_DOWN_LONG,
	KEY_LEFT,
	KEY_LEFT_LONG,
	KEY_RIGHT,
	KEY_RIGHT_LONG,
	KEY_ENTER,
	KEY_ENTER_LONG,
	KEY_BACK,
	KEY_BACK_LONG,
	KEY_POWER,
	KEY_POWER_LONG,
	KEY_VALUE_MAX
}key_value_t;

extern char gCode[CODE_NUM];

void dis_timeout_handler(void *arg);

void keyboard_msg_handler(sys_state_t *sys, key_value_t key);

void ir_commond_handler(sys_state_t *sys, ir_commond_t cmd);

void KeyScanLoop(void);


#endif


