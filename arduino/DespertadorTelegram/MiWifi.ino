
void conectarWifi() {

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  Serial.println("Conectando con Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi Conectado");
    Serial.println("Mi IP es: ");
    Serial.println(WiFi.localIP());
    estado = noBot;
  }

  MDNS.begin(NombreESP);
  configurarOTA();

  MDNS.addService("telnet", "tcp", 23);
  TelnetStream.begin();

  configurarBot();
}


void actualizarWifi() {

  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi No Conectada!");
    delay(500);
    estado = noWifi;
    return;
  } else {
    estado = noBot;
  }

#if defined(ESP8266)
  MDNS.update();
#endif
  ArduinoOTA.handle();
  delay(10);

}


void LeerTelnet() {
  if (TelnetStream.available()) {
    char Letra = TelnetStream.read();
    switch (Letra) {
      case 'R':
        TelnetStream.stop();
        delay(100);
        ESP.restart();
        break;
      case 'e':
      case 'E':
        // Todo: estado del sistema
        break;
      case 'f':
      case 'F':
        if (!LittleFS.format()) {
          TelnetStream.println("Error formatiando");
          return;
        } else {
          TelnetStream.println("Se boro todo");
        }
        break;
    }
  }
}
