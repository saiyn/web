#include "common_head.h"

#define GPIO_PORTD_CR_R     (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_LOCK_R   (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTF_CR_R     (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_LOCK_R   (*((volatile unsigned long *)0x40025520))

void bsp_gpio_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_LOCK_R = 0x4C4F434B;

	GPIO_PORTF_CR_R = 0x1;
	GPIO_PORTD_CR_R = (1<<7);
	
	/*keyboard realated gpio init*/
	GPIOPinTypeGPIOInput(POWER_KEY_PORT, POWER_KEY_PIN);
	GPIOIntTypeSet(POWER_KEY_PORT, POWER_KEY_PIN, GPIO_RISING_EDGE);
	
	
	GPIOPinTypeGPIOInput(ENTER_KEY_PORT, ENTER_KEY_PIN);
	GPIOPinTypeGPIOInput(BACK_KEY_PORT, BACK_KEY_PIN);
	GPIOPinTypeGPIOInput(LEFT_KEY_PORT, LEFT_KEY_PIN);
	GPIOPinTypeGPIOInput(RIGHT_KEY_PORT, RIGHT_KEY_PIN);
	GPIOPinTypeGPIOInput(UP_KEY_PORT, UP_KEY_PIN);
	GPIOPinTypeGPIOInput(DOWN_KEY_PORT, DOWN_KEY_PIN);
	
	/*LCD realated gpio init*/
	GPIOPinTypeGPIOOutput(LCD_RW_PORT, LCD_RW_PIN);
	GPIOPinTypeGPIOOutput(LCD_RS_PORT, LCD_RS_PIN);
	GPIOPinTypeGPIOOutput(LCD_EN_PORT, LCD_EN_PIN);
	GPIOPinTypeGPIOOutput(LCD_DB4_PORT, LCD_DB4_PIN);
	GPIOPinTypeGPIOOutput(LCD_DB5_PORT, LCD_DB5_PIN);
	GPIOPinTypeGPIOOutput(LCD_DB6_PORT, LCD_DB6_PIN);
	GPIOPinTypeGPIOOutput(LCD_DB7_PORT, LCD_DB7_PIN);
	
	/*LED realated gpio init*/
	GPIOPinTypeGPIOOutput(BLUE_LED_PORT, BLUE_LED_PIN);
	GPIOPinTypeGPIOOutput(RED_LED_PORT, RED_LED_PIN);
	RLED_SET(1);
	
	/*power management gpio init*/
	GPIOPinTypeGPIOOutput(DC5V_EN_PORT, DC5V_EN_PIN);
	GPIOPinTypeGPIOOutput(AC_STANDBY_PORT, AC_STANDBY_PIN);
	DC5V_OFF();
	AC_STANDBY(0);
	
	/*eeprom releated gpio init*/
  GPIOPinTypeGPIOOutput(EPROM_SDA_PORT, EPROM_SDA_PIN);
	GPIOPinTypeGPIOOutput(EPROM_SCL_PORT, EPROM_SCL_PIN);
	
	/*IR related gpio init*/
	GPIOPinInputPu(FRONT_IR_PORT, FRONT_IR_PIN);
	GPIOIntTypeSet(FRONT_IR_PORT, FRONT_IR_PIN, GPIO_BOTH_EDGES);
	IntEnable(FRONT_IR_IRQ);
	GPIOIntEnable(FRONT_IR_PORT, FRONT_IR_PIN);
	
	GPIOPinInputPu(IR_IN_PORT, IR_IN_PIN);
	GPIOIntTypeSet(IR_IN_PORT, IR_IN_PIN, GPIO_BOTH_EDGES);
	IntEnable(IR_IN_IRQ);
	GPIOIntEnable(IR_IN_PORT, IR_IN_PIN);
	
	GPIOPinTypeGPIOOutput(IR_OUT_PORT, IR_OUT_PIN);
	GPIOPinTypeGPIOInput(IR_IN_DETECT_PORT, IR_IN_DETECT_PIN);
	
	
	
	/*NJW1195 related gpio init*/
	GPIOPinTypeGPIOOutput(NJW1195_DATA_PORT, NJW1195_DATA_PIN);
	GPIOPinTypeGPIOOutput(NJW1195_CLK_PORT, NJW1195_CLK_PIN);
	GPIOPinTypeGPIOOutput(NJW1195_CS_PORT, NJW1195_CS_PIN);
  GPIO_PIN_SET(NJW1195_CS_PORT, NJW1195_CS_PIN, 1);
	
	/*cs4251 related gpio init*/
	GPIOPinTypeGPIOOutput(CS4251_MOSI_PORT, CS4251_MOSI_PIN);
	GPIOPinTypeGPIOInput(CS4251_MISO_PORT, CS4251_MISO_PIN);
	GPIOPinTypeGPIOOutput(CS4251_CLK_PORT, CS4251_CLK_PIN);
	GPIOPinTypeGPIOOutput(CS4251_CS_PORT, CS4251_CS_PIN);
	GPIOPinTypeGPIOOutput(CS4251_RST_PORT, CS4251_RST_PIN);
	GPIO_PIN_SET(CS4251_CS_PORT, CS4251_CS_PIN, 1);
	GPIO_PIN_SET(CS4251_RST_PORT, CS4251_RST_PIN, 0);
	
	/*CS48520 related gpio init*/
	GPIOPinTypeGPIOOutput(CS48520_RST_PORT,CS48520_RST_PIN); 
  GPIOPinTypeGPIOInput(CS48520_BUSY_PORT,CS48520_BUSY_PIN); //Busy
  GPIOPinTypeGPIOInput(CS48520_IRQ_PORT, CS48520_IRQ_PIN);   //Irq
  GPIOPinTypeGPIOOutput(CS48520_CS_PORT, CS48520_CS_PIN);     //CS
	GPIO_PIN_SET(CS48520_CS_PORT, CS48520_CS_PIN, 1);
	GPIOPinTypeGPIOOutput(CS48520_MOSI_PORT, CS48520_MOSI_PIN);
	GPIOPinTypeGPIOInput(CS48520_MISO_PORT, CS48520_MISO_PIN);
	GPIOPinTypeGPIOOutput(CS48520_CLK_PORT, CS48520_CLK_PIN);
	
	/*Detect related gpio init*/
	GPIOPinTypeGPIOInput(AUDIO_SENSE_PORT, AUDIO_SENSE_PIN);
	GPIOPinTypeGPIOInput(TRIGGER_IN_PORT, TRIGGER_IN_PIN);
	GPIOPinTypeGPIOInput(OVER_TEMP_PORT, OVER_TEMP_PIN);
	GPIOPinTypeGPIOInput(OVER_VOLTAGE_OR_SHORT_PORT, OVER_VOLTAGE_OR_SHORT_PIN);
	GPIOPinTypeGPIOInput(OVER_CLIP_PORT, OVER_CLIP_PIN);
	GPIOPinTypeGPIOInput(OVER_DC_ERROR_PORT, OVER_DC_ERROR_PIN);
	GPIOPinTypeGPIOInput(OVER_CURRENT_PORT, OVER_CURRENT_PIN);
	GPIOPinTypeGPIOInput(OVER_15V_PORT, OVER_15V_PIN);
	
	/*AMP control related gpio*/
	GPIOPinTypeGPIOOutput(AMP_EN_PORT, AMP_EN_PIN); 
	GPIOPinTypeGPIOOutput(AMP_MUTE_PORT, AMP_MUTE_PIN); 
	
	/*pcm1804 gpio init*/
	GPIOPinTypeGPIOOutput(PCM1804_RST_PORT, PCM1804_RST_PIN);
