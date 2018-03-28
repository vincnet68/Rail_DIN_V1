/* Version du 24/03/2018 par vincnet68

 * SKETCH fonctionnant sur wemos D1 MINI POUR COMMANDE DE VOLET ROULANT FILAIRE AVEC RENVOI DE LA POSITION DU VOLET EN POURCENTAGE
 * HARD Module sur Rail DIN (Alcor_fr et Rolrider): https://www.jeedom.com/forum/viewtopic.php?f=185&t=25017&sid=c757bad46d600f07820dab2a45ec8b33
 * LIBRAIRIES : https://github.com/marvinroger/arduino-shutters
 *              https://github.com/mathertel/OneButton
 * Possibilité d'enregistrer l'adresse IP de son broker MQTT
 * Possibilité d'enregistrer le temp de course
 * Possibilité d'enregistrer le nom du module (utilisé pour le WifiManager portal, l'OTA et la publication MQTT)
 * Utilisation de 2 boutons poussoir pour la commande (commande local, et envoie de double click et long click par MQTT)
 * Module démarre avec uniquement les commandes locale (bouton sans MQTT) si les 2 timeout du wifimanager sont dépassés
 */
#include <FS.h>
#include <Shutters.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <OneButton.h>


//Ticker ticker;
 /******** CONSTANTES ********/
 #define CONST_TRUE "1"
#define CONST_FALSE "0"
#define DEFAULT_COURSE_TIME 50
#define VERSION "1.0.2- 28.03.2018"



/************** Variables globales *******************/
bool DisableRazFunction = false; //Disable the raz function with the button
bool debug = true;  //Affiche sur la console si True
bool raz = false;   //Réinitialise la zone SPIFFS et WiFiManager si True
char ESP8266Client[20]  = "VR_Empty"; //Nom par défaut du module
unsigned long upCourseTime = DEFAULT_COURSE_TIME * 1000; //Valeur par défaut du temps de course en montée
unsigned long downCourseTime = DEFAULT_COURSE_TIME * 1000; //Valeur par défaut du temps de course en descente
bool isMoving = false; //Indique si les volets sont en mouvement
char mqtthost[16] = ""; //Variable qui sera utilisée par WiFiManager pour enregistrer l'adresse IP du broker MQTT
bool localModeOnly = true; //Disable the mqtt send while it is not connected
bool wificonnected = false; //Disable all communication if not connected to Wifi
bool shutterInitialized = false; //Indicate if the shutter library has been initialized
WiFiClient espClientVR_Test;  // A renommer pour chaque volets
PubSubClient client(espClientVR_Test); // A renommer pour chaque volets

Shutters shutters; //shutter librairy
WiFiManager wifiManager;
/************* Variables ESP_Volet **************/

long lastMsg = 0; //Utilisé pour le check de la connexion MQTT
int doubleLongPressStart = 0; //Utilisé pour le temps d'appui


/********************** NOMAGE MQTT*******************/
#define relais1_topic "/Relais1"
#define relais2_topic "/Relais2"
#define position_topic "/Position"
#define BtUpClick_topic "/BtUpClick"
#define BtUpLongClick_topic "/BtUpLongClick"
#define BtUpDblClick_topic "/BtUpDblClick"
#define BtDownClick_topic "/BtDownClick"
#define BtDownLongClick_topic "/BtDownLongClick"
#define BtDownDblClick_topic "/BtDownDblClick"
#define JeedomIn_topic "/in"
#define JeedomOut_topic "/out"
#define prefix_topic "Volet/"


/*************** DEFINITION DES GPIOS ********************/
//RELAIS 1
const int R1Pin = 4;
//RELAIS 2
const int R2pin = 5;
//ENTREE 1
const int In1pin = 14;
//ENTREE 2
const int In2pin = 12;
// Setup a new OneButton
OneButton button1(In1pin, false);
// Setup a new OneButton
OneButton button2(In2pin, false);


