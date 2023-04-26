/*can_net.c - File contains functions that relate to CAN processing*/

/************************************************************************************************
*   Included Headers										*
************************************************************************************************/
#include "can_net.h"
#include "BbProtocol.h"
#include "UiElevSchema.h"
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"

/************************************************************************************************
*   Variables - File Scope									*
************************************************************************************************/
CAN_RxHeaderTypeDef   RxCAN1Header;
uint8_t               RxCAN1Data[8];
CAN_RxHeaderTypeDef   RxCAN2Header;
uint8_t               RxCAN2Data[8];

TCANTXCircBuff        CAN1MsgTXAll;
TCANTXCircBuff        CAN2MsgTXAll;
TCANRXCircBuff        CANMsgRXAll;

uint32_t CANTxCntr = 0;
uint32_t CANRxCntr = 0;
uint32_t totalCANFIFO0cnt = 0;
uint32_t totalCANFIFO1cnt = 0;
uint32_t errorRxFIFOcan1cnt = 0;
uint32_t msgPendingFIFO0FillLevel = 0;
//uint32_t loopCounter = 0;

/************************************************************************************************
*   Function Prototypes - File Scope  								*
************************************************************************************************/
void processCANMessage(CANMsg_t mMsg);
void CAN1MessageRX(CANMsg_t *CANMessage);
void CAN2MessageRX(CANMsg_t *CANMessage);
void CANMessageRX(CANMsg_t *CANMessage, uint8_t Port);
uint8_t CANMessageTX(TCANTxObject *CANMessage);
void updateCircBuffer(TCANTXCircBuff *CANMsgTXAll, TCANTxObject *CANMessage);
unsigned char CANTransmit(unsigned char CANPort, CANMsg_t *msg);
uint8_t NO_HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CANMsg_t *msg);
uint8_t NO_HAL_CAN_IsTxMessagePending(CAN_HandleTypeDef *hcan, uint8_t TxMailboxes);

/************************************************************************************************
*   Function Definitions									*
************************************************************************************************/
/************************************************************************************************
* Function: CAN1MsgGet                                                             
* Purpose:  Put HAL CAN1 message into a CAN structure 
* Usage:	 
* Input:    CAN_RxHeaderTypeDef *RXHeader - CAN message header | uint8_t *RXData - array of CAN data
* Output:   N/A
************************************************************************************************/
void CAN1MsgGet(CAN_RxHeaderTypeDef *RXHeader, uint8_t *RXData)
{

  CANMsg_t msg;
  
  msg.stdID = RXHeader->StdId;
  msg.extID = RXHeader->ExtId;
  
  msg.IDType = (RXHeader->IDE == CAN_ID_STD) ? CAN_ID_STD: CAN_ID_EXT;
  msg.isRTR = (RXHeader->RTR == CAN_RTR_REMOTE) ? 1 : 0;
  msg.DL = RXHeader->DLC;
  
  for(unsigned char i = 0; i < 8; i++)
  {
    msg.data.D8[i] = RXData[i];
  }
  
  msg.RXFI = RXHeader->FilterMatchIndex;
  
  CAN1MessageRX((CANMsg_t*)&msg);
}

/************************************************************************************************
* Function: CAN2MsgGet                                                             
* Purpose:  Put HAL CAN2 message into a CAN structure 
* Usage:
* Input:    CAN_RxHeaderTypeDef *RXHeader - CAN message header | uint8_t *RXData - array of CAN data
* Output:   N/A
************************************************************************************************/
void CAN2MsgGet(CAN_RxHeaderTypeDef *RXHeader, uint8_t *RXData)
{

  CANMsg_t msg;
  
  msg.stdID = RXHeader->StdId;
  msg.extID = RXHeader->ExtId;
  
  msg.IDType = (RXHeader->IDE == CAN_ID_STD) ? CAN_ID_STD: CAN_ID_EXT;
  msg.isRTR = (RXHeader->RTR == CAN_RTR_REMOTE) ? 1 : 0;
  msg.DL = RXHeader->DLC;
  
  for(unsigned char i = 0; i < 8; i++)
  {
    msg.data.D8[i] = RXData[i];
  }
  
  msg.RXFI = RXHeader->FilterMatchIndex;
  
  CAN2MessageRX((CANMsg_t*)&msg);
}

