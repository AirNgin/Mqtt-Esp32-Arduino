
//===========================================================================================
//                                          AP Mode
//===========================================================================================

bool _NeedReboot = false;

String Query_Data() {
  String data = "";
  try {
    if (_Server.hasArg("data")) data = String(_Server.arg("data"));
    else if (_Server.hasArg("Data")) data = String(_Server.arg("Data"));
    else if (_Server.hasArg("DATA")) data = String(_Server.arg("DATA"));
    if (data == "") {
      String noNeed = ",RANDOM,Random,random,Rnd,RND,rnd,TOKEN,Token,token,TKN,Tkn,tkn";
      for (int i = 0; i < _Server.args(); i++) {
        if (noNeed.indexOf(_Server.argName(i)) >= 0) continue;
        data = String(_Server.arg(i));
        break;
      }
    }
  } catch (...) {}
  return data;
}


void Config__ResponseJSON(String json, bool needRestart = false) {
  json.replace("'", "\"");
  _Server.send(200, "text/json", json);
  if (needRestart) {
    delay(500);
    Tools__Reboot();
  }
}



void Config__API_GetSerial() {
  try {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("***** Get Serial... ");
    //...................................................
    _Server.sendHeader("Connection", "close");
    _Server.send(200, "text/plain", _SerialCloud);
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Get Serial OK ******");
  } catch (...) {}
}



void Config__SetJson_Network(DynamicJsonDocument doc) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("API-Config: Network-Save");
  String tmp;
  JsonObject jsn = doc.as<JsonObject>();
  try {
    if (jsn["network"]) jsn = jsn["network"].as<JsonObject>();
  } catch (...) {}
  try {
    if (jsn["projectCode"]) _ProjectCode = jsn["projectCode"].as<String>();
  } catch (...) {}
  try {
    if (jsn["modemSsid"]) _ModemSSID = jsn["modemSsid"].as<String>();
  } catch (...) {}
  try {
    if (jsn["modemPass"]) _ModemPass = jsn["modemPass"].as<String>();
  } catch (...) {}
  String ch = "";
  try {
    if (jsn["modemChannel"]) ch = jsn["modemChannel"].as<String>();
  } catch (...) {}
  _ModemChannel = Tools__StringToByte(ch);
  try {
    if (jsn["mqttBroker"]) _MqttBroker = jsn["mqttBroker"].as<String>();
  } catch (...) {}
  try {
    if (jsn["mqttUser"]) _MqttUser = jsn["mqttUser"].as<String>();
  } catch (...) {}
  try {
    if (jsn["mqttPass"]) _MqttPass = jsn["mqttPass"].as<String>();
  } catch (...) {}
  try {
    if (jsn["clientId"]) {
      String cid = jsn["clientId"].as<String>();
      cid.trim();
      if (cid.indexOf("-" + _ProjectCode + "-") >= 3 && cid.indexOf("-" + _SerialCloud + "-") > 10) _CloudClientId = cid;
      else _CloudClientId = "";
    }
  } catch (...) {}
  try {  // correction
    if (_MqttBroker == "" || _MqttBroker.substring(0, 8) == "192.168.") _CloudClientId = "";
  } catch (...) {}
  Tools__SettingSave();
}


String Config__SetJson(String data) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\n***** SetJson Recieved Data ******");
  _NeedReboot = false;
  String res = "", tmp;
  try {
    if (data != "") {
      DynamicJsonDocument doc(1024 + (2 * data.length()));
      doc.clear();
      deserializeJson(doc, data.c_str());
      JsonObject jsn;
      //............................................................................

      String mode = "", sec = "";
      jsn = doc.as<JsonObject>();
      try {
        if (data.indexOf("\"requestMode\"") > 0) mode = jsn["requestMode"].as<String>();
        else if (data.indexOf("\"mode\"") > 0) mode = jsn["mode"].as<String>();
        mode.toLowerCase();
      } catch (...) {}
      try {
        if (data.indexOf("\"closeConfig\"") > 0) _NeedReboot = jsn["closeConfig"].as<bool>() ? true : false;
      } catch (...) {}
      try {
        if (data.indexOf("\"section\"") > 0) sec = jsn["section"].as<String>();
        sec.toLowerCase();
      } catch (...) {}
      if (mode == "network") {
        sec = "network";
        mode = "";
      }
      if (mode == "reboot" || mode == "close") {
        sec = "general";
        mode = "exit";
      }
      //...................................................... GENERAL
      if (sec == "general") {
        if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson: general");
        //..................................................
        if (mode == "reboot") {
          if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson: Reboot All");
          Tools__SetMode("reboot_sendall", false);
          _NeedReboot = true;
          res = "{'result':'ok'}";
          goto response_end;
        } else if (mode == "exit") {
          if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson: Exit (Closing Config)");
          Tools__SetMode("normal", false);
          _NeedReboot = true;
          res = "{'result':'ok'}";
          goto response_end;
        }
      }
      //...................................................... NETWORK
      else if (sec == "network") {
        if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson: Network");
        bool sendtoall = false;
        try {
          if (data.indexOf("\"target\"") > 0) sendtoall = (jsn["target"].as<String>() == "all" ? true : false);
        } catch (...) {}
        //..................................................
        if (mode == "restore") {
          if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson: Network-Restore");
          Tools__SettingDefault();
          Tools__SettingSave();
          Tools__SetMode(sendtoall ? "config_sendall" : "config_panel", false);  // Set Restart-Point
          res = "{'result':'ok'}";
          goto response_end;
        }
        //..................................................
        else {
          Config__SetJson_Network(doc);
          if (mode == "save" && sendtoall) {
            if (sendtoall) Tools__SetMode("config_sendall", false);
            res = "{'result':'ok'}";
          } else {
            res = "{'result':'ok'}";
          }
          goto response_end;
        }
      }
      //...................................................... TUNING
      else {
        res = "{'result':'wrong'}";
      }
      doc.clear();
    }
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("SetJson Error ");
  }
