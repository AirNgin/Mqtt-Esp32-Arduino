
void Debug_ConsolePrint(String p1) {
  if (!SOFTWARE_DEBUGMODE) return;
  try {
    Serial.print(p1);
  } catch (...) {
    Serial.println("\r\nErr Debug_ConsolePrint\r\n");
  }
}


void Debug_ConsolePrintln(String p1) {
  if (!SOFTWARE_DEBUGMODE) return;
  try {
    Serial.println(p1);
    Serial.flush();
  } catch (...) {
    Serial.println("\r\nErr Debug_ConsolePrintln\r\n");
  }
}


void Debug__ConsoleBegin() {
  Serial.setRxBufferSize(1024);
  Serial.begin(9600);
  Serial.flush();
}


byte Tools__StringToByte(String inp) {
  byte r = 0;
  try {
    r = (byte)((inp).toInt() & 0xFF);
  } catch (...) {}
  return r;
}


String Tools__ByteToHexstring(long b) {
  String Str = String(b, HEX);
  Str.toUpperCase();
  return (Str.length() % 2 == 0 ? "" : "0") + Str;
}



uint64_t Tools__HexToUInt64(String p) {
  p.toUpperCase();
  uint64_t accumulator = 0;
  for (size_t i = 0; isxdigit((unsigned char)p[i]); ++i) {
    char c = p[i];
    accumulator *= 16;
    if (isdigit(c)) accumulator += c - '0';  // '0' .. '9'
    else accumulator += c - 'A' + 10;        //'A'  .. 'F'
  }
  return accumulator;
}



long Tools__HexToDecimal(String value) {
  int base = 16;
  int length = value.length() + 1;
  char valueAsArray[length];
  value.toCharArray(valueAsArray, length);
  return strtol(valueAsArray, NULL, base);
}


String Tools__StringToHexStr(String value) {
  String res = "";
  int len = value.length();
  for (int i = 0; i < len; i++) res += Tools__ByteToHexstring((byte)value.charAt(i));
  return res;
}



void CA_CopyStr(char output[], String str, int max = 0) {
  int i, c, len = str.length();
  for (i = 0; i < len; i++) {
    if (max > 0 && i >= max) break;
    c = str.charAt(i);
    if (c == '\0') break;
    output[i] = c;
  }
  output[i] = '\0';
}


String CA_ToString(char input[], int start = 0, int len = 0) {
  int i, end = strlen(input);
  String str = "";
  if (len == 0) len = end - start;
  end = start + len;
  for (i = start; i < end; i++) {
    if (input[i] == '\0') break;
    str += input[i];
  }
  return str;
}




bool Tools__File_TextWrite(const String& filename, const String& data) {
  bool res = false;
  try {
    File file = SPIFFS.open(filename.c_str(), FILE_WRITE);  // Open the file for writing
    try {
      if (!file) return false;
      res = file.print(data);
    } catch (...) {}
    file.close();
  } catch (...) {}
  return res;
}


String Tools__File_TextRead(const String& filename) {
  String data = "";
  try {
    File file = SPIFFS.open(filename.c_str(), FILE_READ);
    try {
      if (!file) return "";
      try {
        data.reserve(8100);
      } catch (...) {}
      data = file.readString();
      //Debug_ConsolePrintln("Tools__File_TextRead data: " + data);
    } catch (...) {}
    file.close();
  } catch (...) {}
  return data;
}


bool Tools__File_Clear(const String& filename) {
  bool res = false;
  try {
    File file = SPIFFS.open(filename.c_str(), FILE_WRITE);  // Open the file for writing
    try {
      if (!file) return false;
      res = file.print("");
    } catch (...) {}
    file.close();
  } catch (...) {}
  return res;
}


void Tools__File_ClearAll() {
  try {
    SPIFFS.format();
  } catch (...) {}
}


void Tools__File_JSONLoad(String filename, JsonVariant& jsn, DynamicJsonDocument& doc) {
  try {
    filename.toLowerCase();
    filename.replace("\\", "/");
    if (filename.substring(0, 1) != "/") filename = "/" + filename;
    if (filename.indexOf(".json") < 0) filename += ".json";
    String data = Tools__File_TextRead(filename);
    Debug_ConsolePrintln("Tools__File_JSONLoad " + filename + " => " + data);
    if (doc == NULL) doc = _JSONDoc;
    if (data != "") {
      doc.clear();
      data = "{\"root\":" + data + "}";
      deserializeJson(doc, data.c_str());
      jsn = doc["root"].as<JsonVariant>();
      // doc.clear();
    }
  } catch (...) {}
}


