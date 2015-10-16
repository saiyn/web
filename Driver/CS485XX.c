/***********************************************************************
* Copyright (c) 2014, Hansong (Nanjing) Technology Ltd.
* All rights reserved.
* 
* ----File Info ----------------------------------------------- -----------------------------------------
* Name:  CS485XX.c, 
* Author:Even.chen
* Date:   2014/9/2
* Summary:     CS485XX Functions, Include Hardware Drivers and Module Operate
* ----Latest Version----------------------------------------------- ---------------------------------
* Version: V1.00
* Date:    2014/9/2
* Author:  Even.chen
* Description: 
***********************************************************************/
/*============================================
* Include 
*============================================*/
/* Target includes. */
#include "common_head.h"
#include "sw_spi.h"

#include "cs485xx.h"

  
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

   
/* Dsp compiler head files*/
#include "api_doc.h"
#include "initial.h"
#include "kickstart.h"
#include "prekickstart.h"
#include "os_p4_48520_01_01_rc79.h"
#include "TriadRackAmp700_p4_48520_01_0100.h"


/*============================================
* Definition 
*============================================*/
#define SLAVE_BOOT      0x80000000
#define SOFT_RESET      0x40000000
#define BOOT_START      0x00000001
#define BOOT_SUCCESS    0x00000002
#define APP_START       0x00000004
#define DSP_ERROR_CNT   10



//Audio stream port define
#define vPortReset(en)      GPIO_PIN_SET(CS48520_RST_PORT, CS48520_RST_PIN, en)
#define vPortCS(en)         GPIO_PIN_SET(CS48520_CS_PORT, CS48520_CS_PIN, en)
#define xPortBusy()         GPIO_ReadSinglePin(CS48520_BUSY_PORT, CS48520_BUSY_PIN)
#define xPortIrq()          GPIO_ReadSinglePin(CS48520_IRQ_PORT, CS48520_IRQ_PIN)


//Function declaration
void CS485xxPortInit(void);
unsigned char xCS485xxWriteBlock(const unsigned char *pucData, unsigned long ulSize);
unsigned char xCS485xxReadBlock(unsigned char *pucData,unsigned long ulSize);
unsigned char xCS485xxWriteCmd(unsigned long ulData);
unsigned long xCS485xxReadCmd(void);
unsigned char xCS485xxWriteRegister(unsigned long ulAddr,unsigned long ulData);
unsigned char xCS485xxReadRegSolicited(unsigned long ulReg, unsigned long * pulRcv);
unsigned char xCS485xxReadRegUnsolicited(void);


unsigned char vCS485xxStatusCheck(unsigned char ucBit)
{
	unsigned long ulErrorCnt = 0;

    if (ucBit)
    {

        while(!xPortBusy())
        {
            SmartDelay_spi(10000);
            ulErrorCnt++;
            if (ulErrorCnt > DSP_ERROR_CNT)
            {
                return false;
            }
        }      
    }
    else
    {
        while(xPortIrq())
        {
            SmartDelay_spi(10000);
            ulErrorCnt++;
            if (ulErrorCnt > DSP_ERROR_CNT)
            {         
                return false;                
            }
        }
    }
    return true;
}



/*======================================================
 *  Function:   xCS485xxWriteBlock
 *  Description:  Write Data to DSP Register
 *  Input:     unsigned char *pucData: Pointer to Send Buffer Start Address
 *                unsigned long ulSize: Block Size
 *  Output:     null
 *  Return:     unsigned char:
 *  Others:     null
 *=====================================================*/
