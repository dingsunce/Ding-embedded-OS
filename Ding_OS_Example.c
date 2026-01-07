
#include "d_list.h"
#include "d_memb.h"
#include "d_message.h"
#include "d_tick.h"
#include "memory.h"
#include "process.h"
#include <stdlib.h>

// example1 : static process  create a process in PROCESS() macro
typedef struct Arg
{
  u8 Status;
} Arg_t;

#define TIMER_CYCLE        0
#define TIMER_LATER        1
#define TIMER_NOW          2
#define TIMER_NOW_WITH_ARG 3
PROCESS(TimerProccess);
//-----------------------------------------------------------------------------------------------------------
PROCESS_HANDLER(TimerProccess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == TIMER_CYCLE || msgId == TIMER_LATER || msgId == TIMER_NOW ||
                         msgId == TIMER_NOW_WITH_ARG);

    if (msgId == TIMER_CYCLE)
    {
      // handler()
    }
    else if (msgId == TIMER_LATER)
    {
      // handler()
    }
    else if (msgId == TIMER_NOW)
    {
      // handler()
    }
    else if (msgId == TIMER_NOW_WITH_ARG)
    {
      if (arg != NULL)
      {
        if (((Arg_t *)arg)->Status == 1)
        {
          // handler(): process will free Arg_t automatically
        }
      }
    }
  }

  PROCESS_SCHEDULE_END();
}
//-----------------------------------------------------------------------------------------------------------
static Arg_t *CreateArg(u8 status)
{
  Arg_t *arg = (Arg_t *)DMem_Malloc(sizeof(Arg_t));
  if (arg != NULL)
  {
    arg->Status = status;
  }

  return arg;
}
//-----------------------------------------------------------------------------------------------------------
void TimerProccessInit(void)
{
  Process_Start(&TimerProccess);

  DMsg_SendCycle(&TimerProccess, TIMER_CYCLE, MSG_NO_ARG,
                 15); // send message cyclically(15 ms period)
  DMsg_SendLater(&TimerProccess, TIMER_LATER, MSG_NO_ARG, 15); // send message 15 ms later
  DMsg_SendInstant(&TimerProccess, TIMER_NOW, MSG_NO_ARG);     // send message Now
  DMsg_SendInstant(&TimerProccess, TIMER_NOW, MSG_NO_ARG);     // send message Now with argument
  DMsg_SendInstant(&TimerProccess, TIMER_NOW, CreateArg(1));   // send message Now with argument

  // DMsg_Cancel(&TimerProccess, TIMER_NOW);  // cancel this message
}

// example2 create process in structure
typedef struct
{

  Process_t DetectProcess;
  Process_t EventProcess;

} ProcessInStructure;

typedef enum MsMsg
{
  MS_MSG_START_DETECT,
  MS_MSG_MOTION_DETECTED,
  MS_MSG_HOLD_TIME_EXPIRE,
  MS_MSG_CANCEL_HOLD_TIMER,
  MS_MSG_NO_MOVEMENT_TIME_EXPIRE,
  MS_MSG_EVENT,
} MsMsg_t;