//	GPIOPinTypeGPIOOutput(PCM1804_FS0_PORT, PCM1804_FS0_PIN);
//	GPIOPinTypeGPIOOutput(PCM1804_FS1_PORT, PCM1804_FS1_PIN);
//	GPIO_PIN_SET(PCM1804_FS0_PORT, PCM1804_FS0_PIN, 0);
//	GPIO_PIN_SET(PCM1804_FS1_PORT, PCM1804_FS1_PIN, 0);
	
	/*lineout switch related gpio init*/
	GPIOPinTypeGPIOOutput(RCA_OUT_S_PORT, RCA_OUT_S_PIN);
	GPIOPinTypeGPIOOutput(BAN_OUT_S_PORT, BAN_OUT_S_PIN);
	
	/*pop noise control gpio init*/
	GPIOPinTypeGPIOOutput(RCA_OUT_MUTE_PORT, RCA_OUT_MUTE_PIN);
	GPIOPinTypeGPIOOutput(BAN_OUT_MUTE_PORT, BAN_OUT_MUTE_PIN);
	GPIOPinTypeGPIOOutput(OUTPUT_MUTE_M_PORT, OUTPUT_MUTE_M_PIN);
	OUTPUT_M_MUTE_OFF();
	RCA_OUT_MUTE_ON();
	BAN_OUT_MUTE_ON();
}




