#include "common_head.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Console_Task.h"
#include "System_Task.h"
#include "serial.h"
#include "Display_Task.h"
#include "web.h"

const char *version = "v0.4.4";


static void hal_init(void);
static void print_version(const char *version);

uint32 gSysClock;

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	  while(1){
			SYS_TRACE("Hardware error\r\n");
		}
}

void vApplicationMallocFailedHook(void) 
{
	   SYS_TRACE("Hardware error vApplicationMallocFailedHook\r\n");
	   while(1){
			
		 }
}

void lwIPHostTimerHandler(void)
{
	
}

int main(void)
{
	hal_init();
	print_version(version);
	xTaskCreate(system_task,(const signed char *)"System", (configMINIMAL_STACK_SIZE*2), NULL, 9, NULL);
	xTaskCreate(display_task,(const signed char *)"display", (configMINIMAL_STACK_SIZE*2), NULL, 10, NULL);
	xTaskCreate(web_task,(const signed char *)"web", (configMINIMAL_STACK_SIZE*8), NULL, 8, NULL);
  xTaskCreate(console_task,(const signed char *)"Console", (configMINIMAL_STACK_SIZE*2), NULL, 3, NULL);
	
	// Start the scheduler
	vTaskStartScheduler();
	return 0;
}

static void hal_init(void)
{
	 gSysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 120000000);
   sys_low_power_handler();	
	 bsp_gpio_init();
	 bsp_uart_init();
	 bsp_pwm0_init();
	 bsp_pwm_for_sense_init();
	 bsp_timer1_init();
	 bsp_timer0_init();
}


static void print_version(const char *version)
{
	s_printf("\r\n***************************************************\r\n");
	s_printf("*      Triad Rock700AMP\r\n");
	s_printf("*      version:%s\r\n",version);
	s_printf("*      Created: %s %s \r\n", __DATE__, __TIME__);
	s_printf("***************************************************\r\n");
}




