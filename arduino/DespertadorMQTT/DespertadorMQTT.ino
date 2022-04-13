#if defined(ESP32)
//Librerias para ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#elif defined(ESP8266)
//Librerias para ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#include <PubSubClient.h>

#include "token.h"

WiFiClient espClient;
PubSubClient client(espClient);

int const Motor = 14;
int const LedWifi = 12;
boolean EstadoMotor = false;
const int TiempoEsperaWifi = 5000;

void setup() {
  Serial.begin(115200);
  pinMode(Motor, OUTPUT);
  digitalWrite(Motor, LOW);
  configurarMultiWifi();
  configurarMQTT();
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();

}

void configurarMultiWifi() {
  Serial.println("\nIniciando multi Wifi");
  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid2, password2);

  WiFi.mode(WIFI_STA);
  Serial.print("Conectando a Wifi ..");
  int i = 0;
  while (wifiMulti.run(TiempoEsperaWifi) != WL_CONNECTED) {
    Serial.print(".");
    i++;
    if (i > 20) {
      i = 0;
      Serial.println();
    }
  }
  Serial.println(".. Conectado");
  Serial.print("SSID:");
  Serial.print(WiFi.SSID());
  Serial.print(" ID:");
  Serial.println(WiFi.localIP());
}

void configurarMQTT() {
  Serial.println("Configurando MQTT");
  client.setServer(brokerMQTT, puertoMQtt);
  client.setCallback(mensajeMQTT);
}

void mensajeMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconectarMQTT() {
  Serial.print("Attempting MQTT connection...");
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  int i = 0;
  while (!client.connected()) {
    if (client.connect(clientId.c_str(), usuarioMQTT, passwordMQTT )) {
      Serial.println("conectado a MQTT");
      client.publish(topicMQTT, "hello world");
      client.subscribe(topicMQTT);
    } else {
      Serial.print("*");
      i++;
      if (i  > 20) {
        i = 0;
        Serial.println();
      }
      delay(5000);
    }
  }
}
