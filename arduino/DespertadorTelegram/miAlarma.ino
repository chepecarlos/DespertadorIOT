
boolean alarmaArmada = false;
boolean alarmaVibrar = false;
int pinVibrador = 13;

int hora = 1;
int minuto = 1;
boolean pm = false;
int dias = 0b0111110;

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


boolean pedirDia(int numero, int id) {
  id = 6 - id;

  if (numero & (1 << id)) {
    return true;
  }
  else {
    return false;
  }
}
