// CP5 - ESP32 com DHT22 e LDR enviando dados para o HiveMQ (MQTT)
// Publica: temperatura, umidade e luminosidade
// Assina:  comando do LED (enviado pelo dashboard do Node-RED)

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

const char* WIFI_NOME = "Wokwi-GUEST";
const char* WIFI_SENHA = "";
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORTA = 1883;

const char* TOPICO_TEMPERATURA = "fiap/cp5/savio/temperatura";
const char* TOPICO_UMIDADE = "fiap/cp5/savio/umidade";
const char* TOPICO_LUMINOSIDADE = "fiap/cp5/savio/luminosidade";
const char* TOPICO_LED = "fiap/cp5/savio/led";

const int PINO_DHT = 15;
const int PINO_LDR = 34;
const int PINO_LED = 2;

WiFiClient conexaoWiFi;
PubSubClient mqtt(conexaoWiFi);
DHTesp dht;
unsigned long ultimoEnvio = 0;

void receberMensagem(char* topico, byte* dados, unsigned int tamanho) {
  String mensagem = "";
  for (unsigned int i = 0; i < tamanho; i++) {
    mensagem += (char)dados[i];
  }
  Serial.print("Comando recebido: ");
  Serial.println(mensagem);

  if (mensagem == "ON") {
    digitalWrite(PINO_LED, HIGH);
  } else if (mensagem == "OFF") {
    digitalWrite(PINO_LED, LOW);
  }
}

// O modulo LDR do Wokwi entrega tensao menor quanto mais luz recebe
float lerLuminosidade() {
  int leitura = analogRead(PINO_LDR);
  return 100.0 - (leitura / 4095.0) * 100.0;
}

void setup() {
  Serial.begin(115200);
  pinMode(PINO_LED, OUTPUT);
  dht.setup(PINO_DHT, DHTesp::DHT22);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_NOME, WIFI_SENHA);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");

  mqtt.setServer(MQTT_BROKER, MQTT_PORTA);
  mqtt.setCallback(receberMensagem);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    return;
  }
  if (!mqtt.connected()) {
    String identificador = "ESP32-" + WiFi.macAddress() + "-" + String(random(0xffff), HEX);
    if (mqtt.connect(identificador.c_str())) {
      Serial.println("MQTT conectado!");
      mqtt.subscribe(TOPICO_LED);
    } else {
      Serial.print("Falha MQTT. Codigo: ");
      Serial.println(mqtt.state());
      delay(3000);
      return;
    }
  }
  mqtt.loop();

  if (millis() - ultimoEnvio >= 2000) {
    ultimoEnvio = millis();

    TempAndHumidity leitura = dht.getTempAndHumidity();
    if (isnan(leitura.temperature) || isnan(leitura.humidity)) {
      Serial.println("Falha ao ler o DHT22");
      return;
    }
    float luminosidade = lerLuminosidade();

    mqtt.publish(TOPICO_TEMPERATURA, String(leitura.temperature, 1).c_str());
    mqtt.publish(TOPICO_UMIDADE, String(leitura.humidity, 1).c_str());
    mqtt.publish(TOPICO_LUMINOSIDADE, String(luminosidade, 1).c_str());

    Serial.print("Temperatura: ");
    Serial.print(leitura.temperature, 1);
    Serial.print(" C | Umidade: ");
    Serial.print(leitura.humidity, 1);
    Serial.print(" % | Luminosidade: ");
    Serial.print(luminosidade, 1);
    Serial.println(" %");
  }
}
