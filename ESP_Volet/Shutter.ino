/**************** Variables ********************************/
const byte eepromOffset = 0; //Zone de départ pour la lecture de EEPROM


//***********************************************************************************
// FONCTIONS LIBRAIRIE position volets


void setup_Volet()
{
  //Shutters //
  Serial.println("Setup Shutter");
   char storedShuttersState[shutters.getStateLength()];
  readInEeprom(storedShuttersState, shutters.getStateLength());
  if (upCourseTime < 1) upCourseTime = DEFAULT_COURSE_TIME * 1000; //Set a time if not set
  if (downCourseTime < 1) downCourseTime = DEFAULT_COURSE_TIME * 1000;//Set a time if not set

  if (debug) {
  Serial.print("storedShuttersState :"); 
  Serial.println(storedShuttersState);  
  Serial.print("upCourseTime :"); 
  Serial.println(upCourseTime);
  Serial.print("downCourseTime :"); 
  Serial.println(downCourseTime);
  }
   shutters
    .setOperationHandler(shuttersOperationHandler)
    .setWriteStateHandler(shuttersWriteStateHandler)
    .restoreState(storedShuttersState)
    .setCourseTime(upCourseTime, downCourseTime)
    .onLevelReached(onShuttersLevelReached)
    .begin();
  shutterInitialized = true;

}

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


//********************* FONCTIONS DIVERS*********************
void readInEeprom(char* dest, byte length) {
  for (byte i = 0; i < length; i++) {
    dest[i] = EEPROM.read(eepromOffset + i);
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




