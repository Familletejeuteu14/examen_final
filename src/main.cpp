/*
  Titre      : Projet Final
  Auteur     : MAXIM TESSA 
  Date       : 09/04/2023
  Description: Un CAPTEUR BME280 qui affiche les données sur une page web   
  Version    : 0.0.1
*/

// Import required libraries
#include <Arduino.h>
#include "WiFi.h"
#include <AsyncElegantOTA.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ArduinoJson.h> // bibliothèque ArduinoJson pour créer un objet JSON pour stocker les mesures du capteur BME280.
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Replace with your network credentials
const char* ssid = "BELL083";
const char* password = "59CC461CDC56";


#define SEALEVELPRESSURE_HPA (1013.25)
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
Adafruit_BME280 bme;


 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  Wire.begin();
  bme.begin(0x76); // Adresse I2C du capteur

  // Initialiser le capteur BME280
  if(!bme.begin(0x76)){
    Serial.println("Impossible de trouver le capteur BME280, vérifiez le câblage !");
    while(1);
  }

  // Initialisation SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connection au Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  

  // pourv servir le fichier HTML ET CSS depuis le système de fichiers SPIFFS ...
      //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

   // Demarre ElegantOTA
  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  // initialisez le serveur
  server.begin();
}
 
void loop(){

   // Lire les mesures du capteur
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // Créer un objet JSON pour stocker les mesures
  StaticJsonDocument<200> json;
  json["temperature"] = temperature;
  json["pressure"] = pressure;
  json["humidity"] = humidity;
  json["altitude"] = altitude;

  // Convertir l'objet JSON en chaîne de caractères
  String jsonString;
  serializeJson(json, jsonString);

  // Écrire la chaîne de caractères dans le fichier mesures.json
  File file = SPIFFS.open("/mesures.json", FILE_WRITE);
  if (!file) {
    Serial.println("Erreur lors de l'ouverture du fichier");
    return;
  }
  file.print(jsonString);
  file.close();

  delay(3000); // Attendre 3 secondes avant la prochaine lecture des mesures
}
  

