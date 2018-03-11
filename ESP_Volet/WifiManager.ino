void setup_WifiManager (WiFiManager &wifiManager)
{
  //WIFI// 

  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);
  //Ajout de la variable de configuration MQTT Server (ou Broker)
  WiFiManagerParameter custom_mqtthost("server", "mqtt server", mqtthost, 16);
  WiFiManagerParameter custom_timeCourse_up("time_course_up", "Time Course Up", timeCourseup, 3);
  WiFiManagerParameter custom_timeCourse_down("time_course_down", "Time Course Down", timeCoursedown, 3);
  WiFiManagerParameter custom_ESP8266Client("ESP8266Client", "ESPName", ESP8266Client, 20);
  wifiManager.addParameter(&custom_mqtthost);
  wifiManager.addParameter(&custom_timeCourse_up);
  wifiManager.addParameter(&custom_timeCourse_down);
  wifiManager.addParameter(&custom_ESP8266Client);

// on affiche l'adresse IP qui nous a été attribuée
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Lecture de la valeur MQTT server enregistrée dans le fichier de config json
  strcpy(mqtthost, custom_mqtthost.getValue());
  strcpy(timeCourseup, custom_timeCourse_up.getValue());
  upCourseTime = (strtoul (timeCourseup, NULL, 10)) * 1000; //On retype la variable (%ul unsigned long) et on la multiplie par 1000 (ce sont des millisecondes)
  strcpy(timeCoursedown, custom_timeCourse_down.getValue());
  downCourseTime = (strtoul (timeCoursedown, NULL, 10)) * 1000;
  strcpy (ESP8266Client, custom_ESP8266Client.getValue());
  //Sauvegarde des valeurs dans le fichier de configuration json
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtthost"] = mqtthost;
    json["timeCourseup"] = timeCourseup;
    json["timeCoursedown"] = timeCoursedown;
    json["ESP8266Client"] = ESP8266Client;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("Erreur lors de l'ouverture du fichier de config json pour enregistrement");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  wifiManager.autoConnect(ESP8266Client, PASS);

//reset settings - for testing
if (raz){
  Serial.println("Réinitialisation de WiFiManager.");
  wifiManager.resetSettings();
  }
}



void setup_ReadSPIFS()
{
    //Lecture du fichier de configuration depuis le FS avec json
  Serial.println("montage du FS...");
  if (SPIFFS.begin()) {
    Serial.println("FS monté");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("Lecture du fichier de config");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("Fichier de config ouvert");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nJson parsé");
          strcpy(mqtthost, json["mqtthost"]);
          strcpy(timeCourseup, json["timeCourseup"]);
          strcpy(timeCoursedown, json["timeCoursedown"]);
          strcpy(ESP8266Client, json["ESP8266Client"]);
          upCourseTime = (strtoul (timeCourseup, NULL, 10)) * 1000; //On retype la variable (%ul unsigned long) et on la multiplie par 1000 (ce sont des millisecondes)
          downCourseTime = (strtoul (timeCoursedown, NULL, 10)) * 1000;
         } else {
          Serial.println("Erreur lors du chargement du fichier de config json");
        }
      }
    }
  } else {
    Serial.println("Erreur lors du montage du FS");
  }
  //end read
}

// FONCTION Reset + Format memoire ESP
void eraz(){
  WiFiManager wifiManager;
if (debug){Serial.println("Réinitialisation de WiFiManager.");}
  wifiManager.resetSettings();
if (debug){Serial.println("Réinitialisation de la configuration (reset SPIFFS).");}
  //SPIFFS.format();
ESP.restart();
}


/************************ CALLBACK *******************
 *
 */

 //callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
  localModeOnly = false;
  //ticker.detach();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  localModeOnly = true;
  //ticker.attach(0.05, loopLocalShutter);
}


