/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef TIMERUTIL_H
#define TIMERUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

	
/************************************************************************************************
*   Included Headers                                          *
************************************************************************************************/
#include "main.h"

/************************************************************************************************
*   Enums - Global Scope                                      *
************************************************************************************************/
  enum TimerInst 
  {

    eTimeCanHeartBeat				= 1,
    eTimeEthernetHeartBeat,
    eTimeMax
  };
	
/************************************************************************************************
*   Structures - Global Scope                                 *
************************************************************************************************/	
  typedef struct 
  {
    uint32_t start;
    uint32_t end;
    uint32_t elapsed;
    uint32_t timeout;
  } TimerDataMonitor;
	
/************************************************************************************************
*   Constants - GlobalScope                                   *
************************************************************************************************/
#define ONE_MILLI_SECOND	1
#define ONE_SECOND		( ONE_MILLI_SECOND * 100)
#define TWO_SECONDS		( ONE_SECOND * 2 )
#define FIVE_SECONDS            ( ONE_SECOND * 5 )	
	
/************************************************************************************************
*   Function Prototypes - Global Scope                        *
************************************************************************************************/
/************************************************************************************************
* Function: InitTimers
* Purpose:  Initialize all timers that are used
* Usage:	 
* Input:	 
* Output: 	
************************************************************************************************/
  extern void InitTimers(void);
  
/************************************************************************************************
* Function: StartTimer
* Purpose:  Start timer for the specified timer in seconds
* Usage:	 
* Input:    Enum of timer
* Output: 	
************************************************************************************************/
  extern void StartTimer(unsigned char timerEnum);
  
/************************************************************************************************
* Function: IsTimerElapsed
* Purpose:  Checks if specified timer has elapsed its timeout
* Usage:	 
* Input:    Enum of timer
* Output: 	Returns one if elapsed, zero if not
************************************************************************************************/
  extern unsigned char IsTimerElapsed(unsigned char timerEnum);
  
/************************************************************************************************
* Function: RestartTimer
* Purpose:  Restart a specified timer
* Usage:	 
* Input:    Enum of timer
* Output: 	
************************************************************************************************/
  extern void RestartTimer(unsigned char timerEnum);


#ifdef __cplusplus
}
#endif

#endif /* TIMERUTIL_H */

