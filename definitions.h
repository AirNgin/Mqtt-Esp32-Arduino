//===========================================================================================
//                                     Software & Hardware
//===========================================================================================
#define BRAND_NAME "ABCD"  // Pre-Serial Code (**** You Must Change ABCD to your 4 Pre-Serial Chars ***)
#define SOFTWARE_VERSION "1.00"
#define SOFTWARE_DEBUGMODE true

//.......................................... Hardware Qty
#define BYTE_1 1
#define BYTE_2 2
#define BYTE_3 3
#define BYTE_10 10
#define BYTE_11 11
#define BYTE_12 12
#define BYTE_16 16
#define BYTE_17 17
#define BYTE_20 20
#define BYTE_32 32
#define BYTE_37 37
#define BYTE_38 38
#define BYTE_100 100
#define BYTE_400 400
//===========================================================================================
//                                           EPPROM
//===========================================================================================
#define EEPROM_SIZE 4000

#define EP_MEMORY_NOT_CLEANABLE 50
#define EP_STARTMODE (EP_MEMORY_NOT_CLEANABLE + 1)
#define EP_SERIAL_S (EP_STARTMODE + 1)
#define EP_SERIAL_E (EP_SERIAL_S + BYTE_10 - 1)
#define EP_MEMORYSTART1 100  // MEMORY POINT 1 BEGIN :  Cleanable Settings, Clear Begin From Here To End Of EPROM

#define EP_PROJECTCODE_S (EP_MEMORYSTART1 + 1)  //........................................................... NETWORK
#define EP_PROJECTCODE_E (EP_PROJECTCODE_S + BYTE_20 - 1)
#define EP_CLIENTID_S (EP_PROJECTCODE_E + 1)
#define EP_CLIENTID_E (EP_CLIENTID_S + BYTE_38 - 1)
#define EP_MQTTUSER_S (EP_CLIENTID_E + 1)
#define EP_MQTTUSER_E (EP_MQTTUSER_S + BYTE_16 - 1)
#define EP_MQTTPASS_S (EP_MQTTUSER_E + 1)
#define EP_MQTTPASS_E (EP_MQTTPASS_S + BYTE_16 - 1)
#define EP_MQTTBROKER_S (EP_MQTTPASS_E + 1)
#define EP_MQTTBROKER_E (EP_MQTTBROKER_S + BYTE_100 - 1)
#define EP_MODEMCHANNEL (EP_MQTTBROKER_E + 1)
#define EP_MODEMSSID_S (EP_MODEMCHANNEL + 1)
#define EP_MODEMSSID_E (EP_MODEMSSID_S + BYTE_32 - 1)
#define EP_MODEMPASS_S (EP_MODEMSSID_E + 1)
#define EP_MODEMPASS_E (EP_MODEMPASS_S + BYTE_20 - 1)


//===========================================================================================
//                                      PREFERENCES MEMORY
//===========================================================================================
#define FILE_WRITE "w"
#define FILE_APPEND "a"
#define FILE_READ "r"
#define FILE_RW "w+"
#define FILE_RA "r+"

DynamicJsonDocument _JSONDoc(8000);

void Tools__File_JSONLoad(String filename, JsonVariant& jsn, DynamicJsonDocument& doc);
void Tools__File_JSONLoad(String filename, JsonVariant& jsn);
bool Tools__File_JSONSave(String filename, JsonVariant& jsn);
bool Tools__File_JSONSave(String filename, String& data);


//===========================================================================================
//                                           CONSTANTS
//===========================================================================================

#define DVC_DEFAULTNAME "smarthome"  // Change it to your desire (Like: device name & serial)
#define DVC_DEFAULTPASS "00000000"   // dont change the password of config

#define HOSTNAME "MQTT_Bridge"

#define TASK_PRIORITY_HIGH 1
#define TASK_PRIORITY_MID 2
#define TASK_PRIORITY_LOW 3
#define TASK_STACKSIZE_SMALL 2048
#define TASK_STACKSIZE_MEDIUM 4096
#define TASK_STACKSIZE_LARGE 8192
#define TASK_CORE_0 0
#define TASK_CORE_1 1
#define TASK_LOOP_DELAY 20


//===========================================================================================
//                                        GENERAL Tools
//===========================================================================================
void Debug_ConsolePrint(String p1);
void Debug_ConsolePrintln(String p1);
void Debug__ConsoleBegin();

void CA_CopyStr(char output[], String str, int max);
void Tools__SetMode(String mode, bool restartNeed);
void Tools__SettingRead();
void Tools__SettingDefault();
void Tools__SettingSave(String key);

void Tools__DvcAddressLoad(String typ);
void Tools__DvcAddressSave(String typ, byte index);

void Tools__Reboot();
void Tools__BackgroundWorks();
void Tools__FreeMemory();
void Tools__WifiPower(bool SetToOn);
uint32_t Tools__GetChipID();
long Tools__Random(long a, long b);


//===========================================================================================
//                                        Config
//===========================================================================================
char _MySenderId[38];
String _CloudClientId = "";
String _StartMode = "";
String _SerialNo = "";
String _SerialHex = "";
String _SerialCloud = "";
String _ProjectCode = "";

String _ModemSSID = "";
String _ModemPass = "";
byte _ModemChannel = 0;

String _MqttBroker = "";
String _MqttUser = "";
String _MqttPass = "";

//.......................
bool _Started_IotRun = false;
bool _Started_ConfigPanel = false;




//===========================================================================================
//                                        IOT / Mqtt
//===========================================================================================
bool _WiFi_IsConnected = false;
bool _WiFi_ConnectWorking = false;
WiFiClient wifiClient;
PubSubClient _MqttObj(wifiClient);
String _CurrentIP = "0.0.0.0";
String _MqttTopic = "";
String _MqttClientId = "";
byte _MqttCon_Steps = 0;
bool _MqttCon_IsConnected = false;
bool _Mqtt_TryConnecting = false;
uint16_t _MqttPort = 1883;
void Mqtt__Send(String topic, String data);

void IOT__Setup(void* param);



//===========================================================================================
//                                       Config / Update
//===========================================================================================
WebServer _Server(80);
void Config__Setup(void* param);
void Config__Loop();



//===========================================================================================
//                                            Dvc
//===========================================================================================
void Dvc__SetPinout();
void Dvc__CloudSendAllStatus();

void Dvc__LastStatusSave();
void Dvc__LastStatusLoad();

void Dvc__SetPinout();
void Dvc__Setup();
void Dvc__Loop(void* param);
