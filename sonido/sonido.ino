#include <WiFi.h>
#include <ThingSpeak.h>
#include <driver/i2s.h>

#define I2S_WS 32
#define I2S_SD 4
#define I2S_SCK 33
#define I2S_PORT I2S_NUM_0
#define bufferLen 64

const char* ssid = "SATAN"; // tu SSID de red Wi-Fi
const char* password = "12345678"; // tu contraseña de red Wi-Fi
unsigned long myChannelNumber = 2538464; // tu número de canal ThingSpeak
const char* myWriteAPIKey = "4YL3H91K50ZA2M3U"; // tu clave API de escritura de ThingSpeak
unsigned long myChannelNumber2 = 2574117; // tu número de canal ThingSpeak
const char* myWriteAPIKey2 = "ELLS0GMTESK07M8K"; // tu clave API de escritura de ThingSpeak
WiFiClient client;

float sensibilidad_mic = 20; // Ejemplo: sensibilidad de 20 dB/Volt
float v_ref = 1.4; // Ejemplo: valor de referencia de 1 Volt

int16_t sBuffer[bufferLen];

void setup() {
  Serial.begin(115200);
  Serial.println("Setup I2S ...");

  WiFi.begin(ssid, password);
  Serial.print("Conectando a ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a la red Wi-Fi");
  ThingSpeak.begin(client); // Inicia la comunicación con ThingSpeak

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(1000);
}

void loop() {
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
  if (result == ESP_OK) {
    int samples_read = bytesIn / 8;
    if (samples_read > 0) {
      float mean = 0;
      for (int i = 0; i < samples_read; ++i) {
        mean += (sBuffer[i]);
      }
      mean /= samples_read;
      unsigned int valor_dB = valor_a_dB(mean);
      int mostrarT = valor_dB;
      if (valor_dB > 1 && valor_dB < 200 && mostrarT < 200 && mostrarT > 1) {
        Serial.print("El valor dB = ");
        Serial.println(valor_dB);
        Serial.print("El de Thing = ");
        Serial.println(mostrarT);
        ThingSpeak.writeField(myChannelNumber, 2, mostrarT, myWriteAPIKey);
         ThingSpeak.writeField(myChannelNumber2, 2, mostrarT, myWriteAPIKey2);
      }
    }
  }
}

void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Convertir la expresión a i2s_mode_t
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

// Función para convertir valor medio de señal a dB
float valor_a_dB(float valor_medio) {
  // Calcula el valor RMS de la señal (aproximadamente proporcional al valor medio)
  float Vrms = valor_medio / sqrt(2);

  // Calcula los decibelios utilizando la fórmula, ajustando la sensibilidad del micrófono
  unsigned int dB = 20.0 * log10(Vrms / v_ref) + sensibilidad_mic;

  return dB;
}
