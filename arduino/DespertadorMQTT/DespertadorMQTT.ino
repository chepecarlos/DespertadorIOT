#if defined(ESP32)
//Librerias para ESP32
#define EEPROM_SIZE 64
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#elif defined(ESP8266)
//Librerias para ESP8266
#define EEPROM_SIZE 12
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#include <EEPROM.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <PubSubClient.h>

#include "token.h"

WiFiClient espClient;
PubSubClient client(espClient);

int const Motor = 14;
int const LedWifi = 12;
boolean EstadoMotor = false;
const int TiempoEsperaWifi = 5000;

char tablaDias[7][12] = {"Sabado", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
const long diferenciaTiempo = -21600;// -6 UTC
WiFiUDP ntpUDP;
NTPClient clienteTiempo(ntpUDP, "pool.ntp.org", diferenciaTiempo);

int ultimoMinuto = -1;
int unsigned despertarHora = 0;
int unsigned despertarMinuto = 0;
#define direccionHora 0
#define direccionMinuto 1

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Despertador ChepeCarlos");

  pinMode(Motor, OUTPUT);
  digitalWrite(Motor, LOW);

  EEPROM.begin(EEPROM_SIZE);
  despertarHora = EEPROM.read(direccionHora);
  despertarHora = constrain(despertarHora, 0, 24);
  despertarMinuto = EEPROM.read(direccionMinuto);
  despertarMinuto = constrain(despertarMinuto, 0, 60);

  configurarMultiWifi();
  configurarMQTT();
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();

  actualizarTiempo();

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


  clienteTiempo.begin();
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

void actualizarTiempo() {
  clienteTiempo.update();
  if (ultimoMinuto != clienteTiempo.getMinutes()) {
    ultimoMinuto = clienteTiempo.getMinutes();
    Serial.printf("Tiempo: %s ", tablaDias[clienteTiempo.getDay()]);
    imprimirTiempo(clienteTiempo.getHours(), clienteTiempo.getMinutes());
    Serial.print(" despertar: ");
    imprimirTiempo(despertarHora, despertarMinuto);
    Serial.println();
  }
}

void imprimirTiempo(int hora, int minuto) {
  boolean pm = false;
  if (hora > 12) {
    hora -= 12;
    pm = true;
  }
  Serial.printf("%d:%d %s", hora, minuto, pm ? "pm" : "am");
}
