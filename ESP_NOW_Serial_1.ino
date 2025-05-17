#include "ESP32_NOW_Serial.h"
#include "MacAddress.h"
#include "WiFi.h"
#include "esp_wifi.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// 0: AP mode, 1: Station mode
#define ESPNOW_WIFI_MODE_STATION 1

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 1

#if ESPNOW_WIFI_MODE_STATION          // ESP-NOW using WiFi Station mode
#define ESPNOW_WIFI_MODE WIFI_STA     // WiFi Mode
#define ESPNOW_WIFI_IF   WIFI_IF_STA  // WiFi Interface
#else                                 // ESP-NOW using WiFi AP mode
#define ESPNOW_WIFI_MODE WIFI_AP      // WiFi Mode
#define ESPNOW_WIFI_IF   WIFI_IF_AP   // WiFi Interface
#endif

#define PINO_POT 7
#define BOTAO1_PIN 2
#define BOTAO2_PIN 3

static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); 

// Set the MAC address of the device that will receive the data
const MacAddress peer_mac({0x34, 0xCD, 0xB0, 0x3D, 0xB6, 0xF8});

ESP_NOW_Serial_Class NowSerial(peer_mac, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IF);

Adafruit_MPU6050 mpu;

TwoWire i2cmpu = TwoWire(1);

void init_oled()
{
  VextON();
  delay(100);

  display.init();
  display.screenRotate(ANGLE_270_DEGREE);
}

void init_gyro()
{
  i2cmpu.begin(4, 5, 400000);

  if (!mpu.begin(0x68, &i2cmpu))
   Serial.println("Erro ao iniciar MPU6050");


  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Variação do aceleremetro para: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Variação do Giroscópio para: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  delay(100);
}

void setup() {
  Serial.begin(115200);

  Serial.print("WiFi Mode: ");
  Serial.println(ESPNOW_WIFI_MODE == WIFI_AP ? "AP" : "Station");
  WiFi.mode(ESPNOW_WIFI_MODE);

  Serial.print("Channel: ");
  Serial.println(ESPNOW_WIFI_CHANNEL);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  while (!(WiFi.STA.started() || WiFi.AP.started())) {
    delay(100);
  }

  Serial.print("MAC Address: ");
  Serial.println(ESPNOW_WIFI_MODE == WIFI_AP ? WiFi.softAPmacAddress() : WiFi.macAddress());

  // Start ESP-NOW
  Serial.println("ESP-NOW communication starting...");
  NowSerial.begin(115200);
  Serial.println("Sending potentiometer data...\n");

  pinMode(BOTAO1_PIN, INPUT_PULLUP);
  pinMode(BOTAO2_PIN, INPUT_PULLUP);

  init_oled();
  init_gyro();
 
}

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void loop() {
  // Read potentiometer
  int valorPot = analogRead(PINO_POT);
  int valorPWM = map(valorPot, 0, 4095, 0, 255);

  int estadoBotao1 = digitalRead(BOTAO1_PIN);
  int estadoBotao2 = digitalRead(BOTAO2_PIN);

  String potString = String(valorPot);
  String bitLed = String(1);

  sensors_event_t a, g, temp;
  
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 20, potString);
  
  if (estadoBotao1 == LOW) {
    NowSerial.println(potString);
    if (estadoBotao2 == LOW){
      NowSerial.println(1);
    }else{
      NowSerial.println(0);
    }    
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Enviando...");

    mpu.getEvent(&a, &g, &temp);
    //Imprime os valores do acelerometro no monitor serial
    NowSerial.println(a.acceleration.x);
    NowSerial.println(a.acceleration.y);
    NowSerial.println(a.acceleration.z);

    NowSerial.println(g.gyro.x);
    NowSerial.println(g.gyro.y);
    NowSerial.println(g.gyro.z);
    
    Serial.println(potString); 
  }

  display.display();
  delay(200);
  
}
  

  
