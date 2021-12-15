# AUTOMATIC FOOD DISPENSER FOR PETS

---
## Table of Contents
---
 * [Abstract](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#abstract)
 * [Hardware Components](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#hardware-components)
 * [How it Works](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#how-it-works)
   * [Flowchart](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#flowchart)
 * [Contact](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#contact)
 * [License](https://github.com/Bernitarist/AUTOMATIC-FOOD-DISPENSER-FOR-PETS#license) <br><br>


## [Abstract](#heading-3)

Existing pet food-dispensers provide minimal adaptability, functionality and user personalization, leading to overfeeding or underfeeding of the pets. This project aims at solving the problems faced by pet owners by allowing them to customize the time interval between feeding times. <br><br>The device will dispense food based on the pet's weight, thereby allowing the right amount of food for each pet. Feedback from the pet is also incorporated. The corresponding feed information is sent to a database to be accessed by the owner, allowing them to plan efficiently. 

 <br> 

## [Hardware components](#heading-3)

| **Component**                        | **Photo**                                                                       |
| ------------------------------------ | ------------------------------------------------------------------------------- |
| Liquid Crystal Display / LCD adapter | <img src="Photos/lcd1.jpg" width="300"> <img src="Photos/lcd2.png" width="350"> |
| ESP 8266-01                          | <img src="Photos/esp.jpg" width="200">                                          |
| Servo motor SG90                     | <img src="Photos/servo.jpg" width="200">                                        |
| Arduino Mega 2560                    | <img src="Photos/mega.jpg" width="250">                                         |
| Load Cell / HX711 (2 pcs)            | <img src="Photos/loadcell.jpg" width="250">                                     |
| RTC DS3231                           | <img src="Photos/rtc.jpg" width="200">                                          |
| Push Buttons (3 pcs)                 | <img src="Photos/push.jpg" width="200">                                         |

<br><br>
## [How it Works](#heading-3)
  
 * The owner will set the preferred feeding times
 * On arrival of the specified time the buzzer will be triggered as an alert to the pet
 * Detection of the pet occurs by measuring its weight 
 * The buzzer goes off and the feeder-door opened to release the corresponding food amount, based on the pet's weight
 * The feeder door is closed and the device starts over
 * The pet's weight and the food consumed is sent to a database for monitoring <br><br>

   * ### Flowchart
     --- 
    <img src="Photos/flowchart.png" width="400"> <br>

## [Contact](#heading-3)

> [Email](berniemukonesi@gmail.com "berniemukonesi@gmail.com") <br>
> [Phone](+254713187640 "+254 713187640") <br>
> [LinkedIn](linkedin.com/in/bernice-mukonesi-a9410618a "linkedin.com/in/bernice-mukonesi-a9410618a") <br>

## [License](#heading-3)