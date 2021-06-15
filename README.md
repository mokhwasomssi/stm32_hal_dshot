# stm32f4_hal_dshot

__You can change speed of drone motor using `Dshot protocol` and `BLHeli_32 ESC`__   
__stm32 MCU makes `Dshot signal` using `PWM` and `DMA`__  


[![Video Label](https://user-images.githubusercontent.com/48342925/122101090-e3d39f80-ce4e-11eb-8edd-042559854eec.png)](https://youtu.be/KLO2bgV3Op8)


## 0. Development environment  
* MCU : [WeAct Black Pill V3.0 (STM32F411CEU6)](https://github.com/WeActTC/MiniF4-STM32F4x1)
* IDE : [STM32Cubeide (STM32Cube HAL)](https://www.st.com/en/development-tools/stm32cubeide)
* Firmware Package : [STM32Cube FW_F4 V1.26.1](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html#overview)
* ESC : [HAKRC BLHeli_32 Bit 35A 2-5S ESC](https://www.aliexpress.com/item/33008402033.html?spm=a2g0s.9042311.0.0.f7794c4dYDQfbS)
* Motor : [iFlight XING E 2207 2450KV](https://shop.iflight-rc.com/index.php?route=product/product&path=20_26_321&product_id=874)


## 1. Feature
* Choose Dshot 150/300/600
* only throttle could be change 
* other command not work


## 2. ESC Firmware and Protocol

### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|BLHeli_32|

### ESC Protocol
||Analogue signal|Digital signal|
|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|Dshot150 <br> Dshot300 <br> Dshot600 <br> Proshot1000|

## 3. Dshot

![image](https://user-images.githubusercontent.com/48342925/105716366-0f1e2680-5f62-11eb-8d5c-651e15907a53.png)  
### (1) Data Frame  
* 11bits : 0 - 47 command, 48 - 2047 throttle
* 1bit : telemetry request
* 4bits : checksum

### (2) Expression of bit 0/1 in dshot data frame  
![image](https://user-images.githubusercontent.com/48342925/105717632-b780ba80-5f63-11eb-9508-b54be4bd544d.png)

||bit 0|bit 1|
|:---:|:---:|:---:|
|duty cycle|37.425%|74.850%|

### (3) Transmission Time
||bits / sec|sec / bit|sec / frame|  
|:---:|:---:|:---:|:---:|  
|Dshot150|150,000 bits/s|6.67us|106.7us|
|Dshot300|300,000  bits/s|3.33us|53.3us|
|Dshot600|600,000 bits/s|1.67us|26.7us|


### (4) Example
<img src = https://user-images.githubusercontent.com/48342925/122077706-ec1fe080-ce36-11eb-9f24-a70c651b41c5.jpg width = "50%" height = "50%">

- throttle : 11 / telemetry request : no / checksum  
- 0 0 0 0 0 0 0 1 0 1 1 / 0 / 0 1 1 1 


## 4. Peripheral setting

- motor 1 : tim5 channel 4, DMA1 Stream 3
- motor 2 : tim2 channel 3, DMA1 Stream 1
- motor 3 : tim2 channel 1, DMA1 Stream 5 
- motor 4 : tim5 channel 2, DMA1 Stream 4

### STM32CubeMX Configuration
- TIM
    - Presacelr and Counter Period is set in code
```
- Mode
    Channalx : PWM Generation CHx

- Configuration
    Parameter Settings
        Counter Settings
            Prescaler                   : 0
            Counter Mode                : Up
            Counter Period              : 0
            Internal Clock Division     : No Division

        Trigger Output Parameters
            Master/Slave Mode           : Disable
            Trigger Event Selection     : Reset

        PWM Generation Channalx
            Mode                        : PWM mode 1
            Pulse                       : 0
            Output compare preload      : Enable
            Fast Mode                   : Disable
            CH Polarity                 : High

```

- DMA
```
- configuration
    DMAx
        DMA Request : TIMx_CHx
        Stream : DMAx Stream x
        Direction : Memory To Peripheral
        Priority : High

- DMA Request Settings
    Mode : Normal

    Use Fifo : Yes
    Threshold : One Quarter Full

    Peripheral
        Increment Address : No
        Data Width : Word
        Burst Size : Single

    Memory
        Increment Address : Yes
        Data Width : Word
        Burst Size : Single  
```

## 5. How to Use

### Precaution
- This code is fit for my peripheral setting

### main.c example
- Only write dshot code
- It's just a example

```c
#include "dshot.h"

extern uint16_t motor_value[4];

int main()
{
    dshot_init(DSHOT600);
    
    // throttle range : 48 - 2047
    motor_value[0] = 100;
    motor_value[1] = 100;
    motor_value[2] = 100;
    motor_value[3] = 100;

    while(1)
    {
        dshot_write();
        HAL_Delay(1);
    }
}

```