/*
 * dshot.c
 *
 *  This library refers to betaflight
 * 
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 */

#include "dshot.h"

void dshot_init()
{
  
}

uint32_t getDshotHz(DSHOT dshot_type)
{
    switch (dshot_type) 
    {
    case(DSHOT600):
        return MOTOR_DSHOT600_HZ;
    case(DSHOT300):
        return MOTOR_DSHOT300_HZ;
    default:
    case(DSHOT150):
        return MOTOR_DSHOT150_HZ;
    }
}

// align 16bit dshot signal : 11bits(command & throttle) + 1bit(telemetry request) + 4bits(checksum)
// command : 0 - 47
// throttle : 48 - 2047
uint16_t prepareDshotPacket(uint16_t value, bool requestTelemetry) {

  uint16_t packet = (value << 1) | (requestTelemetry ? 1 : 0);
  requestTelemetry = false;    // reset telemetry request to make sure it's triggered only once in a row

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

  return packet;
}

// DEC packet -> BIN dshot single(pwm)
uint8_t loadDmaBufferDshot(uint32_t *dmaBuffer, int stride, uint16_t packet)
{
    int i;
    for (i = 0; i < 16; i++) {
        dmaBuffer[i * stride] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;  // MSB first
        packet <<= 1;
    }
    dmaBuffer[i++ * stride] = 0;
    dmaBuffer[i++ * stride] = 0;

    return DSHOT_DMA_BUFFER_SIZE;
}
