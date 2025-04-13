#ifndef ESP32SMARTMETERREADER_H
#define ESP32SMARTMETERREADER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

class ESP32SmartMeterReader {
public:
    ESP32SmartMeterReader();

    void setIPAddress(const String& newIP);
    void setDebugEnabled(bool enabled);  // Neue Methode zum Setzen des Debug-Flags

    bool begin(const String& newIP);
    bool isOnline();
    bool sendRequest(const String& command, String& response);
    bool getNetworkStatus(String &hostname, String &ipAddress, String &gateway, String &subnetMask, String &macAddress);
    bool getSensorData(float &E_in, float &E_out, float &Power, float &L1, float &L2, float &L3);

private:
    String ipAddress;
    long timeout;
    bool debugEnabled;  // Neues Flag für Debug-Ausgaben

    void debugPrint(const String& message);  // Neue Methode für Debug-Ausgaben
};

#endif
