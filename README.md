# stm32f4_hal_dshot

__You can change speed of drone motor using `Dshot protocol` and `BLHeli_32 ESC`__   
__stm32f4 MCU makes `Dshot signal` using `DMA` and `PWM`__  


## 0. Development environment  
* IDE : [STM32Cubeide (STM32Cube HAL)](https://www.st.com/en/development-tools/stm32cubeide)
* MCU : [WeAct Black Pill V3.0 (STM32F411CEU6)](https://github.com/WeActTC/MiniF4-STM32F4x1)
* ESC : [HAKRC BLHeli_32 Bit 35A 2-5S ESC](https://www.aliexpress.com/item/33008402033.html?spm=a2g0s.9042311.0.0.f7794c4dYDQfbS)
* Motor : [iFlight XING E 2207 2450KV](https://shop.iflight-rc.com/index.php?route=product/product&path=20_26_321&product_id=874)

## 1. Feature
* Dshot 600
* only throttle could be change 
* other command not work


## 2. User Configuration

### (1) STM32CubeMX
* TIMx : 100Mhz
```
- Mode
    Channalx : PWM Generation CHx

- Configuration
    Parameter Settings
        Counter Settings
            Prescaler                   : 21-1
            Counter Mode                : Up
            Counter Period              : 20
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

* DMA
```
- configuration
    DMAx
        DMA Request : TIMx_CHx
        Stream : DMAx Stream x
        Direction : Memory To Peripheral
        Priority : Low

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

## 3. ESC Firmware and Protocol

### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|BLHeli_32|

### ESC Protocol
||Analogue signal|Digital signal|
|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|Dshot150 <br> Dshot300 <br> Dshot600 <br> Dshot 1200|


## 4. Dshot

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

### (3) Timing
||bits / sec|sec / bit|frames / sec|sec / frame|
|:---:|:---:|:---:|:---:|:---:|
|DShot150|150,000 bits/s|6.67us|9375 frames/s|106.7us|
|DShot300|300,000  bits/s|3.33us|18750 frames/s|53.3us|
|DShot600|600,000 bits/s|1.67us|37500 frames/s|26.7us|
|DShot1200|1,200,000 bits/s|0.83us|75000 frames/s|13.3us|