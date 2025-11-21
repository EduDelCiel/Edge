#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

// Configurações de WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

// Configuração do display LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuração do sensor DHT22 (temperatura e umidade)
#define DHTTYPE DHT22
const int dhtPin = 15;
DHT dht(dhtPin, DHTTYPE);

// Definição dos pinos dos sensores e atuadores
const int trigPin = 5;      // Pino trigger do sensor ultrassônico
const int echoPin = 18;     // Pino echo do sensor ultrassônico
const int ldrPin = 34;      // Pino do sensor de luz (LDR)
const int buzzerPin = 23;   // Pino do buzzer
const int redPin = 32;      // Pino do LED vermelho
const int greenPin = 33;    // Pino do LED verde
const int yellowPin = 25;   // Pino do LED amarelo

// Variáveis para armazenar dados dos sensores
float postureDistance = 0;  // Distância medida pelo ultrassônico (cm)
int lightLevel = 0;         // Nível de luz bruto do LDR
int lightPercentage = 0;    // Nível de luz em porcentagem
float temperature = 0;      // Temperatura em °C
float humidity = 0;         // Umidade em %
unsigned long workStartTime = 0;  // Tempo de início do trabalho

// Variáveis para status dos sensores
String postureStatus = "";   // Status da postura
String lightStatus = "";     // Status da iluminação
String tempStatus = "";      // Status da temperatura
String humidityStatus = "";  // Status da umidade
String overallStatus = "";   // Status geral do sistema

// Função callback para mensagens MQTT recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida em: ");
  Serial.println(topic);
  
  // Converte o payload para String
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Conteúdo: ");
  Serial.println(message);
}

// Função para reconectar ao broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Reconectando ao MQTT...");
    // Tenta conectar com ID único
    if (client.connect("Ergosense-ESP32")) {
      Serial.println("OK!");
      // Se conectado, subscreve no tópico de comandos
      client.subscribe("ergosense/cmd");
    } else {
      Serial.println("Falhou. Tentando em 2s...");
      delay(2000);
    }
  }
}

// Função para publicar dados no MQTT
void publishData(){
  // Cria payload JSON com todos os dados dos sensores
  String payload = 
  "{ \"temperatura\": " + String(temperature) +
  ", \"umidade\": " + String(humidity) +
  ", \"luz\": " + String(lightPercentage) +
  ", \"postura\": " + String(postureDistance) +
  ", \"status\": \"" + overallStatus + "\" }";

  // Publica dados no tópico
  client.publish("ergosense/dados", payload.c_str());

  // Publica qual LED está ativo
  String led = "nenhum";
  if (digitalRead(redPin) == LOW) led = "vermelho";
  else if (digitalRead(yellowPin) == LOW) led = "amarelo";
  else if (digitalRead(greenPin) == LOW) led = "verde";

  client.publish("ergosense/led", led.c_str());
}

// Função de configuração inicial
void setup() {
  Serial.begin(115200);

  // Configura os pinos como entrada ou saída
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);

  // Inicializa LEDs desligados (LOW = ligado, HIGH = desligado)
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(yellowPin, HIGH);

  // Inicializa display LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ErgoSense Wokwi");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");

  // Inicializa sensor DHT
  dht.begin();

  // Conecta ao WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10){
    Serial.print(".");
    delay(500);
    attempts++;
  }
  Serial.println("\nWiFi conectado!");

  // Configura cliente MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Marca tempo de início do trabalho
  workStartTime = millis();
  delay(1500);
  lcd.clear();
}

// Função para ler sensor ultrassônico e calcular distância
float readUltrasonic() {
  // Envia pulso ultrassônico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mede tempo de retorno do eco
  long duration = pulseIn(echoPin, HIGH);
  // Calcula distância (velocidade do som = 0.034 cm/μs)
  return duration * 0.034 / 2;
}

// Função para avaliar a postura baseada na distância
void evaluatePosture() {
  if (postureDistance <= 100)
    postureStatus = "OK";           // Distância adequada
  else if (postureDistance <= 200)
    postureStatus = "APROXIME";     // Muito longe, precisa se aproximar
  else if (postureDistance <= 400)
    postureStatus = "PESSIMA";      // Postura muito ruim
  else
    postureStatus = "ERRO";         // Erro na medição
}

