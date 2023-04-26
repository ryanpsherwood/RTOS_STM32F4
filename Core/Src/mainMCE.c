/************************************************************************************************
*   Included Headers							                        *
************************************************************************************************/
#include "cmsis_os.h"
#include "lwip.h"
#include "usb_device.h"
#include "can_net.h"
#include "tcpethernet.h"
#include "timerUtil.h"

/************************************************************************************************
*   Variables - File Scope								        *
************************************************************************************************/
osThreadId canTaskHandle;
//osThreadId ethRxProcessTaskHandle;
//osThreadId ethTxProcessTaskHandle;
osThreadId ethProcessTaskHandle;

uint32_t transmitIncrement = 0;

/************************************************************************************************
*   Function Prototypes - File Scope							        *
************************************************************************************************/
void StartCanTask(void const * argument);

/************************************************************************************************
*   Function Definitions									*
************************************************************************************************/
/************************************************************************************************
* Function: mainMCE                                                             
* Purpose:  Take the main function for the CSC out of main.c so when making changes with
            cube software it won't erase additions made to file.
* Usage:	  
* Input:    N/A
* Output:   N/A	
************************************************************************************************/
void mainMCE(void)
{

  InitTimers();
  /* init code for LWIP */
  MX_LWIP_Init();

  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();
  
  tcp_server_socket_init();

  osThreadDef(canTask, StartCanTask, osPriorityNormal, 0, 512);
  canTaskHandle = osThreadCreate(osThread(canTask), NULL);
  
  //osThreadDef(ethRxProcessTask, StartEthRxProcessTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
  //ethRxProcessTaskHandle = osThreadCreate(osThread(ethRxProcessTask), NULL);
  
  //osThreadDef(ethTxProcessTask, StartEthTxProcessTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
  //ethTxProcessTaskHandle = osThreadCreate(osThread(ethTxProcessTask), NULL);
  
  osThreadDef(ethProcessTask, StartEthProcessTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*4);
  ethProcessTaskHandle = osThreadCreate(osThread(ethProcessTask), NULL);
}

/************************************************************************************************
* Function: StartCanTask                                                             
* Purpose:  Start CAN task that process CAN circular queues 
* Usage:	 
* Input:    void const * argument - not used
* Output:   N/A
************************************************************************************************/
void StartCanTask(void const * argument)
{

  for( ;; )
  {
    ProcessCANQueue();
    osDelay(1);
  }
}

/**
  * @brief This function provides minimum delay (in milliseconds) based 
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(uint32_t Delay)
{
  //The weak defined HAL_Delay uses a systick to keep track of time.
  //Since we are using RTOS I went ahead and used the RTOS delay.
  osDelay(1);
}
