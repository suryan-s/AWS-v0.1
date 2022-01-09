
/*                                       -------------------------
                                         | O      | USB |      O |
                                         |        -------        |
                                     3V3 | [ ]               [ ] | VIN
                                     GND | [ ]               [ ] | GND
     Touch3 / HSPI_CS0 / ADC2_3 / GPIO15 | [ ]               [ ] | GPIO13 / ADC2_4 / HSPI_ID / Touch4
 CS / Touch2 / HSPI_WP / ADC2_2 /  GPIO2 | [ ]               [ ] | GPIO12 / ADC2_5 / HSPI_Q / Touch5
      Touch0 / HSPI_HD / ADC2_0 /  GPIO4 | [ ]               [ ] | GPIO14 / ADC2_6 / HSPI_CLK / Touch6
                         U2_RXD / GPIO16 | [ ]               [ ] | GPIO27 / ADC2_7 / Touch7
                         U2_TXD / GPIO17 | [ ]               [ ] | GPIO26 / ADC2_9 / DAC2
                      V_SPI_CS0 /  GPIO5 | [ ]  ___________  [ ] | GPIO25 / ADC2_8 / DAC1
                SCK / V_SPI_CLK / GPIO18 | [ ] |           | [ ] | GPIO33 / ADC1_5 / Touch8 / XTAL32
        U0_CTS / MSIO / V_SPI_Q / GPIO19 | [ ] |  ESP32    | [ ] | GPIO32 / ADC1_4 / Touch9 / XTAL32
                 SDA / V_SPI_HD / GPIO21 | [ ] |           | [ ] | GPIO35 / ADC1_7 
                  CLK2 / U0_RXD /  GPIO3 | [ ] |           | [ ] | GPIO34 / ADC1_6 
                  CLK3 / U0_TXD /  GPIO1 | [ ] |           | [ ] | GPIO39 / ADC1_3 / SensVN 
        SCL / U0_RTS / V_SPI_WP / GPIO22 | [ ] |           | [ ] | GPIO36 / ADC1_0 / SensVP 
                MOSI / V_SPI_WP / GPIO23 | [ ] |___________| [ ] | EN 
                                         |                       |
                                         |  |  |  ____  ____  |  |
                                         |  |  |  |  |  |  |  |  |
                                         |  |__|__|  |__|  |__|  |
                                         | O                   O |
                                         -------------------------

*/ 

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <Wire.h>
#include<math.h>
#include<string.h>

// Replace with your network credentials
const char * ssid = "XXXXXXXXXXX";
const char * password = "XXXXXXXXXXX";

// REPLACE with your Domain name and URL path or IP address with path
const char * serverName = "http://your_domain.000webhostapp.com/esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /esp-data.php also needs to have the same key 
//apiKeyValue length must be 13
//https://passwordsgenerator.net/ for generating the code

String apiKeyValue = "XXXXXXXXXXXXX";
                                     
String sensorName = "YOUR_SENSOR_NAME";
String sensorLocation = "LOCATION_OF_YOUR_SENSOR";

Adafruit_AHTX0 aht;
Adafruit_BMP085 bmp;
Adafruit_Sensor * aht_humidity, * aht_temp;
BH1750 lightMeter;

int Z, count = 0, trend, Po, Pressure_C, Temperature_C, Humidity_C, Altitude_C, Intensity_C, UV_C, UVOUT = 32, REF_3V3 = 4, Rain_Analog = 35, Rain_Power = 34, Rain_Val, pArr[18] = {0};
char forecast[40] = {"Loading"};

