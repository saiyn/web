/***********************************************************************
* Copyright (c) 2013, Hansong (Nanjing) Technology Ltd.
* All rights reserved.
* 
* ----File Info ----------------------------------------------- -----------------------------------------
* Name:  CS485XX.h, 
* Author: Jim.Wang
* Date:   2013/9/27
* Summary:     CS485XX Functions, Include Hardware Drivers and Module Operate
* ----Latest Version----------------------------------------------- ---------------------------------
* Version: V1.00
* Date:    2013/9/27
* Author:  Jim.Wang
* Description: 
***********************************************************************/

#ifndef _CS485XX_H_
#define _CS485XX_H_

#include "sw_spi.h"

#ifndef CS485XX_GLOBALS
#define CS485XX_EXT extern
#else
#define CS485XX_EXT
#endif

/************************** cs48560  port define **********************/
#define PORT_DSP0_RST			GPIO_PORTM_BASE
#define PIN_DSP0_RST			GPIO_PIN_1

#define PORT_DSP0_CS			GPIO_PORTB_BASE
#define PIN_DSP0_CS			    GPIO_PIN_4

#define PORT_DSP0_BUSY		    GPIO_PORTM_BASE
#define PIN_DSP0_BUSY			GPIO_PIN_0

#define PORT_DSP0_IRQ			GPIO_PORTL_BASE
#define PIN_DSP0_IRQ			GPIO_PIN_0
/***********************************************************************/
extern void CS485xxPortInit(void);
extern unsigned char xCS485xxWriteBlock(const unsigned char *pucData, unsigned long ulSize);
extern unsigned char xCS485xxReadBlock(unsigned char *pucData,unsigned long ulSize);
extern unsigned char xCS485xxWriteCmd(unsigned long ulData);
extern unsigned long xCS485xxReadCmd(void);
unsigned char Dsp0_WriteRegister(unsigned long ulAddr, unsigned long ulData);
extern unsigned char xCS485xxWriteRegister(unsigned long ulAddr,unsigned long ulData);
extern unsigned char xCS485xxReadRegSolicited(unsigned long ulReg, unsigned long * pulRcv);
extern unsigned char xCS485xxReadRegUnsolicited(void);
extern int xCS485xxSlaveBoot(void);
extern unsigned char vCS485xxStatusCheck(unsigned char ucBit);
int dsp_boot_init(void);
/***********************************************************************/
#endif
