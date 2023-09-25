
void MultiCore( void * pvParameters ) {

  inicializarAlarma();
  configurarRTC();
  configurarPantalla();

  while (true) {
    actualizarRTC();
    actualizarPantalla();
    actualizarAlarma();
    delay(255);
  }
}
