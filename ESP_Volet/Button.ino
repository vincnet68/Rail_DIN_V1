
void setup_button()
{
 // link the button 1 functions.
  button1.attachClick(click1);
  button1.attachDoubleClick(doubleclick1);
  button1.attachLongPressStart(longPressStart1);
  button1.attachLongPressStop(longPressStop1);
  button1.attachDuringLongPress(longPress1);

  // link the button 2 functions.
  button2.attachClick(click2);
  button2.attachDoubleClick(doubleclick2);
  button2.attachLongPressStart(longPressStart2);
  button2.attachLongPressStop(longPressStop2);
  button2.attachDuringLongPress(longPress2);

 }

 
//***********************************************************************************
//FONCTION SORTIES RELAIS EN FONCTION DES ENTREES POUR COMMANDE LOCAL
// ----- button 1 callback functions
// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void click1() {

 if (isMoving){
    if (debug){
      Serial.println("Click Up Shutter Moving" );    
    }
      if (!shutterInitialized) stp();
      else shutters.stop();
    }
   else{
     if (debug){
          Serial.println("Click Up Shutter Up" );
     }
              if (shutters.getCurrentLevel() != 100){
    
      if (!shutterInitialized) up();
      else shutters.setLevel(100);
     }
   }
  mqttPublish(BtUpClick_topic, CONST_TRUE);
  mqttPublish(BtUpClick_topic, CONST_FALSE);
} // click1

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
  Serial.println("Button 1 doubleclick.");
    mqttPublish(BtUpDblClick_topic,CONST_TRUE); 
    mqttPublish(BtUpDblClick_topic,CONST_FALSE); 
} // doubleclick1

// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() {
  Serial.println("Button 1 longPress start");
  mqttPublish(BtUpLongClick_topic,CONST_TRUE); 
  mqttPublish(BtUpLongClick_topic,CONST_FALSE); 
} // longPressStart1

// This function will be called often, while the button1 is pressed for a long time.
void longPress1() {
  Serial.println("Button 1 longPress...");
} // longPress1

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStop1() {
  Serial.println("Button 1 longPress stop");
} // longPressStop1

// ... and the same for button 2:

void click2() {
 if (isMoving){
    if (debug){
                 
      Serial.println("Click Down Shutter Moving" );    ;
    }
      if (!shutterInitialized) stp();
      else shutters.stop();
    }
   else{
 if (debug){
      Serial.println("Click Down Shutter Down" );    ;
                 
    }
          if (shutters.getCurrentLevel() != 0){
      if (!shutterInitialized) dwn();
      else shutters.setLevel(0);
   }
   }
   mqttPublish(BtDownClick_topic,CONST_TRUE); 
   mqttPublish(BtDownClick_topic,CONST_FALSE); 
} // click2

void doubleclick2() {
  Serial.println("Button 2 doubleclick.");
  mqttPublish(BtDownDblClick_topic,CONST_TRUE); 
  mqttPublish(BtDownDblClick_topic,CONST_FALSE); 
} // doubleclick2

void longPressStart2() {
  Serial.println("Button 2 longPress start");
  mqttPublish(BtDownLongClick_topic,CONST_TRUE); 
  mqttPublish(BtDownLongClick_topic,CONST_FALSE); 
} // longPressStart2

void longPress2() {
  Serial.println("Button 2 longPress...");
} // longPress2



void longPressStop2() {

  Serial.println("Button 2 longPress stop");
} // longPressStop2


