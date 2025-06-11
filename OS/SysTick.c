/*!*****************************************************************************
 * file		SysTick.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "SysTick.h"
#include "list.h"
#include "memb.h"

static volatile u8  Tick200us = 0;
static volatile u32 Tick1ms = 0;

#if (SYS_TICK_REAL_TIME_CALLBACK == 1)

#ifndef SYS_200us_CALL_BACK_NUM
#define SYS_200us_CALL_BACK_NUM 10
#endif

typedef struct SysCallBackElement
{
  struct SysCallBackElement *Next;
  T_SYS_CALLBACK             CallBack;
} SysCallBackElement_t;

LIST(Sys200usCallBackElementList);
MEMB(Sys200usCallBackElementElementMem, SysCallBackElement_t, SYS_200us_CALL_BACK_NUM);

#endif

u32 SysTick_Get1msTicks(void)
{
  return Tick1ms;
}
//-----------------------------------------------------------------------------------------------------------
u8 SysTick_Get200usTicks(void)
{
  return Tick200us;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Decrease200usTicks(void)
{
  Tick200us--;
}
//-----------------------------------------------------------------------------------------------------------
u32 sys_now(void)
{
  return Tick1ms;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Init(void)
{
  Tick200us = 0;
  Tick1ms = 0;

#if (SYS_TICK_REAL_TIME_CALLBACK == 1)
  List_Init(Sys200usCallBackElementList);
  Memb_Init(&Sys200usCallBackElementElementMem);
#endif
}
#if (SYS_TICK_REAL_TIME_CALLBACK == 1)
//-----------------------------------------------------------------------------------------------------------
void SysTick_Register200usCallback(T_SYS_CALLBACK callBack)
{
  SysCallBackElement_t *e = Memb_Alloc(&Sys200usCallBackElementElementMem);
  if (e == NULL)
    return;

  e->CallBack = callBack;
  List_Add(Sys200usCallBackElementList, e);
}
//-----------------------------------------------------------------------------------------------------------
static void Trigger200usCallback(void)
{
  SysCallBackElement_t *e;
  for (e = (SysCallBackElement_t *)List_Head(Sys200usCallBackElementList); e != NULL;
       e = (SysCallBackElement_t *)List_ItemNext(e))
  {
    if (e->CallBack != NULL)
      e->CallBack();
  }
}
#endif
//-----------------------------------------------------------------------------------------------------------
static volatile u8 TickCount = 0;
void               SysTick_Handler(void)
{
#if (SYS_TICK_REAL_TIME_CALLBACK == 1)
  Trigger200usCallback();
#endif

  Tick200us++;
  TickCount++;
  if (TickCount >= 5)
  {
    TickCount = 0;
    Tick1ms++;
  }
}
//-----------------------------------------------------------------------------------------------------------
u32 SysTick_GetDelayMs(u32 nOldTime)
{
  u32 delay;

  if (Tick1ms >= nOldTime)
    delay = Tick1ms - nOldTime;
  else
    delay = 0xffffffff - nOldTime + Tick1ms;

  return delay;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_DelayMs(u32 delay)
{
  uint32_t tickstart = sys_now();
  uint32_t wait = delay;

  while ((sys_now() - tickstart) < wait)
  {
  }
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Update(u32 pTime)
{
  Tick1ms = Tick1ms + pTime;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Reset(void)
{
  Tick200us = 0;
  Tick1ms = 0;
}
