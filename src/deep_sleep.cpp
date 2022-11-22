// #include <Arduino.h>
// #include <DFRobot_QMC5883.h>
// #include <Wire.h>

// #define uS_TO_S_FACTOR 1000000
// #define TIME_TO_SLEEP 10 // Seconds

// RTC_DATA_ATTR int bootCount = 0;

// DFRobot_QMC5883 compass(&Wire, /*I2C addr*/ QMC5883_ADDRESS);

// void setup()
// {
//     Serial.begin(115200);
//     delay(1000); // Take some time to open up the Serial Monitor
//     pinMode(26, OUTPUT);
//     digitalWrite(26, HIGH);
//     delay(2000); // Take some time to open up the Serial Monitor

//     Wire.begin(21, 22);

//     ++bootCount;
//     Serial.println("Boot number: " + String(bootCount));
//     esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//     Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
//                    " Seconds");

//     while (!compass.begin())
//     {
//         Serial.println("Could not find a valid 5883 sensor, check wiring!");
//         delay(500);
//     }

//     if (compass.isQMC())
//     {
//         Serial.println("Device is QMC5883");
//         compass.setMeasurementMode(QMC5883_CONTINOUS);
//         Serial.print("compass measurement mode is:");
//         Serial.println(compass.getMeasurementMode());
//     }

//     float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
//     compass.setDeclinationAngle(declinationAngle);
//     sVector_t mag = compass.readRaw();
//     compass.getHeadingDegrees();
//     Serial.print("X:");
//     Serial.print(mag.XAxis);
//     Serial.print(" Y:");
//     Serial.print(mag.YAxis);
//     Serial.print(" Z:");
//     Serial.println(mag.ZAxis);
//     Serial.print("Degress = ");
//     Serial.println(mag.HeadingDegress);

//     Serial.println("Going to sleep now");
//     delay(1000);
//     Serial.flush();
//     digitalWrite(26, LOW);
//     esp_deep_sleep_start();
//     Serial.println("This will never be printed");
// }

// void loop()
// {
//     // This is not going to be called
// }