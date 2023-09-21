

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
  Serial << "Leer en archivo: " << direccion << "\n";
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
