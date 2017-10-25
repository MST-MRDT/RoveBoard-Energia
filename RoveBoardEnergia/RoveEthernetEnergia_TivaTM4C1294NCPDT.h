/* Programmer: Gbenga Osibodu
 * Editor/revisor: Drue Satterfield
 * Date of creation: for the original library, who knows, 2015 season I think. Reorganized into roveboard in september 2017.
 * Microcontroller used: Tiva TM4C1294NCPDT
 * Hardware components used by this file: Ethernet hardware
 *    Dependent on System Tick timer, which usually is set up in Clocking.h on the tiva to interrupt every millisecond;
 *    the clocked information that this library uses all depend on that interrupt
 *
 *
 * Description: This library is used to implement UDP networking over the tiva's ethernet port.
 *
 *
 * Warnings: There is currently an unknown error where when too many packets are received at once, a memory leak occurs.
 * See 2016's arm commands for an example of how much proved to be too much. Error did not occur on any other system,
 * as far as could be told, but was proven to be due to internal errors here.
 */

#ifndef ROVEETHERNET_TIVATM4C1294NCPDT_H_
#define ROVEETHERNET_TIVATM4C1294NCPDT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Energia.h"
#include "RoveEthernet.h"

#define ROVE_IP_ADDR_NONE INADDR_NONE
#define UDP_RX_MAX_PACKETS 32
#define UDP_TX_PACKET_MAX_SIZE 2048

inline bool operator != (IPAddress & lhs, const IPAddress & rhs) { return !(lhs == rhs); }

void roveEthernet_NetworkingStart(roveIP myIP);
roveEthernet_Error roveEthernet_UdpSocketListen(uint16_t port);
roveEthernet_Error roveEthernet_SendUdpPacket(roveIP destIP, uint16_t destPort, const uint8_t* msg, size_t msgSize);
roveEthernet_Error roveEthernet_GetUdpMsg(roveIP* senderIP, void* buffer, size_t bufferSize);

//attach a function to be ran automatically whenever a udp packet is received.
void roveEthernet_attachUdpReceiveCb(void (*userFunc)());

#endif
