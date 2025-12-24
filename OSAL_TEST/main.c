#include "SysTick.h"
#include "app_ipc_test.h"
#include "app_msg_test.h"
#include "d_os.h"
#include "osal.h"

static os_thread_t *TaskStartThread;

static os_return_t App_TaskStart(void *p_arg)
{
  (void)p_arg;

  /*
    init D_OS and other app in preemptive os thread

    systick is not working only if we have started preemptive os like ucos, free rtos
 */
  DOS_Init();

  app_msg_test_start();

  app_pic_test_start();

  while (!os_thread_should_stop(TaskStartThread))
  {
    /* Task body, always written as an infinite loop.           */
    os_msleep(1000);
  }

  OS_RETURN(TaskStartThread);
}

static void onems_timer(os_timer_t *timer, void *arg)
{
  SysTick_On();
}

int main(int argc, char **argv)
{
  os_init();
  TaskStartThread = os_thread_create("os_task", 3, 256, App_TaskStart, NULL);
  os_start(); // start first thread

  os_timer_t *timer = os_timer_create(1, onems_timer, NULL, false);
  os_timer_start(timer);

  while (1)
  {
  }

  return 1;
}