/************************************************************************************************
* Function: HandleCANCallbacks                                                             
* Purpose:  Handle a CAN callback from either FIFO 0/1 
* Usage: 
* Input:    CAN_HandleTypeDef *hcan - pointer to CAN handle structure | uint32_t fifoNum - FIFO number
* Output:   N/A
************************************************************************************************/

void HandleCANCallbacks(CAN_HandleTypeDef *hcan, uint32_t fifoNum)
{
  uint8_t counter = 0;
  if(hcan->Instance == CAN1)
  {
    while((msgPendingFIFO0FillLevel = HAL_CAN_GetRxFifoFillLevel(hcan, fifoNum)) >  0)
    {
      //loopCounter++;
      counter++;
      if(HAL_CAN_GetRxMessage(hcan, fifoNum, &RxCAN1Header, RxCAN1Data) == HAL_OK)
      {
        // Receive message from FIFO and buffer
        CAN1MsgGet(&RxCAN1Header, RxCAN1Data);
      }
      else
      {
        //Reception Error
        errorRxFIFOcan1cnt++;
        printf("Error receiving CAN message from FIFO");
        //Error_Handler();
      }
      if(counter == 3)
      {
        break;
      }
    }
  }
  else
  {
    if(HAL_CAN_GetRxMessage(hcan, fifoNum, &RxCAN2Header, RxCAN2Data) == HAL_OK)
    {
      // Receive message from FIFO and buffer
      CAN2MsgGet(&RxCAN2Header, RxCAN2Data);
    }
    else
    {
      //Reception Error
      printf("Error receiving CAN message from FIFO");
      //Error_Handler();
    }
  }
}

/************************************************************************************************
* Function: ConfigureCanFilter                                                             
* Purpose:  Configures the CAN interface filters 
* Usage: 
* Input:    CAN_HandleTypeDef hcan - pointer to CAN handle structure | uint8_t canHandle - CAN1 or CAN2
* Output:   N/A
************************************************************************************************/
void ConfigureCanFilter(CAN_HandleTypeDef *hcan, uint8_t canHandle)
{

  CAN_FilterTypeDef  sFilterConfigCAN1;
  CAN_FilterTypeDef  sFilterConfigCAN2;
  
  switch(canHandle)
  {
  case 1:
    sFilterConfigCAN1.FilterBank = 0;
    sFilterConfigCAN1.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfigCAN1.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfigCAN1.FilterIdHigh = 0x0000;
    sFilterConfigCAN1.FilterIdLow = 0x0000;
    sFilterConfigCAN1.FilterMaskIdHigh = 0x0000;
    sFilterConfigCAN1.FilterMaskIdLow = 0x0000;
    sFilterConfigCAN1.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfigCAN1.FilterActivation = ENABLE;
    sFilterConfigCAN1.SlaveStartFilterBank = 14;
    
    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfigCAN1) != HAL_OK)
    {
      /* Filter configuration Error */
      Error_Handler();
    }
    
    /*## Start the CAN peripheral ###########################################*/
    if (HAL_CAN_Start(hcan) != HAL_OK)
    {
      /* Start Error */
      Error_Handler();
    }
    /* Enable the selected interrupts */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    
    break;
    
  case 2:
    sFilterConfigCAN2.FilterBank = 14;
    sFilterConfigCAN2.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfigCAN2.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfigCAN2.FilterIdHigh = 0x0000;
    sFilterConfigCAN2.FilterIdLow = 0x0000;
    sFilterConfigCAN2.FilterMaskIdHigh = 0x0000;
    sFilterConfigCAN2.FilterMaskIdLow = 0x0000;
    sFilterConfigCAN2.FilterFIFOAssignment = CAN_RX_FIFO1;
    sFilterConfigCAN2.FilterActivation = ENABLE;
    sFilterConfigCAN2.SlaveStartFilterBank = 14;
    
    if(HAL_CAN_ConfigFilter(hcan, &sFilterConfigCAN2) != HAL_OK)
    {
      /* Filter configuration Error */
      Error_Handler();
    }
    
    /*## Start the CAN peripheral ###########################################*/
    if (HAL_CAN_Start(hcan) != HAL_OK)
    {
      /* Start Error */
      Error_Handler();
    }
    
    /* Enable the selected interrupts */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
    
    break;
  }
}

