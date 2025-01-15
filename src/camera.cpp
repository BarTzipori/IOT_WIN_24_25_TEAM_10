#include "camera.h"
#include <Wire.h>

// Database path to store the image
String databaseImgPath = "/images/";
const char *serverUrl = "http://192.168.1.87:5015/upload";

bool setupCamera()
{
  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_1;
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
  //config.frame_size = FRAMESIZE_UXGA;
  config.frame_size = FRAMESIZE_QVGA; // Lower resolution (e.g., QVGA or CIF) is recommended for faster processing
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

bool CaptureAndUploadImage(FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config)

{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return false;
  }
  else
  {
    Serial.println("Image captured successfully");
  }

  // Generate a Firebase storage path
  String path = "/images/esp32_image_" + FormatTime(millis(), true, false) + ".jpg";

  config.host = FIREBASE_HOST_STORAGE;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);

  // Upload to Firebase Storage
  if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, fb->buf, fb->len, path.c_str(), "image/jpeg"))
  {
    Serial.printf("Upload successful: %s\n", fbdo.stringData().c_str());
    Serial.println("Image uploaded to: " + path);
    esp_camera_fb_return(fb);
    return true;
  }
  else
  {
    Serial.printf("Upload failed: %s\n", fbdo.errorReason().c_str());
    esp_camera_fb_return(fb);
    return false;
  }

  // Return the frame buffer
  esp_camera_fb_return(fb);
}

camera_fb_t *capturePicture()
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Failed to capture image");
    esp_camera_fb_return(fb);
    return nullptr;
  }

  // Ensure the image is in JPEG format
  if (fb->format != PIXFORMAT_JPEG)
  {
    Serial.println("Image format is not JPEG");
    esp_camera_fb_return(fb);
    return nullptr;
  }

  Serial.println("Image captured successfully");
  return fb;
}

bool savePictureToSD(camera_fb_t *fb, bool wifi_flag)
{
  if (!SD_MMC.begin("/sdcard", true))
  {
    Serial.println("SD card initialization failed");
    // esp_camera_fb_return(fb);
    return false;
  }

  String path = "/images/esp32_image_" + FormatTime(millis(), wifi_flag, true) + ".jpg";

  File file = SD_MMC.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file on SD card");
    // esp_camera_fb_return(fb);
    return false;
  }

  file.write(fb->buf, fb->len);
  file.close();
  esp_camera_fb_return(fb);
  Serial.println("Image saved to SD card successfully");
  Serial.println("Image saved as: " + path);
  return true;
}

String FormatTime(unsigned long currentMillis, bool wifi_flag, bool sd_flag)
{
  String formattedTime;
  if (!wifi_flag)
  {
    unsigned long hours = currentMillis / 3600000;             // 3600000 milliseconds in one hour
    unsigned long minutes = (currentMillis % 3600000) / 60000; // 60000 milliseconds in one minute
    unsigned long seconds = (currentMillis % 60000) / 1000;    // 1000 milliseconds in one second
    unsigned long milliseconds = currentMillis % 1000;         // Remaining milliseconds

    // Format the output as HH:MM:SS.MS
    if (sd_flag)
    {
      formattedTime = String(hours) + "_" + (minutes < 10 ? "0" : "") + String(minutes) + "_" +
                      (seconds < 10 ? "0" : "") + String(seconds);
    }
    else
    {
      formattedTime = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" +
                      (seconds < 10 ? "0" : "") + String(seconds) + "." + String(milliseconds);
    }
  }
  else
  {
    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return "";
    }

    // Format the current date and time
    char timeStr[20];
    if (sd_flag)
    {
      strftime(timeStr, sizeof(timeStr), "%d-%m-%Y__%H_%M_%S", &timeinfo);
    }
    else
    {
      strftime(timeStr, sizeof(timeStr), "%d-%m-%Y_%H:%M:%S", &timeinfo);
    }

    formattedTime = String(timeStr);
  }
  return formattedTime;
}

bool UploadImage(FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config, camera_fb_t *fb)
{

  // Generate a Firebase storage path
  String path = "/images/esp32_image_" + FormatTime(millis(), true, false) + ".jpg";

  config.host = FIREBASE_HOST_STORAGE;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);

  // Upload to Firebase Storage
  if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, fb->buf, fb->len, path.c_str(), "image/jpeg"))
  {
    Serial.printf("Upload successful: %s\n", fbdo.stringData().c_str());
    Serial.println("Image uploaded to: " + path);

    return true;
  }
  else
  {
    Serial.printf("Upload failed: %s\n", fbdo.errorReason().c_str());

    return false;
  }

  // Return the frame buffer
}

bool CaptureObstacle(FirebaseData &fbdo, FirebaseAuth &auth, FirebaseConfig &config, bool wifi_flag)
{
  bool result = false;
  camera_fb_t *fb = capturePicture();
  if (!fb)
  {
    return false;
  }
  if (savePictureToSD(fb, wifi_flag))
    result = true;
  if (UploadImage(fbdo, auth, config, fb))
    result = true;

  esp_camera_fb_return(fb);
  return result;
}

