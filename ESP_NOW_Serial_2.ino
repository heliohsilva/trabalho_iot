/*
 ESP-NOW Serial Example - Unicast transmission
 Lucas Saavedra Vaz - 2024
 Send data between two ESP32s using the ESP-NOW protocol in one-to-one (unicast) configuration.
 Note that different MAC addresses are used for different interfaces.
 The devices can be in different modes (AP or Station) and still communicate using ESP-NOW.
 The only requirement is that the devices are on the same Wi-Fi channel.
 Set the peer MAC address according to the device that will receive the data.
 Example setup:
 - Device 1: AP mode with MAC address F6:12:FA:42:B6:E8
 Peer MAC address set to the Station MAC address of Device 2 (F4:12:FA:40:64:4C)
 - Device 2: Station mode with MAC address F4:12:FA:40:64:4C
 Peer MAC address set to the AP MAC address of Device 1 (F6:12:FA:42:B6:E8)
 The device running this sketch will also receive and print data from any device that has its MAC address set as the peer MAC address.
 To properly visualize the data being sent, set the line ending in the Serial Monitor to "Both NL & CR".
*/
#include "ESP32_NOW_Serial.h"
#include "MacAddress.h"
#include "WiFi.h"
#include "esp_wifi.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

// 0: AP mode, 1: Station mode
#define ESPNOW_WIFI_MODE_STATION 1

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 1

#if ESPNOW_WIFI_MODE_STATION // ESP-NOW using WiFi Station mode
#define ESPNOW_WIFI_MODE WIFI_STA // WiFi Mode
#define ESPNOW_WIFI_IF WIFI_IF_STA // WiFi Interface
#else // ESP-NOW using WiFi AP mode
#define ESPNOW_WIFI_MODE WIFI_AP // WiFi Mode
#define ESPNOW_WIFI_IF WIFI_IF_AP // WiFi Interface
#endif

#define LED_PIN 7


static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); 

// Set the MAC address of the device that will receive the data
// For example: F4:12:FA:40:64:4C
const MacAddress peer_mac({0xB0, 0x81, 0x84, 0x9A, 0xD0, 0x10});

ESP_NOW_Serial_Class NowSerial(peer_mac, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IF);
String bufferRecebido = ""; // Para montar valor do potenciômetro recebido
int valorPWM = 0;

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
  
  // Start the ESP-NOW communication
  Serial.println("ESP-NOW communication starting...");
  NowSerial.begin(115200);
  Serial.println("You can now send data to the peer device using the Serial Monitor.\n");
  
  pinMode(LED_PIN, OUTPUT);

  VextON();
  delay(100);

  display.init();
  display.screenRotate(ANGLE_270_DEGREE);
}

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void loop() {
  static String linhasRecebidas[8];
  static int linhaAtual = 0;

  // Ler dados da serial
  while (NowSerial.available()) {
    
    char c = (char)NowSerial.read();

    if (c == '\n') {
      linhasRecebidas[linhaAtual] = bufferRecebido;
      bufferRecebido = "";
      linhaAtual++;

      if (linhaAtual == 8) {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "Recebido:");

        // Valor do potenciômetro
        int valorPot = linhasRecebidas[0].toInt();
        int valorPWM = map(valorPot, 0, 4095, 0, 255);
        
        // Bit verificador (deve ser '1' ou '0')
        char bitVerificador = linhasRecebidas[1].charAt(0);

        // Acelerômetro
        float accX = linhasRecebidas[2].toFloat();
        float accY = linhasRecebidas[3].toFloat();
        float accZ = linhasRecebidas[4].toFloat();

        // Giroscópio
        float gyroX = linhasRecebidas[5].toFloat();
        float gyroY = linhasRecebidas[6].toFloat();
        float gyroZ = linhasRecebidas[7].toFloat();

        // Exibir valor do potenciômetro
        display.setFont(ArialMT_Plain_24);
        display.drawString(0, 20, linhasRecebidas[0]);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 50, "gyro: ");
        display.drawString(0, 70, "x: " + linhasRecebidas[5]);
        display.drawString(0, 80, "y: " + linhasRecebidas[6]);
        display.drawString(0, 90, "z: " + linhasRecebidas[7]);
        display.display();

        // Aplicar PWM conforme bit verificador
        if (bitVerificador == '1') {
          analogWrite(LED_PIN, valorPWM);
          Serial.println("bit verificador: 1");
        } else {
          analogWrite(LED_PIN, 0);
          Serial.println("bit verificador: 0");
        }

        // Debug serial
        Serial.print("Valor potenciômetro: "); Serial.println(valorPot);
        Serial.print("PWM: "); Serial.println(valorPWM);
        Serial.println("Acelerômetro:");
        Serial.print("X: "); Serial.println(accX);
        Serial.print("Y: "); Serial.println(accY);
        Serial.print("Z: "); Serial.println(accZ);
        Serial.println("Giroscópio:");
        Serial.print("X: "); Serial.println(gyroX);
        Serial.print("Y: "); Serial.println(gyroY);
        Serial.print("Z: "); Serial.println(gyroZ);

        // Limpar dados para próxima recepção
        linhaAtual = 0;
      }
    } else {
      bufferRecebido += c;
    }
  }

  delay(1);
}
