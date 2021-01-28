/*
 * dshot.h
 *
 *  This library refers to betaflight
 * 
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 * 
 */

#ifndef _DSHOT_H_
#define _DSHOT_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>


#define MOTOR_BIT_0           7
#define MOTOR_BIT_1           14
#define MOTOR_BITLENGTH       20

#define DSHOT_FRAME_SIZE 18


void dshot600(uint32_t *motor, uint16_t value);


#endif /* _DSHOT_H_ */
