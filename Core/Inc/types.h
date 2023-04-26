/*
 *##############################################################################
 *#
 *# filename:     Types.h
 *# project:      Programmable Hydraulic/Traction Controller
 *# description:  file description.
 *#
 *##############################################################################
 */

#ifndef __TYPES_H__
  #define __TYPES_H__

typedef union tagBit8
{
    uint8_t Byte;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    };
} bit8_t;

typedef union
{
    uint64_t D64;
    uint32_t D32[2];
    uint16_t D16[4];
    uint8_t D8[8];
    bit8_t BB[8];
} TCANData, *pTCANData;

#endif