#include <Arduino.h>
#include "Application.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <WiFi.h>
#include "esp_camera.h"
#include "wifi_stuff.h"
// #define GPIO_TRANSMIT_BUTTON 4
#include "config.h"

// #define RED_DISPLAY
#define CYD_DISPLAY

#ifdef CYD_DISPLAY
const char* ssid = "ESP32-CAM-AP";
const char* password = "123456789";
#else
const char* ssid = "ESP32-CAM-AP_red";
const char* password = "123456789";
#endif
// our application
Application *application;

void setup()
{
  Serial.begin(115200);
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
  WiFi.mode(WIFI_AP_STA);
  // WiFi.setSleep(WIFI_PS_NONE);
  // WiFi.mode(WIFI_AP);
  Serial.println("Setting up WiFi");
  //ssid = "ESP32-CAM-AP_red";
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  // start up the application
  application = new Application();
  // Serial.println("Application created");
  application->begin();
  // Serial.println("Application started");
  init_camera();
  startCameraServer();

  //iterate through pins and make them input pins for pinmode
  // for (int j = 2; j<8; j++)
  // {
  //   pinMode(j, INPUT);
  // }
  // pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLDOWN);

}

void loop()
{
  // nothing to do - the application is doing all the work
  vTaskDelay(pdMS_TO_TICKS(1000));
  // Serial.println(digitalRead(GPIO_TRANSMIT_BUTTON));
  // //iterate through pins and print their values
  // for (int j = 2; j<8; j++)
  // {
  //   Serial.print("Pin ");
  //   Serial.print(j);
  //   Serial.print(": ");
  //   Serial.println(digitalRead(j));
  // }
}
