#include "common_head.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "CS4251X.h"


#define CS4251X_ADDR	0x9E

typedef struct 
{
	unsigned char ratio;
	unsigned char rvcr_status;
	unsigned char recevier_err;
}cx4251x_state_t;



#define SPICLK          	SetDaClk
#define SPIDO           	SetDaOut
#define SPIDI             	GetDaIn
#define CS4251X_CS_L   	SetDaCs(0)
#define CS4251X_CS_H  	SetDaCs(1)
#define CS4251X_RST_L    	SetDaRst(0)
#define CS4251X_RST_H    	SetDaRst(1)
#define HAL_Spi1Put(b)  	xSPI2PutByte(b)
#define HAL_DelayMs(ms)  	DelayUs(ms*1000)

void SmartDelay_spi2(unsigned char time)
{
    for(;time >0;time--)
    {
        NOP();
    }
}
  

 unsigned char xSPI2PutByte(unsigned char ucSend)
 {
     unsigned char i;
     unsigned char ucRecv = 0;
     unsigned char ucMask = 0x80;
 
     vPortEnterCritical();
     
     SmartDelay_spi2(5);
     for (i = 0; i < 8; i++)
     {
         SmartDelay_spi2(5);
         SPICLK(0);
         //SmartDelay_spi(5);  
         if (SPIDI())
             ucRecv |= ucMask;
         ucMask >>= 1;           
         SmartDelay_spi2(5);    
         if (ucSend & 0x80)   SPIDO(1);
         else               SPIDO(0);
         ucSend <<= 1;    
         SPICLK(1);
     }    
     SPIDO(1);
 
     vPortExitCritical();
     
     return ucRecv;
 }

int cs4251x_write(unsigned char reg, unsigned char value)
{
	unsigned char ret;
	
	CS4251X_CS_L;
	HAL_Spi1Put(CS4251X_ADDR);
	HAL_Spi1Put(reg);
	HAL_Spi1Put(value);
	CS4251X_CS_H;	

	ret = cs4251x_read(reg);	

	if(ret != value)
	{
		return 1;
	}

	return 0;
}

unsigned char cs4251x_read(unsigned char reg)
{
	unsigned char ret;

	CS4251X_CS_L;
	HAL_Spi1Put(CS4251X_ADDR);
	HAL_Spi1Put(reg);
	CS4251X_CS_H;

	CS4251X_CS_L;
	HAL_Spi1Put(CS4251X_ADDR|0x01);
	ret = HAL_Spi1Put(0xFF);
	CS4251X_CS_H;

	return ret;
}

/*
 * initialise the cs42516/8/28 codec.
 * register the mixer and dsp interfaces with the kernel
 */
int CS4251X_Init(void)
{
	unsigned char id;
    
	CS4251X_CS_H;
	CS4251X_RST_L;
	bsp_delay_ms(50);
	CS4251X_RST_H;
	bsp_delay_ms(100);
	
	/* identify the chip */
	id = cs4251x_read(CS4251X_CHIPID);

  SYS_TRACE("CS4251X_Init id=%x\r\n", id);
	// mute on
  cs4251x_write(CS4251X_MUTE, 0xff);

    //
	//Power on ADC /DAC
	//
    cs4251x_write(CS4251X_PM, 0x00);

    //
    //Reg:03
	//Bit[7~6]: codec_sp 96k(01)
	//Bit[5~4]: sai_sp      96k(01)
	//Bit[3~2]=10 :Serial data on SAI_SDOUT pin, 
	//                      clocked from the SAI_SP. No S/PDIF data available.
	cs4251x_write(CS4251X_FUNCMODE, 0x08);    


    //
	// codec_sp slave, sai_sp slave
	//
	cs4251x_write(CS4251X_MISCCTL, 0x00);

	// 
	//omck 24.5760M
	//
	cs4251x_write(CS4251X_CLKCTL,CS4251X_CLKSRC_OMCK_245760MHZ|CS4251X_CLKSRC_OMCK);

    
	cs4251x_write(CS4251X_RECERRMASK, 0x18);
	cs4251x_write(CS4251X_RCVMODECTL2, 0x01);

    
	//cs4251x_write(CS4251X_VOLTRANSCTL, 0x3B);
	cs4251x_write(CS4251X_MUTEC, 0x3F);

    
	cs4251x_write(CS4251X_MUTE, 0);

	SYS_TRACE("CS4251X_Init done\r\n");
	return 0;
}


