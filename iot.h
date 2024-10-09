bool IOT__Trying = false;
bool _MqttRcv_IsWorking = false;

void Mqtt__Connect() {
  if (_Mqtt_TryConnecting) return;
  _Mqtt_TryConnecting = true;
  try {
    if (!_MqttObj.connected() || _MqttObj.state() < 0) {
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Reconnecting...");
      delay(1500);
      if (_MqttObj.connect(_MqttClientId.c_str(), _MqttUser.c_str(), _MqttPass.c_str())) {
        //........................................................... Reconnected
        delay(10);
        _MqttCon_IsConnected = true;
        if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Success Status :: " + String(_MqttObj.state()));
        _MqttObj.subscribe((_ProjectCode + "/DeviceToDevice").c_str());
        _MqttObj.subscribe((_ProjectCode + "/ServerToDevice").c_str());
        _MqttObj.subscribe((_ProjectCode + "/DeviceSetting").c_str());
        _MqttObj.subscribe((_ProjectCode + "/Share").c_str());
        _MqttObj.subscribe("Time/Tehran");
        Dvc__CloudSendAllStatus();  // send last status
      }
      //........................................................... Cant Reconnect
      else {
        _MqttCon_IsConnected = false;
        if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Fail Status :: " + String(_MqttObj.state()));
        delay(119000);  // 2 Min Delay If Retry Failed (To Prevent Hanging Manual Touch)
      }
      delay(500);
    }
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt_Connect Error");
  }
  _Mqtt_TryConnecting = false;
}



void Mqtt__OnRecieve(char *topic, uint8_t *payload, unsigned int length) {
  try {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nMqtt Rcv...\r\n    length : " + String(length) + "\r\n    Project/Topic: " + topic);
    //----------------------------------------------------------------------------- Cloud MqttBroker
    //.................................... Check Project
    String projectTopic = CA_ToString(topic);
    int p = projectTopic.indexOf('/');
    if (projectTopic.substring(0, p) != _ProjectCode) return;
    projectTopic = projectTopic.substring(p + 1);
    DynamicJsonDocument doc(1024 + (2 * length));
    doc.clear();
    deserializeJson(doc, payload);
    //.................................... Check Topic
    if (projectTopic == "DeviceSetting") {
      String opr = doc["operationName"].as<String>();
      if (opr == "save_scenario" || opr == "delete_scenario") {
        // do somethings
      } else if (opr == "save_setting") {
        if (doc["deviceSerial"].as<String>() == _SerialCloud) {
          String cmd, d = doc["value"].as<String>();
          if (d != "") {
            deserializeJson(doc, d);
            if (doc["request"]["commandName"] && doc["request"]["commandData"]) {
              cmd = doc["request"]["commandName"].as<String>();
              if (cmd == "saveScenarioOperation") {
                JsonVariant inp = doc["scenarioOperation"].as<JsonVariant>();
                // do somethings
              } else if (cmd == "saveCommandOperation") {
                JsonVariant inp = doc["commandOperation"].as<JsonVariant>();
                // do somethings
              } else {
                d = doc["request"]["commandData"].as<String>();
                // do somethings
              }
            }
          }
        }
      }
    } else if (projectTopic == "ServerToDevice") {
      String deviceSerial = doc["deviceSerial"].as<String>();
      String cmd = "";
      try {
        if (doc["operationName"]) cmd = doc["operationName"].as<String>();
      } catch (...) {}
      if (cmd != "") {
        if (cmd == "disable_scenario" || cmd == "enable_scenario" || cmd == "run_scenario") {
          // do somethings
        } else if (cmd == "debugger") {
          if (doc["value"]) {
            String value = doc["value"].as<String>();
            if (value == "reboot") Tools__Reboot();

            else if (value == "clean") {
              // do somethings
              Tools__Reboot();
            } else if (value == "reset_factory") {
              // do somethings
              Tools__Reboot();
            } else if (value == "config") {
              Tools__SetMode("config_panel", true);
            } else if (value == "special")
              ;
          }
        }
      } else if (doc["data"]) {
        cmd = doc["data"].as<String>();
        if (cmd != "") {
          // do somethings
        }
      }
    } else if (projectTopic == "DeviceToDevice") {
       //.................................... Extarct Data
      // do somethings
      // do somethings (if need to react to other devices. for example pass data to analizer : Dvc__Analize(data); )
    }
    doc.clear();
  } catch (...) {}
}


