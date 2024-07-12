#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "SATAN"; // tu SSID de red Wi-Fi
const char* password = "12345678"; // tu contraseña de red Wi-Fi
unsigned long myChannelNumber = 2538464; // tu número de canal ThingSpeak
const char* myWriteAPIKey = "4YL3H91K50ZA2M3U"; // tu clave API de escritura de ThingSpeak
unsigned long myChannelNumber2 = 2574117; // tu número de canal ThingSpeak
const char* myWriteAPIKey2 = "ELLS0GMTESK07M8K"; // tu clave API de escritura de ThingSpeak
WiFiClient client;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Definir pines de LED
const int numLEDs = 10;
int LEDPins[numLEDs] = {0, 2, 4, 12, 13, 14, 25, 26, 27, 32};
/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
  
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(115200);
  Serial.println("Light Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  //use tsl.begin() to default to Wire,
  //tsl.begin(&Wire2) directs api to use Wire2, etc.
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();

  /* We're ready to go! */
  Serial.println("");

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

  // Configurar pines de LED como salidas
  for (int i = 0; i < numLEDs; i++) {
    pinMode(LEDPins[i], OUTPUT);
  }
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{  
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    Serial.print(event.light);  Serial.println(" lux");
    ThingSpeak.writeField(myChannelNumber, 3 , event.light, myWriteAPIKey);
        ThingSpeak.writeField(myChannelNumber2, 1 , event.light, myWriteAPIKey2);
    // Encender LEDs basado en el valor de lux
    int numLedsToTurnOn = map(event.light, 0, 700, 0, numLEDs);
    for (int i = 0; i < numLEDs; i++) {
      if (i < numLedsToTurnOn) {
        digitalWrite(LEDPins[i], HIGH);
      } else {
        digitalWrite(LEDPins[i], LOW);
      }
    }
  }
  else
  {
    Serial.println("Sensor overload");
  }
  delay(800);
}