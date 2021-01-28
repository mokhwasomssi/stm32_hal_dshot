/*
 * dshot.c
 *
 *  This library refers to betaflight
 * 
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 */

#include "dshot.h"


void dshot600(uint32_t *motor, uint16_t value)
{
  uint16_t packet = value << 1;

  // compute checksum
  int csum = 0;
  int csum_data = packet;
  for (int i = 0; i < 3; i++) {
    csum ^=  csum_data;   // xor data by nibbles
    csum_data >>= 4;
  }
  csum &= 0xf;

  // append checksum
  packet = (packet << 4) | csum;

  // encoding
  int i;
  for (i = 0; i < 16; i++)
  {
      motor[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;  // MSB first
      packet <<= 1;
  }

  motor[i++] = 0;
  motor[i++] = 0;
}
