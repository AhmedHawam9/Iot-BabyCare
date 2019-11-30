# <center> Baby Care </center>
## Features:
    - Reading the temperature of the baby
    - shaking the baby's bed if he is crying or his parents desire to
    - measuring the heartbeat of the baby
## Libraries Used:
    - Developing ESP8266 on arduino IDE:
        - ESP8266: From Boards Manager after adding this link to the Additional Boards Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json
        - CH340G Driver: https://wiki.wemos.cc/downloads
        - CP2014 Driver: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
    - Web Server: https://github.com/me-no-dev/ESPAsyncWebServer
    - File System for uploading html file: https://github.com/esp8266/arduino-esp8266fs-plugin/releases
    - Heat Sensor:
        - Arduino-Temperature-Control-Library: https://github.com/milesburton/Arduino-Temperature-Control-Library
        - One Wire: https://github.com/PaulStoffregen/OneWire
## References Used:
    - Developing ESP8266 on arduino IDE: https://www.youtube.com/watch?v=AFUAMVFzpWw
    - Web Server: https://www.youtube.com/watch?v=dWM4p_KaTHY
    - File System for HTML page view: https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/
    - Heat Sensor: https://www.xtronical.com/basics/environmental-sensors/temperature-sensor-dallas-ds18b20/?fbclid=IwAR1KimdQGGY1xwxKlgO0dgLF0hZm5AZJNZMdwky8zbKmG8aijRV-uwgqC_Y
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