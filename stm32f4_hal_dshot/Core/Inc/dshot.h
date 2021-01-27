/*
 * dshot.h
 *
 *  This library refers to betaflight
 * 
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 * 
 */

#ifndef INC_DSHOT_H_
#define INC_DSHOT_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>


/* User Configuration */

#define USER_MCU      STM32F411CEU6

#define MOTOR1_TIMER        htim2
#define MOTOR2_TIMER        htim3
#define MOTOR3_TIMER        htim4
#define MOTOR4_TIMER        htim5
/* User Configuration */


#define MHZ_TO_HZ(x) ((x) * 1000000)

// throttle range
#define DSHOT_MIN_THROTTLE       48
#define DSHOT_MAX_THROTTLE     2047
#define DSHOT_RANGE (DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

// dshot type
#define MOTOR_DSHOT600_HZ     MHZ_TO_HZ(12)
#define MOTOR_DSHOT300_HZ     MHZ_TO_HZ(6)
#define MOTOR_DSHOT150_HZ     MHZ_TO_HZ(3)
typedef enum
{
    DSHOT150,
    DSHOT300,
    DSHOT600
    //    DSHOT1200, removed
} DSHOT;


#define MOTOR_BIT_0           7
#define MOTOR_BIT_1           14
#define MOTOR_BITLENGTH       20

#define DSHOT_DMA_BUFFER_SIZE 18  // 0-15 : dshot frame,  16-17 : frame reset

// dshot command
#define DSHOT_MAX_COMMAND 47

typedef enum {
    DSHOT_CMD_MOTOR_STOP = 0,
    DSHOT_CMD_BEACON1,
    DSHOT_CMD_BEACON2,
    DSHOT_CMD_BEACON3,
    DSHOT_CMD_BEACON4,
    DSHOT_CMD_BEACON5,
    DSHOT_CMD_ESC_INFO, // V2 includes settings
    DSHOT_CMD_SPIN_DIRECTION_1,
    DSHOT_CMD_SPIN_DIRECTION_2,
    DSHOT_CMD_3D_MODE_OFF,
    DSHOT_CMD_3D_MODE_ON,
    DSHOT_CMD_SETTINGS_REQUEST, // Currently not implemented
    DSHOT_CMD_SAVE_SETTINGS,
    DSHOT_CMD_SPIN_DIRECTION_NORMAL = 20,
    DSHOT_CMD_SPIN_DIRECTION_REVERSED = 21,
    DSHOT_CMD_LED0_ON, // BLHeli32 only
    DSHOT_CMD_LED1_ON, // BLHeli32 only
    DSHOT_CMD_LED2_ON, // BLHeli32 only
    DSHOT_CMD_LED3_ON, // BLHeli32 only
    DSHOT_CMD_LED0_OFF, // BLHeli32 only
    DSHOT_CMD_LED1_OFF, // BLHeli32 only
    DSHOT_CMD_LED2_OFF, // BLHeli32 only
    DSHOT_CMD_LED3_OFF, // BLHeli32 only
    DSHOT_CMD_AUDIO_STREAM_MODE_ON_OFF = 30, // KISS audio Stream mode on/Off
    DSHOT_CMD_SILENT_MODE_ON_OFF = 31, // KISS silent Mode on/Off
    DSHOT_CMD_MAX = 47
} dshotCommands_e;

typedef enum {
    DSHOT_CMD_TYPE_INLINE = 0,    // dshot commands sent inline with motor signal (motors must be enabled)
    DSHOT_CMD_TYPE_BLOCKING       // dshot commands sent in blocking method (motors must be disabled)
} dshotCommandType_e;


void dshot_init();
uint32_t getDshotHz(DSHOT dshot_type);
uint16_t prepareDshotPacket(uint16_t value, bool requestTelemetry);
uint8_t loadDmaBufferDshot(uint32_t *dmaBuffer, int stride, uint16_t packet);


#endif /* INC_DSHOT_H_ */
