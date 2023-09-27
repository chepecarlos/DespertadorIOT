
boolean alarmaActiva = false;

boolean alarmaVibrar = false;
boolean alarmaVibrarAnteior = true;

int pinVibrador = 13;
boolean estadoVibrador = false;

int hora = 12;
int minuto = 31;
boolean pm = false;
int dias = 0b0111110;

int ultimaAlarma = -1;

void inicializarAlarma() {
  pinMode(pinVibrador, OUTPUT);
  digitalWrite(pinVibrador, LOW);

  hora = leerArchivo("/hora.txt");
  minuto = leerArchivo("/minuto.txt");
  dias = leerArchivo("/dia.txt");
  pm = leerArchivo("/pm.txt");
  alarmaActiva = leerArchivo("/alarma.txt");
  alarmaVibrar = leerArchivo("/vibrar.txt");
  ultimaAlarma = leerArchivo("/ultima_alarma.txt");

  Serial << "Alarma: " << hora << ":" << (minuto < 10 ? "0" : "") << minuto << " " << (pm ? "PM" : "AM") << "\n";

  Serial <<  "Dias: ";
  for (int dia = 0; dia < 7; dia++) {
    Serial << (pedirDia(dias, dia) ? NombresDia[dia] : "") << " ";
  }
  Serial << "\n";
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
    } else {
      cambiarVibrador.detach();
      cambiarMelodia.detach();
      digitalWrite(pinVibrador, LOW);
      noTone(pinBuzzer);
    }
    alarmaVibrarAnteior = alarmaVibrar;
  }

  if (alarmaActiva) {
    // mejora usar diferencial

    if (ultimaAlarma == obtenerDia()) return;
    if (!pedirDia(dias, diaSemana())) return;
    if (pm != esTarde()) return;
    if (hora != obtenerHora()) return;
    if (minuto != obtenerMinuto()) return;

    Serial.println("Empezando a despertar a ChepeCarlos");
    enviarMensaje("Empezando a despertar a ChepeCarlos");

    alarmaVibrar = true;
    ultimaAlarma = obtenerDia();
    
    char pollo[10];
    String pollo_tmp = String(ultimaAlarma);
    pollo_tmp.toCharArray(pollo, 10);
    escrivirArchivo("/ultima_alarma.txt", pollo);
    pollo_tmp = String(alarmaVibrar);
    pollo_tmp.toCharArray(pollo, 10);
    escrivirArchivo("/vibrar.txt", pollo);
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
