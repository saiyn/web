#ifndef _SYS_DATABASE_H_
#define _SYS_DATABASE_H_


#define ROOM_EQ_FILTER_NUM  6
#define CUSTMER_EQ_FILTER_NUM 2
#define PRESET_EQ_FILTER_NUM  2
#define SUB_MODE_EQ_FILTER_NUM 8

#define TYPE_ROOM_EQ 1
#define TYPE_PRESET_EQ 2
#define TYPE_SUB_EQ  3

#define SUB_CHANNEL 0
#define LINEOUT_CHANNEL 1

typedef enum{
	WELCOM_MENU = 0,
	FACTORY_SUB_SELECT_MENU,
	FACTOTY_SUB_CONFIRM_MENU,
	VOLUME_MENU,
	SOURCE_MENU,
	PRESET_MENU,
	BRIGHTLESS_MENU,
	INSTALL_MENU,
	IP_ADDRESS_MENU,
	SUB_MODEL_MENU,
	INPUT_GAIN_MENU,
	PHRASE_MENU,
	SUB_LOWPASS_MENU,
	SUB_HIGHPASS_MENU,
	LINEOUT_HIGHPASS_MENU,
	DELAY_ROUTER_MENU,
	DELAY_TIME_MENU,
	POWER_ON_METHOD_MENU,
	SENSE_THRESHOLD,
	SENSE_OFF_TIME,
	RESET_MENU,
	RESET_CONFIRM_MENU,
	MENU_NUM,
	DIS_OVER_TEMP,
	DIS_OVER_VOLTAGE_OR_SHORT,
	DIS_OVER_CLIP,
	DIS_OVER_DC_ERROR,
	DIS_OVER_CURRENT,
	DIS_OVER_15V,
	DIS_PEQ_STATE,
	DIS_VERSION_INFOR
}menu_id_t;

typedef enum{
IR_VOL_UP =0,
IR_VOL_DOWN,
IR_ON,
IR_OFF,
IR_NORMAL,
IR_CINEMA,
IR_NIGHT,
IR_USER1,
IR_USER2,
IR_USER3,
IR_BACK,
IR_UP,
IR_LFET,
IR_CENTER,
IR_RIGHT,
IR_DOWN,
IR_MUTE,
IR_PQE_ON,
IR_PQE_OFF,
IR_COMMOND_NUM
}ir_commond_t;

#define MENU_NODE  IP_ADDRESS_MENU


typedef enum{
	DETECT_AUDIO = 0,
	DETECT_TRIGGER,
	DETECT_OVER_TEMP,
	DETECT_OVER_VOLTAGE_OR_SHORT,
	DETECT_OVER_CLIP,
	DETECT_OVER_DC_ERROR,
	DETECT_OVER_CURRENT,
	DETECT_OVER_15V,
	NUM_OF_DETECT
}detect_event_t;

extern const char *version;

typedef struct{
	size_t index;
	size_t min;
	size_t max;
}menu_context_t;

struct menu{
	menu_id_t id;
	menu_context_t *context;
	struct menu *next;
	struct menu *prev;
	struct menu *child;
	struct menu *parent;
};

typedef struct menu menu_t;

typedef enum{
	DELAY_SUB=0,
	DELAY_LINEOUT,
	DELAY_OFF,
	DELAY_ROUTER_MAX
}delay_router_t;

#define FILTER_SLOPE_12DB    1
#define FILTER_SLOPE_24DB    2
#define FILTER_SLOPE_48DB    3

typedef struct{
	uint16 freq;
	uint8  Q;
	int    gain;
	uint32 a;
	uint32 b;
	uint32 g;
}b_eq_t;

typedef struct{
	uint8 phrase;
	uint16 lowpass;
	uint8 lowpass_slope;
	uint16 highpass;
	uint8 highpass_slope;
	uint16 delay;
}sub_state_t;


typedef enum{
	MANUAL=0,
	AUDIO,
	TRIGGER,
	IR_IP,
	POWER_ON_MAX
}power_on_t;

