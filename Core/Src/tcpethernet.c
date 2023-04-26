/**
  ******************************************************************************
  * @file    tcpethernet.c
  * @author  Ryan Sherwood
  * @brief   TCP server implementation using LwIP socket API   
  ******************************************************************************
*/ 

/************************************************************************************************
*   Included Headers										*
************************************************************************************************/
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "semphr.h"
#include "string.h"
#include <stdio.h>

#include "main.h"
#include "tcpethernet.h"
#include "can_net.h"
#include "BbProtocol.h"
#include "timerUtil.h"

/************************************************************************************************
*   Macro Definitions									        *
************************************************************************************************/
#define TCPSERVER_THREAD_PRIO    ( osPriorityNormal )
#define NUM_TCP_CONNECTIONS      6

/************************************************************************************************
*   Structures - File Scope					                                *
************************************************************************************************/
typedef struct
{
  unsigned short inIndex;
  unsigned short outIndex;
  unsigned short OvlCntr;
  unsigned char sockMask;
  char buffer[ETHERNET_BUFFER_SIZE];
} EthernetBufferStruct;

typedef struct
{
  int socket_desc;
  osThreadId threadId;
  unsigned char used;
} threadInfo;

/************************************************************************************************
*   Variables - Global Scope					                                *
************************************************************************************************/
int mConnections[NUM_TCP_CONNECTIONS];
unsigned short gSourceConnection;

/************************************************************************************************
*   Variables - File Scope					                                *
************************************************************************************************/
threadInfo threadConnArray[NUM_TCP_CONNECTIONS];

static EthernetBufferStruct EthernetRX;
static EthernetBufferStruct EthernetTX;

uint32_t EthTXCntr = 0;
uint32_t EthRXCntr = 0;
//uint32_t EthRXProcessCntr = 0;

/************************************************************************************************
*   Function Definitions									*
************************************************************************************************/
/************************************************************************************************
* Function: InitializeConnectionStructures
* Purpose:  Initialize the structure that will house connection information
* Usage:	 
* Input:    None
* Output:   None
************************************************************************************************/
void InitializeConnectionStructures(void)
{
  for(unsigned char iter = 0; iter < NUM_TCP_CONNECTIONS; iter++)
  {
    threadConnArray[iter].socket_desc = 0;
    threadConnArray[iter].threadId = 0;
    threadConnArray[iter].used = 0;
  } 
}

/************************************************************************************************
* Function: PushEthernetRxPacket
* Purpose:  Pushes an ethernet packet into the receive buffer
* Usage:	 
* Input:    data: pointer to data, dl: size of packet, source: mask for connection
* Output:   None
************************************************************************************************/
static void PushEthernetRxPacket(char* data, int dl, unsigned char source)
{
  int i;
  EthernetRX.sockMask = source;
  for(i = 0; i < dl; i++)
  {
    //
    // Put the message into slot
    //
    EthernetRX.buffer[EthernetRX.inIndex] = data[i];

    if(++EthernetRX.inIndex >= ETHERNET_BUFFER_SIZE)
    {
      EthernetRX.inIndex = 0;
    }

    //
    // if the new message causes the in and out indexes to be equal
    // then an overflow has occurred. Set the overflow flag
    // and increment the counter
    //
    if(EthernetRX.inIndex == EthernetRX.outIndex)
    {
      EthernetRX.OvlCntr++;
    }
  }
}

/************************************************************************************************
* Function: PopEthernetRxPacket
* Purpose:  Pops a packet off the top of the receive buffer one byte at a time.
*               Function needs to be called in a loop
* Usage:	 
* Input:    source: Pointer to socket connection
* Output:   One byte of the receive buffer
************************************************************************************************/
static char PopEthernetRxPacket(unsigned char* source)
{

  char result = 0;
  *source = EthernetRX.sockMask;

  //
  // Put the message into slot
  //
  result = EthernetRX.buffer[EthernetRX.outIndex];

  if(++EthernetRX.outIndex >= ETHERNET_BUFFER_SIZE)
  {
    EthernetRX.outIndex = 0;
  }

  return result;
}

