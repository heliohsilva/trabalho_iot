#include <WiFi.h>
#include <SimplePgSQL.h>
#include "esp_mac.h"

const char* WIFI_SSID = "Rep Tocaia_2.4";
const char* WIFI_PASSWORD = "reptocaia21a";

const char *host = "api.thingspeak.com";        // This should not be changed
const int httpPort = 80;                        // This should not be changed
const String channelID = "6";             

IPAddress PGIP(192,168,2,116);
int DB_PORT = 5432;
const char* DB_USER = "lixeiro";
const char* DB_PASS = "123";
const char* DB_NAME = "lixeira";

const int trigPin = 7;
const int echoPin = 6;

#define SOUND_SPEED 0.034

typedef struct {
  String id;
  String status;
} Data;

long duration;
float distanceCm;

WiFiClient client;

char buffer[1024];

PGconnection conn(&client, 0, 1024, buffer);

Data data;

void setup() {
  Serial.begin(115200); 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  Serial.print("Connecting to Wi-Fi...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.println(WiFi.localIP());

  conn.setDbLogin(PGIP, DB_USER, DB_PASS, DB_NAME, "utf8", DB_PORT);
  Serial.print("Connecting to PostgreSQL database...");

  while (conn.status() != CONNECTION_OK) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Database connection successful");

  data.id = getDefaultMacAddress();

}

void loop() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  if (distanceCm < 10){
    data.status = "full";
  }else {
    data.status = "empty";
  }

  Serial.print(data.status);
  Serial.print(data.id);

  delay(1000);
}

void sendToDatabase(String message) {
  
}

String getDefaultMacAddress() {

  String mac = "";

  unsigned char mac_base[6] = {0};

  if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }

  return mac;
}