typedef enum{
	STATUS_STANDBY=0,
	STATUS_INITING,
	STATUS_WORKING,
	STATUS_SHUTDOWN,
	STATUS_MAX
}sys_status_t;

typedef enum{
	BALANCE=0,
	RCA,
	SPEAHER,
	SRC_NUM
}src_t;

typedef enum{
	PRE_NORMAL=0,
	PRE_CINEMA,
	PRE_NIGHT,
	PRE_USER1,
	PRE_USER2,
	PRE_USER3,
	PRESET_NUM
}preset_t;

typedef enum{
	BRI_LOW=0,
	BRI_MEDIUM,
	BRI_HIGH,
	BRI_NUM
}lcd_bright_t;

typedef enum{
	MINI_INROOM=0,
	MINI_INCEILING,
	MINI_FLEXSUB,
	BRONZE_IWOW_SLIM,
	BRONZE_DOWN_SLIM,
	BRONZE_INROOM,
	BRONZE_IWORIC,
	SILVER_INROOM,
	SILVER_INWALL,
	SILVER_FLEXSUB,
	GOLD_INROOM,
	PLATINUM_INROOM,
	SUB_FLAT,
	SUB_MODEL_NUM
}sub_model_t;

typedef enum{
	SENSE_3MV=0,
	SENSE_6MV,
	SENSE_9MV,
	SENSE_12MV,
	SENSE_15MV,
	SENSE_THRESHOLD_NUM
}sense_threshold_t;

typedef enum{
	SE_TIMEOUT_5=0,
	SE_TIMEOUT_10,
	SE_TIMEOUT_20,
	SE_TIMEOUT_30,
	SE_TIMEOUT_60,
	SE_TIMEOUT_NUM
}sense_timeout_t;



extern b_eq_t gEqTemp;

typedef struct{
	b_eq_t filter[ROOM_EQ_FILTER_NUM];
}Rooom_eq_t;

typedef struct{
	b_eq_t filter[PRESET_EQ_FILTER_NUM];
	int vol;
}Preset_eq_t;

typedef struct{
	b_eq_t filter[CUSTMER_EQ_FILTER_NUM];
	int vol;
}Custmer_eq_t;

typedef struct{
	Rooom_eq_t req;
	Preset_eq_t peq[PRESET_NUM];
}EQ_t;


typedef struct{
	uint32 ip;
	uint32 netmask;
	uint32 gateway;
	uint8  mac[6];
	uint8  ip_mode;
	power_on_t method;
	int master_vol;
	src_t src;
	preset_t preset;
	lcd_bright_t bright;
	sub_model_t sub_model;
	int input_gain;
	uint16 lineout_highpass;
	uint8 lineout_highpass_slope;
	uint16 line_delay_time;
	delay_router_t d_router;
	sense_threshold_t sense_thres;
	sense_timeout_t se_timeout;
	uint8 room_eq_index;
	uint8 eq_on_off;
	uint32 sys_crc32;
	uint32 sub_crc32;
	uint32 eq_crc32;
	
	
	uint8 end_of_nv;
	
	sub_state_t *sub;
	EQ_t *eq;
	uint8 cur_room_eq;
	uint8 cur_cust_eq;
	bool do_reset;
	bool confirm_reset;
	uint8 code_index;
	bool is_mute;
	uint8 protect_bitmap;
  uint8 is_in_factory;
	bool is_auto_power_on_enable;
	bool is_trigger_off_need_delay;
	bool is_in_protection;
	bool is_short_circuit;
	bool is_out_of_fix_hardware;
	bool is_dis_timeout;
	sub_model_t temp_sub_model;
	volatile sys_status_t status;
}sys_state_t;




#define SIZEOF_SYS_NV   ((uint8 *)&gSystem_t->end_of_nv - (uint8 *)&gSystem_t->ip)
#define SIZEOF_SUB_NV   (sizeof(sub_state_t) * SUB_MODEL_NUM)
#define SIZEOF_EQ_NV    (sizeof(EQ_t))


#endif


