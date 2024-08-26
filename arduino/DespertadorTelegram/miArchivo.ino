void escrivirArchivo(const char *ruta, String texto) {
  char temporalChar[10];
  texto.toCharArray(temporalChar, 10);
  escrivirArchivo(ruta, temporalChar);
}

void escrivirArchivo(const char *ruta, const char *texto) {
  // Serial << "Escribiendo en archivo: " << ruta << "\n";

  LittleFS.begin();
  File archivo = LittleFS.open(ruta, "w");
  if (!archivo) {
    Serial.println("Archivo no se puede abir");
    return;
  }

  if (archivo.print(texto)) {
    Serial.println("Archivo Escribiendo");
  } else {
    Serial.println("Error Escribiendo");
  }
  archivo.close();
  LittleFS.end();
}


uint32_t leerArchivo(const char *ruta) {

  LittleFS.begin();
  File archivo = LittleFS.open(ruta, "r");
  if (!archivo) {
    Serial.println("Archivo no se puede abir");
    return -1;
  }

  String Mensaje;
  while (archivo.available()) {
    Mensaje = archivo.readString();
  }
  archivo.close();
  LittleFS.end();

  return Mensaje.toInt();
}


void escrivirArchivo(int id, const char *texto) {
  char direccion[30] = "/";
  char numero[10];
  itoa(id, numero, 10);
  strcat(direccion, numero);
  strcat(direccion, ".txt");
  TelnetStream.print("Escribiendo en archivo: ");
  TelnetStream.println(direccion);
  LittleFS.begin();
  File archivo = LittleFS.open(direccion, "w");
  if (!archivo) {
    TelnetStream.println("Archivo no se puede abir");
    return;
  }

  if (archivo.print(texto)) {
    Serial.println("Archivo Escribiendo");
  } else {
    TelnetStream.println("Error Escribiendo");
  }
  archivo.close();
  LittleFS.end();
}

String leerArchivo(int id) {
  char direccion[30] = "/";
  char numero[10];
  itoa(id, numero, 10);
  strcat(direccion, numero);
  strcat(direccion, ".txt");
  // Serial << "Leer en archivo: " << direccion << "\n";
  LittleFS.begin();
  File archivo = LittleFS.open(direccion, "r");
  if (!archivo) {
    Serial.println("Archivo no se puede abir");
    return "";
  }

  Serial.print("Contiene: ");
  String Mensaje;
  while (archivo.available()) {
    Mensaje = archivo.readString();
  }
  archivo.close();
  LittleFS.end();
  return Mensaje;
}

void cargarEstado() {
  //Todo: Estado actual
}

boolean formatiarMemoria() {

#if defined(ESP32)
  if (!LittleFS.begin(true)) {
    Serial.println("Error formatiando");
    return false;
  }
#endif

  if (!LittleFS.format()) {
    Serial.println("Error formatiando");
    return false;
  } else {
    Serial.println("Se boro todo");
  }

  Serial.println("Listo de LittleFS");

  LittleFS.end();

  return true;
}
