
void configurarBot() {
  miBot.setTelegramToken(token);
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
  TBMessage msg;

  if (CTBotMessageText == miBot.getNewMessage(msg)) {
    Serial << "Mensaje: " << msg.sender.firstName << " - " <<  msg.text << "\n";
    miBot.sendMessage(msg.sender.id, msg.text);
  }
}
