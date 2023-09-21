#include "RTClib.h"

RTC_DS1307 rtc;

String NombresDia[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };

void configurarRTC() {
  if (!rtc.begin()) {
    Serial.println("No encontrado RTC");
    estado = errorEstado;
  }
   Serial.println("RTC Cargado");
}