/*
void captureAndUploadImage(FirebaseData &fbdo) {
  // Capture image
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Convert image to Base64
  String base64Image = base64::encode((uint8_t*)fb->buf, fb->len);

  // Firebase path with unique ID
  String path = databaseImgPath + String(millis());

  // Upload Base64 image to Firebase Realtime Database
  Serial.println("Uploading image...");
  if (Firebase.setString(fbdo, path, base64Image)) {
    Serial.println("Image uploaded successfully!");
  } else {
    Serial.printf("Image upload failed: %s\n", fbdo.errorReason().c_str());
  }

  // Return the framebuffer to free memory
  esp_camera_fb_return(fb);
}


void savePictureToSD(camera_fb_t *fb) {
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD card initialization failed");
    return;
  }

  File file = SD_MMC.open("/picture.jpg", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file on SD card");
    return;
  }

  file.write(fb->buf, fb->len);
  file.close();
  esp_camera_fb_return(fb);
  Serial.println("Image saved to SD card successfully");
}


camera_fb_t *capturePicture() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture image");
    return nullptr;
  }

  // Ensure the image is in JPEG format
  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("Image format is not JPEG");
    esp_camera_fb_return(fb);
    return nullptr;
  }

  Serial.println("Image captured successfully");
  return fb;
}
/*
void uploadImageToFirebase(const char *filePath, FirebaseData &fbdo) {
  File file = SD_MMC.open(filePath, FILE_READ);

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  size_t fileSize = file.size();
  uint8_t *fileBuffer = (uint8_t *)malloc(fileSize);
  if (!fileBuffer) {
    Serial.println("Failed to allocate memory for file buffer");
    file.close();
    return;
  }

  file.read(fileBuffer, fileSize);
  file.close();

  String firebaseStoragePath = "/images/picture.jpg";

  Serial.println("Uploading image to Firebase...");
  if (Firebase.storage().upload(&fbdo, FIREBASE_HOST, FIREBASE_AUTH, firebaseStoragePath.c_str(), "image/jpeg", fileBuffer, fileSize)) {
    Serial.println("Image uploaded successfully!");
  } else {
    Serial.printf("Image upload failed: %s\n", fbdo.errorReason().c_str());
  }

  free(fileBuffer);
}
*/
/*
void uploadImageToRealtimeDB(const char *filePath, FirebaseData &fbdo) {
  File file = SD_MMC.open(filePath, FILE_READ);

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  } else {
    Serial.println("File opened successfully");
  }

  size_t fileSize = file.size();
  uint8_t *fileBuffer = (uint8_t *)malloc(fileSize);
  if (!fileBuffer) {
    Serial.println("Failed to allocate memory for file buffer");
    file.close();
    return;
  } else {
    Serial.println("Memory allocated successfully");
  }

  file.read(fileBuffer, fileSize);
  file.close();

  // Convert image to Base64
  String base64Image = base64::encode(fileBuffer, fileSize);
  free(fileBuffer);
  Serial.println("Image converted to Base64 successfully");
  // Upload Base64 string to Firebase Realtime Database
  if (Firebase.setString(fbdo, "/images/picture", base64Image)) {
    Serial.println("Image uploaded to Realtime Database successfully!");
  } else {
    Serial.printf("Image upload failed: %s\n", fbdo.errorReason().c_str());
  }
}

void uploadImageToRealtimeDBInChunks(const char *filePath, FirebaseData &fbdo) {
  File file = SD_MMC.open(filePath, FILE_READ);

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  } else {
    Serial.println("File opened successfully");
  }

  size_t fileSize = file.size();
  uint8_t *fileBuffer = (uint8_t *)malloc(fileSize);
  if (!fileBuffer) {
    Serial.println("Failed to allocate memory for file buffer");
    file.close();
    return;
  } else {
    Serial.println("Memory allocated successfully");
  }

  file.read(fileBuffer, fileSize);
  file.close();

  String base64Image = base64::encode(fileBuffer, fileSize);
  free(fileBuffer);
  Serial.println("Image converted to Base64 successfully");
  // Split Base64 string into smaller chunks
  const size_t chunkSize = 512; // Adjust chunk size if needed
  size_t offset = 0;

  while (offset < base64Image.length()) {
    String chunk = base64Image.substring(offset, offset + chunkSize);
    Serial.printf("Uploading chunk %d...\n", offset / chunkSize);
    // Write each chunk to a different path in Firebase
    String path = "/images/picture_chunks/" + String(offset / chunkSize);
    if (!Firebase.setString(fbdo, path, chunk)) {
      Serial.printf("Failed to upload chunk: %s\n", fbdo.errorReason().c_str());
      return;
    } else {
      Serial.printf("Uploaded chunk %d successfully\n", offset );
    }

    offset += chunkSize;
  }

  Serial.println("Image uploaded in chunks successfully!");
}


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
http.setTimeout(70000);  // Set timeout to 90 seconds

  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  Serial.println("Uploading image to server...");
  int httpResponseCode = http.POST(fb->buf, fb->len);

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

*/