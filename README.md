. Ding embedded os is a tiny operating system that can run on any microcontrollers including 8bits mcu.

. It can work under no os envrionment, 
  There is no need to change legacy code which is running under raw envirionment to adapt to this OS.

. It can work under other preemptive os envrionment, 
  preemptive os provides thread context to execute the Ding os.

. It provides OSAL(OS application layer) that unifys os function names,
  eliminating api name difference between different preemptive os.
  With the help of OSAL, Ding OS can run under rt-thread, ucos, free rtos, linux, windows also under non-os

. It contains basic OS function, including process management, memory management, timer, message management......

. It uses a small context(4 bytes) for process, then you can use it as mush as you want,
  benefiting from process communication, message transaction.. without worrying about
  memory consumption

. It runs the taskes in sequence, you can use it without worrying about critical data/code protection.



