#include "ESP32SmartMeterReader.h"
#include <WiFi.h>
#include <ArduinoJson.h>

ESP32SmartMeterReader::ESP32SmartMeterReader() : ipAddress("192.168.1.5"), timeout(5000), debugEnabled(false) {}

void ESP32SmartMeterReader::setIPAddress(const String& newIP) {
    ipAddress = newIP;
}

void ESP32SmartMeterReader::setDebugEnabled(bool enabled) {
    debugEnabled = enabled;
}

void ESP32SmartMeterReader::debugPrint(const String& message) {
    if (debugEnabled) {
        Serial.println(message);
    }
}

bool ESP32SmartMeterReader::begin(const String& newIP) {
    if (newIP.length() > 0) {
        setIPAddress(newIP);  // Setzt die IP-Adresse, wenn angegeben
    }
    return true;  // Erfolgreiche Initialisierung
}

bool ESP32SmartMeterReader::isOnline() {
    WiFiClient client;
    return client.connect(ipAddress.c_str(), 80);
}

bool ESP32SmartMeterReader::sendRequest(const String& command, String& response) {
    WiFiClient client;
    if (!client.connect(ipAddress.c_str(), 80)) {
        debugPrint("Verbindung zum Smart Meter fehlgeschlagen.");
        return false;
    }

    String output = "GET /cm?cmnd=" + command + " HTTP/1.1\r\n" + "Host: " + ipAddress + "\r\n" + "Connection: close\r\n\r\n";
    debugPrint(output);

    client.print(output);
    
    long int timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > this->timeout) {
            client.stop();
            debugPrint("Timeout Antwort");
            return false;
        }
    }

    while (client.available()) {
        response += client.readString();
    }
    
    debugPrint("Antwort empfangen:");
    debugPrint(response);
    
    client.stop();
    return true;
}

bool ESP32SmartMeterReader::getNetworkStatus(String &hostname, String &ipAddress, String &gateway, String &subnetMask, String &macAddress) {
    String response;
    if (!sendRequest("Status%205", response)) {
        debugPrint("Fehler beim Senden der Anfrage.");
        return false;
    }

    debugPrint("Antwort empfangen:");
    debugPrint(response);

    // Den JSON-Inhalt extrahieren
    int jsonStartIndex = response.indexOf('{'); // Suche den Beginn des JSON-Inhalts
    if (jsonStartIndex == -1) {
        debugPrint("Kein JSON-Inhalt gefunden.");
        return false;
    }

    // JSON-Teile in einer neuen Variable speichern
    String jsonResponse = response.substring(jsonStartIndex);
    jsonResponse.replace("0\r\n", ""); // Entfernen des "0\r\n" am Ende der Antwort

    debugPrint("Extrahierter JSON-Inhalt:");
    debugPrint(jsonResponse);

    DynamicJsonDocument doc(2048); // Anpassung der Größe
    DeserializationError error = deserializeJson(doc, jsonResponse);

    if (error) {
        debugPrint(String("JSON-Desequenzierungsfehler: ") + error.c_str());
        return false;
    }

    // Daten extrahieren
    hostname = String(doc["StatusNET"]["Hostname"].as<const char*>());
    ipAddress = String(doc["StatusNET"]["IPAddress"].as<const char*>());
    gateway = String(doc["StatusNET"]["Gateway"].as<const char*>());
    subnetMask = String(doc["StatusNET"]["Subnetmask"].as<const char*>());
    macAddress = String(doc["StatusNET"]["Mac"].as<const char*>());

    return true;
}

bool ESP32SmartMeterReader::getSensorData(float &E_in, float &E_out, float &Power, float &L1, float &L2, float &L3) {
    String response;
    debugPrint("Sende Anfrage an den Smart Meter...");
    
    if (!sendRequest("Status%208", response)) {
        debugPrint("Fehler beim Senden der Anfrage.");
        return false;
    }

    debugPrint("Antwort empfangen:");
    debugPrint(response);

    // Den JSON-Inhalt extrahieren
    int jsonStartIndex = response.indexOf('{'); // Suche den Beginn des JSON-Inhalts
    if (jsonStartIndex == -1) {
        debugPrint("Kein JSON-Inhalt gefunden.");
        return false;
    }

    // JSON-Teile in einer neuen Variable speichern
    String jsonResponse = response.substring(jsonStartIndex);
    jsonResponse.replace("0\r\n", ""); // Entfernen des "0\r\n" am Ende der Antwort

    debugPrint("Extrahierter JSON-Inhalt:");
    debugPrint(jsonResponse);

    DynamicJsonDocument doc(2048); // Anpassung der Größe
    DeserializationError error = deserializeJson(doc, jsonResponse);

    if (error) {
        debugPrint(String("JSON-Desequenzierungsfehler: ") + error.c_str());
        return false;
    }

    // Überprüfen, ob die erwarteten Daten vorhanden sind
    if (!doc["StatusSNS"].containsKey("MT175")) {
        debugPrint("Fehlender MT175 Schlüssel in der Antwort.");
        return false;
    }

    // Daten extrahieren
    E_in = doc["StatusSNS"]["MT175"]["E_in"];
    E_out = doc["StatusSNS"]["MT175"]["E_out"];
    Power = doc["StatusSNS"]["MT175"]["Power"];
    L1 = doc["StatusSNS"]["MT175"]["L1"];
    L2 = doc["StatusSNS"]["MT175"]["L2"];
    L3 = doc["StatusSNS"]["MT175"]["L3"];

    debugPrint(String("E_in: ") + E_in + ", E_out: " + E_out + ", Power: " + Power + ", L1: " + L1 + ", L2: " + L2 + ", L3: " + L3);

    return true;
}
