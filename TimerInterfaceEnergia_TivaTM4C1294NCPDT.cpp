#include "TimerInterfaceEnergia_TivaTM4C1294NCPDT.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "inc/hw_ints.h"

const uint32_t F_PIOSC = 16000000; //speed of the internal precision clock is 16Mhz

static uint32_t intIdToTivawareConfig[] = {TIMER_CFG_PERIODIC};
static uint32_t intIdToTivawareIntA[] = {TIMER_TIMA_TIMEOUT};
static uint32_t intIdToTivawareIntB[] = {TIMER_TIMB_TIMEOUT};

//struct is built to potentially accomadate each timer's A and B sections
typedef struct TimerData
{
  uint8_t timerId;
  uint32_t timerAInterrupt;
  uint32_t timerBInterrupt;
  uint32_t timerAConfig;
  uint32_t timerBConfig;
  uint32_t timerALoad;
  uint32_t timerBLoad;
  uint32_t timerBase;
  uint32_t timerPeriphId;
  uint32_t timerAIntEnableId;
  uint32_t timerBIntEnableId;
  void (*attachedAFunction)(void);
  void (*attachedBFunction)(void);

  TimerData(uint8_t tId, uint32_t tAI, uint32_t tBI, uint32_t tAC,
            uint32_t tBC, uint32_t tAL, uint32_t tBL, uint32_t tB, uint32_t tPI, uint32_t tAII, uint32_t tBII, void (*intAHandler)(void), void (*intBHandler)(void))
  {
    timerId = tId;
    timerAInterrupt = tAI;
    timerBInterrupt = tBI;
    timerAConfig = tAC;
    timerBConfig = tBC;
    timerALoad = tAL;
    timerBLoad = tBL;
    timerBase = tB;
    timerPeriphId = tPI;
    timerAIntEnableId = tAII;
    timerBIntEnableId = tBII;

    //register interrupt functions
    TimerIntRegister(timerBase, TIMER_A, intAHandler);
    TimerIntRegister(timerBase, TIMER_A, intBHandler);
  }
} TimerData;


static TimerData* getTimerData(uint8_t timerId);
static void assertTimerId(uint8_t timerId);
static void assertInterruptId(uint8_t interruptId);
static void setupTimerData(TimerData * timerData, uint8_t timerInterruptId, uint32_t timerTimeout_us);
static void timer0AHandler();
static void timer0BHandler();
static void timer1AHandler();
static void timer1BHandler();
static void timer2AHandler();
static void timer2BHandler();
static void timer3AHandler();
static void timer3BHandler();
static void timer4AHandler();
static void timer4BHandler();
static void timer5AHandler();
static void timer5BHandler();
static void timer6AHandler();
static void timer6BHandler();
static void timer7AHandler();
static void timer7BHandler();
static void genAHandler(TimerData * timerData);
static void genBHandler(TimerData * timerData);
static void genAHandler(TimerData * timerData);

static TimerData timer0Data(Timer0, 0, 0, 0, 0, 0, 0, TIMER0_BASE, SYSCTL_PERIPH_TIMER0, INT_TIMER0A, INT_TIMER0B, timer0AHandler, timer0BHandler);
static TimerData timer1Data(Timer1, 0, 0, 0, 0, 0, 0, TIMER1_BASE, SYSCTL_PERIPH_TIMER1, INT_TIMER1A, INT_TIMER1B, timer1AHandler, timer1BHandler);
static TimerData timer2Data(Timer2, 0, 0, 0, 0, 0, 0, TIMER2_BASE, SYSCTL_PERIPH_TIMER2, INT_TIMER2A, INT_TIMER2B, timer2AHandler, timer2BHandler);
static TimerData timer3Data(Timer3, 0, 0, 0, 0, 0, 0, TIMER3_BASE, SYSCTL_PERIPH_TIMER3, INT_TIMER3A, INT_TIMER3B, timer3AHandler, timer3BHandler);
static TimerData timer4Data(Timer4, 0, 0, 0, 0, 0, 0, TIMER4_BASE, SYSCTL_PERIPH_TIMER4, INT_TIMER4A, INT_TIMER4B, timer4AHandler, timer4BHandler);
static TimerData timer5Data(Timer5, 0, 0, 0, 0, 0, 0, TIMER5_BASE, SYSCTL_PERIPH_TIMER5, INT_TIMER5A, INT_TIMER5B, timer5AHandler, timer5BHandler);
static TimerData timer6Data(Timer6, 0, 0, 0, 0, 0, 0, TIMER6_BASE, SYSCTL_PERIPH_TIMER6, INT_TIMER6A, INT_TIMER6B, timer6AHandler, timer6BHandler);
static TimerData timer7Data(Timer7, 0, 0, 0, 0, 0, 0, TIMER7_BASE, SYSCTL_PERIPH_TIMER7, INT_TIMER7A, INT_TIMER7B, timer7AHandler, timer7BHandler);

