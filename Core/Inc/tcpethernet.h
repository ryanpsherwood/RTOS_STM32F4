/**
  ******************************************************************************
  * @file    tcpethernet.h
  * @author  Ryan Sherwood
  * @brief   header file for tcpethernet.c
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCPETHERNET_H__
#define __TCPETHERNET_H__

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define ETHERNET_BUFFER_SIZE     1024
/* Exported types ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
extern void PushEthernetTxPacket(char* data, int dl, unsigned char source);

extern unsigned char AllConnections(void);

extern void tcp_server_socket_init(void);

//extern void StartEthRxProcessTask(void const * argument);

//extern void StartEthTxProcessTask(void const * argument);

extern void StartEthProcessTask(void const * argument);

/* Private defines -----------------------------------------------------------*/

#endif /* __TCPETHERNET_H */