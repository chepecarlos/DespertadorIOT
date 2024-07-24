
void MultiCore(void* pvParameters) {

  inicializarAlarma();
  configurarRTC();
  configurarPantalla();

  while (true) {
    actualizarRTC();
    escribirRTC();
    actualizarPantalla();
    actualizarAlarma();
    vTaskDelay(250 / portTICK_RATE_MS);
  }
}
