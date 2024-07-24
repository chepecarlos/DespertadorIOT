
boolean alarmaActiva = false;
boolean alarmaVibrar = false;
boolean alarmaVibrarAnteior = true;

int pinVibrador = 13;
boolean estadoVibrador = false;

int hora = 12;
int minuto = 31;
boolean pm = false;
int dias = 0b0111110;

void siquienteAlarma() {
  if (dias == 0) return;

  tiempoAlarma = DateTime(tiempoActual.year(), tiempoActual.month(), tiempoActual.day(), hora, minuto);

  if (!pm && hora == 12) {
    tiempoAlarma = tiempoAlarma - TimeSpan(0, 12, 0, 0);
  } else if (pm) {
    tiempoAlarma = tiempoAlarma + TimeSpan(0, 12, 0, 0);
  }

  while (tiempoAlarma < tiempoActual || !pedirDia(dias, tiempoAlarma.dayOfTheWeek())) {
    tiempoAlarma = tiempoAlarma + TimeSpan(1, 0, 0, 0);
  }

  char pollo[20];
  String pollo_tmp = String(tiempoAlarma.unixtime());
  pollo_tmp.toCharArray(pollo, 20);
  escrivirArchivo("/tiempoalarma.txt", pollo);

  // Serial << tiempoAlarma.unixtime() << " " << pollo_tmp << " " << pollo << "\n";
  // TelnetStream << tiempoAlarma.unixtime() << " " << pollo_tmp << " " << pollo << "\n";
}

void inicializarAlarma() {
  pinMode(pinVibrador, OUTPUT);
  digitalWrite(pinVibrador, LOW);

  hora = leerArchivo("/hora.txt");
  minuto = leerArchivo("/minuto.txt");
  dias = leerArchivo("/dia.txt");
  pm = leerArchivo("/pm.txt");
  alarmaActiva = leerArchivo("/alarma.txt");
  alarmaVibrar = leerArchivo("/vibrar.txt");
  alarmaVibrarAnteior = !alarmaVibrar;
  tiempoAlarma = DateTime(leerArchivo("/tiempoalarma.txt"));

  // // Serial << "Alarma: " << hora << ":" << (minuto < 10 ? "0" : "") << minuto << " " << (pm ? "PM" : "AM") << "\n";
  // // TelnetStream << "Alarma: " << hora << ":" << (minuto < 10 ? "0" : "") << minuto << " " << (pm ? "PM" : "AM") << "\n";

  // // Serial << "Dias: ";
  // // TelnetStream << "Dias: ";
  // for (int dia = 0; dia < 7; dia++) {
  //   Serial << (pedirDia(dias, dia) ? NombresDia[dia] : "") << " ";
  // }
  // Serial << "\n";
  // TelnetStream << "\n";

  // Serial << "Unix" << tiempoAlarma.unixtime() << "\n";
  // TelnetStream << "Unix" << tiempoAlarma.unixtime() << "\n";
}

void actualizarVibrador() {
  estadoVibrador = !estadoVibrador;
  if (estadoVibrador) {
    digitalWrite(pinVibrador, HIGH);
  } else {
    digitalWrite(pinVibrador, LOW);
  }
}

void actualizarAlarma() {

  if (alarmaVibrar != alarmaVibrarAnteior) {
    if (alarmaVibrar) {
      cambiarVibrador.attach(1.5, actualizarVibrador);
      cambiarMelodia.attach(0.25 , MelodiaDesarmada);
      enviarMensajeDesperta = true;
    } else {
      cambiarVibrador.detach();
      cambiarMelodia.detach();
      digitalWrite(pinVibrador, LOW);
      noTone(pinBuzzer);
    }
    alarmaVibrarAnteior = alarmaVibrar;
  }

  if (alarmaActiva && !alarmaVibrar) {
    if (tiempoActual > tiempoAlarma) {
      alarmaVibrar = true;
      char pollo[10];
      String pollo_tmp = String(alarmaVibrar);
      pollo_tmp.toCharArray(pollo, 10);
      escrivirArchivo("/vibrar.txt", pollo);
      siquienteAlarma();

      Serial.println("Empezando a despertar a ChepeCarlos");
      TelnetStream.println("Empezando a despertar a ChepeCarlos");
      enviarMensajeDesperta = true;
    }
  }
}

void cambiarVibrar(boolean estado) {
  alarmaVibrar = estado;
}


boolean pedirDia(int numero, int id) {
  id = 6 - id;

  if (numero & (1 << id)) {
    return true;
  } else {
    return false;
  }
}