void bsp_timer1_init(void)
{
	/*timer1 100us*/ 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER1_BASE, TIMER_A, gSysClock/10000);   

	IntEnable(INT_TIMER1A);
	
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	
	TimerEnable(TIMER1_BASE, TIMER_A);
	
}

void bsp_timer0_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xffffffff);

	TimerEnable(TIMER0_BASE, TIMER_A);
}

uint32 bsp_timer0_get_time(void)
{
	  return TimerValueGet(TIMER0_BASE, TIMER_A)/TIME0_DIVISION;	
}

void bsp_pwm0_init(void)
{
	/*Enable device*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	/*Set clock divider*/
  PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_64);
	/*Enable PWM pin*/
  GPIOPinConfigure(LCD_PWM_CHANNEL);
  GPIOPinTypePWM(LCD_PWM_PORT, LCD_PWM_PIN);
	/*Configure PWM generator*/
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0,(PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC));
	/*Set PWM timer period*/
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0,gSysClock/10000);
	/*Set width for PWM0*/
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 50*PWMGenPeriodGet(PWM0_BASE,PWM_GEN_0)/100);
	/*Enable output*/
  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 0);
  /*Enable Generator*/
  PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void bsp_pwm_for_sense_init(void)
{
  /*Enable device*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  /*Set clock divider*/
  PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_1);  
  /*Enable PWM pin*/
  GPIOPinConfigure(GPIO_PK5_M0PWM7);
  GPIOPinTypePWM(SENSE_THRES_PORT, SENSE_THRES_PIN);
  /*Configure PWM generator*/
  PWMGenConfigure(PWM0_BASE, PWM_GEN_3,(PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC));
  /*Set PWM timer period*/
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3,gSysClock/1000000);   
  /*Set width for PWM0*/
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 1*PWMGenPeriodGet(PWM0_BASE,PWM_GEN_3)/5);
  /*ensable output*/
  PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, 1);
  /*Enable Generator*/
  PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	
}

void bsp_pwm_for_sense_set(uint16 x, uint16 y)
{
	 if(x <= y){
		 PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, x*PWMGenPeriodGet(PWM0_BASE,PWM_GEN_3)/y);
	 }
}



void bsp_lcd_bright_control(uint8 duty)
{
	 if(duty){
		  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 1);
		  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, duty*PWMGenPeriodGet(PWM0_BASE,PWM_GEN_0)/100);
	 }else{
		  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, 0);
	 }
}

extern void KeyScanLoop(void);
extern void ir_timeout_check(void);

void Timer1IntHandler(void)
{
	static uint16 cnt = 0;
	
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

  if(cnt++ == 500){
		 KeyScanLoop();
		cnt = 0;
	}
	
	ir_timeout_check();
}


void s_printf(const char *fmt, ...)
{
	    va_list ap;
	    char buf[512] = {0};
      size_t len;
      size_t index = 0;

      va_start(ap, fmt);
	    vsprintf(buf, fmt, ap);
      va_end(ap);

      len = strlen(buf);
      while(index < len){
				while(UARTCharPutNonBlocking(UART0_BASE, buf[index]) == false);
				index++;
			}
}

UINT8 GPIO_ReadSinglePin(UINT32 ulPort, UINT8 ucPins)
{
	if ((HWREG(ulPort + (GPIO_O_DATA + (ucPins << 2)))) != 0)
		return 1;
	else
		return 0;
}


void bsp_delay_us(uint32 time)
{
	SysCtlDelay(40 * time);
}


void bsp_delay_ms(uint32 time)
{
	 for(;time;time--)
		 bsp_delay_us(1000);
}

void GPIOPinInputPu(UINT32 ulPort, UINT8 ucPins)
{
	GPIOPinTypeGPIOInput(ulPort, ucPins);
	HWREG(ulPort + GPIO_O_PUR) |= ucPins;
}


