
#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WString.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

#include "definitions.h"
#include "common.h"
#include "config.h"
#include "iot.h"
#include "dvc.h"



void setup() {
  //...................................... 2 Seconds Delay to Stable Power
  delay(2000);
  Dvc__SetPinout();
  delay(500);
  //-------------------------------------------------------- Preparing Serial & DebugPrint
  if (SOFTWARE_DEBUGMODE) {
    Debug__ConsoleBegin();
    Debug_ConsolePrintln("\r\n=========================\r\n======= Restarted =======\r\n=========================");
  }

  //-------------------------------------------------------- Preparing EEPROM
  while (!EEPROM.begin(EEPROM_SIZE)) {
    delay(2000);
  }

  //-------------------------------------------------------- Preparing File RW
  byte fail = 0;
  while (!SPIFFS.begin(true) && fail < 5) {  // Set the true parameter to format the file system if it fails to mount
    if (!SOFTWARE_DEBUGMODE) Debug__ConsoleBegin();
    Serial.println("SPIFFS Mount Failed");
    delay(1000);
  }
  fail = (SPIFFS.totalBytes() - SPIFFS.usedBytes()) * 100 / SPIFFS.totalBytes();
  if (fail < 3) {
    if (!SOFTWARE_DEBUGMODE) Debug__ConsoleBegin();
    while (true) {
      Serial.println("\r\n!!!!!!!!!!!!!!!!!\r\nSPIFFS Is Full (" + String(fail) + "%) " + String(SPIFFS.usedBytes()) + " of " + String(SPIFFS.totalBytes()) + " Bytes");
      Serial.flush();
      delay(100);
    }
  }
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("FILES Space OK (" + String((byte)(100 * SPIFFS.usedBytes() / SPIFFS.totalBytes())) + "\% Used / " + String(SPIFFS.usedBytes() / 1024) + " of " + String(SPIFFS.totalBytes() / 1024) + " Kb)\r\n");

  //-------------------------------------------------------- Loading Data
  Tools__SettingRead();
  Tools__SerialBarcodeReload();
  if (SOFTWARE_DEBUGMODE) Tools__SettingShowInfo();

  Dvc__LastStatusLoad();

  //-------------------------------------------------------- Preparing
  if (_StartMode == "config_panel") {
    _Started_ConfigPanel = true;
  } else {  // Started_Normal
    _Started_IotRun = true;
  }
  // Check to go config mode if not configured
  if (_ProjectCode == "" || _MqttBroker == "" || _ModemSSID == "") {
    _Started_ConfigPanel = true;
    _Started_IotRun = false;
  }

  //...................................... Set WiFi-Power
  Tools__WifiPower(true);

  //...................................... Setup LED & TouchKey (To Use Manualy)
  Dvc__Setup();

  //...................................... Setup On Mode-Config
  if (_Started_ConfigPanel) {
    Config__Setup(NULL);
  }
  //...................................... Setup On Mode IOT / Mqtt
  else if (_Started_IotRun) {
    IOT__Setup(NULL);
  }
}



void loop() {
  delay(100);
}
