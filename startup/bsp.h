#ifndef _BSP_H_
#define _BSP_H_


/*keyboard releated gpio*/
#define POWER_KEY_PORT       GPIO_PORTE_BASE
#define POWER_KEY_PIN        GPIO_PIN_2
#define INT_KEY_POWER        INT_GPIOE

#define IS_POWER_KEY_DOWN()   (GPIO_ReadSinglePin(POWER_KEY_PORT, POWER_KEY_PIN) == 1? 1:0)

#define ENTER_KEY_PORT       GPIO_PORTE_BASE
#define ENTER_KEY_PIN        GPIO_PIN_1
#define INT_KEY_ENTER        INT_GPIOE

#define BACK_KEY_PORT       GPIO_PORTE_BASE
#define BACK_KEY_PIN        GPIO_PIN_0
#define INT_KEY_BACK        INT_GPIOE

#define LEFT_KEY_PORT       GPIO_PORTK_BASE
#define LEFT_KEY_PIN        GPIO_PIN_0
#define INT_KEY_LEFT        INT_GPIOK

#define RIGHT_KEY_PORT      GPIO_PORTK_BASE
#define RIGHT_KEY_PIN       GPIO_PIN_1
#define INT_KEY_RIGHT       INT_GPIOK

#define UP_KEY_PORT         GPIO_PORTK_BASE
#define UP_KEY_PIN          GPIO_PIN_2
#define INT_KEY_UP          INT_GPIOK

#define DOWN_KEY_PORT       GPIO_PORTK_BASE
#define DOWN_KEY_PIN        GPIO_PIN_3
#define INT_KEY_DOWN        INT_GPIOK

/*lcd releated gpio*/
#define LCD_RW_PORT 			GPIO_PORTQ_BASE
#define LCD_RW_PIN 			 	GPIO_PIN_0

#define LCD_RS_PORT 			GPIO_PORTQ_BASE
#define LCD_RS_PIN 			 	GPIO_PIN_1

#define LCD_EN_PORT 			GPIO_PORTD_BASE
#define LCD_EN_PIN  			GPIO_PIN_3

#define PORT_LCD_DATA     GPIO_PORTD_BASE

#define LCD_DB4_PORT GPIO_PORTD_BASE
#define LCD_DB4_PIN  GPIO_PIN_2

#define LCD_DB5_PORT GPIO_PORTD_BASE
#define LCD_DB5_PIN  GPIO_PIN_0

#define LCD_DB6_PORT GPIO_PORTD_BASE
#define LCD_DB6_PIN  GPIO_PIN_7

#define LCD_DB7_PORT GPIO_PORTD_BASE
#define LCD_DB7_PIN  GPIO_PIN_5

#define LCD_PWM_PORT GPIO_PORTF_BASE
#define LCD_PWM_PIN  GPIO_PIN_0
#define LCD_PWM_CHANNEL  GPIO_PF0_M0PWM0


/*power management related gpio*/
#define DC5V_EN_PORT  GPIO_PORTN_BASE
#define DC5V_EN_PIN   GPIO_PIN_2
#define DC5V_ON()     GPIO_PIN_SET(DC5V_EN_PORT, DC5V_EN_PIN, 1)
#define DC5V_OFF()    GPIO_PIN_SET(DC5V_EN_PORT, DC5V_EN_PIN, 0)

#define AC_STANDBY_PORT GPIO_PORTN_BASE
#define AC_STANDBY_PIN  GPIO_PIN_1
#define AC_STANDBY(x)    GPIO_PIN_SET(AC_STANDBY_PORT, AC_STANDBY_PIN, x)


/*LED related gpio*/
#define BLUE_LED_PORT   GPIO_PORTC_BASE
#define BLUE_LED_PIN    GPIO_PIN_7
#define RED_LED_PORT    GPIO_PORTC_BASE
#define RED_LED_PIN     GPIO_PIN_6

