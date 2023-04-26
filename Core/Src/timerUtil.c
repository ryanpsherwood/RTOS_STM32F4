/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/************************************************************************************************
*   Included Headers										*
************************************************************************************************/
#include "timerUtil.h"

/************************************************************************************************
*   Variables - File Scope									*
************************************************************************************************/
TimerDataMonitor mTimers[eTimeMax];

/************************************************************************************************
*   Function Definitions								        *
************************************************************************************************/
/************************************************************************************************
* Function: InitTimers
* Purpose:  Initialize all timers that are used
* Usage:	 
* Input:	 
* Output: 	
************************************************************************************************/
void InitTimers(void)
{
  
  mTimers[eTimeCanHeartBeat].timeout = FIVE_SECONDS;
  mTimers[eTimeCanHeartBeat].elapsed = 0.0;
  
  mTimers[eTimeEthernetHeartBeat].timeout = TWO_SECONDS;
  mTimers[eTimeEthernetHeartBeat].elapsed = 0.0;
}

/************************************************************************************************
* Function: StartTimer
* Purpose:  Start timer for the specified timer in seconds
* Usage:	 
* Input:    Enum of timer
* Output: 	
************************************************************************************************/
void StartTimer(unsigned char timerEnum)
{
  
  mTimers[timerEnum].start = HAL_GetTick();
}

/************************************************************************************************
* Function: IsTimerElapsed
* Purpose:  Checks if specified timer has elapsed its timeout
* Usage:	 
* Input:    Enum of timer
* Output:   Returns one if elapsed, zero if not
************************************************************************************************/
unsigned char IsTimerElapsed(unsigned char timerEnum)
{
  
  unsigned char retVal = 0;
  mTimers[timerEnum].end = HAL_GetTick();
  mTimers[timerEnum].elapsed = (mTimers[timerEnum].end - mTimers[timerEnum].start);
  if (mTimers[timerEnum].elapsed >= mTimers[timerEnum].timeout)
  {
    retVal = 1;
    mTimers[timerEnum].start = mTimers[timerEnum].end;
  }
  return retVal;
}

/************************************************************************************************
* Function: RestartTimer
* Purpose:  Restart a specified timer
* Usage:	 
* Input:    Enum of timer
* Output: 	
************************************************************************************************/
void RestartTimer(unsigned char timerEnum)
{
  
  mTimers[timerEnum].start = mTimers[timerEnum].end;
}