/************************************************************************************************
* Function: CAN_Receive                                                             
* Purpose:  Calls the correct callback fuction 
* Usage: 
* Input:    CAN_HandleTypeDef *hcan - pointer to CAN handle structure, uint8_t FIFOnumber - FIFO number
* Output:   N/A
************************************************************************************************/
/*
void CAN_Receive(CAN_HandleTypeDef *hcan, uint8_t FIFOnumber)
{
  totalCANFIFO0cnt++;
  HandleCANCallbacks(hcan, FIFOnumber);
}
*/
/************************************************************************************************
* Function: CAN1MessageRX                                                             
* Purpose:  Calls CANMessageRX for a message from CAN port 1 
* Usage: 
* Input:    CANMsg_t *CANMessage - pointer to CAN message
* Output:   N/A
************************************************************************************************/
void CAN1MessageRX(CANMsg_t *CANMessage)
{
  CANMessageRX(CANMessage, CAN_PORT1);
}

/************************************************************************************************
* Function: CAN2MessageRX                                                             
* Purpose:  Calls CANMessageRX for a message from CAN port 2 
* Usage: 
* Input:    CANMsg_t *CANMessage - pointer to CAN message
* Output:   N/A
************************************************************************************************/
void CAN2MessageRX(CANMsg_t *CANMessage)
{
  CANMessageRX(CANMessage, CAN_PORT2);
}

/************************************************************************************************
* Function: CANMessageRX                                                             
* Purpose:  Receive a CAN message and put into circular buffer 
* Usage: 
* Input:    CANMsg_t *CANMessage - pointer to CAN message | uint8_t Port - CAN port received on
* Output:   N/A
************************************************************************************************/
void CANMessageRX(CANMsg_t *CANMessage, uint8_t Port)
{

  //
  // Put the Message into the slot.
  //
  CANMsgRXAll.Message[CANMsgRXAll.BuffIn].Port = Port;
  CANMsgRXAll.Message[CANMsgRXAll.BuffIn].MsgObject = *CANMessage;

  //CANMsgRXAll.Message[CANMsgRXAll.BuffIn].MsgObject.timeStamp = SysTimer;

  //
  // Check buffer In slot
  //
  if (++CANMsgRXAll.BuffIn >= CAN_RX_CIRC_BUFF_SIZE)
  {
    CANMsgRXAll.BuffIn = 0;
  }

  //
  //if the new message causes the in and out indexes to be equal
  // then an overflow has occurred. Set the overflow flag
  // and increment the counter.
  //
  if ((CANMsgRXAll.BuffIn) == CANMsgRXAll.BuffOut)
  {
    CANMsgRXAll.OvlCntr++;
    CANMsgRXAll.CANflgsStat.CANflgsBits.OvlFlg = TRUE;
  }

}

/************************************************************************************************
* Function: PopCANRXMessage                                                             
* Purpose:  Takes a CAN message from the receive buffer for processing 
* Usage:
* Input:    N/A
* Output:   CANMsg_t tempCANMsg - CAN message
************************************************************************************************/
CANMsg_t PopCANRXMessage(void)
{
  
  CANMsg_t tempCANMsg;
  
  tempCANMsg = CANMsgRXAll.Message[CANMsgRXAll.BuffOut].MsgObject;
  
  if(++CANMsgRXAll.BuffOut >= CAN_RX_CIRC_BUFF_SIZE)
  {
    CANMsgRXAll.BuffOut = 0;
  }
  
  return tempCANMsg;
}

