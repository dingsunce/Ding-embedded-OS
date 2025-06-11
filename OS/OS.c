/*!*****************************************************************************
 * file		OS.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "OS.h"
#include "SysTick.h"
#include "memory.h"
#include "message.h"
#include "task.h"
//-----------------------------------------------------------------------------------------------------------
void OS_Init(void)
{
  SysTick_Init();
  Process_Init();
  Mem_Init();
  Msg_Init();
  Task_Init();
}