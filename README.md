# <center> Baby Care </center>
## Demo
- Please check the demo video we made about the project [here](https://youtu.be/ymee8DEb5Fk)
## How To Run:
1. Upload the arduino code into Arduino
2. Upload the ESP code into ESP8266
3. Upload the data folder into ESP8266 using file system library
4. Follow the connection in the website
5. Hopefully run successfully and try to maintain any fire or explosions :)

## Features:
- A website to monitor the baby and control his environment
- Shaking the baby's bed if he is crying or his parents desire to
- Reading the temperature of the baby
- Measuring the heartbeat of the baby

## Names And Emails
1. DiaaEldin Magdi Amin - DiaaAbdElmaksoud@hotmail.com
2. Mohamed Reda Elsaygh - moelsaygh7@gmail.com 
3. Asmaa Ibrahim Mahfouz - ai782273@gmail.com 
4. Amr Osama Zakzouk - amro.osama159@gmail.com 
5. Ahmed Hamdy Hawam - ahmed.hawam2030@gmail.com
6. Mohammed Moataz Atef - mohamedmoataz12@gmail.com 
7. Mahmoud Reda Abd-Elhady - Mahmoud.reda84@yahoo.com
8. Nada Makram Mokhtar - nadamakram42@gmail.com 
9. Mariam Ali Eldoweny - mariam.eldoweny.2016@gmail.com
10. Omar Ahmed El-Alfy - asdasdasd3.as@gmail.com
11. Mahmoud Maher Al-Mekkawy - Mahmoudalmakawy@gmail.com
## Libraries Used:
- Developing ESP8266 on arduino IDE:
    - ESP8266: From Boards Manager after adding this link to the Additional Boards Manager URLs: [here](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
    - CH340G Driver: [here](https://wiki.wemos.cc/downloads)
    - CP2014 Driver: [here](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
- Web Server: [here](https://github.com/me-no-dev/ESPAsyncWebServer)
- File System for uploading html file: [here](https://github.com/esp8266/arduino-esp8266fs-plugin/releases)
- Temprature Sensor:
    - Arduino-Temperature-Control-Library: [here](https://github.com/milesburton/Arduino-Temperature-Control-Library)
    - One Wire: [here](https://github.com/PaulStoffregen/OneWire)
## References Used:
- Developing ESP8266 on arduino IDE: [here](https://www.youtube.com/watch?v=AFUAMVFzpWw)
- Web Server:
    - [here](https://www.youtube.com/watch?v=dWM4p_KaTHY)
    - [here](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html)
- File System for HTML page view: [here](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/)
- Temprature Sensor: [here](https://www.xtronical.com/basics/environmental-sensors/temperature-sensor-dallas-ds18b20/?fbclid=IwAR1KimdQGGY1xwxKlgO0dgLF0hZm5AZJNZMdwky8zbKmG8aijRV-uwgqC_Y)
- Heartbeat Sensor: [here](https://github.com/Arafa-microsys/Projects/blob/master/PulseSensor/pulse_sensor.ino)
- connecting arduino and esp8266 via serial ports: [here](https://www.youtube.com/watch?v=6-RXqFS_UtU)
# <center> Version 1.1 </center>
## What's new:
1. adding new features
    - making the bed stop shaking after one minute for demonstration purposes
## bugs handled:
1. Servo motor
    - making the motor begin from zero position
    - reducing the lag that happen when we refresh the web page as much as possible (1 sec)
2. Mic sensor
    - responding to noise around regardless of the previous state of the noise
3. Website
    - responding to the shake and calm buttons Regardless the state of the child
