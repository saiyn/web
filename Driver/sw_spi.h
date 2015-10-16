/*=========================================================================
 *        software spi driver head file
 *        sw_spi.h
 *        Jim.wang
 *        2013.4.23
 *========================================================================*/

#ifndef	_SW_SPI_H_
#define	_SW_SPI_H_

typedef enum
{
	typeDsp0 = 0,
	typeDsp1,
} eTypeDsp;

/****************************** SPI0 port define ************************************/
#define PORT_SPI0CLK	GPIO_PORTB_BASE
#define PIN_SPI0CLK		GPIO_PIN_5

#define PORT_SPI0DO		GPIO_PORTE_BASE
#define PIN_SPI0DO		GPIO_PIN_4
				
#define PORT_SPI0DI		GPIO_PORTE_BASE
#define PIN_SPI0DI		GPIO_PIN_5

/**********************************************************************************/

extern void SpiDelay(uint32 time);
extern void Spi_Init(eTypeDsp ucFlag);
extern unsigned char SWReadSPI(void);
extern unsigned char SWWriteSPI(unsigned char cData);

extern void SmartDelay_spi(unsigned long time);



#endif