void Tools__File_JSONLoad(String filename, JsonVariant& jsn) {
  try {
    filename.toLowerCase();
    filename.replace("\\", "/");
    if (filename.substring(0, 1) != "/") filename = "/" + filename;
    if (filename.indexOf(".json") < 0) filename += ".json";
    String data = Tools__File_TextRead(filename);
    Debug_ConsolePrintln("Tools__File_JSONLoad " + filename + " => " + data);
    if (data != "") {
      _JSONDoc.clear();
      data = "{\"root\":" + data + "}";
      deserializeJson(_JSONDoc, data.c_str());
      jsn = _JSONDoc["root"].as<JsonVariant>();
      //_JSONDoc.clear();
    }
  } catch (...) {}
}


bool Tools__File_JSONSave(String filename, JsonVariant& jsn) {
  try {
    String data = "";
    try {
      data = jsn.as<String>();
    } catch (...) {}
    if (data == "") serializeJson(jsn, data);
    if (data.length() <= 8000) {
      Debug_ConsolePrintln("Tools__File_JSONSave " + filename + " => " + data);
      try {
        data.reserve(8100);
      } catch (...) {}
      filename.toLowerCase();
      filename.replace("\\", "/");
      if (filename.substring(0, 1) != "/") filename = "/" + filename;
      if (filename.indexOf(".json") < 0) filename += ".json";
      Tools__File_TextWrite(filename, data);
      return true;
    }
  } catch (...) {}
  return false;
}


bool Tools__File_JSONSave(String filename, String& data) {
  try {
    if (data.length() <= 8000) {
      // Debug_ConsolePrintln("Tools__File_JSONSave data: " + data);
      Debug_ConsolePrintln("Tools__File_JSONSave " + filename + " => " + data);
      try {
        data.reserve(8100);
      } catch (...) {}
      filename.toLowerCase();
      filename.replace("\\", "/");
      if (filename.substring(0, 1) != "/") filename = "/" + filename;
      if (filename.indexOf(".json") < 0) filename += ".json";
      Tools__File_TextWrite(filename, data);
      return true;
    }
  } catch (...) {}
  return false;
}


void MemoWriteByte(int p_start, byte val) {
  EEPROM.write(p_start, val);
  EEPROM.commit();
  delay(1);
}


void MemoWriteString(int p_start, int p_end, String val) {
  int max = val.length();
  byte v[max];
  val.getBytes(v, max + 1);
  for (int i = p_start, c = 0; i <= p_end; i++, c++) {
    EEPROM.write(i, c < max ? v[c] : 0);
  }
  EEPROM.commit();
  delay(1);
}


byte MemoReadByte(int p_start) {
  byte r = 0;
  try {
    r = 0 + EEPROM.read(p_start);
  } catch (...) {}
  return r;
}


String MemoReadString(int p_start, int p_end) {
  String val = "";
  int i, empty = 0;
  char v, cFF = char(0xFF);
  for (i = p_start; i <= p_end; i++) {
    try {
      v = EEPROM.read(i);
      if (v == '\0') break;
      if (v == cFF) empty++;
      val += ("" + String(v));
    } catch (...) {}
  }
  if (val.length() == empty) val = "";
  return val;
}


bool Tools__Memory_StrSet(String key, String val) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Tools__Memory_StrSet > " + key + " : " + val);
  try {
    if (key == "_StartMode") {
      MemoWriteByte(EP_STARTMODE,
                    val == "normal"           ? 0
                    : val == "config_panel"   ? 9
                                              : 0);
    } else if (key == "_SerialNo") {
      val.toUpperCase();
      MemoWriteString(EP_SERIAL_S, EP_SERIAL_E, val);
    } else if (key == "_CloudClientId") {
      MemoWriteString(EP_CLIENTID_S, EP_CLIENTID_E, val);
    } else if (key == "_ProjectCode") {
      val.toUpperCase();
      MemoWriteString(EP_PROJECTCODE_S, EP_PROJECTCODE_E, val);
    } else if (key == "_MqttBroker") {
      MemoWriteString(EP_MQTTBROKER_S, EP_MQTTBROKER_E, val);
      MemoWriteString(EP_MQTTBROKER_S, EP_MQTTBROKER_E, val);
    } else if (key == "_MqttUser") {
      MemoWriteString(EP_MQTTUSER_S, EP_MQTTUSER_E, val);
    } else if (key == "_MqttPass") {
      MemoWriteString(EP_MQTTPASS_S, EP_MQTTPASS_E, val);
    } else if (key == "_ModemChannel") {
      byte b = Tools__StringToByte(val);
      b = (b <= 14 && b > 0 ? b : 0);
      MemoWriteByte(EP_MODEMCHANNEL, b);
    } else if (key == "_ModemSSID") {
      MemoWriteString(EP_MODEMSSID_S, EP_MODEMSSID_E, val);
    } else if (key == "_ModemPass") {
      MemoWriteString(EP_MODEMPASS_S, EP_MODEMPASS_E, val);
    }
    return true;
  } catch (...) {}
  return false;
}


