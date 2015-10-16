#include "common_head.h"
#include "njw1195_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSAL_Console.h"
#include "CS4251x.h"

#define NJW1195_DATA(x) GPIO_PIN_SET(NJW1195_DATA_PORT, NJW1195_DATA_PIN, x);njw1195_delay(40)
#define NJW1195_CLK(x)  GPIO_PIN_SET(NJW1195_CLK_PORT, NJW1195_CLK_PIN, x);njw1195_delay(40)
#define NJW1195_CS(x)   GPIO_PIN_SET(NJW1195_CS_PORT, NJW1195_CS_PIN, x);njw1195_delay(40)

/*Bit[3~0] chip address*/
#define CHIPADDR  (0 << 0)
#define INPUTASEL (4 << 4)
#define INPUTBSEL (5 << 4)
#define VOLCON1A  (0 << 4)
#define VOLCON1B  (1 << 4)
#define VOLCON2A  (2 << 4)
#define VOLCON2B  (3 << 4)

static void njw1195_delay(uint16 time)
{
	uint16 j;
	
	for(j = 0; j < time; j++);
}



static void njw1195_write(uint16 data)
{
	uint8 j;
	
	
	vPortEnterCritical();
	NJW1195_CS(0);
	NJW1195_CLK(1);
	njw1195_delay(20);
	
	for(j = 0; j < 16; j++){
		if(data & (0x8000 >> j)){
			NJW1195_DATA(1);
		}else{
			NJW1195_DATA(0);
		}
		NJW1195_CLK(1);
		njw1195_delay(40);
		NJW1195_CLK(0);
		njw1195_delay(20);
	}
	NJW1195_CLK(1);
	njw1195_delay(20);
	NJW1195_CS(1);
	njw1195_delay(50);
	vPortExitCritical();
}


int njw1195_init(void)
{
	njw1195_write(0xff00);
	njw1195_write(0xff10);
	njw1195_write(0x0010);
	njw1195_write(0x0030);
	njw1195_write(0x0040);
	njw1195_write(0x0050);
	
	return 0;
}

void njw1195_input_select(njw1194_channel_t channel)
{

	switch(channel){
		case NJW_BALANCE:
			njw1195_write((1 << 13) | (1 << 10) | INPUTASEL | CHIPADDR);
		  njw1195_write((1 << 13) | (1 << 10) | INPUTBSEL | CHIPADDR);
		  cs4251x_write(0x0e, (1<<1)| (1<<0) | (0<<3));
		  //SYS_TRACE("1195 write [%x][%x]\r\n", (1 << 13) | (1 << 10) | INPUTASEL | CHIPADDR, (1 << 13) | (1 << 10) | INPUTBSEL | CHIPADDR);
			break;
		
		case NJW_RCA:
			njw1195_write((3 << 13) | (3 << 10) | INPUTASEL | CHIPADDR);
		  njw1195_write((3 << 13) | (3 << 10) | INPUTBSEL | CHIPADDR);
		  cs4251x_write(0x0e, (0<<1)| (1<<2) | (1<<3));	
		  //SYS_TRACE("1195 write [%x][%x]\r\n", (3 << 13) | (3 << 10) | INPUTASEL | CHIPADDR, (3 << 13) | (3 << 10) | INPUTBSEL | CHIPADDR);
			break;
		
		case NJW_SPEAK:
			njw1195_write((4 << 13) | (4 << 10) | INPUTASEL | CHIPADDR);
		  njw1195_write((4 << 13) | (4 << 10) | INPUTBSEL | CHIPADDR);
		  cs4251x_write(0x0e, (1<<0)| (1<<2) | (1<<3) | (1 << 1));	
		  //SYS_TRACE("1195 write [%x][%x]\r\n", (4 << 13) | (4 << 10) | INPUTASEL | CHIPADDR, (4 << 13) | (4 << 10) | INPUTBSEL | CHIPADDR);
			break;
		
		default:
			break;
	}
}

void njw1195_volume_set(uint8 vol, uint8 channel)
{
  if(channel == SUB_CHANNEL){
		njw1195_write((vol << 8) | VOLCON1A | CHIPADDR);
		njw1195_write((vol << 8) | VOLCON1B | CHIPADDR);
		
		SYS_TRACE("njw1195_volume_set:[%x][%x]\r\n", (vol << 8) | VOLCON1A | CHIPADDR, (vol << 8) | VOLCON1B | CHIPADDR);
	}else if(channel == LINEOUT_CHANNEL){
		njw1195_write((vol << 8) | VOLCON2A | CHIPADDR);
		njw1195_write((vol << 8) | VOLCON2B | CHIPADDR);
	}
}

void njw1195_channel_mute(void)
{
	SYS_TRACE("NJW1195 Mute\r\n");
	
	njw1195_write(0 | INPUTASEL | CHIPADDR);
	njw1195_write(0 | INPUTBSEL | CHIPADDR); 
	njw1195_volume_set(0xff, SUB_CHANNEL);
	njw1195_volume_set(0xff, LINEOUT_CHANNEL);
}




int clk_gpio_test(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	 
	if(strcmp(argv[1], "H") == 0){
			NJW1195_CLK(1);
		  SYS_TRACE("set clk H\r\n");
	}else{
		  NJW1195_CLK(0);
		  SYS_TRACE("set clk L\r\n");
	}
	
	
	return 0;
}

OSAL_CMD(clk, 2, clk_gpio_test, "clk gpio test");





