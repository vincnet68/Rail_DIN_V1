/**************** Variables ********************************/

long lastConnect = 0; //Utilisé pour le temps de connexion MQTT
char message_buff[100];

void setup_mqtt()
{
//MQTT//
  
  client.setServer(mqtthost, 1883);
  Serial.print("host MQTT :");
  Serial.println(mqtthost);
  client.setCallback(callback);
}


//************************ FONCTION communication MQTT JEEDOM VERS ESP
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.println(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
   uint8_t niv = msgString.toInt();
   
  if (niv >= 0 && niv <= 100 &&  msgString != "up" && msgString != "dwn" && msgString != "stp" && msgString != "raz" && msgString != "shutraz"){
    shutters.setLevel(niv);
  }
    else if ( msgString == "up" ) {
    shutters.setLevel(100); 
  } else if ( msgString == "dwn" ){
    shutters.setLevel(0);  
  } else if ( msgString == "stp" ){
    shutters.stop();  
  } else if ( msgString == "raz" ){
  eraz();
  } else if ( msgString == "shutraz" ){
  shutterRaz();
  }
}


//Fonction reconnexion MQTT
void reconnect() { 
  // Loop until we're reconnected
  long now1 = millis();
  if (debug){    
    Serial.print("Attente de connexion MQTT...");}
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on to indicate the the module is not Ready
  //ticker.attach(0.05, loopLocalShutter);
    // Attempt to connect
    if (client.connect(ESP8266Client)) {  
      float connectedepuis = (now1 - lastConnect)/1000;
      if (debug){Serial.print("connected depuis :");
      Serial.println(connectedepuis);}
      // Once connected, publish an announcement...
      String topicString = String();
      topicString = String(prefix_topic) + String(ESP8266Client) + String(JeedomIn_topic);
      client.publish(string2char(topicString), String(connectedepuis).c_str());
      lastConnect = now1;
      // ... and resubscribe
      topicString = String(prefix_topic) + String(ESP8266Client) + String(JeedomOut_topic);
      client.subscribe(string2char(topicString));
    digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off to indicate the the module is Ready  
    //ticker.detach();
    } else {
  if (debug){    
      Serial.print("Erreur, rc=");
      Serial.print(client.state());}               
    }
  }

// FONCTION MQTT ESP VERS JEEDOM
void mqttPublish(char* topic, char* value)
{  
  if (!localModeOnly){
     if (!client.connected()) {
      reconnect();
    }
    else {  
      if (debug){
      Serial.print("envoi MQTT");
    Serial.print(topic);
    Serial.print(" : ");
    Serial.println(value);
      }
     String topicString = String();
      topicString = String(prefix_topic) + String(ESP8266Client) + String(topic);
      client.publish(string2char(topicString), value);
    }
  }
}
void mqttInit(){
  char level[4];
  int levelInt;
  levelInt =  shutters.getCurrentLevel();
  
  sprintf(level, "%d", levelInt); 
    if (debug){
    Serial.println("envoi MQTT d'initialisation");
    }
    mqttPublish(position_topic, level);
    mqttPublish(BtUpClick_topic,CONST_FALSE);
    mqttPublish(BtUpLongClick_topic,CONST_FALSE);
    mqttPublish(BtUpDblClick_topic,CONST_FALSE);
    mqttPublish(BtDownClick_topic,CONST_FALSE);
    mqttPublish(BtDownLongClick_topic,CONST_FALSE);
    mqttPublish(BtDownDblClick_topic,CONST_FALSE);

}

