#include "CTBot.h"
#include "Utilities.h"

CTBot miBot;
boolean bienbenidaBot = false;

void configurarBot() {
  miBot.setTelegramToken(token);
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
        if (msg.text.equalsIgnoreCase("/opciones") ) {
          Serial.println("Enviando opciones");
          TelnetStream.println("Enviando opciones");
          String mensaje = "Opciones disponibles:\n";
          mensaje += "/estado estado del sistema\n";
          mensaje += "/vibrar enciende vibrador\n";
          mensaje += "/novibrar apaga vibrador\n";
          mensaje += "/opciones comandos disponibles\n";
          miBot.sendMessage(msg.sender.id, mensaje);
        } else if (msg.text.equalsIgnoreCase("/estado") ) {
          Serial.println("Estado Actual");
          TelnetStream.println("Estado Actual");
          PedirEstado(msg.sender.id);
        } else if (msg.text.equalsIgnoreCase("/vibrar") || msg.text.equalsIgnoreCase("/si")) {
          cambiarVibrar(true);
          Serial.println("Empezando a Vibrar");
          TelnetStream.println("Empezando a Vibrar");
          miBot.sendMessage(msg.sender.id, "Empezando a Vibrar");
        } else if (msg.text.equalsIgnoreCase("/novibrar") || msg.text.equalsIgnoreCase("/no")) {
          cambiarVibrar(false);
          Serial.println("Parando el Vibrar");
          TelnetStream.println("Parando el Vibrar");
          miBot.sendMessage(msg.sender.id, "Parando el Vibrar");
        }  else {
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
  Mensaje += "\n";

  Mensaje += "Alarma a ";
  Mensaje += hora;
  Mensaje += ":";
  Mensaje += (minuto < 10 ? "0" : "");
  Mensaje += minuto;
  Mensaje += " ";
  Mensaje += (pm ? "pm" : "am");
  Mensaje += ": ";
  Mensaje += (alarmaArmada ? "Activo" : "Apagado");
  Mensaje += "\n";

  Mensaje += "Vibracion: ";
  Mensaje += (alarmaVibrar ? "Encendido" : "Apagado");
  Mensaje += "\n";

  TelnetStream.println(Mensaje);
  Serial.println(Mensaje);
  miBot.sendMessage(IDchat, Mensaje);

}
