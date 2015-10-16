#ifndef _CS4251X_H_
#define _CS4251X_H_

#define CS4251X_NOIRQ	(-1)


#define SetDaCs(b)			GPIO_PIN_SET(CS4251_CS_PORT, CS4251_CS_PIN, b)
#define SetDaClk(b)			GPIO_PIN_SET(CS4251_CLK_PORT, CS4251_CLK_PIN, b)
#define SetDaRst(b)			GPIO_PIN_SET(CS4251_RST_PORT, CS4251_RST_PIN, b)
#define GetDaIn()				GPIO_ReadSinglePin(CS4251_MISO_PORT, CS4251_MISO_PIN)
#define SetDaOut(b)			GPIO_PIN_SET(CS4251_MOSI_PORT, CS4251_MOSI_PIN, b)


/*
 * Codec clock control
 * see manual pages 53-54, chapter 6.7
 */

/* always use OMCK clock */
#define CS4251X_CLKSRC_OMCK		(1 << 1)
/* always use PLL recovered clk */
#define CS4251X_CLKSRC_PLL		(0 << 1)
/* keep current; on unlock stay with OMCK forever */
#define CS4251X_CLKSRC_CURRENT_OMCK	(2 << 1)
/* use PLL when locked, otherwise OMCK */
#define CS4251X_CLKSRC_AUTO_PLL_OMCK	(3 << 1)
#define CS4251X_CLKSRC_MCK_MASK		(3 << 1)

/* PLL input is SPDIF receiver */
#define CS4251X_CLKSRC_PLL_SPDIF	0
/* PLL input is SAI_LRCLK */
#define CS4251X_CLKSRC_PLL_LRCLK	(1 << 3)	

/* force lock on PLL. CLKSRC_OMCK MUST be used! */
#define CS4251X_CLKSRC_PLL_FORCE_LOCK	(1 << 0)

#define CS4251X_CLKSRC_RMCK_DIV_1	(0 << 7)
#define CS4251X_CLKSRC_RMCK_DIV_2	(1 << 7)
#define CS4251X_CLKSRC_RMCK_DIV_4	(2 << 7)
#define CS4251X_CLKSRC_RMCK_MUL_2	(3 << 7)

#define CS4251X_CLKSRC_OMCK_112896MHZ	(0 << 4)
#define CS4251X_CLKSRC_OMCK_122880MHZ	(0 << 4)
#define CS4251X_CLKSRC_OMCK_169344MHZ	(1 << 4)
#define CS4251X_CLKSRC_OMCK_184320MHZ	(1 << 4)
#define CS4251X_CLKSRC_OMCK_225792MHZ	(2 << 4)
#define CS4251X_CLKSRC_OMCK_245760MHZ	(2 << 4)
#define CS4251X_CLKSRC_OMCK_FMASK	(3 << 4)

/*
 * ADC serial interface flags
 * See manual page 49, (ch 6.4.3)
 */
/* ADC data on CX_SDOUT, clock provided by CODEC_SP,
 * SPDIF data on SAI_SDOUT
 */
#define CS4251X_ADCDAI_CXSDOUT_CODECSP_CLK	(0 << 2)
/* ADC data on CX_SDOUT, clock provided by SAI_SP.
 * SPDIF data on SAI_SDOUT
 */
#define CS4251X_ADCDAI_CXSDOUT_SAISP_CLK	(1 << 2)
/* ADC data on SAI_SDOUT, clock provided by SAI_SP.
 * No SPDIF data
 */
#define CS4251X_ADCDAI_SAISDOUT_SAISP_CLK	(2 << 2)

/** miscctl flags */
/* SAI interface is I2S master/slave */
#define CS4251X_MISC_SAISP_MASTER	(1 << 0)
#define CS4251X_MISC_SAISP_SLAVE	0
/* CODEC_SP interface is I2S master/slave */
#define CS4251X_MISC_CODECSP_MASTER	(1 << 1)
#define CS4251X_MISC_CODECSP_SLAVE	0

#define CS4251X_MISC_HIGHPASS_FREEZE	(1 << 2)

#define CS4251X_MISC_DAC_FAST_ROLLOFF	(1 << 3)
#define CS4251X_MISC_DAC_SLOW_ROLLOFF	0

#define CS4251X_MISC_RMCK_HIGHZ		(1 << 6)

/** CS4251X internal registers **/

/* register 0 does not exist */
#define CS4251X_CHIPID		0x01
#define CS4251X_PM		0x02
#define CS4251X_FUNCMODE	0x03
#define CS4251X_IFFMT		0x04
#define CS4251X_MISCCTL		0x05
#define CS4251X_CLKCTL		0x06
#define CS4251X_PLLOMCKRATIO	0x07
#define CS4251X_RCVSTAT		0x08
#define CS4251X_VOLTRANSCTL	0x0D
#define CS4251X_MUTE		0x0E
#define CS4251X_L1VOL		0x0F
#define CS4251X_R1VOL		0x10
#define CS4251X_L2VOL		0x11
#define CS4251X_R2VOL		0x12
#define CS4251X_L3VOL		0x13
#define CS4251X_R3VOL		0x14
#define CS4251X_L4VOL		0x15
#define CS4251X_R4VOL		0x16
#define CS4251X_LADCGAIN	0x1C
#define CS4251X_RADCGAIN	0x1D
#define CS4251X_RCVMODECTL	0x1E
#define CS4251X_RCVMODECTL2	0x1F
#define CS4251X_IRQSTAT		0x20
#define CS4251X_IRQMASK		0x21
#define CS4251X_RECERR		0x26
#define CS4251X_RECERRMASK	0x27
#define CS4251X_MUTEC		0x28
#define CS4251X_GPIO_0		0x29
/* there are more, but irrelevant for now ;-) */
#define CS4251X_REGNUM		0x52

/*
 * GPIO control
 */
#define CS4251X_RXP_0			7
#define CS4251X_RXP_1			6
#define CS4251X_RXP_2			5
#define CS4251X_RXP_3			4
#define CS4251X_RXP_4			3
#define CS4251X_RXP_5			2
#define CS4251X_RXP_6			1
#define CS4251X_RXP_7			0

#define CS4251X_GPIO_MODE_RXP		0
#define CS4251X_GPIO_MODE_MUTE		1
#define CS4251X_GPIO_MODE_GPOLOW	2
#define CS4251X_GPIO_MODE_GPOHI		3

#define CS4251X_GPIO_MUTEPOL_LOW	0
#define CS4251X_GPIO_MUTEPOL_HIGH	1

/* just use as low-level GPO */
#define CS4251X_FUNCMODE_GPO_DRIVELOW	0
/* pin indicates ADC overflow */
#define CS4251X_FUNCMODE_GPO_OVERFLOW	1

/* in mutemode, a bitmask determines which DACs are muted by which RXP
 * pins.  Please have a look at CS42518_PP5.pdf datasheet, page 71 for
 * the matrix */
#define CS4251X_FUNCMODE_MUTE_MASK	0x1f	

extern unsigned char cs4251x_read(unsigned char reg);
extern int cs4251x_write(unsigned char reg, unsigned char value);
extern int CS4251X_Init(void);
extern void SW_SPI2_Init(void);


extern void test_csadcdac(void);


#endif

