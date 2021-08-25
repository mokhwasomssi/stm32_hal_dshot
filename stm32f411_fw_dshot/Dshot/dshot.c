/*
 * dshot.h
 *
 *
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 *
 */


#include "dshot.h"


/* Variables */
static uint32_t motor1_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor2_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor3_dmabuffer[DSHOT_DMA_BUFFER_SIZE];
static uint32_t motor4_dmabuffer[DSHOT_DMA_BUFFER_SIZE];


/* Static functions */
// dshot init
static uint32_t dshot_choose_type(dshot_type_e dshot_type);
static void dshot_set_timer(dshot_type_e dshot_type);
static void dshot_dma_tc_callback(DMA_HandleTypeDef *hdma);
static void dshot_put_tc_callback_function();
static void dshot_start_pwm();

// dshot write
static uint16_t dshot_prepare_packet(uint16_t value);
static void dshot_prepare_dmabuffer(uint32_t* motor_dmabuffer, uint16_t value);
static void dshot_prepare_dmabuffer_all();
static void dshot_dma_start();
static void dshot_enable_dma_request();


/* Functions */
void dshot_init(dshot_type_e dshot_type)
{
	dshot_set_timer(dshot_type);
	dshot_put_tc_callback_function();
	dshot_start_pwm();
}

void dshot_write(uint16_t* motor_value)
{
	dshot_prepare_dmabuffer_all(motor_value);
	dshot_dma_start();
	dshot_enable_dma_request();
}


/* Static functions */
static uint32_t dshot_choose_type(dshot_type_e dshot_type)
{
	switch (dshot_type)
	{
		case(DSHOT600):
				return DSHOT600_HZ;

		case(DSHOT300):
				return DSHOT300_HZ;

		default:
		case(DSHOT150):
				return DSHOT150_HZ;
	}
}

static void dshot_set_timer(dshot_type_e dshot_type)
{
	uint16_t dshot_prescaler;
	uint32_t timer_clock = TIMER_CLOCK; // all timer clock is same as SystemCoreClock in stm32f411

	// Calculate prescaler by dshot type
	dshot_prescaler = lrintf((float) timer_clock / dshot_choose_type(dshot_type) + 0.01f) - 1;

	// motor1
	__HAL_TIM_SET_PRESCALER(MOTOR_1_TIM, dshot_prescaler);
	__HAL_TIM_SET_AUTORELOAD(MOTOR_1_TIM, MOTOR_BITLENGTH);

	// motor2
	__HAL_TIM_SET_PRESCALER(MOTOR_2_TIM, dshot_prescaler);
	__HAL_TIM_SET_AUTORELOAD(MOTOR_2_TIM, MOTOR_BITLENGTH);

	// motor3
	__HAL_TIM_SET_PRESCALER(MOTOR_3_TIM, dshot_prescaler);
	__HAL_TIM_SET_AUTORELOAD(MOTOR_3_TIM, MOTOR_BITLENGTH);

	// motor4
	__HAL_TIM_SET_PRESCALER(MOTOR_4_TIM, dshot_prescaler);
	__HAL_TIM_SET_AUTORELOAD(MOTOR_4_TIM, MOTOR_BITLENGTH);
}

// __HAL_TIM_DISABLE_DMA is needed to eliminate the delay between different dshot signals
// I don't know why :(
static void dshot_dma_tc_callback(DMA_HandleTypeDef *hdma)
{
	TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

	if (hdma == htim->hdma[TIM_DMA_ID_CC1])
	{
		__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC2])
	{
		__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC3])
	{
		__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC4])
	{
		__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
	}
}

static void dshot_put_tc_callback_function()
{
	// TIM_DMA_ID_CCx depends on timer channel
	MOTOR_1_TIM->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = dshot_dma_tc_callback;
	MOTOR_2_TIM->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = dshot_dma_tc_callback;
	MOTOR_3_TIM->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = dshot_dma_tc_callback;
	MOTOR_4_TIM->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = dshot_dma_tc_callback;
}

