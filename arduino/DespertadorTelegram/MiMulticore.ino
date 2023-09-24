
void MultiCore( void * pvParameters ) {

  configurarRTC();
  configurarPantalla();

  while (true) {
    actualizarRTC();
    actualizarPantalla();
    delay(255);
  }
}
