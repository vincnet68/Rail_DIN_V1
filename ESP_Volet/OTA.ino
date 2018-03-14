/**************** Variables ********************************/

void setup_OTA()
{
  Serial.println("Setup OTA");
   // Configuration OTA
  //Port 8266 (defaut)
  ArduinoOTA.setPort(8266);
  //Hostname 
  ArduinoOTA.setHostname(ESP8266Client);
  //Mot de passe
  ArduinoOTA.setPassword(PASS);
  //Depart de la mise a jour
  ArduinoOTA.onStart([]() {
    Serial.println("Maj OTA");
  });
  //Fin de la mise a jour
  ArduinoOTA.onEnd([]() {
    Serial.print("\n Maj terminee");
  });
  //Pendant la mise a jour
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progression : %u%%\r", (progress / (total / 100)));
  });
  //En cas d'erreur
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erreur[%u]: ", error);
    switch (error) {
      //Erreur d'authentification
      case OTA_AUTH_ERROR :     Serial.println("Erreur d'authentification lors de la mise à jour");
                                break;
      case OTA_BEGIN_ERROR :    Serial.println("Erreur lors du lancement de la mise à jour");
                                break;
      case OTA_CONNECT_ERROR :  Serial.println("Erreur de connexion lors de la mise à jour");
                                break;
      case OTA_RECEIVE_ERROR :  Serial.println("Erreur de reception lors de la mise à jour");
                                break;
      case OTA_END_ERROR :      Serial.println("Erreur lors de la phase finale de la mise à jour");
                                break;
      default:                  Serial.println("Erreur inconnue lors de la mise à jour");
    }
  });
  ArduinoOTA.begin();
  //Fin conf OTA

}