unsigned char xCS485xxWriteBlock(const unsigned char *pucData, 
        unsigned long ulSize)
{
	unsigned long ulIdx = 0;

	//SYS_TRACE("ulSize err", (ulSize%4 == 0));

    vPortEnterCritical();

    if (!vCS485xxStatusCheck(1))
    {
        goto _err;            
    }
    
    vPortCS(0);
    SWWriteSPI(0x80);  
#if 1
    do
    {       
        if (ulIdx < ulSize)
        {
            SWWriteSPI(pucData[ulIdx++]);
            SWWriteSPI(pucData[ulIdx++]);
            SWWriteSPI(pucData[ulIdx++]);
            SWWriteSPI(pucData[ulIdx++]);    
            if (!vCS485xxStatusCheck(1))
            {
                goto _err;            
            }
        }
        else
        {
            if (vCS485xxStatusCheck(1))
            {
                goto _done;            
            }
        }
        SmartDelay_spi(10);        
        //vPortCS(1);
        //vTaskDelay(1/portTICK_RATE_MS);        
    }while(ulIdx < ulSize);
#else
    do
    {           
        vPortCS(0);
        SWWriteSPI(0x80);  
        SWWriteSPI(pucData[ulIdx++]);
        SWWriteSPI(pucData[ulIdx++]);
        SWWriteSPI(pucData[ulIdx++]);
        SWWriteSPI(pucData[ulIdx++]);    
        vPortCS(1);
        if (!vCS485xxStatusCheck(1))
        {
            goto _err;            
        }        
        //SmartDelay_spi(10);
        //vTaskDelay(1/portTICK_RATE_MS);        
    }while(ulIdx < ulSize);

#endif

_done:    
    vPortCS(1);
#if 1    
    vTaskDelay(3/portTICK_RATE_MS);
//    SmartDelay_spi(1000);

#endif   
   
    //SYS_TRACE("CS485xx write %d\r\n", ulIdx);		
    vPortExitCritical();
    return true;

_err:
    vPortCS(1);
    SYS_TRACE("vCS485xxStatusCheck Error!\r\n");    
    vPortExitCritical();
    return false;
}

/*======================================================
 *  Function:   xCS485xxReadBlock
 *  Description:  Read Data to DSP Register
 *  Input:     unsigned char *pucData: Pointer to Recv Buffer Start Address
 *                unsigned long ulSize: Block Size
 *  Output:     null
 *  Return:     unsigned char:
 *  Others:     null
 *=====================================================*/
unsigned char xCS485xxReadBlock(unsigned char *pucData, 
        unsigned long ulSize)
{
	unsigned long ulIdx = 0;

	
    vPortEnterCritical();
#if 1
    if (!vCS485xxStatusCheck(1))
    {
        goto _err;            
    }
    if (!vCS485xxStatusCheck(0))
    {
        goto _err;       
    }            
#endif    
    vPortCS(0);
    SWWriteSPI(0x81);  
    do
    {    
        if (ulIdx < ulSize)
        {
            #if 0
            ulRcv = SW_SPI_Read_INT32();
            pucData[ulIdx++] = (unsigned char)(ulRcv>>24);
            pucData[ulIdx++] = (unsigned char)(ulRcv>>16);
            pucData[ulIdx++] = (unsigned char)(ulRcv>>8);
            pucData[ulIdx++] = (unsigned char)ulRcv;         
            #else
            pucData[ulIdx++] = SWReadSPI();
            pucData[ulIdx++] = SWReadSPI();
            pucData[ulIdx++] = SWReadSPI();
            pucData[ulIdx++] = SWReadSPI();    
            #endif
        }
        else
        {
#if 0        
            if (!vCS485xxStatusCheck(0))
            {
                goto _done;       
            }    
            SWReadSPI();  
            SWReadSPI();
            SWReadSPI();
            SWReadSPI();
#else
            goto _done;       
#endif            
        }
        //vTaskDelay(1/portTICK_RATE_MS);        
    }while(1);

_done:    
    vPortCS(1);
    #if 1
  vTaskDelay(3/portTICK_RATE_MS);        
 //   SmartDelay_spi(1000);
    #endif
    vPortExitCritical();
		//SYS_TRACE("CS485xx read %d\r\n", ulIdx);
    return true;

_err:
    vPortCS(1);    
    vPortExitCritical();
		SYS_TRACE("CS485xx read overtime!\r\n");
    return false;
}





unsigned char xCS485xxWriteCmd(unsigned long ulData)
{
    unsigned char ucBuf[4];

    ucBuf[0] = (unsigned char)(ulData >> 24);
    ucBuf[1] = (unsigned char)(ulData >> 16);
    ucBuf[2] = (unsigned char)(ulData >> 8);
    ucBuf[3] = (unsigned char)(ulData);
    if (xCS485xxWriteBlock(ucBuf, 4))
    { 
			 // SYS_TRACE("CS485xx write %08x\n", ulData);
        return true;
    }            
    return false;
}

/*======================================================
 *  Function:   vPortCS485xxRead
 *  Description:  Read Data to DSP Register
 *  Input:     null
 *  Output:     null
 *  Return:     unsigned long: Register Value
 *  Others:     null
 *=====================================================*/
unsigned long xCS485xxReadCmd(void)
{
    unsigned char ucBuf[4];
	unsigned long msg;

    if (xCS485xxReadBlock(ucBuf, 4))
    {
        msg = ((ucBuf[0]<<24)+(ucBuf[1]<<16)+(ucBuf[2]<<8)+ucBuf[3]);
                 
        //SYS_TRACE("CS485xx read %08x\n", msg);
        return (msg);   
    }
    return false;
}