void setupTimer(uint32_t timerId, uint32_t interruptId, uint32_t timerTimeout_us)
{
  assertTimerId(timerId);
  assertInterruptId(interruptId);

  TimerData * timerData = getTimerData(timerId);

  setupTimerData(timerData, interruptId, timerTimeout_us);

  //enable timer hardware
  SysCtlPeripheralEnable(timerData->timerPeriphId);

  //wait for hardware to boot up
  while(!SysCtlPeripheralReady(timerData->timerPeriphId))
  {
  }

  //set clock to internal precision clock of 16 Mhz
  TimerClockSourceSet(timerData->timerBase, TIMER_CLOCK_PIOSC);

  //configure timer for its specified operation
  TimerConfigure(timerData->timerBase, timerData->timerAConfig);

  //set timer load
  TimerLoadSet(timerData->timerBase, TIMER_A, timerData->timerALoad);

  //set up interrupts. The order here is actually important, TI's forums reccomend
  //setting up new interrupts in this exact fashion
  TimerIntClear(timerData->timerBase, timerData->timerAInterrupt);
  TimerIntEnable(timerData->timerBase, timerData->timerAInterrupt);
  IntEnable(timerData->timerAIntEnableId);

  //enable master system interrupt
  IntMasterEnable();
}

void startTimer(uint32_t timerId)
{
  assertTimerId(timerId);
  TimerData * timerData = getTimerData(timerId);

  TimerIntClear(timerData->timerBase, timerData->timerAInterrupt);
  TimerIntEnable(timerData->timerBase, timerData->timerAInterrupt);
  TimerEnable(timerData->timerBase, TIMER_A);
}

void stopTimer(uint32_t timerId)
{
  assertTimerId(timerId);
  TimerData * timerData = getTimerData(timerId);

  TimerIntClear(timerData->timerBase, timerData->timerAInterrupt);
  TimerDisable(timerData->timerBase, TIMER_A);
  TimerIntDisable(timerData->timerBase, timerData->timerAInterrupt);
}

void attachTimerInterrupt(uint32_t timerId, void (*interruptFunc)(void) )
{
  TimerData * timerData = getTimerData(timerId);

  timerData->attachedAFunction = interruptFunc;
}

static TimerData* getTimerData(uint8_t timerId)
{
  switch(timerId)
  {
    case Timer0:
      return &timer0Data;

    case Timer1:
      return &timer1Data;

    case Timer2:
      return &timer2Data;

    case Timer3:
      return &timer3Data;

    case Timer4:
      return &timer4Data;

    case Timer5:
      return &timer5Data;

    case Timer6:
      return &timer6Data;

    case Timer7:
      return &timer7Data;

    default:
	 return 0;
  }
}

static void assertTimerId(uint8_t timerId)
{
  if(timerId == Timer0 || timerId == Timer1 || timerId == Timer2 || timerId == Timer3 || timerId == Timer5 || timerId == Timer6 || timerId == Timer7)
  {
    return;
  }
  else
  {
    //#error Incorrect timer id used in SimpleTimer_TivaTM4C1294
  }
}

static void assertInterruptId(uint8_t interruptId)
{
  if(interruptId == TimerPeriodicInterrupt)
  {
    return;
  }
  else
  {
    //#error Incorrect interrupt id used in SimpleTimer_TivaTM4C1294
  }
}

static void timer0AHandler()
{
  genAHandler(&timer0Data);
}

static void timer0BHandler()
{
  genBHandler(&timer0Data);
}

static void timer1AHandler()
{
  genAHandler(&timer1Data);
}
static void timer1BHandler()
{
  genBHandler(&timer1Data);
}
static void timer2AHandler()
{
  genAHandler(&timer2Data);
}
static void timer2BHandler()
{
  genBHandler(&timer2Data);
}
static void timer3AHandler()
{
  genAHandler(&timer3Data);
}
static void timer3BHandler()
{
  genBHandler(&timer3Data);
}
static void timer4AHandler()
{
  genAHandler(&timer4Data);
}
static void timer4BHandler()
{
  genBHandler(&timer4Data);
}
static void timer5AHandler()
{
  genAHandler(&timer5Data);
}
static void timer5BHandler()
{
  genBHandler(&timer5Data);
}
static void timer6AHandler()
{
  genAHandler(&timer6Data);
}
static void timer6BHandler()
{
  genBHandler(&timer6Data);
}
static void timer7AHandler()
{
  genAHandler(&timer7Data);
}
static void timer7BHandler()
{
  genBHandler(&timer7Data);
}

static void genAHandler(TimerData * timerData)
{
  TimerIntClear(timerData->timerBase, timerData->timerAInterrupt);
  timerData->attachedAFunction();
}

static void genBHandler(TimerData * timerData)
{
  TimerIntClear(timerData->timerBase, timerData->timerBInterrupt);
  timerData->attachedBFunction();
}

static void setupTimerData(TimerData * timerData, uint8_t timerInterruptId, uint32_t timerTimeout_us)
{
  uint32_t timerLoad = (float)F_PIOSC * ((float)timerTimeout_us/1000000.0); // timer clock cycle (16Mhz cycle/second) * (microsecond timeout/10000000 to convert it to seconds) = cycles till the timeout passes

  timerData->timerALoad = timerLoad;
  timerData->timerAConfig = intIdToTivawareConfig[timerInterruptId];
  timerData->timerAInterrupt = intIdToTivawareIntA[timerInterruptId];
}
