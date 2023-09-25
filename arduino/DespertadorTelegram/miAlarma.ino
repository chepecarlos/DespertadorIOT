
boolean alarmaArmada = false;
boolean alarmaVibrar = false;
int pinVibrador = 13;

int hora = 1;
int minuto = 1;
boolean pm = false;
int dia = 1;

void inicializarAlarma() {
  pinMode(pinVibrador, OUTPUT);
  digitalWrite(pinVibrador, LOW);
}

void actualizarAlarma() {
  if (alarmaArmada) {

  }

  if (alarmaVibrar) {
    digitalWrite(pinVibrador, HIGH);
  } else {
    digitalWrite(pinVibrador, LOW);
  }

}

void cambiarVibrar(boolean estado) {
  alarmaVibrar = estado;
}