//-----------------------------------------------------------------------------------------------------------
static u8 DetectProcessHandler(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  ProcessInStructure *ins = ContainerOf(process, ProcessInStructure, DetectProcess);
  PROCESS_SCHEDULE_BEGIN()

  while (true)
  {
    PROCESS_WAIT_FOR_MSG(msgId == MS_MSG_START_DETECT);

  VacantState:
    // EnterVacantState(ins, msgId);

    PROCESS_WAIT_FOR_MSG(msgId == MS_MSG_MOTION_DETECTED);
    goto OccupyMovementState;

  OccupyMovementState:
    // EnterOccupyMovementState(ins);

    PROCESS_WAIT_FOR_MSG(msgId == MS_MSG_MOTION_DETECTED ||
                         msgId == MS_MSG_NO_MOVEMENT_TIME_EXPIRE);

    if (msgId == MS_MSG_MOTION_DETECTED)
    {
      goto OccupyMovementState;
    }
    else if (msgId == MS_MSG_NO_MOVEMENT_TIME_EXPIRE)
    {
      goto OccupyNoMovementState;
    }

  OccupyNoMovementState:
    DMsg_SendLater(&ins->EventProcess, MS_MSG_EVENT, MSG_NO_ARG, 30);

    PROCESS_WAIT_FOR_MSG(msgId == MS_MSG_MOTION_DETECTED || msgId == MS_MSG_HOLD_TIME_EXPIRE ||
                         msgId == MS_MSG_CANCEL_HOLD_TIMER);

    if (msgId == MS_MSG_MOTION_DETECTED)
    {
      goto OccupyMovementState;
    }
    else if (msgId == MS_MSG_HOLD_TIME_EXPIRE || msgId == MS_MSG_CANCEL_HOLD_TIMER)
    {
      goto VacantState;
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static u8 EventProcessHandler(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  ProcessInStructure *ins = ContainerOf(process, ProcessInStructure, EventProcess);
  PROCESS_SCHEDULE_BEGIN()

  while (true)
  {
    PROCESS_WAIT_FOR_MSG(msgId == MS_MSG_EVENT);

    if (msgId == MS_MSG_EVENT)
    {
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
void ProcessInStructure_Init(ProcessInStructure *ins)
{
  Process_InitStruct(&ins->DetectProcess, DetectProcessHandler, "MotionSensorDetect");
  Process_InitStruct(&ins->EventProcess, EventProcessHandler, "MotionSensorEvent");

  Process_Start(&ins->DetectProcess);
  Process_Start(&ins->EventProcess);
  DMsg_SendInstant(&ins->DetectProcess, MS_MSG_START_DETECT, MSG_NO_ARG);
}

// example3 memory allocate from global memory
void AllocateFromGlobalMemory(void)
{
  OS_Init();

  Arg_t *arg = (Arg_t *)DMem_Malloc(sizeof(Arg_t));
  DMem_Free(arg);
}

// example4 memory allocate from memory block
typedef struct MembTest
{
  u8 Id;
} MyBlock_t;

DMEMB(MemBlock, MyBlock_t, 10);

void AllocateFromMemoryBlock(void)
{
  DMemb_Init(&MemBlock);
  MyBlock_t *block = (MyBlock_t *)DMemb_Alloc(&MemBlock);
  DMemb_Free(&MemBlock, block);
}

// example5 memory block and list
LIST(ElementList);

void MemoryBlockAndList(void)
{
  DMemb_Init(&MemBlock);
  List_Init(&ElementList);

  MyBlock_t *block1 = (MyBlock_t *)DMemb_Alloc(&MemBlock);
  MyBlock_t *block2 = (MyBlock_t *)DMemb_Alloc(&MemBlock);

  List_Add(ElementList, block1);
  List_Add(ElementList, block2);

  MyBlock_t *e;
  for (e = (MyBlock_t *)List_Head(ElementList); e != NULL; e = (MyBlock_t *)List_ItemNext(e))
  {
  }

  MyBlock_t *element1 = List_Pop(ElementList);
  MyBlock_t *element2 = List_Pop(ElementList);

  DMemb_Free(&MemBlock, element1);
  DMemb_Free(&MemBlock, element2);
}

// example6 memory , list in structure
#define DALI_TX_SIZE 100

typedef struct TxEntry
{
  LIST_HEADER;
  u8 Id;
  u8 Type;
} TxEntryT;

typedef struct Bus
{
  LIST_STRUCT(Tx_List);
  DMEMB_STRUCT(Tx_Mem, TxEntryT, DALI_TX_SIZE);
} BusT;

void MemoryBlockAndListInStructure(BusT *bus)
{
  LIST_STRUCT_INIT(bus, Tx_List);
  DMEMB_STRUCT_INIT(bus, Tx_Mem, TxEntryT, DALI_TX_SIZE);

  TxEntryT *entry = DMemb_Alloc(&bus->Tx_Mem);
  List_Add(bus->Tx_List, entry);

  TxEntryT *entryPop = (TxEntryT *)List_Pop(bus->Tx_List);
  DMemb_Free(&bus->Tx_Mem, entryPop);
}

// example7 os api in interruption
// do not use any os api in interrupt except void Process_Poll(Process_t *p);
// example:
PROCESS(uartProccess);
//-----------------------------------------------------------------------------------------------------------
PROCESS_HANDLER(uartProccess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN();

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_POLL_PROCESS);
    // hander uart data
  }

  PROCESS_SCHEDULE_END();
}

void UartInterrption(void)
{
  Process_Poll(&uartProccess) // send SYS_MSG_POLL_PROCESS to uartProccess
}

void SysTick_Handler(void)
{
  DTick_On();
}

// main function
void main(void)
{
  DOS_Init();

  DTick_Reset(); // reset the tick to 0;
  // call SysTick_Handler in interuption every tick
  while (1)
  {
    DOS_Run();

    if (DTick_IsTickOn())
    {
      DTick_ResetTickOn();

      DOS_RunOneTick();
    }
  }
}