static void dshot_start_pwm()
{
	// Start the timer channel now.
    // Enabling/disabling DMA request can restart a new cycle without PWM start/stop.
  	HAL_TIM_PWM_Start(MOTOR_1_TIM, MOTOR_1_TIM_CHANNEL);
  	HAL_TIM_PWM_Start(MOTOR_2_TIM, MOTOR_2_TIM_CHANNEL);
	HAL_TIM_PWM_Start(MOTOR_3_TIM, MOTOR_3_TIM_CHANNEL);
	HAL_TIM_PWM_Start(MOTOR_4_TIM, MOTOR_4_TIM_CHANNEL);
}

static uint16_t dshot_prepare_packet(uint16_t value)
{
	uint16_t packet;
	bool dshot_telemetry = false;

	packet = (value << 1) | (dshot_telemetry ? 1 : 0);

	// compute checksum
	unsigned csum = 0;
	unsigned csum_data = packet;

	for(int i = 0; i < 3; i++)
	{
        csum ^=  csum_data; // xor data by nibbles
        csum_data >>= 4;
	}

	csum &= 0xf;
	packet = (packet << 4) | csum;

	return packet;
}

// Convert 16 bits packet to 16 pwm signal
static void dshot_prepare_dmabuffer(uint32_t* motor_dmabuffer, uint16_t value)
{
	uint16_t packet;
	packet = dshot_prepare_packet(value);

	for(int i = 0; i < 16; i++)
	{
		motor_dmabuffer[i] = (packet & 0x8000) ? MOTOR_BIT_1 : MOTOR_BIT_0;
		packet <<= 1;
	}

	motor_dmabuffer[16] = 0;
	motor_dmabuffer[17] = 0;
}

static void dshot_prepare_dmabuffer_all(uint16_t* motor_value)
{
	dshot_prepare_dmabuffer(motor1_dmabuffer, motor_value[0]);
	dshot_prepare_dmabuffer(motor2_dmabuffer, motor_value[1]);
	dshot_prepare_dmabuffer(motor3_dmabuffer, motor_value[2]);
	dshot_prepare_dmabuffer(motor4_dmabuffer, motor_value[3]);
}

static void dshot_dma_start()
{
	HAL_DMA_Start_IT(MOTOR_1_TIM->hdma[TIM_DMA_ID_CC4], (uint32_t)motor1_dmabuffer, (uint32_t)&MOTOR_1_TIM->Instance->CCR4, DSHOT_DMA_BUFFER_SIZE);
	HAL_DMA_Start_IT(MOTOR_2_TIM->hdma[TIM_DMA_ID_CC3], (uint32_t)motor2_dmabuffer, (uint32_t)&MOTOR_2_TIM->Instance->CCR3, DSHOT_DMA_BUFFER_SIZE);
	HAL_DMA_Start_IT(MOTOR_3_TIM->hdma[TIM_DMA_ID_CC1], (uint32_t)motor3_dmabuffer, (uint32_t)&MOTOR_3_TIM->Instance->CCR1, DSHOT_DMA_BUFFER_SIZE);
	HAL_DMA_Start_IT(MOTOR_4_TIM->hdma[TIM_DMA_ID_CC2], (uint32_t)motor4_dmabuffer, (uint32_t)&MOTOR_4_TIM->Instance->CCR2, DSHOT_DMA_BUFFER_SIZE);
}

static void dshot_enable_dma_request()
{
	__HAL_TIM_ENABLE_DMA(MOTOR_1_TIM, TIM_DMA_CC4);
	__HAL_TIM_ENABLE_DMA(MOTOR_2_TIM, TIM_DMA_CC3);
	__HAL_TIM_ENABLE_DMA(MOTOR_3_TIM, TIM_DMA_CC1);
	__HAL_TIM_ENABLE_DMA(MOTOR_4_TIM, TIM_DMA_CC2);
}