/************************************************************************************************
* Function: ProcessCANQueue                                                             
* Purpose:  Process CAN messages in receive and transmit buffers
* Usage: 
* Input:    N/A
* Output:   N/A
************************************************************************************************/
void ProcessCANQueue(void)
{

  while(CANMsgRXAll.BuffOut != CANMsgRXAll.BuffIn)
  {
    
    CANMsg_t CANRXPopMsg = PopCANRXMessage();
    processCANMessage(CANRXPopMsg);
  }

  while(CAN1MsgTXAll.BuffOut != CAN1MsgTXAll.BuffIn)
  {
    if(CANTransmit(1, (CANMsg_t*)&CAN1MsgTXAll.Message[CAN1MsgTXAll.BuffOut].MsgObject))
    {
      CAN1MsgTXAll.CANflgsStat.CANflgsBits.OvlFlg = FALSE;
      
      //
      // Check buffer out slot
      //
      if (++CAN1MsgTXAll.BuffOut >= CAN_TX_CIRC_BUFF_SIZE)
      {
        CAN1MsgTXAll.BuffOut = 0;
      }
    }
    else
    {
      // Something went wrong with transmitting CAN object
      break;
    }
  }

  while(CAN2MsgTXAll.BuffOut != CAN2MsgTXAll.BuffIn)
  {
    if(CANTransmit(2, (CANMsg_t*)&CAN2MsgTXAll.Message[CAN2MsgTXAll.BuffOut].MsgObject))
    {
      CAN2MsgTXAll.CANflgsStat.CANflgsBits.OvlFlg = FALSE;
      
      //
      // Check buffer out slot
      //
      if(++CAN2MsgTXAll.BuffOut >= CAN_TX_CIRC_BUFF_SIZE)
      {
        CAN2MsgTXAll.BuffOut = 0;
      }
    }
    else
    {
      // Something went wrong with transmitting CAN object
      break;
    }
  }
}

/************************************************************************************************
* Function: processTestCSCMessage                                                             
* Purpose:  Test function to process a predetermined CAN message 
* Usage:
* Input:    CANMsg_t mMsg - CAN message to process
* Output:   N/A
************************************************************************************************/
void processTestCSCMessage(CANMsg_t mMsg)
{

  TCANTxObject tempTxCAN;
  tempTxCAN.CanData.ID = mMsg.stdID;
  tempTxCAN.CanData.MsgObject.DL = mMsg.DL;
  for(unsigned char i = 0; i < tempTxCAN.CanData.MsgObject.DL; i++)
  {
    tempTxCAN.CanData.MsgObject.data.D8[i] = mMsg.data.D8[i];
    gBbData[i+1] = tempTxCAN.CanData.MsgObject.data.D8[i];
  }
  
  gBbData[0] = tempTxCAN.CanData.ID;
  
  CreateBbMessage(0x01, 'S', SCHMA_STATUS_CSC_TESTING, 0, tempTxCAN.CanData.MsgObject.DL);
}

/************************************************************************************************
* Function: sendBackOnCAN                                                             
* Purpose:  Test function to process a predetermined CAN message 
* Usage:
* Input:    CANMsg_t mMsg - CAN message to process
* Output:   N/A
************************************************************************************************/
void sendBackOnCAN(CANMsg_t mMsg)
{
  
  TCANTxObject tempTxCAN;
  memset(&tempTxCAN, 0, sizeof(tempTxCAN));
  
  tempTxCAN.CanData.ID = 0x12;
  tempTxCAN.CanData.MsgObject.stdID = 0x12;
  tempTxCAN.CanData.Port = CAN_PORT1;
  
  tempTxCAN.CanData.MsgObject.DL = mMsg.DL;
  tempTxCAN.CanData.MsgObject.extID = 0;
  tempTxCAN.CanData.MsgObject.isRTR = mMsg.isRTR;
  tempTxCAN.CanData.MsgObject.IDType = mMsg.IDType;
  
  for(unsigned char i = 0; i < 8; i++)
  {
    tempTxCAN.CanData.MsgObject.data.D8[i] = mMsg.data.D8[i];
  }
  
  if(CANMessageTX(&tempTxCAN) == 0)
  {
    // Error adding to queue
    printf("Error adding CAN message to Transmit queue");
  }
}

