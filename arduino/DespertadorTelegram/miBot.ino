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
    Serial.print("En Linea, Sistema: ");
    Serial.println(NombreESP);
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
    Serial.print("Mensaje: '");
    Serial.print(msg.text);
    Serial.print("' de ");
    Serial.print(msg.sender.username);
    Serial.print(" ID:");
    Serial.println(msg.sender.id);
    // TelnetStream << "Mensaje: " << msg.text << " de " << msg.sender.username << " ID:" << msg.sender.id << "\n";
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
          mensaje += "/dia +[dia](lun mar ... dom o todos) Condigura dia de la alarma\n";
          mensaje += "/actualizarHora Manda la nueva Hora del Reloc\n";
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
        } else if (msg.text.indexOf("/actualizarHora") == 0) {
          cambiarHora(msg.text, msg.sender.id);
        } else {
          Serial.println("Enviar /opciones");
          TelnetStream.println("Enviar /opciones");
          miBot.sendMessage(msg.sender.id, "Prueba /opciones");
        }
        return;
      }
    }
    // Serial << "Usuario no reconocido\n";
    // Serial << "Nombre: " << msg.sender.firstName << " - " << msg.sender.lastName << "\n";
    // Serial << "Usuario: " << msg.sender.username << " ID: " << int64ToAscii(msg.sender.id) << "\n";

    // TelnetStream << "Usuario no reconocido\n";
    // TelnetStream << "Nombre: " << msg.sender.firstName << " - " << msg.sender.lastName << "\n";
    // TelnetStream << "Usuario: " << msg.sender.username << " ID: " << int64ToAscii(msg.sender.id) << "\n";

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

MiHora buscarHora(String mensaje) {
  MiHora horaActual = { -1, 0, false };
  int dosPuntos = mensaje.indexOf(":");
  int espacio = mensaje.indexOf(" ", dosPuntos + 1);
  int final = mensaje.length();
  Serial.print(":");
  Serial.print(dosPuntos);
  Serial.print(" Espacio ");
  Serial.print(dosPuntos);
  Serial.print(" Final ");
  Serial.print(dosPuntos);

  if (dosPuntos > 0) {
    horaActual.Hora = mensaje.substring(0, dosPuntos).toInt();
  } else if (espacio > 0) {
    horaActual.Hora = mensaje.substring(0, espacio).toInt();
  } else {
    horaActual.Hora = mensaje.substring(0, final).toInt();
  }
  Serial.print(" Hora ");
  Serial.print(horaActual.Hora);
  Serial.println();

  if (dosPuntos > 0) {
    if (espacio > 0) {
      horaActual.Minuto = mensaje.substring(dosPuntos + 1, espacio).toInt();
    } else {
      horaActual.Minuto = mensaje.substring(dosPuntos + 1, final).toInt();
    }
  }

  if (espacio > 0) {
    String TextoPm = mensaje.substring(espacio + 1, final);
    if (!(TextoPm.equalsIgnoreCase("am") || TextoPm.equalsIgnoreCase("pm"))) {
      horaActual.Hora = -1;
      return horaActual;
    };
    horaActual.Pm = TextoPm.equalsIgnoreCase("pm");
  }

  if (horaActual.Hora < 0 || horaActual.Hora > 24) {
    horaActual.Hora = -1;
    return horaActual;
  } else if (horaActual.Hora > 12) {
    horaActual.Hora = horaActual.Hora - 12;
    horaActual.Pm = true;
  }

  if (horaActual.Minuto < 0 || horaActual.Minuto > 59) {
    horaActual.Hora = -1;
    return horaActual;
  }

  return horaActual;
}

void actualizarHora(String mensaje, int ID_chat) {
  int espacioPrimer = mensaje.indexOf(" ");
  int final = mensaje.length();
  String mensajeSinComando = mensaje.substring(espacioPrimer + 1, final);
  MiHora horaNueva = buscarHora(mensajeSinComando);
  if (horaNueva.Hora < 0) {
    Serial.println("Error en formato de Hora");
    TelnetStream.println("Error en formato de Hora");
    miBot.sendMessage(ID_chat, "Error en formato de Hora");
    return;
  }

  hora = horaNueva.Hora;
  minuto = horaNueva.Minuto;
  pm = horaNueva.Pm;

  char pollo[10];

  String pollo_tmp = String(horaNueva.Hora);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/hora.txt", pollo);

  pollo_tmp = String(horaNueva.Minuto);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/minuto.txt", pollo);

  pollo_tmp = String(horaNueva.Pm);
  pollo_tmp.toCharArray(pollo, 10);
  escrivirArchivo("/pm.txt", pollo);

  Serial.println("Actualizando Hora");
  TelnetStream.println("Actualizando Hora");
  miBot.sendMessage(ID_chat, "Actualizando Hora");
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

    if (diaActual.equalsIgnoreCase("todos")) {
      for (int i = 0; i < 7; i++) {
        elNumero = elNumero | (1 << (6 - i));
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

void cambiarHora(String mensaje, int ID_chat) {
  int espacioPrimer = mensaje.indexOf(" ");
  int final = mensaje.length();
  String mensajeSinComando = mensaje.substring(espacioPrimer + 1, final);
  if (mensajeSinComando == "") return;
  MiHora horaNueva = buscarHora(mensajeSinComando);

  if (horaNueva.Hora < 0) {
    Serial.println("Error en formato de Hora");
    TelnetStream.println("Error en formato de Hora");
    miBot.sendMessage(ID_chat, "Error en formato de Hora");
    return;
  }

  Serial.print("Hora nueva: ");
  Serial.print(horaNueva.Hora);
  Serial.print(":");
  Serial.print(horaNueva.Minuto);
  Serial.print("");
  Serial.println(horaNueva.Pm ? "pm" : "am");

  programarRTC = true;
  horaActualizar = horaNueva;

  Serial.println("Configurando Hora");
  TelnetStream.println("Configurando Hora");
  miBot.sendMessage(ID_chat, "Configurando Hora");
}
