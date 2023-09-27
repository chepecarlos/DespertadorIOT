template<class T> inline Print &operator<<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}

#if defined(ESP32)
//Librerias para ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
WiFiMulti wifiMulti;

#elif defined(ESP8266)
//Librerias para ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
ESP8266WiFiMulti wifiMulti;
#endif


#include "token.h"

#include <Ticker.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TelnetStream.h>
#include "LittleFS.h"

String NombresDia[7] = { "Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab" };

Ticker cambiarLed;

#define noWifi 0
#define noBot 1
#define conectado 2
#define errorEstado 3

int ledEstado = 4;
boolean EstadoLed = false;
int estado = noWifi;
int estadoAnterior = -1;

int botonApagado = 27;
int buzzer = 4;

void funcionLed() {
  EstadoLed = !EstadoLed;
  digitalWrite(ledEstado, EstadoLed ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando Despertador De ChepeCarlos");
  Serial.println();

  pinMode(ledEstado, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(botonApagado, INPUT);

  //Activando codigo a cargarse en procesador 0
  //Procesador 1 Exclusico para Wifi
  //Procesador 0 Actualizar pantalla
  xTaskCreatePinnedToCore(
    MultiCore,   /* Nombre de la funcion */
    "MultiCore", /* Nombre del proceso  */
    10000,      /* Tamano de palabra */
    NULL,       /* parametros de entrada */
    0,          /* Prioridas del proceso */
    NULL,       /* Manejo del proceso  */
    0);  /* Procesador a poner la operacion */
  delay(100);

  actualizarEstado();
  conectarWifi();
  Melodia();
}

void loop() {
  actualizarWifi();
  actualizarBot();
  actualizarEstado();
}