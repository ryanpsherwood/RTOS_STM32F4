/* 
 * File:   CANdataTypes.h
 * Author: msaka
 *
 * Created on October 26, 2017, 2:44 PM
 */

#ifndef __CANDATATYPES_H__
  #define	__CANDATATYPES_H__


  #ifdef	__cplusplus
extern "C"
{
  #endif
  
  #include "Types.h"

  #include "LocalBufferSizes.h"

  #pragma pack()
  typedef struct
  {
      TCANData data;
      unsigned long stdID;                                           //Standard CAN ID this parameter can be a value between 0 to 0x7FF.
      unsigned long extID;                                           //Extended CAN ID this parameter can be a value between 0 to 0x1FFFFFFF.
      unsigned long timeStamp;                                       //time stamp for when the message is sent or arrives
      unsigned char IDType;                                          //Specifies the type of identifier for the message Standard or Extended.
      unsigned char isRTR;                                           //CAN remote transmission request flag
      unsigned char DL;                                              //length of the frame can be a value between 0 to 8
      unsigned char RXFI;                                            //RX filter index
  }CANMsg_t;

  typedef struct
  {
      uint8_t Port;
      CANMsg_t MsgObject;
      uint16_t ID;
      uint16_t SeqNumber;
      uint32_t TimeStamp;
  }TCAN, *pTCAN;

  typedef struct
  {
      TCAN CanData;
      union
      {
          uint8_t flags;
          struct
          {
              uint8_t Send :1;
              uint8_t TimerExpired :1;
              uint8_t autoSend :1;
              uint8_t RTR :1;
          }CANflgsBits;
      }CANflgsStat;
  }TCANTxObject;

  typedef struct
  {
      TCAN CanData;
      union
      {
          uint8_t flags;
          struct
          {
              uint8_t RecvFlg :1;
              uint8_t ProcFlg :1;
              uint8_t InitFlg :1;
              uint8_t ChangeFlg :1;
              uint8_t rxPort :2;
          }CANflgsBits;
      }CANflgsStat;
  }TCANRxObject;

  typedef struct
  {
      TCAN Message[CAN_TX_CIRC_BUFF_SIZE];
      uint16_t BuffSize;
      uint16_t CanPort;
      uint16_t BuffIn;
      uint16_t BuffOut;
      uint16_t OvlCntr;
      uint32_t MaxMsgs;
      uint32_t AvgMsgs;
      uint32_t LastMsgs;
      uint32_t SendFailed;
      uint32_t Iterations;
      uint32_t LastIteration;
      uint32_t LastDelta;
      uint32_t MaxDelta;
      union
      {
          uint8_t flags;
          struct
          {
              uint8_t OvlFlg :1;
              uint8_t autoTxInhibit :1;
          }CANflgsBits;
      }CANflgsStat;
  }TCANTXCircBuff;

  typedef struct
  {
      TCAN Message[CAN_RX_CIRC_BUFF_SIZE];
      uint16_t BuffSize;
      uint16_t BuffIn;
      uint16_t BuffOut;
      uint16_t OvlCntr;
      uint32_t MaxMsgs;
      uint32_t AvgMsgs;
      uint32_t LastMsgs;
      uint32_t Iterations;
      union
      {
          uint8_t flags;
          struct
          {
              uint8_t OvlFlg :1;
          }CANflgsBits;
      }CANflgsStat;
  }TCANRXCircBuff;
  
  #ifdef	__cplusplus
}
  #endif


#endif  /* CANDATATYPES_H */