// Função para avaliar nível de iluminação
void evaluateLight() {
  // Converte leitura analógica para porcentagem (invertida)
  lightPercentage = map(lightLevel, 0, 4095, 100, 0);

  if (lightPercentage >= 70)
    lightStatus = "OK";             // Iluminação boa
  else if (lightPercentage >= 30)
    lightStatus = "MEDIA";          // Iluminação média
  else
    lightStatus = "PESSIMA";        // Iluminação ruim
}

// Função para avaliar temperatura
void evaluateTemperature() {
  if (temperature <= 18)
    tempStatus = "FRIO";            // Temperatura baixa
  else if (temperature <= 24)
    tempStatus = "OK";              // Temperatura adequada
  else if (temperature <= 100)
    tempStatus = "QUENTE";          // Temperatura alta
  else
    tempStatus = "ERRO";            // Erro na leitura
}

// Função para avaliar umidade
void evaluateHumidity() {
  if (humidity >= 40 && humidity <= 60)
    humidityStatus = "OK";          // Umidade ideal
  else if ((humidity >= 30 && humidity < 40) || (humidity > 60 && humidity <= 70))
    humidityStatus = "MEDIA";       // Umidade aceitável
  else if (humidity < 30 || humidity > 70)
    humidityStatus = "PESSIMA";     // Umidade inadequada
  else
    humidityStatus = "ERRO";        // Erro na leitura
}

// Função para avaliar status geral baseado em todos os sensores
void evaluateOverallStatus() {
  if (postureStatus == "PESSIMA" || lightStatus == "PESSIMA" ||
      tempStatus == "FRIO" || tempStatus == "QUENTE" ||
      humidityStatus == "PESSIMA") {
    overallStatus = "PESSIMO";      // Condições críticas
  } 
  else if (postureStatus == "APROXIME" || lightStatus == "MEDIA" ||
           humidityStatus == "MEDIA") {
    overallStatus = "MEDIO";        // Condições moderadas
  } 
  else {
    overallStatus = "OK";           // Todas condições boas
  }
}

// Função para controlar LEDs e buzzer baseado no status geral
void controlLEDsAndBuzzer() {
  // Desliga todos os LEDs e buzzer primeiro
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(yellowPin, HIGH);
  noTone(buzzerPin);

  // Aciona indicadores conforme status
  if (overallStatus == "PESSIMO") {
    digitalWrite(redPin, LOW);      // LED vermelho + buzzer
    tone(buzzerPin, 1000, 300);
  } 
  else if (overallStatus == "MEDIO") {
    digitalWrite(yellowPin, LOW);   // LED amarelo
  }
  else {
    digitalWrite(greenPin, LOW);    // LED verde
  }
}

// Função principal para ler todos os sensores
void readSensors() {
  // Lê dados de todos os sensores
  postureDistance = readUltrasonic();
  lightLevel = analogRead(ldrPin);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Verifica se leituras do DHT são válidas
  if (isnan(temperature) || isnan(humidity)) {
    temperature = 23.0;    // Valores padrão em caso de erro
    humidity = 50.0;
  }

  // Avalia todos os sensores
  evaluatePosture();
  evaluateLight();
  evaluateTemperature();
  evaluateHumidity();
  evaluateOverallStatus();
}

// Função para atualizar o display LCD
void updateDisplay() {
  // Calcula tempo de trabalho em minutos
  unsigned long workTime = (millis() - workStartTime) / 60000;

  lcd.clear();
  lcd.setCursor(0, 0);

  // Mostra status geral na primeira linha
  if (overallStatus == "PESSIMO") lcd.print("ALERTA!");
  else if (overallStatus == "MEDIO") lcd.print("ATENCAO!");
  else {
    lcd.print("TUDO OK ");
    lcd.print(workTime);
    lcd.print("m");
  }

  // Mostra dados dos sensores na segunda linha
  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print((int)postureDistance);
  lcd.print("cm L:");
  lcd.print(lightPercentage);
  lcd.print("%");
}

// Loop principal do programa
void loop() {
  // Gerencia conexão MQTT
  if (!client.connected()) reconnect();
  client.loop();

  // Executa funções principais
  readSensors();              // Lê e processa dados dos sensores
  controlLEDsAndBuzzer();     // Controla LEDs e buzzer
  updateDisplay();            // Atualiza display LCD
  publishData();              // Publica dados via MQTT

  delay(2000);  // Aguarda 2 segundos entre leituras
}