int averageAnalogRead(int pin) {
        byte numberOfReadings = 8;
        unsigned int runningValue = 0;

        for (int x = 0; x < numberOfReadings; x++)
                runningValue += analogRead(pin);
        runningValue /= numberOfReadings;

        return (runningValue);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float uvSensor() {
        int uvLevel = averageAnalogRead(UVOUT);
        int refLevel = averageAnalogRead(REF_3V3);
        float outputVoltage = 3.3 / refLevel * uvLevel;                    //Use the 3.3V power pin as a reference to get a very accurate output value from sensor  
        float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //To convert the voltage to a UV intensity level
        return (uvIntensity);
}

void Zambretti_forecast(int P, int T, int A) {

        digitalWrite(LED_BUILTIN, HIGH);
        count += 1;
        Po = P * pow(1 - ((0.0065 * A) / (T + (0.0065 * A) + 273.15)), -5.257); //Converting the on read pressure to sea pressure equivalent 
        Serial.printf("Pressure:%d, Sea-Preassure converted: %d ", P, Po);
        if (count <= 17) {
                pArr[count] = Po;
        } else {
                for (int k = 0; k < 17; k++) {
                        pArr[k] = pArr[k + 1];
                }
                pArr[17] = Po;
                /*
                pArr[] takes 18 readings in 3 hours. The array is the divided into two halfs and average is taken as before and after readings.
                Based on the pressure drop between the before and after pArr[] values, trend type  and Forecast index is calculated.
                */
                int P_avg_b = (pArr[0] + pArr[1] + pArr[2] + pArr[3] + pArr[4] + pArr[5] + pArr[6] + pArr[7] + pArr[8]) / 9;
                int P_avg_a = (pArr[9] + pArr[10] + pArr[11] + pArr[12] + pArr[13] + pArr[14] + pArr[15] + pArr[16] + pArr[17]) / 9;

                if (P_avg_b > P_avg_a) {
                        //droping
                        trend = 1;
                        Z = 127 - 0.12 * Po;
                } else if (P_avg_b < P_avg_a) {
                        //rising
                        trend = 2;
                        Z = 185 - 0.16 * Po;
                } else {
                        //steady
                        trend = 3;
                        Z = 144 - 0.13 * Po;
                }

                Serial.printf("\n Forecast index z:%d", Z);

        }

        if (trend == 1) {
                Serial.print("\n Dropping");
                switch (Z) {
                case 1:
                        strcpy(forecast, "Settled Fine");
                        break;
                case 2:
                        strcpy(forecast, "Fine Weather");
                        break;
                case 3:
                        strcpy(forecast, "Fine, Becoming Less Settled");
                        break;
                case 4:
                        strcpy(forecast, "Fairly Fine, Showery Late");
                        break;
                case 5:
                        strcpy(forecast, "Showery, Becoming More Unsettled");
                        break;
                case 6:
                        strcpy(forecast, "Unsettled, Rain Later");
                        break;
                case 7:
                        strcpy(forecast, "Rain at Times, Worse Later");
                        break;
                case 8:
                        strcpy(forecast, "Rain at Times, Becoming Very Unsettled");
                        break;
                case 9:
                        strcpy(forecast, "Very Unsettled, Rain");
                        break;
                default:
                        strcpy(forecast, "error");
                        break;
                }
        } else if (trend == 2) {
                Serial.print("\n Rising");
                switch (Z) {
                case 20:
                        strcpy(forecast, "Settled Fine");
                        break;
                case 21:
                        strcpy(forecast, "Fine Weather");
                        break;
                case 22:
                        strcpy(forecast, "Becoming Fine");
                        break;
                case 23:
                        strcpy(forecast, "Fairly Fine, Improving");
                        break;
                case 24:
                        strcpy(forecast, "Fairly Fine, Possibly Showers Early");
                        break;
                case 25:
                        strcpy(forecast, "Showery Early, Improving");
                        break;
                case 26:
                        strcpy(forecast, "Changeable, Mending");
                        break;
                case 27:
                        strcpy(forecast, "Rather Unsettled, Clearing Later");
                        break;
                case 28:
                        strcpy(forecast, "Unsettled, Probably Improving");
                        break;
                case 29:
                        strcpy(forecast, "Unsettled, Short Fine Intervals");
                        break;
                case 30:
                        strcpy(forecast, "Very Unsettled, Finer at Times");
                        break;
                case 31:
                        strcpy(forecast, "Stormy, Possibly Improving");
                        break;
                case 32:
                        strcpy(forecast, "Stormy, Much Rain");
                        break;
                default:
                        strcpy(forecast, "error");
                        break;
                }
        } else if (trend == 3) {
                Serial.print("\n Steady");
                switch (Z) {
                case 10:
                        strcpy(forecast, "Settled Fine");
                        break;
                case 11:
                        strcpy(forecast, "Fine Weather");
                        break;
                case 12:
                        strcpy(forecast, "Fine, Possibly Showers");
                        break;
                case 13:
                        strcpy(forecast, "Fairly Fine, Showers Likely");
                        break;
                case 14:
                        strcpy(forecast, "Showery, Bright Intervals");
                        break;
                case 15:
                        strcpy(forecast, "Changeable, Some Rain");
                        break;
                case 16:
                        strcpy(forecast, "Unsettled, Rain at Times");
                        break;
                case 17:
                        strcpy(forecast, "Rain at Frequent Intervals");
                        break;
                case 18:
                        strcpy(forecast, "Very Unsettled, Rain");
                        break;
                case 19:
                        strcpy(forecast, "Stormy, Much Rain");
                        break;
                default:
                        strcpy(forecast, "error");
                        break;
                }

        }
        Serial.printf("\n Forecast result: %s", forecast);
        Serial.println("\n\n");
        digitalWrite(LED_BUILTIN, LOW);
        
}

void Sensor(void) {
         /***
         * IRLZ44N is used here to increase the lifespan of the rain sensor. Thus after the turing on the transistor,
          the rain sensor activates. after reading, the transistor is turned off.
         * A 10 millisecond delay is given for the settling the power. 
        ***/
        digitalWrite(LED_BUILTIN, HIGH); 
        digitalWrite(Rain_Power, HIGH);
        delay(10);
        aht_temp = aht.getTemperatureSensor();
        aht_humidity = aht.getHumiditySensor();
        sensors_event_t humidity, temp;
        aht.getEvent( & humidity, & temp);

        //Sensor reading starts:

        Pressure_C = (int) bmp.readPressure() / 100;
        Altitude_C = (int) bmp.readAltitude();
        Temperature_C = (int) temp.temperature;
        Humidity_C = (int) humidity.relative_humidity;
        Intensity_C = lightMeter.readLightLevel();
        UV_C = uvSensor();
        Rain_Val = analogRead(Rain_Analog);

        
        /***
         * Print data to serial monitor
         * Serial.print section is still kept in the code for debugging in case of faulty data upload to the database.
         * After testing the sensor readings and SQL uploaded data, you could remove the serial.print section.
         * SerialBT could be used to monitor the total status of the weather station via a serial bluetooth terminasl 
         ***/

        Serial.println("\nTemperature AHT10: ");
        Serial.print(Temperature_C);
        Serial.println(" deg C");
        Serial.println("Humidity: ");
        Serial.print(Humidity_C);
        Serial.println(" % rH");
        Serial.println("Pressure: ");
        Serial.print(Pressure_C);
        Serial.println(" millibar");
        Serial.println("Altitude: ");
        Serial.print(Altitude_C);
        Serial.println(" meters");
        Serial.println("Light: ");
        Serial.print(Intensity_C);
        Serial.println(" lx");
        Serial.println(" ML8511 UV intensity: ");
        Serial.print(UV_C);
        Serial.println(" mW/cm^2");
        if (Rain_Val < 100) {
                Serial.println("Raining!");
        } else Serial.println("No rain detected..");
        digitalWrite(Rain_Power, HIGH);  // Rain sensor turned off
        digitalWrite(LED_BUILTIN, LOW);
}

void mySQL_dataUpdate() {

        digitalWrite(LED_BUILTIN, HIGH);        
        if (WiFi.status() == WL_CONNECTED) {
                WiFiClient client;
                HTTPClient http;

                http.begin(client, serverName);

                http.addHeader("Content-Type", "application/x-www-form-urlencoded");

                // Prepare your HTTP POST request data
                String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName +
                        "&location=" + sensorLocation + "&Temperature=" + Temperature_C +
                        "&Humidity=" + Humidity_C + "&Pressure=" + Pressure_C +
                        "&UV_index=" + UV_C + "&Light_LUX=" + Intensity_C + "";
                Serial.print("httpRequestData: ");
                Serial.println(httpRequestData);

                //Instead of this httpRequestData, you could send the request as in a single line by uncommenting the below line:
                //String httpRequestData = "api_key=XXXXXXXXXXXXX&sensor=XXXXX&location=XXXXx&Temperature=24.75&Humidity=49.54&Pressure=1005.14&UV_index=1005.14&Light_LUX=1005.14";

                // Send HTTP POST request
                int httpResponseCode = http.POST(httpRequestData);
                if (httpResponseCode > 0) {
                        Serial.print("HTTP Response code: ");
                        Serial.println(httpResponseCode);
                } else {
                        Serial.print("Error code: ");
                        Serial.println(httpResponseCode);
                }
                // Free resources
                http.end();
        } else {
                Serial.println("WiFi Disconnected");
        }
        digitalWrite(LED_BUILTIN, LOW); 
}

void setup() {
        Serial.begin(115200);
        WiFi.begin(ssid, password);
        Serial.println("Connecting");
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
        Wire.begin();
        pinMode(LED_BUILTIN, OUTPUT);
        pinMode(UVOUT, INPUT);
        pinMode(REF_3V3, INPUT);
        pinMode(Rain_Analog, INPUT);
        

        digitalWrite(LED_BUILTIN, HIGH); 
        delay(100);                     
        digitalWrite(LED_BUILTIN, LOW); 
        delay(100);

        if (!aht.begin()) {
                Serial.println("Could not find AHT! Check wiring");
                while (1) delay(10);
        } else Serial.println("AHT10 Found!");

        if (!bmp.begin()) {
                Serial.println("Could not find a valid BMP180! Check wiring!");
                while (1) delay(10);
        } else Serial.println("BMP180 Found!");
        if (!lightMeter.begin()) {
                Serial.println("Could not find BH1750! Check wiring");
                while (1) delay(10);
        } else Serial.println("BH1750 Found!");
}

void loop() {

        digitalWrite(LED_BUILTIN, HIGH); 
        delay(100);                     
        digitalWrite(LED_BUILTIN, LOW); 
        delay(100);

        Sensor();
        Zambretti_forecast(Pressure_C, Temperature_C, Altitude_C);
        mySQL_dataUpdate();
        delay(1000ul*60*15);//delay for 15 minutes
        
}