/************************************************************************************************
* Function: EthernetRxBufferDataAvailable
* Purpose:  Checks if their is any data in the receive buffer
* Usage:	 
* Input:    None
* Output:   Returns false if buffer is empty, true if not
************************************************************************************************/
static unsigned char EthernetRxBufferDataAvailable(void)
{

  if(EthernetRX.inIndex == EthernetRX.outIndex)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/************************************************************************************************
* Function: PushEthernetTxPacket
* Purpose:  Pushes an ethernet packet into the transmit buffer
* Usage:	 
* Input:    data: pointer to data, dl: size of packet, source: mask for connection
* Output:   None
************************************************************************************************/
void PushEthernetTxPacket(char* data, int dl, unsigned char source)
{
  
  int i;
  EthernetTX.sockMask = source;
  for(i = 0; i < dl; i++)
  {
    //
    // Put the message into slot
    //
    EthernetTX.buffer[EthernetTX.inIndex] = data[i];

    if(++EthernetTX.inIndex >= ETHERNET_BUFFER_SIZE)
    {
      EthernetTX.inIndex = 0;
    }

    //
    // if the new message causes the in and out indexes to be equal
    // then an overflow has occurred. Set the overflow flag
    // and increment the counter
    //
    if(EthernetTX.inIndex == EthernetTX.outIndex)
    {
      EthernetTX.OvlCntr++;
    }
  }
}

/************************************************************************************************
* Function: PopEthernetTxPacket
* Purpose:  Pops a packet off the top of the transmit buffer one byte at a time.
*               Function needs to be called in a loop
* Usage:	 
* Input:    source: Pointer to socket connection
* Output:   One byte of the transmit buffer
************************************************************************************************/
static char PopEthernetTxPacket(unsigned char* source)
{
  char result = 0;
  *source = EthernetTX.sockMask;

  //
  // Put the message into slot
  //
  result = EthernetTX.buffer[EthernetTX.outIndex];

  if(++EthernetTX.outIndex >= ETHERNET_BUFFER_SIZE)
  {
    EthernetTX.outIndex = 0;
  }

  return result;
}

/************************************************************************************************
* Function: EthernetTxBufferDataAvailable
* Purpose:  Checks if their is any data in the transmit buffer
* Usage:	 
* Input:    None
* Output:   Returns false if buffer is empty, true if not
************************************************************************************************/
static unsigned char EthernetTxBufferDataAvailable(void)
{

  if(EthernetTX.inIndex == EthernetTX.outIndex)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/************************************************************************************************
* Function: AllConnections
* Purpose:  Returns the amount of current ethernet connections
* Usage:	 
* Input:	 
* Output: 	Amount of current ethernet connections
************************************************************************************************/
unsigned char AllConnections(void)
{

  unsigned char result = 0;
  unsigned char i;
  
  for (i = 0; i < NUM_TCP_CONNECTIONS; i++)
  {
    if (mConnections[i] != 0)
    {
      result += 0x01<<i;
    }
  }
  return result;
}

/************************************************************************************************
* Function: SearchForSocket
* Purpose:  Loop through connection array to see which offset the sockets are a match
* Usage:	 
* Input:    Socket for matching
* Output:   Return -1 if no match, otherwise offset of match
************************************************************************************************/
static char SearchForSocket(unsigned int sock)
{

  unsigned char i;
  char result = 0;
  
  for (i = 0; i < NUM_TCP_CONNECTIONS; i++)
  {
    if (sock == mConnections[i])
    {
      result = i+1;
      return result;
    }
  }
  return result;
}

/************************************************************************************************
* Function: connection_handler
* Purpose:  Handles a new connection
* Usage:	 
* Input:    Socket descriptor
* Output:   None
************************************************************************************************/
static void connection_handler(void *arg)
{
  int sock = (int)arg;
  int read_size;
  char index = 0;
  unsigned char mySource = gSourceConnection;
  char client_message[ETHERNET_BUFFER_SIZE];
  osThreadId tempThreadId = 0;
  
  while((read_size = recv(sock, client_message, ETHERNET_BUFFER_SIZE, 0)) > 0)
  {
    // Put message into buffer
    PushEthernetRxPacket(client_message, read_size, mySource);
    EthRXCntr++;
  }
  
  if(read_size == 0)
  {
    // Client has disconnected
    if(  (index = SearchForSocket(sock)) > 0)
    {
      mConnections[index-1] = 0;
    }
    for(unsigned char i = 0; i < NUM_TCP_CONNECTIONS; i++)
    {
      if(threadConnArray[i].socket_desc == sock)
      {
        threadConnArray[i].used = 0;
        threadConnArray[i].socket_desc = 0;
        tempThreadId = threadConnArray[i].threadId;
        threadConnArray[i].threadId = 0;
        close(sock);
        osThreadTerminate(tempThreadId);
        break;
      }
    }
  }
  else if(read_size == -1)
  {
    // Error receive failed
    if(  (index = SearchForSocket(sock)) > 0)
    {
      mConnections[index-1] = 0;
    }
    for(unsigned char i = 0; i < NUM_TCP_CONNECTIONS; i++)
    {
      if(threadConnArray[i].socket_desc == sock)
      {
        threadConnArray[i].used = 0;
        threadConnArray[i].socket_desc = 0;
        tempThreadId = threadConnArray[i].threadId;
        threadConnArray[i].threadId = 0;
        close(sock);
        osThreadTerminate(tempThreadId);
        break;
      }
    }
  }
}

/************************************************************************************************
* Function: tcp_server_listen_thread
* Purpose:  Listenes for a tcp connection. Creates new task connection_handler for receiving data
* Usage:	 
* Input:    arg: pointer on argument(not used here)
* Output:   None
************************************************************************************************/
static void tcp_server_listen_thread(void *arg)
{

  int socket_desc, new_socket, c, new_sock;
  struct sockaddr_in server, client;
  osThreadId newThread;

  while(1)
  {

    /* create a TCP socket */
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1)
    {
      // Error could not create socket
      printf("Socket could not be created!");
    }
    else
    {
      /* prepare the sockaddr_in structure */
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(4555);
      
      /* bind to port 4555 at any interface */
      if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
      {
        // Error bind failed
      }
      else
      {
        /* listen for incoming connections (TCP listen backlog = 5) */
        listen(socket_desc, 5);
        
        c = sizeof(struct sockaddr_in);
        while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
        {
          gSourceConnection = 0x00;
          for(unsigned char i = 0; i < NUM_TCP_CONNECTIONS; i++)
          {
            if(mConnections[i] == 0)
            {
              mConnections[i] = new_socket;
              if(i == 0)
              {
                gSourceConnection = 0x01;
              }
              else
              {
                gSourceConnection = 0x01<<i;
              }
              break;
            }
          } 
          new_sock = new_socket;
          newThread = sys_thread_new("Connection_Handler", connection_handler, (void *)new_sock, configMINIMAL_STACK_SIZE*4, TCPSERVER_THREAD_PRIO);
          if(newThread == NULL)
          {
            //Was connection handler thread even created
            printf("Create new thread for tcp connection failed!!!");
          }
          for(unsigned char i = 0; i < NUM_TCP_CONNECTIONS; i++)
          {
            if(threadConnArray[i].used == 0)
            {
              threadConnArray[i].socket_desc = new_sock;
              threadConnArray[i].threadId = newThread;
              threadConnArray[i].used = 1;
              break;
            }
          }
        }
        if(new_socket < 0)
        {
          // Error accept function of socket failed
          printf("What happened to the socket???");
        }
      }
    }
  }
}

/************************************************************************************************
* Function: tcp_server_socket_init
* Purpose:  Initialize the TCP server (start its thread)
* Usage:	 
* Input:    None
* Output:   None
************************************************************************************************/
void tcp_server_socket_init(void)
{
  sys_thread_new("TCP_SERVER_LISTEN", tcp_server_listen_thread, NULL, configMINIMAL_STACK_SIZE*4, TCPSERVER_THREAD_PRIO);
}

/************************************************************************************************
* Function: StartEthRxProcessTask
* Purpose:  Pops packet/s off receive buffer and processes the data
* Usage:	 
* Input:    argument: not used
* Output:   None
************************************************************************************************/
void StartEthRxProcessTask(void const * argument)
{

  unsigned int i;
  unsigned char source;
  unsigned int byteCounter = 0;
  unsigned char newByte = 0;
  unsigned char RxPacket[ETHERNET_BUFFER_SIZE];

  for( ;; )
  {
    byteCounter = 0;
    while(EthernetRxBufferDataAvailable() && byteCounter < ETHERNET_BUFFER_SIZE)
    {
      newByte = PopEthernetRxPacket(&source);
      RxPacket[byteCounter++] = newByte;
      
      if(newByte == 'b')
      {
        // break;
      }
    }
    if(byteCounter)
    {
      for(i = 0; i < byteCounter; i++)
      {
        ProcessBbMessage(RxPacket[i], source);
      }
      //EthRXProcessCntr++;
    }
    else
    {
      osDelay(1);
    }
  }
}

/************************************************************************************************
* Function: StartEthTxProcessTask
* Purpose:  Pops packet/s off transmit buffer and sends them over tcp
* Usage:	 
* Input:    argument: not used
* Output:   None
************************************************************************************************/
void StartEthTxProcessTask(void const * argument)
{
  
  unsigned int byteCounter = 0;
  unsigned char i, dest;
  int sendSocket, totalBytesWritten, bytesWritten;
  unsigned char TxMessage[ETHERNET_BUFFER_SIZE];
  
  for( ;; )
  {
    
    byteCounter = 0;
    while(  (byteCounter < ETHERNET_BUFFER_SIZE) && EthernetTxBufferDataAvailable() )
    {
      TxMessage[byteCounter++] = PopEthernetTxPacket(&dest);
    }
    if(byteCounter)
    {
      for(i = 0; i < NUM_TCP_CONNECTIONS; i++)
      {
        if(  (dest>>i) & 1)
        {
          sendSocket = mConnections[i];
          if(sendSocket != 0)
          {
            totalBytesWritten = 0;
            while(totalBytesWritten < byteCounter)
            {
              bytesWritten = send(sendSocket, TxMessage, (byteCounter-totalBytesWritten), 0);
              if(bytesWritten == -1)
              {
                // Write did not work
                printf("Writting to tcp socket failed!!!");
              }
              // Restart heartbeat timer
              totalBytesWritten += bytesWritten;
              EthTXCntr++;
            }
          }
          else
          {
            // Do Nothing
          }
        }
      }
    }
    else
    {
      osDelay(1);
    }
  }
}

/************************************************************************************************
* Function: StartEthProcessTask
* Purpose:  Looks to see if there are packets in the RX or TX ethernet buffers for processing
* Usage:	 
* Input:    argument: not used
* Output:   None
************************************************************************************************/
void StartEthProcessTask(void const * argument)
{

  unsigned int byteRxCounter = 0;
  unsigned int byteTxCounter = 0;
  unsigned char newByte = 0;
  unsigned char connIter, dest, source;
  int sendSocket, totalBytesWritten, bytesWritten, dataIter;
  unsigned char ethBuffer[ETHERNET_BUFFER_SIZE];
  
  StartTimer(eTimeEthernetHeartBeat);
  
  for( ;; )
  {
    byteRxCounter = 0;
    byteTxCounter = 0;
    while(  (byteTxCounter < ETHERNET_BUFFER_SIZE) && EthernetTxBufferDataAvailable() )
    {
      ethBuffer[byteTxCounter++] = PopEthernetTxPacket(&dest);
    }
    if(byteTxCounter)
    {
      for(connIter = 0; connIter < NUM_TCP_CONNECTIONS; connIter++)
      {
        if(  (dest>>connIter) & 1)
        {
          sendSocket = mConnections[connIter];
          if(sendSocket != 0)
          {
            totalBytesWritten = 0;
            while(totalBytesWritten < byteTxCounter)
            {
              bytesWritten = send(sendSocket, ethBuffer, (byteTxCounter-totalBytesWritten), 0);
              if(bytesWritten == -1)
              {
                // Write did not work
                printf("Writting to tcp socket failed!!!");
              }
              // Restart heartbeat timer
              RestartTimer(eTimeEthernetHeartBeat);
              totalBytesWritten += bytesWritten;
              EthTXCntr++;
            }
          }
          else
          {
            // Do Nothing
          }
        }
      }
    }
    
    while(EthernetRxBufferDataAvailable() && byteRxCounter < ETHERNET_BUFFER_SIZE)
    {
      newByte = PopEthernetRxPacket(&source);
      ethBuffer[byteRxCounter++] = newByte;
      
      if(newByte == 'b')
      {
        // break;
      }
    }
    if(byteRxCounter)
    {
      for(dataIter = 0; dataIter < byteRxCounter; dataIter++)
      {
        ProcessBbMessage(ethBuffer[dataIter], source);
      }
      //EthRXProcessCntr++;
    }
      
    osDelay(1);
  }
}