//*************** SETUP *****************************
void setup() {

  //Not Ready
  localModeOnly = true;
  wificonnected = false;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on to indicate the the module is starting


//SERIAL//
  Serial.begin(115200);
  delay(100);
  #ifdef ESP8266
  EEPROM.begin(512);
  #endif
  if (debug) {
    Serial.println();
    Serial.println("*** Starting ***");
    Serial.println(ESP8266Client);
  }

// INITIALYZE GPIO
  pinMode(R1Pin, OUTPUT);
  pinMode(R2pin, OUTPUT);

//Setup Button
  setup_button();

 //RAZ SPIFS used for debugging
if (raz){
  Serial.println("Réinitialisation de la configuration (reset SPIFFS).");
  SPIFFS.format();
 }

//Read SPIFS memory
  setup_ReadSPIFS();

//Start Wifi

  setup_WifiManager(wifiManager);

 //Start OTA
 setup_OTA();

  //Setup Volet
  setup_Volet();

//Connect MQTT
if (wificonnected) setup_mqtt();

if (debug) {
  Serial.println("Shutter Begin");
}
//Send the MQTT
if (wificonnected) mqttInit();

//device Ready
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off to indicate the the module is Ready
  //ticker.detach();
}



//***********************************************************************************

//***********************************************************************************


//***********************************************************************************
// LOOP
void loop(void){
  unsigned long currentMillis = millis();
  unsigned long now = millis();
  //Serial.println(" now :");
  //Serial.println(now);

  if (now - lastMsg > 5000) {
   /* if (debug) {
      Serial.print("localModeOnly : ");
      Serial.print(localModeOnly);
      Serial.print(" WifiConnected : ");
      Serial.println(wificonnected);
    }*/
    lastMsg = now;
    if (wificonnected && !client.connected()) {
      if (debug){Serial.println("client reconnexion");}
      reconnect();
    }
  }
  ArduinoOTA.handle();
  if (wificonnected) client.loop();
  loopLocalShutter();
}

//Local shutter management
void loopLocalShutter()
{
  if (shutterInitialized) shutters.loop();
  button1.tick();
  button2.tick();

   //Detect the longpress on both button
  if (!DisableRazFunction && button1.isLongPressed() && button2.isLongPressed())
  { if (doubleLongPressStart == 0)
  {
    doubleLongPressStart = millis();
  }
      
    if ( millis() - doubleLongPressStart > 10*1000) //More the 10 second long Press
    {
      if (debug) {Serial.println("DoubleLongpress  Raz and Restart");}
      shutters.stop();
     eraz();
    }
 }
  else
    doubleLongPressStart = 0;
}


//***********************************************************************************
// FONCTIONS MOUVEMENTS VOLET
void up(){
  if (debug) {Serial.println("Action up");}
  digitalWrite(R1Pin, 1);
  digitalWrite(R2pin, 0);
  isMoving = true;
  if (!localModeOnly)
  {
    mqttPublish(relais1_topic, CONST_TRUE);
  }
 }

void dwn(){
  if (debug) {Serial.println("Action down");}
  digitalWrite(R1Pin, 0);
  digitalWrite(R2pin, 1);
  isMoving = true;
    if (!localModeOnly)
  {
    mqttPublish(relais2_topic, CONST_TRUE);
  }
  }

void stp(){
  if (debug) {Serial.println("Action stop");}
  digitalWrite(R1Pin, 0);
  digitalWrite(R2pin, 0);
  isMoving = false;
  char level[4];
  int levelInt;
  if (!localModeOnly)
  {
      levelInt =  shutters.getCurrentLevel();
      sprintf(level, "%d", levelInt);
      mqttPublish(position_topic, level);
      mqttPublish(relais1_topic, CONST_FALSE);
      mqttPublish(relais2_topic, CONST_FALSE);

  }

  }


char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }

}
