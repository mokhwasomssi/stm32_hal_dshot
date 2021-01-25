# stm32f4_hal_dshot

__Change speed of drone motor using `dshot protocol` and `BLHeli_32 ESC`__  
__Send `dshot signal` using `DMA` and `PWM`__    

## 0. Development environment  
* IDE : [STM32Cubeide (STM32Cube HAL)](https://www.st.com/en/development-tools/stm32cubeide)
* MCU : [WeAct Black Pill V3.0 (STM32F411CEU6)](https://github.com/WeActTC/MiniF4-STM32F4x1)
* ESC : [HAKRC BLHeli_32 Bit 35A 2-5S ESC](https://www.aliexpress.com/item/33008402033.html?spm=a2g0s.9042311.0.0.f7794c4dYDQfbS)
* Motor : [iFlight XING E 2207 2450KV](https://shop.iflight-rc.com/index.php?route=product/product&path=20_26_321&product_id=874)

## 1. ESC Firmware and Protocol
### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|BLHeli_32|

### ESC Protocol
||Analogue signal|Digital signal|
|:---:|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|Dshot150 <br> Dshot300 <br> Dshot600 <br> Dshot 1200|


## 2. Dshot

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



## 1. FEATURE



## 2. User Configuration