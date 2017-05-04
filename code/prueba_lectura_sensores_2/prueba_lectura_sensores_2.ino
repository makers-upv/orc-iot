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

//WIFI CREDENTIALS
const char* ssid = "SSID";
const char* password = "PASSWORD";

//THINGSPEAK
const char* thingSpeakAddress = "api.thingspeak.com"; // Your domain
const int httpPort = 80;
String THINGSPEAK_API_KEY = "XXXXXXXXXXXXXXXXX";


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
      temp2,
      humidity
      ;
      
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
 * Main function, get and show the temperature
 */

void loop (void)
{
  UpdateVariables();

  String tsData = "&field1="+String(temp1)+"&field2="+String(temp2)+"&field3="+String(humidity)+"&field4="+String(lux1);
  updateThingSpeak(tsData);

  delay(10000);
}

void UpdateVariables(void)
{ 
  DS18B20.requestTemperatures(); // Send the command to get temperatures
  temp1=DS18B20.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temp1);
  Serial.println(" oC");

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
  
}

void updateThingSpeak(String tsData) {
  WiFiClient client;
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + THINGSPEAK_API_KEY + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);

    if (client.connected()) {
      Serial.println("Dato enviado correctamente: "+ tsData);
      Serial.println();
    }
  }
}