void Mqtt__Send(String topic, String data) {
  try {
    //................................. Reject If Mesh Not Connected
    if (data == "") return;
    topic = _ProjectCode + "/" + topic;
    //----------------------------------------------------------------------------- Cloud SMS
    if (!_MqttCon_IsConnected || !_MqttObj.connected() || _MqttObj.state() < 0) {
      return;
    }
    //----------------------------------------------------------------------------- Cloud MqttBroker
    else {
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Mqtt Send... " + topic + "  >  " + data);
      _MqttObj.publish(topic.c_str(), data.c_str());
    }
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("MqttSend Error ");
  }
}





void IOT__WiFiStart() {
  _WiFi_ConnectWorking = true;
  try {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Connecting to Wi-Fi...");
    Tools__WifiPower(true);
    const char *ssid = _ModemSSID.c_str();
    const char *password = _ModemPass.c_str();
    WiFi.begin(ssid, password);
  } catch (...) {}
  Dvc__SetPinout();
  delay(100);
  _WiFi_ConnectWorking = false;
}


void IOT__WiFiEvent(WiFiEvent_t event) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("[WiFi-event] event: " + String(event));
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("WiFi connected, IP: " + WiFi.localIP().toString());
      _WiFi_IsConnected = true;
      _MqttCon_Steps = 2;
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      _WiFi_IsConnected = false;
      _MqttCon_IsConnected = false;
      _MqttCon_Steps = 0;
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("WiFi lost connection");
      break;
      // default:
      // 	break;
  }
  delay(100);
}


void IOT__Checker(void *param) {
  while (true) {
    //......................................
    try {
      if (!_MqttCon_IsConnected && !_WiFi_ConnectWorking && !_Mqtt_TryConnecting) {
        if (_MqttCon_Steps == 0) {
          IOT__WiFiStart();
          if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("############ _MqttCon_Steps 0 :: WiFiStart()");
          _MqttCon_Steps = 1;
        } else if (_MqttCon_Steps == 2) {
          const char *mqtt_broker = _MqttBroker.c_str();
          const int mqtt_port = 1883;  //5000
          _MqttObj.setCallback(Mqtt__OnRecieve);
          _MqttObj.setServer(mqtt_broker, mqtt_port);
          _MqttObj.setBufferSize(11000);
          _MqttObj.setSocketTimeout(7);
          _MqttObj.setKeepAlive(7);
          _MqttCon_Steps = 3;
          if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("############ _MqttCon_Steps 1 :: MqttObj.setServer()");
          //......................................
        } else if (_MqttCon_Steps == 3) {
          Mqtt__Connect();
		  delay(2000);
        }
      }
    } catch (...) {}
    delay(1000);
  }
}




void IOT__Loop() {
  while (true) {
    try {
      if (_MqttCon_IsConnected) {
        _MqttObj.loop();
        delay(30);
      } else {
        delay(100);
      }
    } catch (...) {
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("IOT__Loop Error ");
    }
    delay(TASK_LOOP_DELAY);
  }
}


void IOT__Setup(void *param) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nIOT__Setup Start....");
  //........................................... WiFi Settings
  _WiFi_ConnectWorking = true;
  if (WiFi.isConnected()) {
    WiFi.disconnect(true);
    delay(100);
  }
  _WiFi_IsConnected = false;
  WiFi.onEvent(IOT__WiFiEvent);
  //............................................ Mqtt Settings
  _MqttClientId = _CloudClientId;
  _MqttCon_IsConnected = false;
  _WiFi_ConnectWorking = false;
  _Mqtt_TryConnecting = false;
  _MqttCon_Steps = 0;
  IOT__Trying = false;

  //........................................... Stay In Loop
  xTaskCreatePinnedToCore(
    IOT__Checker,          // Function that should be called
    "IOT__Checker",        // Name of the task (for debugging)
    TASK_STACKSIZE_LARGE,  // Stack size (bytes)
    NULL,                  // Parameter to pass
    TASK_PRIORITY_HIGH,    // Task priority (Max:1)
    NULL,                  // Task handle
    TASK_CORE_1);          // Core (0/1)
  //...........................................

  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("IOT__Setup End\r\n");
  //........................................... Stay In Loop
  IOT__Loop();
}