response_end:
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("*********************");
  res.replace("'", "\"");
  return res;
}


void Config__API_Set() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\n***** Config API-Set Recieved Data Start ******");
  try {
    _Server.sendHeader("Connection", "close");
    //.......................................................................
    String data = Query_Data();
    if (data != "") Config__ResponseJSON(Config__SetJson(data));
  } catch (...) {}
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("****** Config API-Set Recieved Data Finish *****");
  if (_NeedReboot) Tools__Reboot();
}







String Config__GetJson_Network() {
  String res = "'network':{";
  res += ",'projectCode':'" + _ProjectCode + "'";
  res += ",'modemChannel':'" + String(_ModemChannel) + "'";
  res += ",'modemSsid':'" + _ModemSSID + "'";
  res += ",'modemPass':'" + _ModemPass + "'";
  res += ",'mqttUser':'" + _MqttUser + "'";
  res += ",'mqttPass':'" + _MqttPass + "'";
  res += ",'mqttBroker':'" + _MqttBroker + "'";
  res += ",'clientId':'" + _CloudClientId + "'";
  res += "}";
  return res;
}


void Config__API_Get() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\n***** API-Get ******");
  _Server.sendHeader("Connection", "close");
  //.......................................................................
  String res = "";
  try {
    String data = Query_Data();
    //...................................................................
    if (data != "") {
      int max = 1024 + (2 * data.length());
      DynamicJsonDocument doc(max);
      doc.clear();
      deserializeJson(doc, data.c_str());
      JsonObject jsn;
      //............................................................................
      String mode = "";
      String sec = "";
      jsn = doc.as<JsonObject>();
      try {
        if (data.indexOf("\"requestMode\"") > 0) mode = jsn["requestMode"].as<String>();
        else if (data.indexOf("\"mode\"") > 0) mode = jsn["mode"].as<String>();
        mode.toLowerCase();
      } catch (...) {}
      try {
        if (data.indexOf("\"section\"") > 0) sec = jsn["section"].as<String>();
        sec.toLowerCase();
      } catch (...) {}
      //...................................................................
      if (mode == "network") {
        sec = "network";
        mode = "";
      } else if (mode == "serial") {
        sec = "serial";
        mode = "";
      }
      //...................................................... Serial
      if (sec == "serial") {
        if (res.length() > 0) res += ",";
        res += "'serial':'" + _SerialCloud + "'";
      }
      //...................................................... Network
      else if (sec == "network") {
        if (res.length() > 0) res += ",";
        res += "'serial':'" + _SerialCloud + "'";
        res += ",'firmware':'" + String(SOFTWARE_VERSION) + (SOFTWARE_DEBUGMODE ? " Debug" : "") + "'";
        res += "," + Config__GetJson_Network();
      }
      //...................................................... Return Result
      res = (res.length() > 0) ? "{'result':'ok'," + res + "}" : "{'result':'wrong'}";
    }
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("API-Get Error");
    res = "{'result':'error'}";
  }
  Config__ResponseJSON(res);
}


void Config__Loop() {
  while (true) {
    try {
      _Server.handleClient();
    } catch (...) {
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Config__Loop Error ");
    }
    delay(TASK_LOOP_DELAY);
  }
}


void Config__Setup(void* param) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nStart Config_Setup ... \r\n");
  _WiFi_ConnectWorking = true;
  try {
    //...................................... Set Powercut Restart-Point
    Tools__Memory_StrSet("_StartMode", "normal");

    //....................................
    IPAddress local_IP(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    //............................................
    WiFi.softAPConfig(local_IP, local_IP, subnet);
    delay(2500);

    String wifiName = _SerialCloud;
    String wifiPass = DVC_DEFAULTPASS;
    char wN[wifiName.length()];
    wifiName.toCharArray(wN, wifiName.length() + 1);
    char wP[wifiPass.length()];
    wifiPass.toCharArray(wP, wifiPass.length() + 1);
    WiFi.softAP(wN, wP);
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("WifiName : " + String(wN) + "  &   Pass : " + String(wP));

    delay(1000);
    WiFi.softAPConfig(local_IP, local_IP, subnet);

    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln(WiFi.softAPIP().toString());

    //............................................
    while (!MDNS.begin(BRAND_NAME)) {
      if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Error setting up MDNS responder!");
      delay(1000);
    }
    // DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    // DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
    _Server.enableCORS();

    _Server.on("/set", Config__API_Set);
    _Server.on("/get", Config__API_Get);
    _Server.on("/getSerial", Config__API_GetSerial);
    _Server.on("/getserial", Config__API_GetSerial);

    _Server.begin();
    MDNS.addService("http", "tcp", 80);
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nConfig_Setup Error\r\n");
  }
  _WiFi_ConnectWorking = false;
  Dvc__SetPinout();
  //........................................... Stay In Loop
  Config__Loop();
}
