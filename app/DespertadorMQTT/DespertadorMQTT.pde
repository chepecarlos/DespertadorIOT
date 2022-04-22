import mqtt.*;

MQTTClient client;
boolean conectadoMQTT = false;

void setup() {
  JSONObject json;
  json = loadJSONObject("token.json");
  String usuario = json.getString("usuario");
  String contrasenna = json.getString("contrasenna");
  String broker = json.getString("servidor");
  int puerto = json.getInt("puerto");

  String urlBroker = "mqtt://"  + usuario + ":" + contrasenna + "@" + broker + ":" + puerto ;
  client = new MQTTClient(this);
  client.connect(urlBroker, "processing");
}

void draw() {
}

void keyPressed() {
  if (conectadoMQTT) {
    client.publish("/hello", "world");
  }
}

void clientConnected() {
  println("client connected");
  conectadoMQTT = true;
  client.subscribe("/hello");
}

void messageReceived(String topic, byte[] payload) {
  println("new message: " + topic + " - " + new String(payload));
}

void connectionLost() {
  println("connection lost");
  conectadoMQTT = false;
}
