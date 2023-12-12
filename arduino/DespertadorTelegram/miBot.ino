#include "CTBot.h"
#include "Utilities.h"

CTBot miBot;
boolean bienbenidaBot = false;

void configurarBot() {
  miBot.setTelegramToken(token);
}

boolean enviarMensaje(String mensaje) {
  if (estado != conectado) {
    return false;
  }

  for (int i = 0; i < cantidadChats; i++) {
    miBot.sendMessage(IDchat[i], mensaje);
  }
  Serial.println("Mensaje Enviado");
  return true;
}

void mensajeBienbenidaBot() {
  if (!bienbenidaBot) {
    bienbenidaBot = true;
    Serial << "En Linea, Sistema: " << NombreESP << "\n";
    for (int i = 0; i < cantidadChats; i++) {
      miBot.sendMessage(IDchat[i], "En Linea, Sistema: " + String(NombreESP));
    }
  }

  if (estado < conectado) {
    bienbenidaBot = false;
  }
}

void actualizarBot() {
  if (estado <= noBot) {
    if (miBot.testConnection()) {
      estado = conectado;
    } else {
      return;
    }
  }

  mensajeBot();
}

void mensajeBot() {
  if (estado != conectado) {
    return;
  }

  mensajeBienbenidaBot();

  TBMessage msg;

  if (miBot.getNewMessage(msg)) {

    Serial.println();
    TelnetStream.println();
    Serial << "Mensaje: " << msg.text << " de " << msg.sender.username << " ID:" << msg.sender.id << "\n";
    TelnetStream << "Mensaje: " << msg.text << " de " << msg.sender.username << " ID:" << msg.sender.id << "\n";
    for (int i = 0; i < cantidadChats; i++) {
      if (msg.sender.id == IDchat[i]) {

        if (msg.text.equalsIgnoreCase("/opciones")) {
          Serial.println("Enviando opciones");
          TelnetStream.println("Enviando opciones");
          String mensaje = "Opciones disponibles:\n";
          mensaje += "/estado estado del sistema\n";
          mensaje += "/vibrar enciende vibrador\n";
          mensaje += "/novibrar apaga vibrador\n";
          mensaje += "/alarma enciende alarma\n";
          mensaje += "/noalarma apaga alarma\n";
          mensaje += "/hora +[hora] configura hora de la alarma\n";
          mensaje += "/dia +[dia](lun mar ... dom) Condigura dia de la alarma\n";
          mensaje += "/formatiar borra memoria interna\n";
          mensaje += "/opciones comandos disponibles\n";
          miBot.sendMessage(msg.sender.id, mensaje);
        } else if (msg.text.indexOf("/hora") == 0) {
          actualizarHora(msg.text, msg.sender.id);
          if (alarmaActiva) siquienteAlarma();
          PedirEstado(msg.sender.id);
        } else if (msg.text.indexOf("/dia") == 0) {
          actualizarDias(msg.text, msg.sender.id);
          if (alarmaActiva) siquienteAlarma();
          PedirEstado(msg.sender.id);
        } else if (msg.text.equalsIgnoreCase("/alarma")) {
          alarmaActiva = true;
          char pollo[10];
          String pollo_tmp = String(alarmaActiva);
          pollo_tmp.toCharArray(pollo, 10);
          escrivirArchivo("/alarma.txt", pollo);
          siquienteAlarma();
          Serial.println("Alarma Activada");
          TelnetStream.println("Alarma Activada");
          miBot.sendMessage(msg.sender.id, "Alarma Activada");

        } else if (msg.text.equalsIgnoreCase("/noalarma")) {
          alarmaActiva = false;
          char pollo[10];
          String pollo_tmp = String(alarmaActiva);
          pollo_tmp.toCharArray(pollo, 10);
          escrivirArchivo("/alarma.txt", pollo);
          Serial.println("Alarma Desactivada");
          TelnetStream.println("Alarma Desactivada");
          miBot.sendMessage(msg.sender.id, "Alarma Desactivada");
        } else if (msg.text.equalsIgnoreCase("/vibrar") || msg.text.equalsIgnoreCase("/si")) {
          alarmaVibrar = true;
          char pollo[10];
          String pollo_tmp = String(alarmaVibrar);
          pollo_tmp.toCharArray(pollo, 10);
          escrivirArchivo("/vibrar.txt", pollo);
          Serial.println("Empezando a Vibrar");
          TelnetStream.println("Empezando a Vibrar");
          miBot.sendMessage(msg.sender.id, "Empezando a Vibrar");
        } else if (msg.text.equalsIgnoreCase("/novibrar") || msg.text.equalsIgnoreCase("/no")) {
          alarmaVibrar = false;
          char pollo[10];
          String pollo_tmp = String(alarmaVibrar);
          pollo_tmp.toCharArray(pollo, 10);
          escrivirArchivo("/vibrar.txt", pollo);
          Serial.println("Parando el Vibrar");
          TelnetStream.println("Parando el Vibrar");
          miBot.sendMessage(msg.sender.id, "Parando el Vibrar");
        } else if (msg.text.equalsIgnoreCase("/estado")) {
          Serial.println("Estado Actual");
          TelnetStream.println("Estado Actual");
          PedirEstado(msg.sender.id);
        } else if (msg.text.equalsIgnoreCase("/formatiar")) {
          if (formatiarMemoria()) {
            miBot.sendMessage(msg.sender.id, "Se formatio la memoria");
          } else {
            miBot.sendMessage(msg.sender.id, "Error formatio la memoria");
          }
        } else {
          Serial.println("Enviar /opciones");
          TelnetStream.println("Enviar /opciones");
          miBot.sendMessage(msg.sender.id, "Prueba /opciones");
        }
        return;
      }
    }
    Serial << "Usuario no reconocido\n";
    Serial << "Nombre: " << msg.sender.firstName << " - " << msg.sender.lastName << "\n";
    Serial << "Usuario: " << msg.sender.username << " ID: " << int64ToAscii(msg.sender.id) << "\n";

    TelnetStream << "Usuario no reconocido\n";
    TelnetStream << "Nombre: " << msg.sender.firstName << " - " << msg.sender.lastName << "\n";
    TelnetStream << "Usuario: " << msg.sender.username << " ID: " << int64ToAscii(msg.sender.id) << "\n";

    miBot.sendMessage(msg.sender.id, "No te conosco, lo siento habla con mi amo");
  }
}

