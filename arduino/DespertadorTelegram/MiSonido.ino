
#include "tonos.h"

int pinBuzzer = 12;
bool primeraMelodia = true;

// TONOs in the melody:
int melodia[8] = {
  TONO_C4, TONO_G3, TONO_G3, TONO_A3, TONO_G3, 0, TONO_B3, TONO_C4
};

// TONO durations: 4 = quarter TONO, 8 = eighth TONO, etc.:
int duraccion[8] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void Melodia() {
  primeraMelodia = true;
  for (int nota = 0; nota < 8; nota++) {
    int notaDuraccion = 1000 / duraccion[nota];
    tone(pinBuzzer, melodia[nota], notaDuraccion);

    int pausaNota = notaDuraccion * 1.30;
    delay(pausaNota);
    noTone(pinBuzzer);
  }
  primeraMelodia = false;
}

int NotaActual = 0;

void MelodiaDesarmada() {
  if (!primeraMelodia) {
    noTone(pinBuzzer);
    tone(pinBuzzer, melodia[NotaActual]);
    NotaActual++;
    if (NotaActual >= 8) {
      NotaActual = 0;
    }
  }
}
