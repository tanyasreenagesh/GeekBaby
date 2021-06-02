!(https://github.com/tanyasreenagesh/GeekBaby/blob/main/Dashboard.PNG)<img width="973" alt="Dashboard" src="https://user-images.githubusercontent.com/35225535/120423989-65d8aa00-c320-11eb-8090-4a214f31cd18.PNG">


# Abstract
The GeekBaby product, with the wireless sensor node and the website dashboard, is a quick and efficient way for parents to check in on their baby’s environment throughout the day, whether that is from another room in the house or many miles away. Designed to keep parents updated about their child, the main goals of the GeekBaby product are to allow parents to view data collected by the light, sound, humidity, temperature and air quality sensors in the device and make decisions about changing the brightness of light around the baby, adjusting room temperature with the built-in fan, or consoling the baby by playing a nursery rhyme. 

The GeekBaby system is designed to send data collected by the sensors using the WiFi module in the board (which I found to be the fastest method to communicate with external servers). The data is received almost instantly on the server side and a response containing the flags (for controlling the actuators) is sent back to the board every 2 seconds or so. As data is received on the server side, it is also displayed on the GeekBaby dashboard which is viewable by parents for monitoring whenever they choose to. All of the data on the dashboard updates continuously and automatically, i.e. the page does not need to be refreshed to see the latest data collected.

## System Architecture

The block diagram shown below is the final architecture of the GeekBaby system. The SparkFun Redboard is connected to a set of sensors and actuators that allow parents to monitor their children’s environment and control it. The following sensors are connected to the system: sound sensor, temperature and humidity sensor, light sensor and an air quality sensor. The actuators connected to this system are: LED to control the brightness of light in the room, Buzzer to play nursery rhymes to console the baby, and finally the Servo which acts as a fan to control the room's temperature. 

The board is connected to a power supply that allows it to be portable and makes use of the in-board WiFi module to communicate with the outside world and send sensor readings to the AWS server. The server stores this information locally and displays it on the GeekBaby dashboard user-interface for parents to monitor. The dashboard also allows parents to control actuators in the baby’s room by sending flags back to the Redboard as a response to data sent. This makes the communication between the Redboard and the AWS Server two-way. 

![BlockDiagram](https://user-images.githubusercontent.com/35225535/120424219-c7991400-c320-11eb-9e84-c713eb747683.png)


## Weak Points & Future Work

One of the main disadvantages of the GeekBaby product is that parents control it using a web-based application and not a mobile app. It would be much more convenient to open up an app and be able to view all of the analytics and control actuators remotely in real time. Also, there may be a slight delay between the data uploaded and the real time data. There is a more substantial delay between button presses on the user-interface and implementation on the Redboard side. As mentioned in the sections above, the POST requests sent from the web server to the board are not consistently registered. Some form of threading may be required when running the Flask application to fix this issue.

Another disadvantage is the setup of the wireless sensor node. My implementation makes use of a variety of sensors, and each of them stick out of the pins of the SparkFun Redboard. Furthermore, the temperature and humidity sensor are connected via the use of a breadboard. Practically, this kind of setup makes it challenging for parents to handle the GeekBaby device. It also has sharp edges, which is not safe to leave unattended around a baby and can be cause for concern.

