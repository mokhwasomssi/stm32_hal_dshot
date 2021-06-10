/*
 * dshot.h
 *
 *
 *  Created on: 2021. 1. 27.
 *      Author: mokhwasomssi
 *
 */


#include "dshot.h"


extern DMA_HandleTypeDef hdma_tim2_ch1;
extern DMA_HandleTypeDef hdma_tim2_ch3_up;
extern DMA_HandleTypeDef hdma_tim5_ch2;
extern DMA_HandleTypeDef hdma_tim5_ch4_trig;


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
	uint32_t timer_clock = SystemCoreClock; // all timer clock is same as SystemCoreClock in stm32f411ceu6

	// Calculate prescaler that fits dshot type
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


// But __HAL_TIM_DISABLE_DMA is needed to Eliminate the delay between the signals
// I don't know why :(
static void dshot_dma_tc_callback(DMA_HandleTypeDef *hdma)
{
	TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

	if (hdma == htim->hdma[TIM_DMA_ID_CC1])
	{
		//__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC2])
	{
		//__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC3])
	{
		//__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
	}
	else if(hdma == htim->hdma[TIM_DMA_ID_CC4])
	{
		//__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
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


void dshot_init(dshot_type_e dshot_type)
{
	dshot_set_timer(dshot_type);

	dshot_put_tc_callback_function(); // the function that starts at transfer complete interrupt

	dshot_start_pwm();

	// Initialize motor value to 0
	for(int i = 0; i < 4; i++)
	{
		motor_value[i] = 0;
	}

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

// Convert 16 bits packet to 16 pwm signal sent to dma
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


static void dshot_dma_start(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t *pData, uint16_t Length)
{
    switch (channel) 
	{
		case TIM_CHANNEL_1:
			HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length);
			break;

		case TIM_CHANNEL_2:
			HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length);
			break;

		case TIM_CHANNEL_3:
			HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3,Length);
			break;

		case TIM_CHANNEL_4:
			HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)pData, (uint32_t)&htim->Instance->CCR4, Length);
			break;
    }
}


static void dshot_dma_stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    switch (Channel) 
	{
		case TIM_CHANNEL_1:
			HAL_DMA_Abort(htim->hdma[TIM_DMA_ID_CC1]);
			__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
			break;

		case TIM_CHANNEL_2:
			HAL_DMA_Abort(htim->hdma[TIM_DMA_ID_CC2]);
			__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
			break;

		case TIM_CHANNEL_3:
			HAL_DMA_Abort(htim->hdma[TIM_DMA_ID_CC3]);
			__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
			break;

		case TIM_CHANNEL_4:
			HAL_DMA_Abort(htim->hdma[TIM_DMA_ID_CC4]);
			__HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
			break;
    }
}


static void dshot_enable_dma_request()
{
	// TIM_DMA_CCx depends on timer channel
	__HAL_TIM_ENABLE_DMA(MOTOR_1_TIM, TIM_DMA_CC4);
	__HAL_TIM_ENABLE_DMA(MOTOR_2_TIM, TIM_DMA_CC3);
	__HAL_TIM_ENABLE_DMA(MOTOR_3_TIM, TIM_DMA_CC1);
	__HAL_TIM_ENABLE_DMA(MOTOR_4_TIM, TIM_DMA_CC2);
}


void dshot_write()
{
	dshot_prepare_dmabuffer(motor1_dmabuffer, motor_value[0]);
	dshot_prepare_dmabuffer(motor2_dmabuffer, motor_value[1]);
	dshot_prepare_dmabuffer(motor3_dmabuffer, motor_value[2]);
	dshot_prepare_dmabuffer(motor4_dmabuffer, motor_value[3]);

	__HAL_TIM_DISABLE_DMA(MOTOR_1_TIM, TIM_DMA_CC4);
	__HAL_TIM_DISABLE_DMA(MOTOR_2_TIM, TIM_DMA_CC3);
	__HAL_TIM_DISABLE_DMA(MOTOR_3_TIM, TIM_DMA_CC1);
	__HAL_TIM_DISABLE_DMA(MOTOR_4_TIM, TIM_DMA_CC2);

	dshot_dma_start(MOTOR_1_TIM, MOTOR_1_TIM_CHANNEL, motor1_dmabuffer, 18);
	dshot_dma_start(MOTOR_2_TIM, MOTOR_2_TIM_CHANNEL, motor2_dmabuffer, 18);
	dshot_dma_start(MOTOR_3_TIM, MOTOR_3_TIM_CHANNEL, motor3_dmabuffer, 18);
	dshot_dma_start(MOTOR_4_TIM, MOTOR_4_TIM_CHANNEL, motor4_dmabuffer, 18);

	dshot_enable_dma_request();
}

