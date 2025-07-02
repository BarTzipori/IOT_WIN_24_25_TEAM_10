#include "camera.h"

// Database path to store the image
String databaseImgPath = "/images/";
const char *serverUrl = "http://132.68.34.85:5015/upload";

struct ts{
  double time;
  double size;
};

struct ts arrayOfTS[100];
int arrayIndex = 0;

bool setupCamera()
{
  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM; // Updated pin name
  config.pin_sccb_scl = SIOC_GPIO_NUM; // Updated pin name
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  // config.frame_size = FRAMESIZE_QVGA; // Lower resolution (e.g., QVGA or CIF) is recommended for faster processing
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize the camera
  if (esp_camera_init(&config) != ESP_OK)
  {
    Serial.println("Camera initialization failed!");
    return false;
  }
  Serial.println("Camera initialized successfully.");
  return true;
}



/*
void sendArrayToServer() {
  HTTPClient http;
  http.begin("https://your-server.com/upload"); // Replace with your endpoint
  http.addHeader("Content-Type", "application/json");

  // Use ArduinoJson to serialize array
  StaticJsonDocument<4096> doc;  // Adjust size as needed

  JsonArray arr = doc.createNestedArray("data");
  for (int i = 0; i < 100; i++) {
    JsonObject obj = arr.createNestedObject();
    obj["time"] = arrayOfTS[i].time;
    obj["size"] = arrayOfTS[i].size;
  }

  String jsonString;
  serializeJson(doc, jsonString);

  int httpResponseCode = http.POST(jsonString);
  if (httpResponseCode > 0) {
    Serial.printf("Server response code: %d\n", httpResponseCode);
  } else {
    Serial.printf("Error sending data: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}
*/





void sendImageToServer() {
  WiFiClient client;
  HTTPClient http;

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  } else {
    Serial.println("Image captured successfully");
  }

  client.setTimeout(90000); // Set timeout to 60 seconds
  //http.setMaxContentLength(1000000); // Increase buffer size to handle large images (1MB)
  http.setTimeout(90000);  // Set timeout to 90 seconds

  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  Serial.println("Uploading image to server...");
  Serial.printf("image size: %d bytes, %.3f kb\n", fb->len,(float)fb->len / 1024);
  unsigned long start = micros();

  int httpResponseCode = http.POST(fb->buf, fb->len);

  unsigned long end = micros();
  unsigned long duration = end - start;
  double_t seconds = duration / 1000000.0;
  Serial.printf("Image upload took %lu microseconds = %.3f seconds\n", duration,seconds);
  arrayOfTS[arrayIndex].time = seconds;
  arrayOfTS[arrayIndex].size = (double)fb->len / 1024.0; // Store size in KB
  if (arrayIndex >= 100) {
    Serial.println("Array index out of bounds, resetting to 0");
    arrayIndex = 0; // Reset index if it exceeds the array size
  } else
    arrayIndex++; 
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();
  esp_camera_fb_return(fb);
}

