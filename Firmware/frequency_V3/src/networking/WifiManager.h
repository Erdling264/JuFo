#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager();
    void connect();
private:
    void printWifiStatus();
};

#endif // WIFI_MANAGER_H