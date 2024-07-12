#include <WiFi.h>
#include <ThingSpeak.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char *ssid = "yo_lo_programe";
const char *password = "kinalenmayuscula";
const unsigned long CHANNEL_ID = 2538464;
const char *thingSpeakApiKey = "4YL3H91K50ZA2M3U";
const unsigned long CHANNEL_ID2 = 2538461;
const char *thingSpeakApiKey2 = "IPRUHRMQUUTNXUMG";
const int oneWireBus = 5; // Pin GPIO al que está conectado el sensor DS18B20
const int relePin = 14; // Pin GPIO al que está conectado el módulo de relé
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
WiFiClient client;
void setup() {
  Serial.begin(115200);
  delay(10);
  // Inicia la conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a la red WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a la red WiFi");
  ThingSpeak.begin(client); // Inicia la comunicación con ThingSpeak
  sensors.begin(); // Inicia la comunicación con el sensor DS18B20
  pinMode(relePin, OUTPUT); // Configura el pin del relé como salida
}

void loop() {
  sensors.requestTemperatures(); // Realiza una lectura de la temperatura
  float temperatureC = sensors.getTempCByIndex(0); // Obtiene la temperatura en grados Celsius
  
  Serial.println("Temperatura Celsius: ");
  Serial.print(temperatureC);
  Serial.print(" °C");

  // Verifica si la temperatura supera los 25 grados Celsius
  if (temperatureC > 30) {
    // Si es así, activa el relé
    digitalWrite(relePin, HIGH);
    Serial.println("Relé activado");
  } else {
    // Si la temperatura es menor o igual a 25 grados Celsius, desactiva el relé
    digitalWrite(relePin, LOW);
    Serial.println("Relé desactivado");
  }

  // Envía las temperaturas a ThingSpeak
  ThingSpeak.writeField(CHANNEL_ID, 1, temperatureC, thingSpeakApiKey);
  ThingSpeak.writeField(CHANNEL_ID2, 1, temperatureC, thingSpeakApiKey2); // Envía temperatura en Celsius al campo 1

  delay(1000); // Espera 1 segundos antes de la siguiente lectura
  
}