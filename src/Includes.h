#pragma once
#include <Arduino.h>
#include <WiFiManager.h>
#include <SPI.h>
#include "Global/GlobalDataController.h"
#include "Global/DebugController.h"
#include "Configuration.h"
#if WEBSERVER_ENABLED
    #include "Network/WebServer.h"
#endif
#include "Network/TimeClient.h"
#include "Network/OpenWeatherMapClient.h"
#include "Network/JsonRequestClient.h"
#include "Clients/RepetierClient.h"
#include "Clients/KlipperClient.h"
#include "Clients/DuetClient.h"
#include "Clients/OctoPrintClient.h"

#include "Sensors/BME280SensorI2C.h"
#include "Sensors/BME280SensorSPI.h"
#include "Sensors/BME680SensorI2C.h"
#include "Sensors/BME680SensorSPI.h"
#include "Sensors/HTU21DI2C.h"
#include "Sensors/BMP180I2C.h"
#include "Sensors/DHT11Wire.h"
#include "Sensors/DHT21Wire.h"
#include "Sensors/DHT22Wire.h"

#ifdef USE_NEXTION_DISPLAY
    #include "Display/NextionDisplay.h"
#else
    #include <SSD1306Wire.h>
    #include <SH1106Wire.h>
    #include "Display/OledDisplay.h"
#endif

// Initilize all needed data
DebugController debugController(DEBUG_MODE_ENABLE);
JsonRequestClient jsonRequestClient(&debugController);
TimeClient timeClient(TIME_UTCOFFSET, &debugController);
OpenWeatherMapClient weatherClient(WEATHER_APIKEY, WEATHER_CITYID, 1, WEATHER_METRIC, WEATHER_LANGUAGE, &debugController, &jsonRequestClient);
GlobalDataController globalDataController(&timeClient, &weatherClient, &debugController);
WebServer webServer(&globalDataController, &debugController);

// Register all printer clients
DuetClient printerClient0(&globalDataController, &debugController, &jsonRequestClient);
KlipperClient printerClient1(&globalDataController, &debugController, &jsonRequestClient);
OctoPrintClient printerClient2(&globalDataController, &debugController, &jsonRequestClient);
RepetierClient printerClient3(&globalDataController, &debugController, &jsonRequestClient);

// Register all sensor types
BME280SensorI2C sensorClient0(&globalDataController, &debugController, &jsonRequestClient);
BME280SensorSPI sensorClient1(&globalDataController, &debugController, &jsonRequestClient);
BME680SensorI2C sensorClient2(&globalDataController, &debugController, &jsonRequestClient);
BME680SensorSPI sensorClient3(&globalDataController, &debugController, &jsonRequestClient);
HTU21DI2C sensorClient4(&globalDataController, &debugController, &jsonRequestClient);
BMP180I2C sensorClient5(&globalDataController, &debugController, &jsonRequestClient);
DHT11Wire sensorClient6(&globalDataController, &debugController, &jsonRequestClient);
DHT21Wire sensorClient7(&globalDataController, &debugController, &jsonRequestClient);
DHT22Wire sensorClient8(&globalDataController, &debugController, &jsonRequestClient);

// Initialize I2C (SPI in main routine)
TwoWire *i2cInterface = &Wire;

// Construct correct display client
#ifdef USE_NEXTION_DISPLAY
    SoftwareSerial displaySerialPort(DISPLAY_RX_PIN, DISPLAY_TX_PIN);
    NextionDisplay displayClient(&displaySerialPort, &globalDataController, &debugController);
#else
    #if DISPLAY_SH1106
        SH1106Wire  display(DISPLAY_I2C_DISPLAY_ADDRESS, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    #else
        SSD1306Wire display(DISPLAY_I2C_DISPLAY_ADDRESS, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    #endif
    OledDisplay displayClient(&display, &globalDataController, &debugController);
#endif