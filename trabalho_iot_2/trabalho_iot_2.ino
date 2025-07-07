#include <WiFi.h>
#include "esp_mac.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "Rep Tocaia_2.4";
const char* WIFI_PASSWORD = "reptocaia21a";

const char *host = "api.thingspeak.com";
const int httpPort = 80;
const String channelID = "6";

const char* mqtt_broker = "192.168.2.116";
const int mqtt_port = 1883;
const char* mqtt_user = "heliohsilva";
const char* mqtt_pass = "123";
const char* mqtt_topic = "sensor/lixeira";


const int trigPin = 7;
const int echoPin = 6;

WiFiClient espclient;

PubSubClient client(espclient);

#define SOUND_SPEED 0.034

typedef struct {
  String id;
  String status;
} Data;

Data data;

long duration;
float distanceCm;

void reconnect(){
  while (!client.connected()){
    Serial.println("Connecting to");
    Serial.println(mqtt_broker);
    if(client.connect("koikoikoi", mqtt_user, mqtt_pass)){
      Serial.println("Connected to ");
      Serial.println(mqtt_broker);
    }else{
      Serial.println("retrying to connect...");
      delay(5000);
    }
  }
}  

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

  client.setServer(mqtt_broker, mqtt_port);

  data.id = getDefaultMacAddress();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distanceCm = duration * SOUND_SPEED / 2;

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  if (distanceCm < 10){
    data.status = "full";

    sendMessage();

  }else {
    data.status = "empty";
  }

  Serial.println(data.status);
  Serial.println(data.id);
  Serial.print("\n\n");

  delay(1000);
}

void sendMessage(){

   StaticJsonDocument<100> payload;
   payload["id"] = data.id;
   payload["status"] = data.status;

   char jsonBuffer[100];
   serializeJson(payload, jsonBuffer);

   client.publish(mqtt_topic, jsonBuffer);
   delay(5000);
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