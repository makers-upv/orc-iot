/* ************************************************* *
 * ------ ORC DATASTREAMING IoT ARDUINO NODE ------- *
 * --------- coded by Jaime Laborda Macari---------- *
 * -------------- ORC2.17 - MAKERSUPV -------------- *
 * ------https://github.com/makers-upv/orc-iot ----- *
 * ************************************************* */

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <WiFi.h>

//CONSTANTS
const int SONOMETRO_MUESTRAS = 50;

//WIFI CREDENTIALS
const char* ssid = "SSID";
const char* password = "PASSWORD";

//DATOS API
const char* APIAddress = "DOMAIN"; // Your domain
const int httpPort = 80;


// Data wire for the temperature sensor
#define ONE_WIRE_BUS 4
#define DHTPIN 26
#define DHTTYPE DHT22

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DS18B20(&oneWire);
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

//VARIABLES
float temp1,
      temp1_ant,
      temp2,
      humidity,
      sound_volts,
      sound_level
      ;

long sound_raw = 0;

uint16_t lux1;

void setup(void)
{
  // start serial port
  Serial.begin(115200);

  // Start up the library
  DS18B20.begin();
  lightMeter.begin();
  dht.begin();

  // Conectar a la red WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { //Espera hasta que nos conectemos
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

/*
   Main function, get and show the temperature
*/

void loop (void)
{
  UpdateVariables();

  String tsData = "temperatura=" + String(temp1) + "&humedad=" + String(humidity) + "&sonido=" + String(sound_level) + "&luminosidad=" + String(lux1);

  updateAPI(tsData);

  delay(10000);
}

void UpdateVariables(void)
{
  DS18B20.requestTemperatures(); // Send the command to get temperatures
  temp1 = DS18B20.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temp1);
  Serial.println(" oC");

  if (temp1 < 0) { //constrain
    temp1 = temp1_ant;
  } else {
    temp1_ant = temp1;
  }

  //---
  humidity = dht.readHumidity();
  temp2 = dht.readTemperature();

  if (isnan(humidity) || isnan(temp2)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temp2: ");
  Serial.print(temp2);
  Serial.print("| Humidity: ");
  Serial.println(humidity);

  //---

  lux1 = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux1);
  Serial.println(" lx");

  //---

  //SONOMETER
  int i;
  for (i = 0; i < SONOMETRO_MUESTRAS; i++) {
    sound_raw += analogRead(34);
  }

  sound_raw = sound_raw / SONOMETRO_MUESTRAS;
  sound_volts = (sound_raw * 5) / 4096.0; //we get volts

  sound_level = 100 - 12.21 * sound_volts;


  Serial.println("SoundLevel: "+String(sound_volts)+" | " + String(sound_level) + " db");

}


void updateAPI(String tsData) {
  WiFiClient client;
  if (client.connect(APIAddress, 80)) {
    client.print("POST /api/post/sensoresFijo HTTP/1.1\r\n");
    client.print("Host: clubdeteniscarlet.com\r\n");
    client.print("Connection: keep-alive\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);

    if (client.connected()) {
      Serial.println("Dato enviado correctamente: " + tsData);
      Serial.println();
    } else {
      Serial.println("Error al enviar el dato a la API");
    }
  } else {
    Serial.println("Error al conectarse a la API");
  }
}
