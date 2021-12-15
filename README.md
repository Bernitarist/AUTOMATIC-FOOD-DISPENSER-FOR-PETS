# AUTOMATIC-FOOD-DISPENSER-FOR-PETS

---
## Table of Contents
---


## [Abstract](#heading-3)

 Existing pet food-dispensers provide minimal adaptability, functionality and user personalization, leading to overfeeding or underfeeding of the pets. This project aims at solving the problems faced by pet owners by allowing them to customize the time interval between feeding times. <br><br>The device will dispense food based on the pet's weight, thereby allowing the right amount of food for each pet. Feedback from the pet is also incorporated. The corresponding feed information is sent to a database to be accessed by the owner, allowing them to plan efficiently. 




## [Hardware components](#heading-3)

   1. Liquid Crystal Display / LCD Adapter

<img src="Photos/lcd1.jpg" width="300">       <img src="Photos/lcd2.png" width="350">

   2. ESP8266-01 

<img src="Photos/esp.jpg" width="200"> 

  3. Servo motor SG90
 
<img src="Photos/servo.jpg" width="200"> 

  4. Arduino Mega 2560
  
<img src="Photos/mega.jpg" width="250"> 

  5. Load Cell / HX711 (2 pcs)
 
<img src="Photos/loadcell.jpg" width="250"> 

  6. RTC DS3231
  
<img src="Photos/rtc.jpg" width="200"> 

  7. Push Buttons (3 pcs) 

<img src="Photos/push.jpg" width="200"> <br><br><br>

## [How it Works (Flowcharts)](#heading-3)
  
 * The owner will set the preferred feeding times
 * On arrival of the specified time the buzzer will be triggered as an alert to the pet
 * Detection of the pet occurs by measuring its weight 
 * The buzzer goes off and the feeder-door opened to release the corresponding food amount, based on the pet's weight
 * The feeder door is closed and the device starts over
 * The pet's weight and the food consumed is sent to a database for monitoring

   * **Main Flowchart** 
 ---
<img src="Photos/flowchart.png" width="250"> 