/************************************************************************************************
* Function: processCANMessage                                                             
* Purpose:  Process a CAN message based off it's ID 
* Usage:
* Input:    CANMsg_t mMsg - CAN message to pass to processing functions
* Output:   N/A
************************************************************************************************/
void processCANMessage(CANMsg_t mMsg)
{
  
  unsigned short ID = mMsg.stdID;
  switch(ID)
  {
    case 0x10:
      CANRxCntr++;
      sendBackOnCAN(mMsg);
      break;
      
    case 0x11:
      CANRxCntr++;
      processTestCSCMessage(mMsg);
      break;
      
    default:
      break;
  }
}

/************************************************************************************************
* Function: CANMessageTX                                                        
* Purpose:  Adds a CAN message to the transmit circular buffer based on port
* Usage:
* Input:    TCANTxObject *CANMessage - pointer to CAN object to add to transmit buffer
* Output:   returns true if message was added, false if ID or length is incorrect
************************************************************************************************/
uint8_t CANMessageTX(TCANTxObject *CANMessage)
{

  //
  // Verify message ID and length .
  //
  if ((CANMessage->CanData.MsgObject.stdID > 0x7FF) || (CANMessage->CanData.MsgObject.DL > 8))
  {
    return (FALSE);
  }

  if (CANMessage->CanData.Port & CAN_PORT1)
  {
    updateCircBuffer(&CAN1MsgTXAll, CANMessage);
  }

  if (CANMessage->CanData.Port & CAN_PORT2)
  {
    updateCircBuffer(&CAN2MsgTXAll, CANMessage);
  }

  return (TRUE);
}

/************************************************************************************************
* Function: updateCircBuffer                                                        
* Purpose:  Add CAN object to add to specified transmit buffer
* Usage:
* Input:    TCANTXCircBuff *CANMsgTXAll - pointer to CAN buffer | TCANTxObject *CANMessage - pointer to CAN object
* Output:   N/A
************************************************************************************************/
void updateCircBuffer(TCANTXCircBuff *CANMsgTXAll, TCANTxObject *CANMessage)
{
  //
  // Put the Message into the slot.
  //
  CANMsgTXAll->Message[CANMsgTXAll->BuffIn].MsgObject = CANMessage->CanData.MsgObject;


  if (++CANMsgTXAll->BuffIn >= CAN_TX_CIRC_BUFF_SIZE)
  {
    CANMsgTXAll->BuffIn = 0;
  }

  //
  //if the new message causes the in and out indexes to be equal
  // then an overflow has occurred. Set the overflow flag
  // and increment the counter.
  //
  if (CANMsgTXAll->BuffIn == CANMsgTXAll->BuffOut)
  {
    CANMsgTXAll->OvlCntr++;
    CANMsgTXAll->CANflgsStat.CANflgsBits.OvlFlg = TRUE;
  }
}

/************************************************************************************************
* Function: CANTransmit                                                          
* Purpose:  Transmit a CAN message
* Usage:
* Input:    unsigned char CANPort - CAN port to transmit message | CANMsg_t *msg - pointer to CAN message
* Output:   return zero if transmition failed, one for success
************************************************************************************************/
unsigned char CANTransmit(unsigned char CANPort, CANMsg_t *msg)
{
 
  unsigned char result = 0;
  uint8_t txMailbox = 4;
  
  switch(CANPort)
  {
  case 1:
    if((msg->stdID == 0) && (msg->extID == 0))
    {
      return(1);
    }

    if((txMailbox = NO_HAL_CAN_AddTxMessage(&hcan1, msg)) != 4)
    {
      result = 1;
      CANTxCntr++;
    }
    else
    {
      printf("No empty mailbox, txMailbox: %i", txMailbox);
    }
    break;
  case 2:
    if((msg->stdID == 0) && (msg->extID == 0))
    {
      return(1);
    }
    
    if((txMailbox = NO_HAL_CAN_AddTxMessage(&hcan2, msg)) != 4)
    {
      result = 1;
      CANTxCntr++;
    }
    else
    {
      printf("No empty mailbox, txMailbox: %i", txMailbox);
    }
    break;
  }
  
  return result;
}

