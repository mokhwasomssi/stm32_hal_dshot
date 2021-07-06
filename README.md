# stm32_hal_dshot

__Dshot is drone ESC digital protocol__   


__You need `MCU`, `BLHeli_32 ESC`, `BLDC`__     
__MCU makes `Dshot signal` using `PWM` and `DMA`__  


[![Video Label](https://user-images.githubusercontent.com/48342925/122101090-e3d39f80-ce4e-11eb-8edd-042559854eec.png)](https://youtu.be/KLO2bgV3Op8)


## 0. Development environment  
* MCU : [STM32F411CEU6 (WeAct Black Pill V3.0)](https://github.com/WeActTC/MiniF4-STM32F4x1)
* IDE : [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide)
* ESC : [HAKRC BLHeli_32 Bit 35A 2-5S ESC](https://www.aliexpress.com/item/33008402033.html?spm=a2g0s.9042311.0.0.f7794c4dYDQfbS)
* BLDC : [iFlight XING E 2207 2450KV](https://shop.iflight-rc.com/index.php?route=product/product&path=20_26_321&product_id=874)


## 1. Feature
* Change motor throttle
* Throttle range : 2000 steps
    - 0 - 0%
    - 2000 - 100%
* Choose Dshot 150/300/600
* Other command : no
* Telemetry : no


## 2. ESC Firmware and Protocol

### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|`BLHeli_32`|

### ESC Protocol
||Analogue signal|`Digital signal`|
|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|`Dshot150` <br>` Dshot300` <br> `Dshot600` <br> Proshot1000|

## 3. Dshot

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


### 4. Example
<img src = https://user-images.githubusercontent.com/48342925/122077706-ec1fe080-ce36-11eb-9f24-a70c651b41c5.jpg width = "50%" height = "50%">

- throttle : 11 / telemetry request : no / checksum  
- 0 0 0 0 0 0 0 1 0 1 1 / 0 / 0 1 1 1 

## 4. Arming sequence

### After power on, Send zero throttle for a while until 1 high beep is ended.

![image](https://user-images.githubusercontent.com/48342925/124644199-f466bb00-decc-11eb-8dc9-396d9b9706d7.png)
(_capture from 7p of BLHeli_32 manual ARM Rev32.x.pdf_)


## 5. User Configuration

### 1. Pin
- MOTOR 1 (PA3)
    - TIM5 Channel 4, DMA1 Stream 3

- MOTOR 2 (PA2)
    - TIM2 Channel 3, DMA1 Stream 1

- MOTOR 3 (PA0)
    - TIM2 Channel 1, DMA1 Stream 5 

- MOTOR 4 (PA1)
    - TIM5 Channel 2, DMA1 Stream 4

### 2. Wiring

![image](https://user-images.githubusercontent.com/48342925/124646706-f41bef00-decf-11eb-848c-0fee8ddc3461.png)

### 3. STM32CubeMX Configuration

#### Project Manager

![image](https://user-images.githubusercontent.com/48342925/124620697-a34acd00-deb4-11eb-8b47-8fe5a3dad001.png)


#### Pinout & Configuration

- TIM2 (TIM5 is also same)

![image](https://user-images.githubusercontent.com/48342925/124617628-0e46d480-deb2-11eb-8aad-5a72027d4d35.png)

![image](https://user-images.githubusercontent.com/48342925/124617830-37fffb80-deb2-11eb-93e4-341fbcec2ac5.png)


- DMA

![image](https://user-images.githubusercontent.com/48342925/124618725-fde32980-deb2-11eb-842a-e863431dd1b8.png)