/*======================================================
 *  Function:   Dsp0_WriteRegister
 *  Description:  Write Datas from DSP Register
 *  Input:     UINT32 ulAddr: 
 *                UINT32 ulData: 
 *  Output:     null
 *  Return:     null
 *  Others:     null
 *=====================================================*/
unsigned char xCS485xxWriteRegister(unsigned long ulAddr, unsigned long ulData)
{  
    
        unsigned char ucBuf[8];
        
        ucBuf[0] = (unsigned char)(ulAddr >> 24);
        ucBuf[1] = (unsigned char)(ulAddr >> 16);
        ucBuf[2] = (unsigned char)(ulAddr >> 8);
        ucBuf[3] = (unsigned char)(ulAddr);
        ucBuf[4] = (unsigned char)(ulData >> 24);
        ucBuf[5] = (unsigned char)(ulData >> 16);
        ucBuf[6] = (unsigned char)(ulData >> 8);
        ucBuf[7] = (unsigned char)(ulData);
        
        if (xCS485xxWriteBlock(ucBuf, 8))
        {
            return true;
        }
    
        //vTaskDelay(100/portTICK_RATE_MS);
        return false;
        
      
    }


unsigned char xCS485xxReadRegSolicited(unsigned long ulReg, unsigned long * pulRcv)
{
    unsigned long ulRcv[2];
    unsigned char ucBuf[8];
    unsigned char i = 0;
    
    ucBuf[0] = (unsigned char)(ulReg >> 24);
    ucBuf[1] = (unsigned char)(ulReg >> 16);
    ucBuf[2] = (unsigned char)(ulReg >> 8);
    ucBuf[3] = (unsigned char)(ulReg);

    //
    // Repeat some times if receive data is not match with sended
    //
    do
    {
        if (i++ > 3)
            goto _err;
    
        //xCS485xxWriteBlock(ucBuf, 4);
        //xCS485xxReadBlock(ucBuf, 8);

        xCS485xxWriteBlock(ucBuf, 4);
        memset(ucBuf, 0, 8);
        xCS485xxReadBlock(ucBuf, 8);
            
        ulRcv[0] = ((ucBuf[0]<<24)+(ucBuf[1]<<16)+(ucBuf[2]<<8)+ucBuf[3]);
        ulRcv[1] = ((ucBuf[4]<<24)+(ucBuf[5]<<16)+(ucBuf[6]<<8)+ucBuf[7]);  

        //UART_DEBUGF(DSP_DEBUG|TIME_DEBUG, ("xCS485xx read register %08x = %08x, %08x\n", ulReg, ulRcv[0], ulRcv[1]));                                    
    }
    while(((ulReg & 0x00FFFFFF) != (ulRcv[0] & 0x00FFFFFF)));

    //UART_DEBUGF(DSP_DEBUG|TIME_DEBUG, ("CS485xx solicited register %08x success!\n", ulReg));                        	
    *pulRcv = ulRcv[1];
    
	return true;	    

_err:
  
    //SYS_TRACE("CS485xx solicited register %08x error!!!\n", ulReg);		
    *pulRcv = 0xFFFFFFFF;
    return false;
}

/*======================================================
 *  Function:   xCS485xxReadRegUnsolicited
 *  Description:  Read Register in DSP without send address
 *  Input:     null
 *  Output:     null
 *  Return:     unsigned char: true/false
 *  Others:     null
 *=====================================================*/
unsigned char xCS485xxReadRegUnsolicited(void)
{
    unsigned long ulMsg[2];
    unsigned char ucBuf[8];

    if (!xCS485xxReadBlock(ucBuf, 8))
        return false;
        
    ulMsg[0] = ((ucBuf[0]<<24)+(ucBuf[1]<<16)+(ucBuf[2]<<8)+ucBuf[3]);
    ulMsg[1] = ((ucBuf[4]<<24)+(ucBuf[5]<<16)+(ucBuf[6]<<8)+ucBuf[7]);
                             
		(void)ulMsg;
    //SYS_TRACE("CS485xx read register %08x = %08x\n", ulMsg[0], ulMsg[1]);
    return true;
}




unsigned long Dsp0_ReadCmd(void)
{
    return xCS485xxReadCmd();
}

unsigned char Dsp0_WriteRegister(unsigned long ulAddr, unsigned long ulData)
{
	return xCS485xxWriteRegister(ulAddr,ulData);
}

unsigned char Dsp0_ReadRegSolicited(unsigned long ulReg, unsigned long * pulRcv)
{
	return xCS485xxReadRegSolicited(ulReg, pulRcv);
}