String Tools__Memory_StrGet(String key) {
  bool checkNull = false;
  String res = "";
  int v = 0;
  try {
     if (key == "_StartMode") {
      v = MemoReadByte(EP_STARTMODE);
      res = (v == 0   ? "normal"
             : v == 9 ? "config_panel"
                      : "normal");
    } else if (key == "_SerialNo") {
      res = MemoReadString(EP_SERIAL_S, EP_SERIAL_E);
      if (res == "" || res[0] == char(255)) res = "0000000000";
    } else if (key == "_CloudClientId") {
      res = MemoReadString(EP_CLIENTID_S, EP_CLIENTID_E);
      if (res[0] == char(255)) res = "";
    } else if (key == "_ProjectCode") {
      res = MemoReadString(EP_PROJECTCODE_S, EP_PROJECTCODE_E);
      //res.toUpperCase();
      checkNull = true;
    } else if (key == "_MqttUser") {
      res = MemoReadString(EP_MQTTUSER_S, EP_MQTTUSER_E);
      checkNull = true;
    } else if (key == "_MqttPass") {
      res = MemoReadString(EP_MQTTPASS_S, EP_MQTTPASS_E);
      checkNull = true;
    } else if (key == "_MqttBroker") {
      res = MemoReadString(EP_MQTTBROKER_S, EP_MQTTBROKER_E);
      checkNull = true;
    } else if (key == "_ModemChannel") {
      v = MemoReadByte(EP_MODEMCHANNEL);
      res = "" + String(v);
    } else if (key == "_ModemSSID") {
      res = MemoReadString(EP_MODEMSSID_S, EP_MODEMSSID_E);
      checkNull = true;
    } else if (key == "_ModemPass") {
      res = MemoReadString(EP_MODEMPASS_S, EP_MODEMPASS_E);
      checkNull = true;
    }
  } catch (...) {}
  if (checkNull && res.charAt(0) == char(255)) res = "";
  return res;
}





long Tools__Random(long a, long b) {
  long d;
  if (a > b) {
    d = a;
    a = b;
    b = d;
  }
  d = (b - a) + 1;
  try {
    return a + (esp_random() % d);
  } catch (...) {
    if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Random Error ");
  }
  return a + (rand() % d);
}


void Tools__Reboot() {
  Dvc__LastStatusSave();
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("!!!!!!!! Rquested To REBOOT !!!!!!!!");
  ESP.restart();
}



void Tools__BackgroundWorks() {
  try {
    //ESP.wdtFeed();
    delay(0);
    //ESP.wdtFeed();
  } catch (const std::exception& ex) {}
}


//-----------------------------------------------------------------------------------------------
//-------------------------------- Set/Get Settings From EPPROM  --------------------------------
//-----------------------------------------------------------------------------------------------

void Tools__SerialBarcodeReload() {
  _SerialNo = Tools__Memory_StrGet("_SerialNo");
  if (_SerialNo.length() != 10) {
    _SerialNo = _SerialNo + "0000000000";
    _SerialNo = _SerialNo.substring(0, 10);
  }
  //............................................. Build Random Serial
  else {
    String chip = (String(Tools__GetChipID())+'0000000').substring(0, 7);
    if (chip == "0000000") chip = (String(Tools__Random(1000000, 9999998)) + "0000000").substring(0, 7);
    _SerialNo = ("000" + chip).substring(0, 10);
  }
  //............................................. Rebuild Some Variables
  _SerialHex = Tools__StringToHexStr(_SerialNo);
  _SerialCloud = BRAND_NAME + _SerialNo;
  CA_CopyStr(_MySenderId, "dvc_" + _SerialNo);
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("_SerialNo : " + _SerialNo);
}


