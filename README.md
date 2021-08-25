# stm32_hal_dshot

## Brief
__Dshot is drone ESC digital protocol__   

You need STM32 MCU, BLHeli_32 ESC, BLDC  
STM32 MCU makes Dshot signal using PWM and DMA

__KEYWORD__ - `DSHOT` `BLHeli_32 ESC` `BLDC` `STM32 HAL` `TIMER` `PWM` `DMA`

![dshot](https://user-images.githubusercontent.com/48342925/130811044-cabbd6df-1b0b-4482-be93-9c13b84f68b6.gif)


## Dev Environment  
- STM32CubeIDE
- STM32 HAL driver
- STM32F411
- BLHeli_32 ESC
- Drone BLDC


## Library Features
* Change motor throttle
    * Throttle range : 2000 steps (0% - 100%)
* Choose Dshot 150/300/600
* Other command : no
* Telemetry : no


## ESC Firmware and Protocol

### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|`BLHeli_32`|

### ESC Protocol
||Analogue signal|`Digital signal`|
|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|`Dshot150` <br>` Dshot300` <br> `Dshot600` <br> Proshot1000|

## Dshot

![image](https://user-images.githubusercontent.com/48342925/105716366-0f1e2680-5f62-11eb-8d5c-651e15907a53.png)  
### 1. Data Frame  
* 11bits : 0 - 47 command, 48 - 2047 throttle
* 1bit : telemetry request
* 4bits : checksum

### 2. Bit 0 / 1
![image](https://user-images.githubusercontent.com/48342925/105717632-b780ba80-5f63-11eb-9508-b54be4bd544d.png)

||bit 0|bit 1|
|:---:|:---:|:---:|
|duty cycle|37.425%|74.850%|

### 3. Transmission Time
||bits / sec|sec / bit|sec / frame|  
|:---:|:---:|:---:|:---:|  
|Dshot150|150,000 bits/s|6.67us|106.7us|
|Dshot300|300,000  bits/s|3.33us|53.3us|
|Dshot600|600,000 bits/s|1.67us|26.7us|


### 4. Signal Example
<img src = https://user-images.githubusercontent.com/48342925/122077706-ec1fe080-ce36-11eb-9f24-a70c651b41c5.jpg width = "50%" height = "50%">

- throttle value : 11 / telemetry request : 0 (no) / checksum 4 bits
- 0 0 0 0 0 0 0 1 0 1 1 / 0 / 0 1 1 1 

### 5. Arming sequence
- After power on, Send zero throttle for a while until 1 high beep is ended.  
- _7p of BLHeli_32 manual ARM Rev32.x.pdf_
<img src = https://user-images.githubusercontent.com/48342925/124644199-f466bb00-decc-11eb-8dc9-396d9b9706d7.png width = "70%" height = "70%">      




## STM32CubeMX

- Project Manager
![image](https://user-images.githubusercontent.com/48342925/124620697-a34acd00-deb4-11eb-8b47-8fe5a3dad001.png)

- TIM
![image](https://user-images.githubusercontent.com/48342925/124617628-0e46d480-deb2-11eb-8aad-5a72027d4d35.png)
![image](https://user-images.githubusercontent.com/48342925/124617830-37fffb80-deb2-11eb-93e4-341fbcec2ac5.png)

- DMA
![image](https://user-images.githubusercontent.com/48342925/124618725-fde32980-deb2-11eb-842a-e863431dd1b8.png)




## Example

### dshot.h
- TIM5, TIM2 is 100MHz
- MOTOR 1 - PA3
- MOTOR 2 - PA2
- MOTOR 3 - PA0
- MOTOR 4 - PA1
- Some instance are not pre-defined, so need to change in person...
    - like `TIM_DMA_ID_CC4`, `TIM_DMA_CC4`, `CCR4`...
```c
/* User Configuration */
// Timer Clock
#define TIMER_CLOCK				100000000	// 100MHz

// MOTOR 1 (PA3) - TIM5 Channel 4, DMA1 Stream 3
#define MOTOR_1_TIM             (&htim5)
#define MOTOR_1_TIM_CHANNEL     TIM_CHANNEL_4

// MOTOR 2 (PA2) - TIM2 Channel 3, DMA1 Stream 1
#define MOTOR_2_TIM             (&htim2)
#define MOTOR_2_TIM_CHANNEL     TIM_CHANNEL_3

// MOTOR 3 (PA0) - TIM2 Channel 1, DMA1 Stream 5
#define MOTOR_3_TIM             (&htim2)
#define MOTOR_3_TIM_CHANNEL     TIM_CHANNEL_1

// MOTOR 4 (PA1) - TIM5 Channel 2, DMA1 Stream 4
#define MOTOR_4_TIM             (&htim5)
#define MOTOR_4_TIM_CHANNEL     TIM_CHANNEL_2
```

### main.c
- only contain dshot things

```c
#include "dshot.h"

// 4 motor value
uint16_t my_motor_value[4] = {0, 0, 0, 0};

int main (void)
{
    // initialize
    dshot_init(DSHOT600);

    while (1)
    {
        // transmit new dshot signals
        dshot_write(my_motor_value);
        HAL_Delay(1);
    }
}
```