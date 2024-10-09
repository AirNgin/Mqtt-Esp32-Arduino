void Dvc__CloudSendAllStatus() {
  // send all status of this device to mqttBroker to update latest status
  // String msg = "";
  // do somethings to build status
  // Mqtt__Send("DeviceToServer", msg)
}

void Dvc__SetPinout() {
  // do somethings to set pinout
}

void Dvc__LastStatusSave() {
  // save last status of relay or somethings if needs
}


void Dvc__LastStatusLoad() {
  // load last relay status or somethings if saved
}


void Dvc__Analize(String data) {
  // analize & control to run action
}


void Dvc__Loop(void* param) {
  while (true) {// task must stay in this loop forever
    try {
      // example : if pressed config button , go to config mode ( by using Tools__SetMode("config_panel",true); )
      // example : if pressed relay button , switch it on/off
    } catch (...) {}
    delay(100);
  }
}

void Dvc__Setup() {
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("\r\nDvc__Setup Start....");
  Dvc__SetPinout();
  //...........................................
  // do somethings to prepare device
  //........................................... Stay In Loop
  xTaskCreatePinnedToCore(
    Dvc__Loop,             // Function that should be called
    "Dvc__Loop",           // Name of the task (for debugging)
    TASK_STACKSIZE_LARGE,  // Stack size (bytes)
    NULL,                  // Parameter to pass
    TASK_PRIORITY_HIGH,    // Task priority (Max:1)
    NULL,                  // Task handle
    TASK_CORE_1);          // Core (0/1)
  //...........................................
  if (SOFTWARE_DEBUGMODE) Debug_ConsolePrintln("Dvc__Setup End\r\n");
}
