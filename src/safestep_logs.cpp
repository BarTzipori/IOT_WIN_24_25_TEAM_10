#include "safestep_logs.h"
#include "WebSerial.h"

String log_filename;
extern Flags flags;

void logData(String data)
{
    Serial.println(data);
    String stamp = FormatTime(millis(),flags.wifi_flag,false) + " :  ";
    if(flags.sd_flag)
        (SD_MMC, log_filename.c_str(), stamp + data);
    if (flags.wifi_flag)
    {
        WebSerial.println(stamp + data);
    }
}

void sendDistanceData(const std::vector<std::pair<int, int>>& distances, String target_ip) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClient client;
        String ip_str = "http://" + target_ip + ":5015/lidar";
        http.begin(client, ip_str.c_str()); // serverURL should be defined as the endpoint URL
        http.addHeader("Content-Type", "application/json");
        
        // Create JSON payload matching your format
        String json = "[";
        
        for (size_t i = 0; i < distances.size(); ++i) {
            int z = distances[i].second; // forward distance
            int h = distances[i].first;  // height (side view: sensor height)
            
            json += "{\"points\":[{\"x\":" + String(z) + ",\"y\":" + String(h) + "}]}";
            
            if (i < distances.size() - 1)
                json += ",";
        }
        
        json += "]";
        
        // Send POST request
        int httpResponseCode = http.POST(json);
        
        if (httpResponseCode > 0) {
            Serial.printf("HTTP Response: %d\n", httpResponseCode);
        } else {
            Serial.printf("HTTP Error: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        
        http.end();
    }
}