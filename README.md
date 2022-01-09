# AWS-v0.1
AWS or Automated Weather Station is an ESP32 based weather monitoring module. The project was started in 2021 by [Suryan S](https://github.com/suryan-s) as an IoT project for WorkspaceDevelopers. The project is licensed under MIT License. 

Many weather forecast websites allow users to view forecasts up to five years in advance, but the exact conditions for a specific location may not be quite accurate. Most of the readings from the station are accurate within a radius of 1-5km. Readings at locations beyond that radius are generally of average to low accuracy. Using a low-cost AWS cluster, the data provided would be much more location-specific and accurate, leading to better weather forecasts and climate analysis. 

![AWS image](https://github.com/suryan-s/AWS-v0.1/blob/main/Docs/aws.jpeg?raw=true)
## Working and Components
**Zambretti equation** is used to forecast the weather for this project. A forecast index is calculated based on the pressure trend calculated from the on-read pressure value. Every 10 minutes the device gather environmental data. The pressure for each reading is stored in an array. The array is divided after 3 hours into a previous pressure reading and a subsequent pressure reading. The trend of dropping, stable, or rising is calculated on the basis of the difference between the two.

In V.01, the forecast index used here does not take into account the season and the wind direction, which will be updated in the future.

The project supports for both **ESP32** and **ESP8266** board. All the sensors expect the UV and Raindrop are I2C based.

Choosing the right sensors for this project is important. Though there are many environmental-based sensors in the market with a variety of price points, sometimes choosing the right one is difficult. An Arduino forum just solved the issue. Based on the work by **lilitu** on comparison of temperature and humidity testing via i2c, broad results were obtained and from the data, the best sensor could be selected.
https://forum.arduino.cc/t/compare-different-i2c-temperature-and-humidity-sensors-sht2x-sht3x-sht85/599609

Even though DHT22 is used most commonly for temperature and humidity, they have a short life compared to others. The hysteresis of the sensor is also another factor. Some sensors give a fluctuating reading at a particular point that requires time to get back to a normal state. 

The required libraary for the arduino IDE is provided in the repo. The most common error that might happen is the anologRead which was a big issue  in ESP32. Analogue libraary is also inlcuded in the repo.
https://github.com/espressif/arduino-esp32/issues/102

### PCB design
The PCB design is based on NodeMCU ESP32-S. Sensors could be attached to the board via a JST 2.5mm 4 pin socket to minimize loose connections. additional 6 I2C pins are provied for board expansion. LDO is used to increase the life of Li-ion battery becasue the board will keep on drawing power from the battery until the internal LDO of the board reaches the minimum voltage. BC547B is used as a switching transistor to save the exposed rain sensor copper pads from corrosions. After getting the reading, the sensor is turned off via the transistor. For solar panel, min 2W rating panels are recommended with 5V output.
The PCB section includes the required gerber files, circuit diagram and 2d 1-scale image of the board.
![PCB circuit](https://github.com/suryan-s/AWS-v0.1/blob/main/PCB/AWS-GERBER-img.jpg?raw=true)

### SQL-ESP32 board
The data obtained by the sensors are updated to the SQL database. The webpage would display the table with all the uploaded sensor data.
AJAX havn't been added to the php script due to the maximum request limitation by the server. also since the data uploaded by the sensors are in between 15 minitues, a continous refreshing is not necessary. In future updates, a complete dashboard for weather station would be included.
![SQL table](https://github.com/suryan-s/AWS-v0.1/blob/main/Docs/sql-web-table.jpg?raw=true)
The tutorial for SQL ESP32:https://randomnerdtutorials.com/esp32-esp8266-mysql-database-php/

The PHP scripts were edited to suit this project which would be in the docs section of the repo. Place them into the public_html of the file manager from your respective hosting service
 **Don’t forget tO change the apiKey and database credentials.**  
### References
All the major links which will help out to know more about the concepts and ideations would be linked down below. This includes the effect of UV in plants, Zambretti Equation and weather station development:

https://www.researchgate.net/publication/238784241_The_effect_of_UV-B_radiation_on_plant_growth_and_development
http://www.truganinaweather.com/projects/zambretti-forecaster-project.htm
https://communities.sas.com/t5/SAS-Analytics-for-IoT/Zambretti-Algorithm-for-Weather-Forecasting/td-p/679487
https://github.com/fandonov/weatherstation
https://earthscience.stackexchange.com/questions/16366/weather-forecast-based-on-pressure-temperature-and-humidity-only-for-implement
 
## BOM:
- ESP32 dev module/ ESP32-S
- AHT10 Temperature and humidity sensor 
- BH1750 light intensity sensor 
- ML8511 Ultra-Violet sensor
- BMP180 pressure sensor
- Raindrop detection sensor 
- MCP1700-2702ETO
- Mini-Slide-Switch
- BC547B
- 18650 2700-3200mAh battery
## Environment Factors:
- Temperature
- Humidity
- UV index
- Light intensity
- Pressure
- Rain detection

## Future Updates:
- Dew point and heat index
- Battery monitoring and ESP deep sleep for power saving
- LoRa and GSM/GPRS based communication
- Updated sensors
- Rain gauge and Anemomter
- Solar panel arrangements and STL files for complete weather station module  