unsigned char Dsp0_ReadRegUnsolicited(void)
{
	return xCS485xxReadRegUnsolicited();
}





int xCS485xxSlaveBoot(void)
{
    unsigned char i;
    unsigned char ucCnt;
    unsigned long ulAddr;
    unsigned long ulData;

    //
    // Set Operation Mode
    //
    vTaskDelay(100/portTICK_RATE_MS);
    vPortReset(0);
    vPortCS(1);
    vTaskDelay(100/portTICK_RATE_MS);
    vPortReset(1);
    vTaskDelay(100/portTICK_RATE_MS);
   
	  SYS_TRACE("Hardware Reset, start boot...\n");

    //vPortEnterCritical();
    
    //
    // Send Customer ULD Files
    //
    if (!xCS485xxWriteCmd(SLAVE_BOOT))
        return -1;
        
    if (xCS485xxReadCmd() != BOOT_START)
    {
        
			  SYS_TRACE("SLAVE_BOOT ack error!\n");
        return -1;    
    }
    
    if (!xCS485xxWriteBlock(OS_P4_48520_01_01_RC79_ULD, Bytes_of_os_p4_48520_01_01_rc79_uld))
        return -1;
        
    if (xCS485xxReadCmd() != BOOT_SUCCESS) 
    {
			  SYS_TRACE("OS uld ack error!\n");
        return -1;
    }
    vTaskDelay(100/portTICK_RATE_MS);

#if 1
    //
    // Send OS ULD Files
    //
    if (!xCS485xxWriteCmd(SLAVE_BOOT))
        return -1;
        
    if (xCS485xxReadCmd() != BOOT_START)
    {
			  SYS_TRACE("SLAVE_BOOT ack error!\n");
        return -1;    
    }
    
    if (!xCS485xxWriteBlock(TRIADRACKAMP700_P4_48520_01_0100_ULD, Bytes_of_TriadRackAmp700_p4_48520_01_0100_uld))
        return -1;
        
    if (xCS485xxReadCmd() != BOOT_SUCCESS) 
    {
			  SYS_TRACE("App uld ack error!\n");
        return -1;
    }
    vTaskDelay(100/portTICK_RATE_MS);    
#endif

    //
    // Send Configure Files
    //
    if (!xCS485xxWriteCmd(SOFT_RESET))
        return -1;
        
    if (xCS485xxReadCmd() != APP_START) 
    {
			  SYS_TRACE("SOFT_RESET ack error!\n");
        return -1;
    }
    //UART_DEBUGF(AUDIO_DEBUG|TIME_DEBUG, ("App start"));
    vTaskDelay(100/portTICK_RATE_MS);    
    
    if (!xCS485xxWriteBlock(PREKICKSTART_CFG, Bytes_of_preKickStart_cfg))
        return -1;
    vTaskDelay(10/portTICK_RATE_MS);       
    
    if (!xCS485xxWriteBlock(INITIAL_CFG, Bytes_of_initial_cfg))
        return -1;        
        
    vTaskDelay(10/portTICK_RATE_MS);       
    if (!xCS485xxWriteBlock(KICKSTART_CFG, Bytes_of_KickStart_cfg))
        return -1;        

    //vPortExitCritical();
    
    vTaskDelay(500/portTICK_RATE_MS);
    if (!vCS485xxStatusCheck(1))
    {
			  SYS_TRACE("DSP Busy... failed\n");
        return -1;
    }

#if 1    
    //
    // Try to read one register to ensure boot success
    //
    ulAddr = 0xf0c00001;
    ucCnt = 0;
		SYS_TRACE("Try to check %8x to ensure boot success...\n", ulAddr);
    for (i = 0; i < 200; i++)
    {
        ucCnt++;    
        //if (!xCS485xxWriteRegister(ulAddr, 1))
        {
        //    UART_DEBUGF(ERROR_DEBUG|TIME_DEBUG, ("Write %d times error occur...\n", ucCnt));
        //    return false;
        }
        if (!xCS485xxReadRegSolicited(ulAddr, &ulData))
        {
					  SYS_TRACE("Read %d times error occur...\n", ucCnt);
            return -1;
        }
    }
#else
    //xCS485xxReadRegUnsolicited();
#endif
    SYS_TRACE("Boot success\n");
    return 0;
}

int dsp_boot_init(void)
{
	uint8 j = 0;
	
	while(xCS485xxSlaveBoot() < 0){
		if(++j == 10) break;
	}
	
	if(j == 10){
		return -1;
	}
	
	return 0;
}