#define BLED_SET(x) do{ \
	GPIO_PIN_SET(BLUE_LED_PORT, BLUE_LED_PIN, !x); \
	GPIO_PIN_SET(RED_LED_PORT, RED_LED_PIN, x); \
}while(0)

#define RLED_SET(x) do{ \
	GPIO_PIN_SET(BLUE_LED_PORT, BLUE_LED_PIN, x); \
	GPIO_PIN_SET(RED_LED_PORT, RED_LED_PIN, !x); \
}while(0)

#define LED_OFF() do{ \
	GPIO_PIN_SET(BLUE_LED_PORT, BLUE_LED_PIN, 1); \
	GPIO_PIN_SET(RED_LED_PORT, RED_LED_PIN, 1); \
}while(0)

/*EPROM related gpio*/
#define EPROM_SDA_PORT   GPIO_PORTB_BASE
#define EPROM_SDA_PIN    GPIO_PIN_3
#define EPROM_SCL_PORT   GPIO_PORTB_BASE
#define EPROM_SCL_PIN    GPIO_PIN_2

/*IR related gpio*/
#define TIME0_DIVISION 12000
#define FRONT_IR_PORT    GPIO_PORTQ_BASE
#define FRONT_IR_PIN     GPIO_PIN_2
#define FRONT_IR_IRQ     INT_GPIOQ2
#define IR_IN_PORT       GPIO_PORTH_BASE
#define IR_IN_PIN        GPIO_PIN_1
#define IR_OUT_PORT      GPIO_PORTH_BASE
#define IR_OUT_PIN       GPIO_PIN_3
#define IR_IN_IRQ        INT_GPIOH
#define IR_IN_DETECT_PORT      GPIO_PORTF_BASE
#define IR_IN_DETECT_PIN       GPIO_PIN_3
#define IS_IR_JACK_IN()    (GPIO_ReadSinglePin(IR_IN_DETECT_PORT, IR_IN_DETECT_PIN) == 1? 1:0)

/*njw1195 related gpio*/
#define NJW1195_DATA_PORT  GPIO_PORTE_BASE
#define NJW1195_DATA_PIN   GPIO_PIN_4
#define NJW1195_CLK_PORT   GPIO_PORTB_BASE
#define NJW1195_CLK_PIN    GPIO_PIN_5
#define NJW1195_CS_PORT    GPIO_PORTN_BASE
#define NJW1195_CS_PIN     GPIO_PIN_0

/*CS4251 related gpio*/
#define CS4251_MOSI_PORT  GPIO_PORTE_BASE
#define CS4251_MOSI_PIN   GPIO_PIN_4
#define CS4251_MISO_PORT  GPIO_PORTP_BASE
#define CS4251_MISO_PIN   GPIO_PIN_4
#define CS4251_CLK_PORT   GPIO_PORTB_BASE
#define CS4251_CLK_PIN    GPIO_PIN_5
#define CS4251_CS_PORT    GPIO_PORTP_BASE
#define CS4251_CS_PIN     GPIO_PIN_3
#define CS4251_RST_PORT   GPIO_PORTQ_BASE
#define CS4251_RST_PIN    GPIO_PIN_4

/*CS48520 related gpio*/
#define CS48520_RST_PORT  GPIO_PORTM_BASE
#define CS48520_RST_PIN   GPIO_PIN_1
#define CS48520_CS_PORT   GPIO_PORTB_BASE
#define CS48520_CS_PIN    GPIO_PIN_4
#define CS48520_BUSY_PORT GPIO_PORTM_BASE
#define CS48520_BUSY_PIN  GPIO_PIN_0
#define CS48520_IRQ_PORT  GPIO_PORTL_BASE
#define CS48520_IRQ_PIN   GPIO_PIN_0
#define CS48520_MOSI_PORT  GPIO_PORTE_BASE
#define CS48520_MOSI_PIN   GPIO_PIN_4
#define CS48520_MISO_PORT  GPIO_PORTE_BASE
#define CS48520_MISO_PIN   GPIO_PIN_5
#define CS48520_CLK_PORT   GPIO_PORTB_BASE
#define CS48520_CLK_PIN    GPIO_PIN_5

