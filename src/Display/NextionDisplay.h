#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "../Global/GlobalDataController.h"
#include "BaseDisplayClient.h"

class NextionDisplay : public BaseDisplayClient {
private:
    GlobalDataController *globalDataController;
    DebugController *debugController;
    SoftwareSerial *serialPort;
    boolean displayOn = true;
    long displayOffEpoch = 0;

public:
    NextionDisplay(SoftwareSerial *serialPort, GlobalDataController *globalDataController, DebugController *debugController);
    void preSetup();
    void postSetup(bool isConfigChange);
    void handleUpdate();
    void flipDisplayUpdate();
    void showBootScreen();
    void showApAccessScreen(String apSsid, String apIp);
    void showWebserverSplashScreen(bool isEnabled);

    bool isInTransitionMode() { return false; };

    void sendCommand(String cmd);
    void sendCommand(const char* cmd);
};
