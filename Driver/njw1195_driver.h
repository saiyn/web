#ifndef _NJW1195_DRIVER_H_
#define _NJW1195_DRIVER_H_


typedef enum{
	MUTE =0,
	NJW_BALANCE,
	NJW_RCA,
	NJW_SPEAK,
	NJW_CHANNEL_MAX
}njw1194_channel_t;

#define NJW1195_0DB_GAIN  0x40
#define NJW1195_MAX_GAIN  0x22
#define NJW1195_4_5DB_GAIN  0x38

void njw1195_input_select(njw1194_channel_t channel);


void njw1195_volume_set(uint8 vol, uint8 channel);

int njw1195_init(void);

void njw1195_channel_mute(void);



#endif