/*Detect related gpio*/
#define AUDIO_SENSE_PORT   GPIO_PORTK_BASE
#define AUDIO_SENSE_PIN    GPIO_PIN_7
#define TRIGGER_IN_PORT    GPIO_PORTL_BASE
#define TRIGGER_IN_PIN     GPIO_PIN_5
#define SENSE_THRES_PORT   GPIO_PORTK_BASE
#define SENSE_THRES_PIN    GPIO_PIN_5
#define OVER_TEMP_PORT     GPIO_PORTP_BASE
#define OVER_TEMP_PIN      GPIO_PIN_1
#define OVER_VOLTAGE_OR_SHORT_PORT   GPIO_PORTP_BASE
#define OVER_VOLTAGE_OR_SHORT_PIN    GPIO_PIN_0
#define OVER_CLIP_PORT     GPIO_PORTJ_BASE
#define OVER_CLIP_PIN      GPIO_PIN_1
#define OVER_DC_ERROR_PORT   GPIO_PORTN_BASE
#define OVER_DC_ERROR_PIN    GPIO_PIN_4
#define OVER_CURRENT_PORT   GPIO_PORTN_BASE
#define OVER_CURRENT_PIN    GPIO_PIN_3
#define OVER_15V_PORT       GPIO_PORTL_BASE
#define OVER_15V_PIN        GPIO_PIN_4

#define IS_OVER_CURRENT  (GPIO_ReadSinglePin(OVER_CURRENT_PORT, OVER_CURRENT_PIN) == 1? 1:0)
#define IS_SHORT_CIRCUIT (GPIO_ReadSinglePin(OVER_VOLTAGE_OR_SHORT_PORT, OVER_VOLTAGE_OR_SHORT_PIN) == 0? 1:0)
#define IS_AUDIO_VALID   (GPIO_ReadSinglePin(AUDIO_SENSE_PORT, AUDIO_SENSE_PIN) == 0? 0:1)
#define IS_TRIGGER_VALID (GPIO_ReadSinglePin(TRIGGER_IN_PORT, TRIGGER_IN_PIN) == 0? 1:0)
#define IS_POWER_READY   (GPIO_ReadSinglePin(OVER_15V_PORT, OVER_15V_PIN) == 1? 1:0)
#define IS_OVER_TEMP     (GPIO_ReadSinglePin(OVER_TEMP_PORT, OVER_TEMP_PIN) == 1? 1:0)
#define IS_DC_ERROR      (GPIO_ReadSinglePin(OVER_DC_ERROR_PORT, OVER_DC_ERROR_PIN) == 1? 1:0)
#define IS_OVER_VOLTAGE  (GPIO_ReadSinglePin(OVER_VOLTAGE_OR_SHORT_PORT, OVER_VOLTAGE_OR_SHORT_PIN) == 1? 1:0)

/*AMP control related gpio*/
#define AMP_EN_PORT        GPIO_PORTJ_BASE
#define AMP_EN_PIN         GPIO_PIN_0
#define AMP_MUTE_PORT      GPIO_PORTN_BASE
#define AMP_MUTE_PIN       GPIO_PIN_5

#define AMP_ENABLE()       GPIO_PIN_SET(AMP_EN_PORT, AMP_EN_PIN, 1)
#define AMP_DISENABLE()    GPIO_PIN_SET(AMP_EN_PORT, AMP_EN_PIN, 0)
#define AMP_MUTE_ON()      GPIO_PIN_SET(AMP_MUTE_PORT, AMP_MUTE_PIN, 0)
#define AMP_MUTE_OFF()     GPIO_PIN_SET(AMP_MUTE_PORT, AMP_MUTE_PIN, 1)