void PedirEstado(int64_t IDchat) {
  String Mensaje = "Estado actual\n";
  Mensaje += "Hora Actual: ";
  Mensaje += horaActual();
  Mensaje += " ";
  Mensaje += (esTarde() ? "PM" : "AM");
  Mensaje += " ";
  Mensaje += fechaActual();
  Mensaje += "\n";

  Mensaje += "Alarma:";
  Mensaje += "\n";

  Mensaje += "Hora: ";
  Mensaje += hora;
  Mensaje += ":";
  Mensaje += (minuto < 10 ? "0" : "");
  Mensaje += minuto;
  Mensaje += " ";
  Mensaje += (pm ? "pm" : "am");
  Mensaje += "\n";

  Mensaje += "Dias: ";
  for (int dia = 0; dia < 7; dia++) {
    if (pedirDia(dias, dia)) {
      Mensaje += NombresDia[dia];
      Mensaje += " ";
    }
  }
  Mensaje += "\n";

  if (alarmaActiva) {
    Mensaje += "Siquiente Alarma: ";
    Mensaje += NombresDia[tiempoAlarma.dayOfTheWeek()];
    Mensaje += ", ";
    Mensaje += diaAlarma();
    Mensaje += "/";
    Mensaje += mesAlarma();
    Mensaje += "/";
    Mensaje += annoAlarma();
    Mensaje += " a ";
    Mensaje += horaAlarma();
    Mensaje += ":";
    Mensaje += (minutoAlarma() < 10 ? "0" : "");
    Mensaje += minutoAlarma();
    Mensaje += " ";
    Mensaje += (pmAlarma() ? "pm" : "am");
    Mensaje += "\n";
  }

  Mensaje += "Alarma: ";
  Mensaje += (alarmaActiva ? "Activo" : "Apagado");
  Mensaje += "\n";

  Mensaje += "Vibracion: ";
  Mensaje += (alarmaVibrar ? "Encendido" : "Apagado");
  Mensaje += "\n";

  TelnetStream.println(Mensaje);
  Serial.println(Mensaje);
  miBot.sendMessage(IDchat, Mensaje);
}

void actualizarHora(String mensaje, int ID) {
  int espacioPrimer = mensaje.indexOf(" ");
  int dosPuestos = mensaje.indexOf(":");
  int espacioSegundo = mensaje.indexOf(" ", espacioPrimer + 1);
  int final = mensaje.length();

  int horaAlarma = mensaje.substring(espacioPrimer + 1, dosPuestos).toInt();
  int minutoAlarma = mensaje.substring(dosPuestos + 1, espacioSegundo).toInt();
  String pmAlarma = mensaje.substring(espacioSegundo + 1, final);

  if (horaAlarma < 0 || horaAlarma > 12) return;
  if (minutoAlarma < 0 || minutoAlarma > 59) return;
  if (!(pmAlarma.equalsIgnoreCase("am") || pmAlarma.equalsIgnoreCase("pm"))) return;
  // TODO: mensaje de hora incorrecta

  hora = horaAlarma;
  minuto = minutoAlarma;
  pm = pmAlarma.equalsIgnoreCase("pm");

  char pollo[10];

  String pollo_tmp = String(hora);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/hora.txt", pollo);

  pollo_tmp = String(minuto);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/minuto.txt", pollo);

  pollo_tmp = String(pm);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/pm.txt", pollo);

  Serial.println("Actualizando Hora");
  TelnetStream.println("Actualizando Hora");
  miBot.sendMessage(ID, "Actualizando Hora");
}

void actualizarDias(String mensaje, int ID) {
  int elNumero = 0;
  boolean Segir = true;
  int espacioPrimero = 0;
  int espacioSiquiente = mensaje.indexOf(" ");
  int final = mensaje.length();
  String diaActual = "";
  do {
    espacioPrimero = espacioSiquiente;
    espacioSiquiente = mensaje.indexOf(" ", espacioPrimero + 1);

    if (espacioSiquiente < 0) {
      Segir = false;
      espacioSiquiente = final;
    }
    diaActual = mensaje.substring(espacioPrimero + 1, espacioSiquiente);

    for (int i = 0; i < 7; i++) {
      if (diaActual.equalsIgnoreCase(NombresDia[i])) {
        elNumero = elNumero | (1 << (6 - i));
        continue;
      }
    }
  } while (Segir);

  dias = elNumero;

  char pollo[10];
  String pollo_tmp = String(dias);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/dia.txt", pollo);

  Serial.println("Dias salvados");
  TelnetStream.println("Dias salvados");
  miBot.sendMessage(ID, "Dias salvados");
}