/************************************************************************************************
* Function: NO_HAL_CAN_AddTxMessage                                                          
* Purpose:  Add a message to the first free Tx mailbox and activate the
*           corresponding transmission request.
* Usage:
* Input:    CAN_HandleTypeDef *hcan - hcan pointer to a CAN_HandleTypeDef structure that contains
*           the configuration information for the specified CAN | CANMsg_t *msg - pointer to CAN message
* Output:   return zero if transmition failed, one for success
************************************************************************************************/
uint8_t NO_HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CANMsg_t *msg)
{
  
  uint8_t transmit_mailbox = 0;
  uint32_t tsr = READ_REG(hcan->Instance->TSR);
  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(msg->IDType));
  assert_param(IS_CAN_RTR(msg->isRTR));
  assert_param(IS_CAN_DLC(msg->DL));

  /* Select one empty transmit mailbox */
  if ((tsr & CAN_TSR_TME0) == CAN_TSR_TME0)
  {
    transmit_mailbox = 0;
  }
  else if ((tsr & CAN_TSR_TME1) == CAN_TSR_TME1)
  {
    transmit_mailbox = 1;
  }
  else if ((tsr & CAN_TSR_TME2) == CAN_TSR_TME2)
  {
    transmit_mailbox = 2;
  }
  else
  {
    transmit_mailbox = 4;
  }

  if (transmit_mailbox != 4)
  {
    /* Set up the Id */
    if (msg->IDType == CAN_ID_STD)
    {
      hcan->Instance->sTxMailBox[transmit_mailbox].TIR = ((msg->stdID << CAN_TI0R_STID_Pos) |
                                                         msg->isRTR);
    }
    else
    {
      hcan->Instance->sTxMailBox[transmit_mailbox].TIR = ((msg->extID << CAN_TI0R_EXID_Pos) |
                                                         msg->IDType |
                                                         msg->isRTR);
    }
    
    /* Set up the DLC */
    msg->DL &= (uint8_t)0x0000000F;
    hcan->Instance->sTxMailBox[transmit_mailbox].TDTR &= (uint32_t)0xFFFFFFF0;
    hcan->Instance->sTxMailBox[transmit_mailbox].TDTR |= msg->DL;

    /* Set up the data field */
    hcan->Instance->sTxMailBox[transmit_mailbox].TDLR = (((uint32_t)msg->data.D8[3] << 24) | 
                                             ((uint32_t)msg->data.D8[2] << 16) |
                                             ((uint32_t)msg->data.D8[1] << 8) | 
                                             ((uint32_t)msg->data.D8[0]));
    hcan->Instance->sTxMailBox[transmit_mailbox].TDHR = (((uint32_t)msg->data.D8[7] << 24) | 
                                             ((uint32_t)msg->data.D8[6] << 16) |
                                             ((uint32_t)msg->data.D8[5] << 8) |
                                             ((uint32_t)msg->data.D8[4]));
    /* Request transmission */
    SET_BIT(hcan->Instance->sTxMailBox[transmit_mailbox].TIR, CAN_TI0R_TXRQ);
  }
  osDelay(1);
  return transmit_mailbox;
}

/************************************************************************************************
* Function: NO_HAL_CAN_IsTxMessagePending                                                          
* Purpose:  Add a message to the first free Tx mailbox and activate the
*           corresponding transmission request.
* Usage:
* Input:    CAN_HandleTypeDef *hcan - hcan pointer to a CAN_HandleTypeDef structure that contains
*           the configuration information for the specified CAN | uint32_t TxMailboxes - TxMailboxes 
*           List of Tx Mailboxes to check. This parameter can be any combination of @arg CAN_Tx_Mailboxes.
* Output:   Zero for no pending transmission request on any selected Tx Mailboxes and
*           One for pending transmission request on at least one of the selected Tx Mailbox
************************************************************************************************/
uint8_t NO_HAL_CAN_IsTxMessagePending(CAN_HandleTypeDef *hcan, uint8_t TxMailboxes)
{
  uint8_t status = 0U;

  /* Check function parameters */
  assert_param(IS_CAN_TX_MAILBOX_LIST(TxMailboxes));

  /* Check pending transmission request on the selected Tx Mailboxes */
  if ((hcan->Instance->TSR & (TxMailboxes << CAN_TSR_TME0_Pos)) != (TxMailboxes << CAN_TSR_TME0_Pos))
  {
    status = 1U;
  }

  /* Return status */
  return status;
}


