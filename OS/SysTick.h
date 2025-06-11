/*!*****************************************************************************
 * file		SysTick.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _SYS_TICKS_H
#define _SYS_TICKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "OS.h"
#include "define.h"

#if (SYS_TICK_REAL_TIME_CALLBACK == 1)
  typedef void (*T_SYS_CALLBACK)(void);
  void SysTick_Register200usCallback(T_SYS_CALLBACK callBack);
#endif

  void SysTick_Init(void);

  u32 SysTick_Get1msTicks(void);

  u8   SysTick_Get200usTicks(void);
  void SysTick_Decrease200usTicks(void);

  void SysTick_Handler(void);
  u32  sys_now(void);
  u32  SysTick_GetDelayMs(u32 nOldTime);

  void SysTick_Update(u32 pTime);
  void SysTick_Reset(void);
  void SysTick_DelayMs(u32 delay);

#ifdef __cplusplus
}
#endif

#endif
