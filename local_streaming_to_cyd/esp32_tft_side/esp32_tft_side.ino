#include <WiFi.h>
#include <TFT_eSPI.h>  // TFT library (make sure to include the correct library for your TFT display)
#include <HTTPClient.h>
#include <TJpg_Decoder.h>

// const char *ssid = "ESP32-CAM-AP";  // SSID of the ESP32-CAM AP
// const char *password = "123456789";  // Password for the AP

// const char* stream_url ="http://192.168.1.120:81/";// "http://192.168.4.1:81/stream";
const char* stream_url ="http://jonny-ubuntu.local:81/";// "http://192.168.4.1:81/stream";
// const char* stream_url ="http://192.168.4.1:81/stream";
// const char* stream_url = "http://rpizero.local:7123/stream11.mjpg";



// const char* ssid = "ESP32_CAM_AP";
// const char* password = "12345678";


const char *ssid = "WiFi";
const char *password = "";

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
WiFiClient client;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length ";//%u\r\n\r\n";

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  if ( y >= tft.height() ) return 0;
  // tft.pushImage(x, y, w, h, bitmap);
  tft.pushImage(x, y, h, w, bitmap);
  return 1;
}

void processImage(uint8_t* image_data, size_t length) {
  Serial.println("Processing full image...");
  TJpgDec.drawJpg(0, 0, (const uint8_t*)image_data, length);
  // Display or process the complete image here (e.g., render it on the TFT)
}

void drawJpeg(uint8_t* image_data, size_t length) {
  Serial.println("Processing full image...");
  TJpgDec.drawJpg(0, 0, (const uint8_t*)image_data, length);
  // Display or process the complete image here (e.g., render it on the TFT)
}

void init_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    return;

    

    
}

void setup() {
  Serial.begin(115200);
  
  tft.init();
  tft.fillScreen(TFT_BLACK);  // Clear screen
  
  // Connect to the ESP32-CAM Access Point
  // WiFi.softAP(ssid, password);
  // Serial.println("Connecting to ESP32-CAM...");
  
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("Connected to ESP32-CAM");
  // } else {
  //   Serial.println("Connection failed.");
  // }

  init_wifi();

    // Initialize the TFT screen
  tft.init();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  tft.invertDisplay(true); //original and aliexpress but not elecros
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_GREEN);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.writecommand(ILI9341_GAMMASET); //Gamma curve selected for original one only but no aliexpress or elecrow.
  tft.writedata(2);
  delay(120);
  // tft.writecommand(ILI9341_GAMMASET); //Gamma curve selected
  tft.writedata(1);
  // analogWrite(21,64);

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  
  // Start the image stream
  // startStream();
  
}

uint8_t *downloadBuffer = NULL;
  int downloadBufferLength = 0;

  uint8_t *mCurrentFrameBuffer = NULL;
    size_t mCurrentFrameLength = 0;
    size_t mCurrentFrameBufferLength = 0;

void loop() {
  // delay(1000);
   if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Start the HTTP connection to the ESP32-CAM stream
    http.begin(stream_url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      WiFiClient * stream = http.getStreamPtr();
      bool in_image = false;
      size_t content_length = 0;
      uint8_t *imageBuffer = nullptr;

      while (stream->connected()) {
        if (stream->available()) {
          String line = stream->readStringUntil('\n');

          // Check for boundary marker
          if (line.startsWith("--")) {
            in_image = false;
            if (imageBuffer) {
              // Display the received image on the TFT
              drawJpeg(imageBuffer, content_length);
              free(imageBuffer);
              imageBuffer = nullptr;
            }
            content_length = 0;  // Reset the content length for the next image
          }

          // Check if the line is the content length header
          if (line.startsWith("Content-Length: ")) {
            content_length = line.substring(15).toInt();  // Use 16 when ends with a semi-colon
            Serial.printf("Content-Length: %d\n", content_length);
            imageBuffer = (uint8_t *)malloc(content_length);  // Allocate memory for the image
            in_image = true;  // Indicate that the image is coming
          }

          // If we're in the image data, read it into the buffer
          if (in_image && imageBuffer) {
            size_t remaining = content_length;
            size_t bytes_read = 0;

            while (remaining > 0 && stream->available()) {
              bytes_read += stream->readBytes(imageBuffer + bytes_read, remaining);
              remaining = content_length - bytes_read;
            }

            if (remaining == 0) {
              //Serial.println("Image received!");
              in_image = false;  // Image is fully received
            }
          }
        }
      }
    } else {
      Serial.printf("HTTP GET failed, error: %d\n", httpCode);
    }

    http.end(); // Close the HTTP connection
  }

  // delay(5000); // Adjust the delay as necessary for the next request
  delay(20);
}

void startStream() {
  // if (client.connect("192.168.4.1", 81)) {  // Connect to the server (replace with your server IP)
  // if (client.connect("192.168.1.113", 81)) {  // Connect to the server (replace with your server IP)
  if (client.connect("192.168.1.120", 81)) {  // Connect to the server (replace with your server IP)
    Serial.println("Connected to the server");

    // Request the MJPEG stream
    // client.print("GET /stream HTTP/1.1\r\n");
    client.print("GET / HTTP/1.1\r\n");
    // client.print("Host: 192.168.4.1\r\n");  // Replace with the server IP
    client.print("Host: 192.168.1.120\r\n");  // Replace with the server IP
    client.print("Connection: close\r\n");
    client.print("\r\n");

    // Read the response
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);

        // Check for the beginning of the image data
        if (line.startsWith(_STREAM_BOUNDARY)) {
          // Read the next line to get the length of the image
          while (client.available()) {
            String imgSizeLine = client.readStringUntil('\n');
            if (imgSizeLine.startsWith(_STREAM_PART)) {
              int imgLength = imgSizeLine.substring(strlen(_STREAM_PART)).toInt();
              //Serial.printf("Image length: %d\n", imgLength);
              
              // Read the image data
              uint8_t *imgData = new uint8_t[imgLength];
              client.readBytes(imgData, imgLength);

              // Display the image on the TFT
              processImage(imgData, imgLength);

              delete[] imgData;  // Free the image data
            }
            if (imgSizeLine.length() == 0) break;  // End of headers
          }
        }
      }
    }
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }
}

void displayImage(uint8_t *imgData, int length) {
  // Assuming imgData is in JPEG format and your TFT library can handle it
  // You may need to implement a function that converts and displays the JPEG data
  tft.setRotation(0);
  tft.pushImage(0, 0, 320, 240, imgData);  // Adjust dimensions as needed
  Serial.println("Image displayed on TFT");
}
