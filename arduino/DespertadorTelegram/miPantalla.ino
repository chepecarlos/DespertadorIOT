#define Ancho_Pantalla 128
#define Alto_Pantalla 32 // 64
#define Direccion_Pantalla 0x3C
#define Reset_Pantalla -1

Adafruit_SSD1306 pantalla(Ancho_Pantalla, Alto_Pantalla, &Wire, Reset_Pantalla);

void configurarPantalla() {
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, Direccion_Pantalla)) {
    Serial.println(F("Error en pantalla OLED"));
    return;
  }

  pantalla.setTextColor(WHITE);
  pantalla.setTextSize(2);
  actualizarPantalla();
}

void actualizarPantalla() {
  
  
  pantalla.clearDisplay();
  pantalla.setTextColor(WHITE);
  pantalla.setTextSize(1);
  pantalla.setCursor(0, 0);
  pantalla.print(temperaturaActual());
  pantalla.print("c ");
  pantalla.print(diaActual());

  pantalla.setCursor(67, 0);
  pantalla.print(fechaActual());

  pantalla.fillRoundRect(0, 8, 128, 24, 5, WHITE);
  pantalla.fillRoundRect(3, 10, 122, 20, 5, BLACK);

  pantalla.setCursor(7, 13);
  pantalla.setTextColor(INVERSE);
  pantalla.setTextSize(2);
  pantalla.print(horaActual());

  pantalla.setTextSize(1);
  pantalla.print(' ');
  pantalla.print(esTarde() ? "PM" : "AM");

  pantalla.display();
}
