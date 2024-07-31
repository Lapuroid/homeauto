#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "wifi_name";
const char* password = "wifi_pass";

// GPIO pins for devices
const int ledPin = D4;
const int fanPin = D5; // Example pin for fan

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi... ");
    Serial.print(WiFi.status());
    Serial.println();
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<html><body>OPEN THE WEBPAGE, NOT THIS</body></html>");
  });

  // Route to control devices
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("device", true) && request->hasParam("status", true)) {
      String device = request->getParam("device", true)->value();
      String status = request->getParam("status", true)->value();
      Serial.print("Received request: Device = ");
      Serial.print(device);
      Serial.print(", Status = ");
      Serial.println(status);

      AsyncWebServerResponse* response;

      if (device == "led") {
        if (status == "on") {
          digitalWrite(ledPin, LOW); // Turn on LED (assuming LOW is ON)
          response = request->beginResponse(200, "text/plain", "LED turned on");
        } else if (status == "off") {
          digitalWrite(ledPin, HIGH); // Turn off LED (assuming HIGH is OFF)
          response = request->beginResponse(200, "text/plain", "LED turned off");
        } else {
          response = request->beginResponse(400, "text/plain", "Invalid status for LED");
        }
      } else if (device == "fan") {
        if (status == "on") {
          digitalWrite(fanPin, LOW); // Turn on Fan (assuming LOW is ON)
          response = request->beginResponse(200, "text/plain", "Fan turned on");
        } else if (status == "off") {
          digitalWrite(fanPin, HIGH); // Turn off Fan (assuming HIGH is OFF)
          response = request->beginResponse(200, "text/plain", "Fan turned off");
        } else {
          response = request->beginResponse(400, "text/plain", "Invalid status for Fan");
        }
      } else {
        response = request->beginResponse(400, "text/plain", "Invalid device");
      }

      response->addHeader("Access-Control-Allow-Origin", "*");
      response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(response);
    } else {
      Serial.println("Error: Missing device or status parameter");
      AsyncWebServerResponse* response = request->beginResponse(400, "text/plain", "Missing device or status parameter");
      response->addHeader("Access-Control-Allow-Origin", "*");
      response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(response);
    }
  });

  // Route to handle CORS preflight requests
  server.on("/update", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse* response = request->beginResponse(204); // No content
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  server.begin();
}

void loop() {
  // No need for code here
}
