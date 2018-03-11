
//***********************************************************************************
// FONCTIONS LIBRAIRIE position volets

void shuttersOperationHandler(Shutters* s, ShuttersOperation operation) {
  switch (operation) {
    case ShuttersOperation::UP:
      if (debug){Serial.println("Shutters going up.");}
        up();       
      break;
    case ShuttersOperation::DOWN:
  if (debug){Serial.println("Shutters going down.");}
        dwn();
      break;
    case ShuttersOperation::HALT:
      if (debug){Serial.println("Shutters halting.");}
        stp();
      break;
  }
}

void shutterRaz(){
  if (debug){Serial.println("Réinitialisation shutter");}
  shutters
    .reset()
    .setOperationHandler(shuttersOperationHandler)
    .setWriteStateHandler(shuttersWriteStateHandler)
    .setCourseTime(upCourseTime, downCourseTime)
    .onLevelReached(onShuttersLevelReached)
    .begin();
}


void shuttersWriteStateHandler(Shutters* shutters, const char* state, byte length) {
  for (byte i = 0; i < length; i++) {
    EEPROM.write(eepromOffset + i, state[i]);
    #ifdef ESP8266
    EEPROM.commit();
    #endif
  }
}

void onShuttersLevelReached(Shutters* shutters, byte level) {
  if (debug){
  Serial.print("Shutters at ");
  Serial.print(level);
  Serial.println("%");
  }
  if ((level % 10) == 0) {
  char charlevel[4];
  sprintf(charlevel, "%d", level); 
  mqttPublish(position_topic,charlevel); 
  }
}




