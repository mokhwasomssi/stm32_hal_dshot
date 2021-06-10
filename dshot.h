/*
 * dshot.h
 *
 *
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 * 
 */


#ifndef _DSHOT_H_
#define _DSHOT_H_


#include "tim.h"    // header from stm32cubemx code generate


/* user pin setting */
// MOTOR1
// PA3, DMA1 Stream 3
#define MOTOR_1_TIM             (&htim5)
#define MOTOR_1_TIM_CHANNEL     TIM_CHANNEL_4

// MOTOR2
// PA2, DMA1 Stream 1
#define MOTOR_2_TIM             (&htim2)
#define MOTOR_2_TIM_CHANNEL     TIM_CHANNEL_3

// MOTOR3
// PA0, DMA1 Stream 5
#define MOTOR_3_TIM             (&htim2)
#define MOTOR_3_TIM_CHANNEL     TIM_CHANNEL_1

// MOTOR4
// PA1, DMA1 Stream 4
#define MOTOR_4_TIM             (&htim5)
#define MOTOR_4_TIM_CHANNEL     TIM_CHANNEL_2


/* definition */
#define NUM_OF_MOTORS          4    // quadcopter

#define MHZ_TO_HZ(x) ((x) * 1000000)

#define DSHOT600_HZ     MHZ_TO_HZ(12)
#define DSHOT300_HZ     MHZ_TO_HZ(6)
#define DSHOT150_HZ     MHZ_TO_HZ(3)

#define MOTOR_BIT_0            7
#define MOTOR_BIT_1            14
#define MOTOR_BITLENGTH        20

#define DSHOT_FRAME_SIZE       16
#define DSHOT_DMA_BUFFER_SIZE   18 /* resolution + frame reset (2us) */

#define DSHOT_MIN_THROTTLE       48
#define DSHOT_MAX_THROTTLE     2047
#define DSHOT_RANGE (DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)


/* enumeration */
typedef enum
{
    DSHOT150,
    DSHOT300,
    DSHOT600

} dshot_type_e;


/* variable */
uint32_t motor1_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
uint32_t motor2_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
uint32_t motor3_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
uint32_t motor4_dmabuffer[DSHOT_DMA_BUFFER_SIZE];

uint16_t motor_value[4]; // motor speed value


/* functions */
void dshot_init(dshot_type_e dshot_type);
void dshot_write();


#endif /* _DSHOT_H_ */
