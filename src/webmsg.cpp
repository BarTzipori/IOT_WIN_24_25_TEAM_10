#include "webmsg.h"

AsyncWebServer msgserver(82);

void onReceive(uint8_t* data, size_t len) {
  // Handle received data from the WebSerial interface
  Serial.print("Received: ");
  for (size_t i = 0; i < len; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();
}


void setupMsgServer(){
    WebSerial.begin(&msgserver);

  // Attach a callback function to handle incoming data
  WebSerial.onMessage(onReceive);

  // Start the web server
  msgserver.begin();

  // Log a message to WebSerial
  WebSerial.println("WebSerial started! Open the interface to interact.");
  Serial.println("WebSerial started! Open the interface to interact.");
  Serial.println("WebSerial Available at: http://" + WiFi.localIP().toString() + ":82/webserial");
}

void msgServerLoop(){
    WebSerial.loop();
}
