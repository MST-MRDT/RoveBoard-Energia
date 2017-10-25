#include "SysInitEnergia_TivaTM4C1294NCPDT.h"
#include "ClockingEnergia_TivaTM4C1294NCPDT.h"
#include "Energia.h"
#include "Debug.h"
#include "rom.h"
#include "sysctl.h"
#include "fpu.h"
#include "eeprom.h"

#define InitFootprintKeyword  0x0000FEFE
static const uint8_t BlockReferenceTableSize = 3;
static const uint32_t DummyMaskInLastWordOfTable = 0b11111111111111111111111111000000;
static const uint32_t ControlBlockPassword = 0x0000EFFE;
static const uint8_t WordLengthInBytes = 4;
static const uint8_t ControlBlock_WordOffset_blockFresh = 1 * WordLengthInBytes; //actually goes from 1 to 3
static const uint8_t ControlBlock_WordLength_blockFresh = 3 * WordLengthInBytes;
static const uint8_t BlockAddress_ControlBlock = 0;
static const uint8_t ControlBlock_WordOffset_initFootprint = 0 * WordLengthInBytes;
static const uint8_t ControlBlock_WordLength_initFootprint = WordLengthInBytes;
extern uint32_t blockFreshTable[BlockReferenceTableSize];
extern uint32_t blockUsedTable[BlockReferenceTableSize];
static bool initialized = false;

static void doFirstInit();
static void setupGlobalTables();
static void rovePermaMem_Init();

void initSystem()
{
  if(!initialized)
  {

    //Floating point computational unit is on by default in some compilers,
    //but put it here anyway just in case we use one that doesn't
    FPUEnable();
    FPULazyStackingEnable();

    //turn on system main clocks and system tick (which is used by things like delay and ethernet),
    //and turn on all GPIO ports as those are pretty much always used

    //clocks already enabled by energia
    //initSystemClocks();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);

    //turn on any roveboard stuff than needs pre-main initialization
    rovePermaMem_Init();

    //let peripherals finish turning on
    delayMicroseconds(100);

    initialized = true;
  }
}

void rovePermaMem_Init()
{
  uint32_t readValue;

  //try turning on the eeprom
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  if(EEPROMInit() == EEPROM_INIT_ERROR)
  {
    SysCtlDelay(1000);
    if(EEPROMInit() == EEPROM_INIT_ERROR)
    {
      debugFault("EEPROM has suffered from fatal internal error. This could mean its lifespan has been exceeded");
    }
  }

  //check if this is the first the eeprom has ever been used. If so, initialize it with our startup settings.
  //Either way, by default EEPROM starts off letting us read and write to blocks that aren't password protected (and none are password
  //protected on reset) so we can just get straight to business
  EEPROMRead(&readValue, BlockAddress_ControlBlock + ControlBlock_WordOffset_initFootprint, ControlBlock_WordLength_initFootprint);
  if(readValue != InitFootprintKeyword)
  {
    doFirstInit();
  }

  //make sure to set up the tables the non private files use before leaving
  setupGlobalTables();

  //set up protection so that nothing can access the EEPROM's blocks unless a password has been set for said blocks and is used.
  //Used to make the whole thing thread safe even with interrupts, as even if interrupted in the middle of a RovePermaMem function call
  //other parts of the program still wont' be able to mess with the EEPROM.
  EEPROMBlockProtectSet(BlockAddress_ControlBlock, EEPROM_PROT_NA_LNA_URW);

  //lock all blocks in the system by writing a password to block 0 and locking it, which locks the whole system until we unlock block 0 again
  uint32_t writeVal = ControlBlockPassword;
  EEPROMBlockPasswordSet(BlockAddress_ControlBlock, &writeVal, 1);
  EEPROMBlockLock(BlockAddress_ControlBlock);
}

static void doFirstInit()
{
  uint32_t writeValArray[3];

  //make sure that blockInformation word is set to our default state.
  writeValArray[0] = 0xFFFFFFFF;
  writeValArray[1] = 0xFFFFFFFF;
  writeValArray[2] = 0xFFFFFFC0; //only set up to the 89th block, rest are dummy values
  EEPROMProgram(writeValArray, BlockAddress_ControlBlock + ControlBlock_WordOffset_blockFresh, ControlBlock_WordLength_blockFresh);

  //set our footprint into the block we've allocated for it, so that we'll know on next startup that this operation was performed.
  writeValArray[0] = InitFootprintKeyword;
  EEPROMProgram(writeValArray, BlockAddress_ControlBlock + ControlBlock_WordOffset_initFootprint, ControlBlock_WordLength_initFootprint);
}

static void setupGlobalTables()
{
  int i;

  //the table that tracks what blocks are being used starts off completely empty, as no blocks have yet to
  //be declared in use on startup
  for(i = 0; i < BlockReferenceTableSize; i++)
  {
    blockUsedTable[i] = 0;
  }

  //block fresh table, meanwhile, comes from control block's word registers 1, 2, and 3
  EEPROMRead(blockFreshTable, BlockAddress_ControlBlock + ControlBlock_WordOffset_blockFresh, ControlBlock_WordLength_blockFresh);

  //should have already erased the dummy values in the eeprom's first initialization, but let's sanity check it everytime we turn it
  //on after. DummyMask sets them all to 0
  blockFreshTable[2] &= DummyMaskInLastWordOfTable;
}