void Tools__SettingRead() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Setting Read... ");
  //...................................... Load Important Data
  _StartMode = Tools__Memory_StrGet("_StartMode");
  //...................................... Read Other Settings
  _CloudClientId = Tools__Memory_StrGet("_CloudClientId");
  _CloudClientId.trim();
  _ProjectCode = Tools__Memory_StrGet("_ProjectCode");

  _ModemSSID = Tools__Memory_StrGet("_ModemSSID");
  _ModemPass = Tools__Memory_StrGet("_ModemPass");
  _ModemChannel = Tools__StringToByte(Tools__Memory_StrGet("_ModemChannel"));
  if (_ModemChannel > 11 || _ModemChannel < 0) _ModemChannel = 0;

  _MqttBroker = Tools__Memory_StrGet("_MqttBroker");
  _MqttUser = Tools__Memory_StrGet("_MqttUser");
  _MqttPass = Tools__Memory_StrGet("_MqttPass"); 
}



void Tools__SettingDefault() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Setting To Dafault >>> ");
  //..................
  _ProjectCode = "";

  _ModemSSID = "";
  _ModemPass = "";
  _ModemChannel = 0;

  _MqttBroker = "";
  _MqttUser = "";
  _MqttPass = "";
}



void Tools__SettingSave() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Setting Save... ");
  try {
    _CloudClientId.trim();
    Tools__Memory_StrSet("_CloudClientId", _CloudClientId);
    Tools__Memory_StrSet("_ProjectCode", _ProjectCode);

    Tools__Memory_StrSet("_ModemChannel", String(_ModemChannel));
    Tools__Memory_StrSet("_ModemSSID", _ModemSSID);
    Tools__Memory_StrSet("_ModemPass", _ModemPass);

    Tools__Memory_StrSet("_MqttBroker", _MqttBroker);
    Tools__Memory_StrSet("_MqttUser", _MqttUser);
    Tools__Memory_StrSet("_MqttPass", _MqttPass);
  } catch (...) {}
}



void Tools__SettingShowInfo() {
  if (SOFTWARE_DEBUGMODE) {
    Debug_ConsolePrintln("\r\n-----------------------\r\nSetting Show Info\r\n-----------------------");
    Debug_ConsolePrintln("_StartMode: " + _StartMode);

    Debug_ConsolePrintln("_SerialNo: " + _SerialNo);
    Debug_ConsolePrintln("_ProjectCode: " + _ProjectCode);

    Debug_ConsolePrintln("\r\n_ModemChannel: " + String(_ModemChannel));
    Debug_ConsolePrintln("_ModemSSID: " + _ModemSSID);
    Debug_ConsolePrintln("_ModemPass: " + _ModemPass);

    Debug_ConsolePrintln("\r\n_MqttBroker: " + _MqttBroker);
    Debug_ConsolePrintln("_MqttUser: " + _MqttUser);
    Debug_ConsolePrintln("_MqttPass: " + _MqttPass);
    Debug_ConsolePrintln("_CloudClientId: " + _CloudClientId);
    Debug_ConsolePrintln("");
  }
}



//===========================================================================================
//                                       GENERAL SETUP
//===========================================================================================

void Tools__FreeMemory() {
  try {
    ESP.getFreeHeap();
    // heap_caps_free(NULL);
  } catch (...) {}
}



void Tools__WifiPower(bool SetToOn) {
  if (SetToOn) {
    //................................. Sets WiFi Power-Sleep to None
    WiFi.setSleep(WIFI_PS_NONE);
    //................................. Sets WiFi RF power output to highest level
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    // WiFi.setTxPower(WIFI_POWER_2dBm);  // Sets WiFi RF power output to low level
  } else {
    WiFi.mode(WIFI_OFF);
  }
}



void Tools__SetMode(String mode, bool restartNeed) {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Set Next Mode To : " + mode);
  //.............................................
  if (mode == "normal") {
  }
  //.............................................
  Tools__Memory_StrSet("_StartMode", mode);
  if (restartNeed) {
    delay(10);
    Tools__Reboot();
  }
}


uint32_t Tools__GetChipID() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}


