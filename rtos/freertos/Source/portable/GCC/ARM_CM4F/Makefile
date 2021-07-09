obj-y += port.o HAL_CM4.o

ccflags-y += -Irtos/freertos/CMSIS/RTOS2/FreeRTOS/Include
ccflags-y += -Irtos/freertos/CMSIS/RTOS2/FreeRTOS/Include1
ccflags-y += -Irtos/freertos/Source/include
ccflags-y += -Irtos/freertos/Source/portable/GCC/ARM_CM4F

ccflags-y += -DOS_CLOCK_NOMINAL=$(OS_CLOCK_NOMINAL)

ifneq ($(OS_TASKCNT),)
ccflags-y += -DOS_TASKCNT=$(OS_TASKCNT)
endif

ifneq ($(OS_SCHEDULERSTKSIZE),)
ccflags-y += -DOS_SCHEDULERSTKSIZE=$(OS_SCHEDULERSTKSIZE)
endif

ifneq ($(OS_IDLESTKSIZE),)
ccflags-y += -DOS_IDLESTKSIZE=$(OS_IDLESTKSIZE)
endif
