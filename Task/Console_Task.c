#include "common_head.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSAL_Console.h"
#include "OSAL_RingBuf.h"
#include "serial.h"
#include "Display_Task.h"


void console_task(void *parameter)
{
	SYS_TRACE("console task running\r\n");
	ring_buffer_init();
	
	//	Enable UART0 interupt
	IntEnable(INT_UART0);
	while(1){
		serial_poll_handler();
		vTaskDelay(50/portTICK_RATE_MS);
	}
}








