#ifndef ROVETYPENAMES_H_
#define ROVETYPENAMES_H_
#include "Energia.h"
#include <IPAddress.h>

typedef enum
{
  ROVE_ETHERNET_ERROR_SUCCESS       =  0,
  ROVE_ETHERNET_ERROR_UNKNOWN       = -1,
  ROVE_ETHERNET_ERROR_SOCKET_IN_USE = -2,
  ROVE_ETHERNET_ERROR_HOST_DOWN     = -3,
  ROVE_ETHERNET_ERROR_WOULD_BLOCK   = -4
} roveEthernet_Error;

typedef IPAddress roveIP;

typedef enum {
    ROVE_BOARD_ERROR_SUCCESS = 0,
    ROVE_BOARD_ERROR_UNKNOWN = -1
} roveBoard_ERROR;

typedef struct roveUART_Handle
{
	unsigned int uart_index;
} roveUART_Handle;

#endif
