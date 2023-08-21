#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTTYPE DHT11  // DHT 11

const char* ssid = "PECHAN";
const char* password = "Carlos123";

#define LED D1  // LED
// DHT Sensor
uint8_t DHTPin = 2;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float Temperature;
float Humidity;

WiFiServer server(80);

/* LDR Luximeter */
// Constants
#define VIN 3.3  // V power voltage, 3.3v in case of NodeMCU
#define R 10000  // Voltage devider resistor value

// Parameters
const int Analog_Pin = 0;  // Analog pin A0

//Variables
int LDR_Val;     // Analog value from the LDR
int Iluminance;  //Lux value

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);  //LED apagado

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {



  Temperature = dht.readTemperature();  // Gets the values of the temperature
  Humidity = dht.readHumidity();        // Gets the values of the humidity
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  LDR_Val = analogRead(A0);
  Iluminance = conversion(LDR_Val);
  Serial.print("Analog reading from sensor = ");
  Serial.println(LDR_Val); // between 0 -
  Serial.print("Iluminance value = ");
  Serial.print(Iluminance); // Converted value
  Serial.println(" Lux");
  // Match the request

  int value = HIGH;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(LED, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(LED, LOW);
    value = LOW;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");  //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("LED is now: ");

  if (value == LOW) {
    client.print("Off");
  } else {
    client.print("On");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("</html>");

  client.println("<br><br>");
  client.print("Temperature: ");
  client.print(Temperature);
  client.print("C");
  client.println("<br><br>");
  client.print("Humidity: ");
  client.print(Humidity);
  client.print("%");
  client.println("<br><br>");
  client.print("Luminosidad: ");
  client.print(Iluminance);
  client.print("Lux");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println(Temperature);
  Serial.println(Humidity);
  Serial.println(Iluminance);
  Serial.println("");
}

int conversion(int raw_val) {
  // Conversion rule
  float Vout = float(raw_val) * (VIN / float(1023));  // Conversion analog to voltage
  float RLDR = (R * (VIN - Vout)) / Vout;             // Conversion voltage to resistance
  int lux = 500 / (RLDR / 1000);                      // Conversion resitance to lumen
  return lux;
}