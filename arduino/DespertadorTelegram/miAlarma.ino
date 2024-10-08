
boolean alarmaActiva = false;
boolean alarmaVibrar = false;
boolean alarmaVibrarAnteior = true;

int pinVibrador = 13;
boolean estadoVibrador = false;

int hora = 12;
int minuto = 31;
boolean pm = false;
int dias = 0b0111110;

int repeticiones = 0;
int cantidadRepeticiones = 5;

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

void actualizarFrecuencia() {
  if (repeticiones > cantidadRepeticiones) {
    float nuevaFrecuencia = random(1, 6);
    cambiarVibrador.attach(nuevaFrecuencia, actualizarVibrador);
    cambiarMelodia.attach(nuevaFrecuencia / 4, MelodiaDesarmada);
    repeticiones = 0;
    cantidadRepeticiones = random(4, 10);
    Serial.print("F: ");
    Serial.print(nuevaFrecuencia);
    Serial.print(" R: ");
    Serial.println(cantidadRepeticiones);
  }
}

void actualizarVibrador() {
  estadoVibrador = !estadoVibrador;
  if (estadoVibrador) {
    digitalWrite(pinVibrador, HIGH);
  } else {
    digitalWrite(pinVibrador, LOW);
    repeticiones++;
  }
}

void actualizarAlarma() {

  if (alarmaVibrar != alarmaVibrarAnteior) {
    if (alarmaVibrar) {
      Serial.println("Encendiendo Alarma");
      cambiarVibrador.attach(1, actualizarVibrador);
      cambiarMelodia.attach(1, MelodiaDesarmada);
      cambiarFrecuencia.attach(1, actualizarFrecuencia);
      enviarMensajeDesperta = true;
    } else {
      Serial.println("Apagando Alarma");
      cambiarVibrador.detach();
      cambiarMelodia.detach();
      cambiarFrecuencia.detach();
      recordatorioAlarma.detach();
      digitalWrite(pinVibrador, LOW);
      noTone(pinBuzzer);
    }
    alarmaVibrarAnteior = alarmaVibrar;
  }



  if (alarmaActiva && !alarmaVibrar) {
    if (tiempoActual > tiempoAlarma) {
      alarmaVibrar = true;
      escrivirArchivo("/vibrar.txt", String(alarmaVibrar));
      siquienteAlarma();
      Serial.println("Empezando a despertar a ChepeCarlos");
      TelnetStream.println("Empezando a despertar a ChepeCarlos");
      enviarMensajeDesperta = true;
      recordatorioAlarma.attach(5 * 60, mendajeRecordatorio);
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
