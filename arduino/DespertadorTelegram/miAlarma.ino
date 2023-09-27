
boolean alarmaArmada = false;
boolean alarmaVibrar = false;
int pinVibrador = 13;

int hora = 12;
int minuto = 31;
boolean pm = false;
int dias = 0b0111110;

void inicializarAlarma() {
  pinMode(pinVibrador, OUTPUT);
  digitalWrite(pinVibrador, LOW);

  hora = leerArchivo("/hora.txt");
  minuto = leerArchivo("/minuto.txt");
  dias = leerArchivo("/dia.txt");
  pm = leerArchivo("/pm.txt");
  alarmaArmada = leerArchivo("/armado.txt");
  alarmaVibrar = leerArchivo("/vibrar.txt");

  Serial << "Alarma: " << hora << ":" << (minuto < 10 ? "0" : "") << minuto << " " << (pm ? "PM" : "AM") << "\n";

  Serial <<  "Dias: ";
  for (int dia = 0; dia < 7; dia++) {
    Serial << (pedirDia(dias, dia) ? NombresDia[dia] : "") << " ";
  }
  Serial << "\n";
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
