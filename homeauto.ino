#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Vivo 1901";
const char* password = "Jul_2822";

// GPIO pins connected to the LED and fan
const int ledPin = D4;
const int fanPin = D5;
String ledStatus = "off";
String fanStatus = "off";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200); // For debugging
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Initialize LED as off
  digitalWrite(fanPin, HIGH); // Initialize fan as off

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<html><body>OPEN THE WEBPAGE, NOT THIS</body></html>");
  });

  // Route to control LED and fan with CORS headers
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("device", true) && request->hasParam("status", true)) {
      String device = request->getParam("device", true)->value();
      String status = request->getParam("status", true)->value();
      AsyncWebServerResponse* response;
      if (device == "led") {
        if (status == "on") {
          digitalWrite(ledPin, LOW);
          ledStatus = "on";
          response = request->beginResponse(200, "text/plain", "LED turned on");
        } else if (status == "off") {
          digitalWrite(ledPin, HIGH);
          ledStatus = "off";
          response = request->beginResponse(200, "text/plain", "LED turned off");
        } else {
          response = request->beginResponse(400, "text/plain", "Invalid LED command");
        }
      } else if (device == "fan") {
        if (status == "on") {
          digitalWrite(fanPin, LOW);
          fanStatus = "on";
          response = request->beginResponse(200, "text/plain", "Fan turned on");
        } else if (status == "off") {
          digitalWrite(fanPin, HIGH);
          fanStatus = "off";
          response = request->beginResponse(200, "text/plain", "Fan turned off");
        } else {
          response = request->beginResponse(400, "text/plain", "Invalid fan command");
        }
      } else {
        response = request->beginResponse(400, "text/plain", "Invalid device");
      }
      response->addHeader("Access-Control-Allow-Origin", "*");
      response->addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(response);
    } else {
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
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("LED status: ");
  Serial.println(ledStatus);
  Serial.print("Fan status: ");
  Serial.println(fanStatus);
  delay(2000);
}
