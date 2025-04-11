#include <Arduino.h>
#include "Application.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "tft_stuff.h"
// our application
Application *application;

// const char* ssid = "ESP32-CAM-AP_red";
// const char* password = "123456789";

#ifdef CYD_DISPLAY
  const char* ssid = "ESP32-CAM-AP";
#else
  const char* ssid = "ESP32-CAM-AP_red";
#endif
const char* password = "123456789";



void setup()
{
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);

  // WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_STA);
// #ifndef USE_ESP_NOW
// ssid = "ESP32-CAM-AP_red";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  WiFi.setSleep(WIFI_PS_NONE);

  tft_init();
  
  #ifdef CYD_DISPLAY
    digitalWrite(21, HIGH); //fudge to get the display to work
  #endif
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  

  
  Serial.print("My IP Address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("My MAC Address is: ");
  Serial.println(WiFi.macAddress());
  // start up the application
  application = new Application();
  // Serial.println("Application created");
  application->begin();
  // Serial.println("Application started");
}

const char *stream_url = "http://192.168.4.1:81/stream";

void loop()
{
  // nothing to do - the application is doing all the work
  //need to read image
  if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            // Start the HTTP connection to the ESP32-CAM stream
            http.begin(stream_url);
            int httpCode = http.GET();

            if (httpCode == HTTP_CODE_OK)
            {
                WiFiClient *stream = http.getStreamPtr();
                bool in_image = false;
                size_t content_length = 0;
                uint8_t *imageBuffer = nullptr;

                while (stream->connected())
                {
                    if (stream->available())
                    {
                        String line = stream->readStringUntil('\n');

                        // Check for boundary marker
                        if (line.startsWith("--"))
                        {
                            in_image = false;
                            if (imageBuffer)
                            {
                                // Display the received image on the TFT
                                drawJpeg(imageBuffer, content_length);
                                free(imageBuffer);
                                imageBuffer = nullptr;
                            }
                            content_length = 0; // Reset the content length for the next image
                        }

                        // Check if the line is the content length header
                        if (line.startsWith("Content-Length: "))
                        {
                            content_length = line.substring(15).toInt(); // Use 16 when ends with a semi-colon
                            // Serial.printf("Content-Length: %d\n", content_length);
                            imageBuffer = (uint8_t *)malloc(content_length); // Allocate memory for the image
                            in_image = true;                                 // Indicate that the image is coming
                        }

                        // If we're in the image data, read it into the buffer
                        if (in_image && imageBuffer)
                        {
                            size_t remaining = content_length;
                            size_t bytes_read = 0;

                            while (remaining > 0 && stream->available())
                            {
                                bytes_read += stream->readBytes(imageBuffer + bytes_read, remaining);
                                remaining = content_length - bytes_read;
                            }

                            if (remaining == 0)
                            {
                                // Serial.println("Image received!");
                                in_image = false; // Image is fully received
                            }
                        }
                    }
                }
            }
            else
            {
                Serial.printf("HTTP GET failed, error: %d\n", httpCode);
            }

            http.end(); // Close the HTTP connection
        }

        // delay(5000); // Adjust the delay as necessary for the next request
        delay(20);

  // vTaskDelay(pdMS_TO_TICKS(1000));
}