/*PCM1804 related gpio*/
#define PCM1804_RST_PORT   GPIO_PORTF_BASE
#define PCM1804_RST_PIN    GPIO_PIN_4
#define PCM1804_RST(x)     GPIO_PIN_SET(PCM1804_RST_PORT, PCM1804_RST_PIN, x)
//#define PCM1804_FS0_PORT   GPIO_PORTN_BASE
//#define PCM1804_FS0_PIN    GPIO_PIN_0
//#define PCM1804_FS1_PORT   GPIO_PORTP_BASE
//#define PCM1804_FS1_PIN    GPIO_PIN_5

/*Lineout switch related gpio*/
#define RCA_OUT_S_PORT     GPIO_PORTA_BASE
#define RCA_OUT_S_PIN      GPIO_PIN_6
#define BAN_OUT_S_PORT     GPIO_PORTA_BASE
#define BAN_OUT_S_PIN      GPIO_PIN_7

#define RCA_OUT_BYPASS()    GPIO_PIN_SET(RCA_OUT_S_PORT, RCA_OUT_S_PIN, 0)
#define RCA_OUT_DSP()      GPIO_PIN_SET(RCA_OUT_S_PORT, RCA_OUT_S_PIN, 1)

#define BAN_OUT_BYPASS()    GPIO_PIN_SET(BAN_OUT_S_PORT, BAN_OUT_S_PIN, 0)
#define BAN_OUT_DSP()       GPIO_PIN_SET(BAN_OUT_S_PORT, BAN_OUT_S_PIN, 1)


/*pop noise control gpio*/
#define RCA_OUT_MUTE_PORT   GPIO_PORTF_BASE
#define RCA_OUT_MUTE_PIN    GPIO_PIN_2
#define BAN_OUT_MUTE_PORT   GPIO_PORTF_BASE
#define BAN_OUT_MUTE_PIN    GPIO_PIN_1
#define OUTPUT_MUTE_M_PORT  GPIO_PORTG_BASE
#define OUTPUT_MUTE_M_PIN   GPIO_PIN_0

#define RCA_OUT_MUTE_ON()    GPIO_PIN_SET(RCA_OUT_MUTE_PORT, RCA_OUT_MUTE_PIN, 0)
#define RCA_OUT_MUTE_OFF()   GPIO_PIN_SET(RCA_OUT_MUTE_PORT, RCA_OUT_MUTE_PIN, 1)

#define BAN_OUT_MUTE_ON()    GPIO_PIN_SET(BAN_OUT_MUTE_PORT, BAN_OUT_MUTE_PIN, 0)
#define BAN_OUT_MUTE_OFF()   GPIO_PIN_SET(BAN_OUT_MUTE_PORT, BAN_OUT_MUTE_PIN, 1)

#define OUTPUT_M_MUTE_ON()    GPIO_PIN_SET(OUTPUT_MUTE_M_PORT, OUTPUT_MUTE_M_PIN, 0)
#define OUTPUT_M_MUTE_OFF()   GPIO_PIN_SET(OUTPUT_MUTE_M_PORT, OUTPUT_MUTE_M_PIN, 1)



#define GPIO_PIN_SET(port,pin,value) do{ \
	if(value){ \
		GPIOPinWrite(port,pin,pin); } \
	else{ \
		GPIOPinWrite(port,pin,0); } \
	}while(0)
	

void bsp_delay_ms(uint32 time);

void bsp_delay_us(uint32 time);

void bsp_gpio_init(void);

void s_printf(const char *fmt, ...);

void bsp_pwm0_init(void);

void bsp_timer1_init(void);
	
void bsp_timer0_init(void);

uint32 bsp_timer0_get_time(void);
	
	
UINT8 GPIO_ReadSinglePin(UINT32 ulPort, UINT8 ucPins);

void bsp_lcd_bright_control(uint8 duty);

void GPIOPinInputPu(UINT32 ulPort, UINT8 ucPins);

void bsp_pwm_for_sense_init(void);

void bsp_pwm_for_sense_set(uint16 x, uint16 y);

#endif






