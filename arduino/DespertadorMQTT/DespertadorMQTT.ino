template<class T> inline Print &operator <<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}
#define endl '\n'

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

struct miTiempo {
  int hora;
  int minuto;
};

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
miTiempo despertar = {0, 0};
miTiempo diferencia = {0, 0};
int unsigned despertarDuracion = 0;
boolean vibrarAlarma = false;
boolean activaAlarma = false;

#define apagar 0
#define encender 1
#define cambiar 2

#define direccionHora 0
#define direccionMinuto 1
#define direccionDuracion 2
#define direccionArmado 3

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Despertador ChepeCarlos");

  pinMode(Motor, OUTPUT);
  digitalWrite(Motor, LOW);

  EEPROM.begin(EEPROM_SIZE);
  despertar.hora = EEPROM.read(direccionHora);
  despertar.hora = constrain(despertar.hora, 0, 23);
  despertar.minuto = EEPROM.read(direccionMinuto);
  despertar.minuto = constrain(despertar.minuto, 0, 59);
  despertarDuracion = EEPROM.read(direccionDuracion);
  activaAlarma = EEPROM.read(direccionArmado);

  configurarMultiWifi();
  configurarMQTT();
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();

  actualizarTiempo();

  actualizarAlarma();

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
  Serial.print("Mensaje [");
  Serial.print(topic);
  Serial.print("] ");
  char mensaje[length + 1];
  for (int i = 0; i < length; i++) {
    mensaje[i] = payload[i];
  }
  Serial.println(mensaje);

  if (strcmp(topic, "alsw/despertador/hora") == 0) {
    despertar.hora  = atoi(mensaje);
    despertar.hora = constrain(despertar.hora, 0, 23);
    EEPROM.put(direccionHora, despertar.hora);
    EEPROM.commit();
    Serial << "Cambiando tiempo: " << despertar.hora << endl;
  } else  if (strcmp(topic, "alsw/despertador/minuto") == 0) {
    despertar.minuto = atoi(mensaje);
    despertar.minuto = constrain(despertar.minuto, 0, 59);
    EEPROM.put(direccionMinuto, despertar.minuto);
    EEPROM.commit();
    Serial << "Cambiando tiempo: " << despertar.minuto << endl;
  } else if (strcmp(topic, "alsw/despertador/duracion") == 0) {
    despertarDuracion  = atoi(mensaje);
    EEPROM.put(direccionDuracion, despertarDuracion);
    EEPROM.commit();
    Serial << "Cambiando duracion: " << despertarDuracion << endl;
  } else if (strcmp(topic, "alsw/despertador/alarma") == 0) {
    int cambio = atoi(mensaje);
    if (cambio == apagar) {
      activaAlarma = false;
    } else if (cambio == encender) {
      activaAlarma = true;
    } else if (cambio == cambiar) {
      activaAlarma = !activaAlarma;
    }
    EEPROM.put(direccionArmado, activaAlarma);
    EEPROM.commit();
    Serial << "Alarma: " << (activaAlarma ? "activa" : "apagada") << endl;
  }
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
    Serial << "Tiempo: " << tablaDias[clienteTiempo.getDay()] << " ";
    imprimirTiempo(clienteTiempo.getHours(), clienteTiempo.getMinutes());
    if (activaAlarma) {
      Serial << " despertar: ";
      imprimirTiempo(despertar.hora, despertar.minuto);
      Serial << " duracion: " << despertarDuracion << endl;
      actualizarFalta();
      actualizarAlarma();
    } else {
      Serial << " alarma: Apagada" << endl;
    }
  }
}

void imprimirTiempo(int hora, int minuto) {
  boolean pm = false;
  if (hora > 12) {
    hora -= 12;
    pm = true;
  }

  Serial << hora << ":" << (minuto < 10 ? "0" : "") << minuto << " " << (pm ? "pm" : "am");
}

void actualizarFalta() {
  int actual  = clienteTiempo.getHours() * 60 + clienteTiempo.getMinutes();
  int tiempoDespertar = despertar.hora * 60 + despertar.minuto;
  int diferenciaMinutos = 0;
  if (tiempoDespertar > actual) {
    diferenciaMinutos = tiempoDespertar - actual;
  } else if (tiempoDespertar < actual) {
    diferenciaMinutos = 24 * 60 - actual + tiempoDespertar;
  }
  diferencia.minuto = diferenciaMinutos % 60;
  diferencia.hora = (diferenciaMinutos - diferencia.minuto) / 60;
  Serial << "Falta: " << diferencia.hora << ":" << (diferencia.minuto < 10 ? "0" : "") << diferencia.minuto << endl;
}

void actualizarAlarma() {
  if ( clienteTiempo.getHours() == despertar.hora && clienteTiempo.getMinutes() == despertar.minuto) {
    Serial.println("Encender Rele");
  }
}
