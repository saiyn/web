/*=========================================================================
 *        software spi driver
 *        sw_spi.c
 *        Jim.wang
 *        2013.4.23
 *========================================================================*/
#include "common_head.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sw_spi.h"


void SpiDelay(UINT32 time)
{
    for(;time >0;time--)
    {
        NOP();NOP();//NOP();//NOP();NOP();
        //NOP();NOP();NOP();NOP();NOP();
        //NOP();NOP();NOP();NOP();NOP();
    }
}
static void SPICLK2(unsigned char en)
{
    GPIO_PIN_SET(CS48520_CLK_PORT, CS48520_CLK_PIN, en);
}
static void SPIDO2(unsigned char en)
{
    GPIO_PIN_SET(CS48520_MOSI_PORT, CS48520_MOSI_PIN, en);
}
static unsigned char SPIDI2(void)
{
 return (GPIO_ReadSinglePin(CS48520_MISO_PORT, CS48520_MISO_PIN)); 
}


 /*
 	this file only can use when SPI slave device isn't MODE 3
 */
 
 unsigned char SWReadSPI(void)
 {
     unsigned char i;
     unsigned char ReceivedByte;
 
     vPortEnterCritical();
 
     SmartDelay_spi(5);
     ReceivedByte = 0;
     for (i = 0; i < 8; i++) 
     {       
         SPICLK2(0);
         //SmartDelay_spi(5);         
         ReceivedByte <<= 1;
         if (SPIDI2())
             ReceivedByte |= 0x01;
         SmartDelay_spi(2);
         SPICLK2(1);
         SmartDelay_spi(5);   
     }
     //SPICLK(0);
 
     vPortExitCritical();
 
     return (ReceivedByte);
 }
unsigned char SWWriteSPI(unsigned char cData)
{
    unsigned char i;
    unsigned char send;

    vPortEnterCritical();
    
    SmartDelay_spi(5);
    send = cData&0xff;
    for (i = 0; i < 8; i++) 
    {
        SPICLK2(0);
        if (send & 0x80)	SPIDO2(1);
        else				SPIDO2(0);
        send <<= 1;		 
        SmartDelay_spi(2);
        SPICLK2(1);		 
        SmartDelay_spi(5);
    }
    //SPICLK(0);
    //SPIDO(0);

    vPortExitCritical();

    return 0;
}
  
 void SmartDelay_spi(unsigned long time)
{
    for(;time >0;time--)
    {
        NOP();NOP();//NOP();//NOP();NOP();
        //NOP();NOP();NOP();NOP();NOP();
        //NOP();NOP();NOP();NOP();NOP();
    }
} 
