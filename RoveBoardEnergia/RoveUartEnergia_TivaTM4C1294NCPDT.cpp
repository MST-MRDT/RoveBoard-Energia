// RoveBoard.cpp for Energia
// Author: Gbenga Osibodu

#include "RoveUartEnergia_TivaTm4c1294NCPDT.h"
#include "ClockingEnergia_TivaTM4C1294NCPDT.h"
#include "Debug.h"
#include "stdint.h"
#include "uart.h"

static HardwareSerial* uartArray[8] = {&Serial , &Serial1, &Serial2, &Serial3,
                                  &Serial4, &Serial5, &Serial6, &Serial7};

roveUART_Handle roveUartOpen(unsigned int uart_index, unsigned int baud_rate, unsigned int txPin, unsigned int rxPin)
{
  return roveUartOpen(uart_index, baud_rate);
}

roveUART_Handle roveUartOpen(unsigned int uart_index, unsigned int baud_rate) {
  
  uartArray[uart_index] -> begin(baud_rate);
  roveUART_Handle handle;
  handle.uart_index = uart_index;
  handle.initialized = true;
  handle.baudRate = baud_rate;

  if(uart_index > 7)
  {
    debugFault("roveUartOpen: uart index is nonsense");
  }

  return handle;
}

roveUart_ERROR roveUartWrite(roveUART_Handle uart, void* write_buffer, size_t bytes_to_write) {
  if(uart.initialized == false)
  {
    debugFault("roveUartWrite: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];
  serial -> write((uint8_t*)write_buffer, bytes_to_write);
  serial -> flush();
    
  return ROVE_UART_ERROR_SUCCESS;
}

int roveUartPeek(roveUART_Handle uart)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartPeek: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  return serial -> peek();
}

roveUart_ERROR roveUartRead(roveUART_Handle uart, void* read_buffer, size_t bytes_to_read) {
  if(uart.initialized == false)
  {
    debugFault("roveUartRead: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  if (bytes_to_read == 0) {
    return ROVE_UART_ERROR_SUCCESS;
  }
  
  if (read_buffer == NULL) {
    return ROVE_UART_ERROR_UNKNOWN;
  }

  for (int i =0; i<bytes_to_read; i++) {
    while(serial -> available() == 0);
    ((unsigned char*)read_buffer)[i] = serial -> read();//Serial.println(temp[i],DEC);
  }
  
  return ROVE_UART_ERROR_SUCCESS;
} 

roveUart_ERROR roveUartReadNonBlocking(roveUART_Handle uart, void* read_buffer, size_t bytes_to_read)
{
  if(roveUartAvailable(uart) < bytes_to_read)
  {
    return ROVE_UART_ERROR_UNKNOWN;
  }
  else
  {
    return roveUartRead(uart, read_buffer, bytes_to_read);
  }
}

int roveUartAvailable(roveUART_Handle uart) {
  if(uart.initialized == false)
  {
    debugFault("roveUartAvailable: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  return serial -> available();
}

roveUart_ERROR roveUartSettings(roveUART_Handle uart, unsigned int paritySettings, unsigned int stopBitSettings, unsigned int wordLengthSettings)
{
  if(uart.initialized == false)
  {
    debugFault("roveUartAvailable: handle not initialized");
  }

  HardwareSerial* serial = uartArray[uart.uart_index];

  uint32_t wordLength, stopBits, parityBits, uartBase;
  switch(paritySettings)
  {
    case NoParity:
      parityBits = UART_CONFIG_PAR_NONE;
      break;

    case EvenParity:
      parityBits = UART_CONFIG_PAR_EVEN;
      break;

    case OddParity:
      parityBits = UART_CONFIG_PAR_ODD;
      break;

    case AlwaysZero:
      parityBits = UART_CONFIG_PAR_ZERO;
      break;

    case AlwaysOne:
      parityBits = UART_CONFIG_PAR_ONE;
      break;

    default:
      return ROVE_UART_ERROR_UNKNOWN;
  }

  switch(stopBitSettings)
  {
    case OneStopBit:
      stopBits = UART_CONFIG_STOP_ONE;
      break;

    case TwoStopBit:
      stopBits = UART_CONFIG_STOP_TWO;
      break;

    default:
      return ROVE_UART_ERROR_UNKNOWN;
  }

  switch(wordLengthSettings)
  {
    case WordLength8:
      wordLength = UART_CONFIG_WLEN_8;
      break;

    case WordLength7:
      wordLength = UART_CONFIG_WLEN_7;
      break;

    case WordLength6:
      wordLength = UART_CONFIG_WLEN_6;
      break;

    case WordLength5:
      wordLength = UART_CONFIG_WLEN_5;
      break;

    default:
      return ROVE_UART_ERROR_UNKNOWN;
  }
  switch(uart.uart_index)
  {
    case 0:
      uartBase = UART0_BASE;
      break;

    case 1:
      uartBase = UART1_BASE;
      break;

    case 2:
      uartBase = UART2_BASE;
      break;

    case 3:
      uartBase = UART3_BASE;
      break;

    case 4:
      uartBase = UART4_BASE;
      break;
      
    case 5:
      uartBase = UART5_BASE;
      break;
      
    case 6:
      uartBase = UART6_BASE;
      break;
      
    case 7:
      uartBase = UART7_BASE;
      break;
      
  }

  //function call automatically stops uart for configuration
  UARTConfigSetExpClk(uartBase, getCpuClockFreq(), uart.baudRate, (wordLength | parityBits |stopBits));

  return ROVE_UART_ERROR_SUCCESS;
}