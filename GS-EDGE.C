#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTTYPE DHT22
const int dhtPin = 15;
DHT dht(dhtPin, DHTTYPE);

const int trigPin = 5;
const int echoPin = 18;
const int ldrPin = 34;
const int buzzerPin = 23;
const int redPin = 32;
const int greenPin = 33;
const int yellowPin = 25;

float postureDistance = 0;
int lightLevel = 0;
int lightPercentage = 0;
float temperature = 0;
float humidity = 0;
unsigned long workStartTime = 0;

String postureStatus = "";
String lightStatus = "";
String tempStatus = "";
String humidityStatus = "";
String overallStatus = "";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida em: ");
  Serial.println(topic);
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Conteúdo: ");
  Serial.println(message);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Reconectando ao MQTT...");
    if (client.connect("Ergosense-ESP32")) {
      Serial.println("OK!");
      client.subscribe("ergosense/cmd");
    } else {
      Serial.println("Falhou. Tentando em 2s...");
      delay(2000);
    }
  }
}


void publishData(){
  String payload = 
  "{ \"temperatura\": " + String(temperature) +
  ", \"umidade\": " + String(humidity) +
  ", \"luz\": " + String(lightPercentage) +
  ", \"postura\": " + String(postureDistance) +
  ", \"status\": \"" + overallStatus + "\" }";

  client.publish("ergosense/dados", payload.c_str());

  // LED ativo
  String led = "nenhum";
  if (digitalRead(redPin) == LOW) led = "vermelho";
  else if (digitalRead(yellowPin) == LOW) led = "amarelo";
  else if (digitalRead(greenPin) == LOW) led = "verde";

  client.publish("ergosense/led", led.c_str());
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(yellowPin, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ErgoSense Wokwi");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");

  dht.begin();

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10){
    Serial.print(".");
    delay(500);
    attempts++;
  }
  Serial.println("\nWiFi conectado!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  workStartTime = millis();
  delay(1500);
  lcd.clear();
}

float readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void evaluatePosture() {
  if (postureDistance <= 100)
    postureStatus = "OK";
  else if (postureDistance <= 200)
    postureStatus = "APROXIME";
  else if (postureDistance <= 400)
    postureStatus = "PESSIMA";
  else
    postureStatus = "ERRO";
}

void evaluateLight() {
  lightPercentage = map(lightLevel, 0, 4095, 100, 0);

  if (lightPercentage >= 70)
    lightStatus = "OK";
  else if (lightPercentage >= 30)
    lightStatus = "MEDIA";
  else
    lightStatus = "PESSIMA";
}

void evaluateTemperature() {
  if (temperature <= 18)
    tempStatus = "FRIO";
  else if (temperature <= 24)
    tempStatus = "OK";
  else if (temperature <= 100)
    tempStatus = "QUENTE";
  else
    tempStatus = "ERRO";
}

void evaluateHumidity() {
  if (humidity >= 40 && humidity <= 60)
    humidityStatus = "OK";
  else if ((humidity >= 30 && humidity < 40) || (humidity > 60 && humidity <= 70))
    humidityStatus = "MEDIA";
  else if (humidity < 30 || humidity > 70)
    humidityStatus = "PESSIMA";
  else
    humidityStatus = "ERRO";
}

void evaluateOverallStatus() {
  if (postureStatus == "PESSIMA" || lightStatus == "PESSIMA" ||
      tempStatus == "FRIO" || tempStatus == "QUENTE" ||
      humidityStatus == "PESSIMA") {
    overallStatus = "PESSIMO";
  } 
  else if (postureStatus == "APROXIME" || lightStatus == "MEDIA" ||
           humidityStatus == "MEDIA") {
    overallStatus = "MEDIO";
  } 
  else {
    overallStatus = "OK";
  }
}

void controlLEDsAndBuzzer() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(yellowPin, HIGH);
  noTone(buzzerPin);

  if (overallStatus == "PESSIMO") {
    digitalWrite(redPin, LOW);
    tone(buzzerPin, 1000, 300);
  } 
  else if (overallStatus == "MEDIO") {
    digitalWrite(yellowPin, LOW);
  }
  else {
    digitalWrite(greenPin, LOW);
  }
}

void readSensors() {
  postureDistance = readUltrasonic();
  lightLevel = analogRead(ldrPin);

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    temperature = 23.0;
    humidity = 50.0;
  }

  evaluatePosture();
  evaluateLight();
  evaluateTemperature();
  evaluateHumidity();
  evaluateOverallStatus();
}

void updateDisplay() {
  unsigned long workTime = (millis() - workStartTime) / 60000;

  lcd.clear();
  lcd.setCursor(0, 0);

  if (overallStatus == "PESSIMO") lcd.print("ALERTA!");
  else if (overallStatus == "MEDIO") lcd.print("ATENCAO!");
  else {
    lcd.print("TUDO OK ");
    lcd.print(workTime);
    lcd.print("m");
  }

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print((int)postureDistance);
  lcd.print("cm L:");
  lcd.print(lightPercentage);
  lcd.print("%");
}

void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  readSensors();
  controlLEDsAndBuzzer();
  updateDisplay();
  publishData();

  delay(2000);
}
