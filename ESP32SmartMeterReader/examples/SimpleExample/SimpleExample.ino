#include <WiFi.h>
#include <ESP32SmartMeterReader.h>

// WLAN-Zugangsdaten
const char* ssid = "WLANGW1339";
const char* password = "dein_passwort";

// IP-Adresse des Smartmeters (optional)
String smartMeterIP = "192.168.1.5";

// Instanz der Klasse
ESP32SmartMeterReader smartMeter;

void setup() {
    Serial.begin(115200);

    // WLAN verbinden
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Verbinde mit WLAN...");
    }
    Serial.println("WLAN verbunden.");

    // Smart Meter Reader initialisieren
    if (smartMeter.begin(smartMeterIP)) {
        Serial.println("Smart Meter verbunden.");
    } else {
        Serial.println("Fehler beim Verbinden mit Smart Meter.");
    }
}

void loop() {
    float E_in, E_out, Power, L1, L2, L3;

    // Sensor-Daten abrufen
    if (smartMeter.getSensorData(E_in, E_out, Power, L1, L2, L3)) {
        Serial.print("E_in: "); Serial.println(E_in);
        Serial.print("E_out: "); Serial.println(E_out);
        Serial.print("Power: "); Serial.println(Power);
        Serial.print("L1: "); Serial.println(L1);
        Serial.print("L2: "); Serial.println(L2);
        Serial.print("L3: "); Serial.println(L3);
    } else {
        Serial.println("Fehler beim Abrufen der Sensor-Daten.");
    }

    delay(5000); // 5 Sekunden warten
}

