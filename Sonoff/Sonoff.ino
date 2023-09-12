#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

MDNSResponder mdns;
ESP8266WebServer server(80);
String webPage = "";

//let the LED know ur wifi 
const char* ssid = "DQSMART";
const char* password = "********";

#define gpioButton 0
#define gpioLed 13 // for checking while debugging (without AC~)
#define gpioRelay 12

bool ledState = false; // Trạng thái hiện tại của LED
bool lastButtonState = HIGH; // Trạng thái nút nhấn lần trước
unsigned long lastDebounceTime = 0; // Thời gian gần đây nhất nút nhấn được đọc
unsigned long debounceDelay = 50;

void setup(void){
 // preparing GPIOs
  pinMode(gpioLed, OUTPUT);
  pinMode(gpioButton, INPUT);
  pinMode(gpioRelay, OUTPUT); 
  Serial.begin(115200); 
  delay(5000);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
 if (mdns.begin("esp8266", WiFi.localIP())) {
  Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, [](void){
    String ledStatus = ledState ? "ON" : "OFF";
    String html = "<html><body>";
    html += "<h1>ESP8285 LED Control</h1>";
    html += "<p>LED is " + ledStatus + "</p>";
    html += "<p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/on", HTTP_GET, [](void){
    String ledStatus = ledState ? "ON" : "OFF";
    String plain = "<html><body>";
    plain += "<h1>ESP8285 LED Control</h1>";
    plain += "<p>LED is " + ledStatus + "</p>";
    plain += "</body></html>";
    toggleLed_on();
    server.send(200, "text/plain", plain);
  });

  server.on("/off", HTTP_GET, [](void){
 //String ledStatus = ledState ? "ON" : "OFF";
  String b = "<html><body>";
  b += "<h1>ESP8285 LED Control</h1>";
  b += "<p>LED is " + ledStatus + "</p>";
  b += "</body></html>";
  toggleLed_off();
  server.send(200, "text/plain", b);
  });
  
  server.begin();
}

void toggleLed_on() {
 digitalWrite(gpioLed, HIGH);
 digitalWrite(gpioRelay, HIGH);
}

void toggleLed_off() {
 digitalWrite(gpioLed, LOW);
 digitalWrite(gpioRelay, LOW);
}

void loop(void){
  server.handleClient();
  int buttonState = digitalRead(gpioButton);
  if (buttonState != lastButtonState) {
  delay(50); // Debounce delay
    if (buttonState == LOW) {
      ledState = !ledState;
      digitalWrite(gpioLed, ledState ? HIGH : LOW);
      digitalWrite(gpioRelay, ledState ? HIGH : LOW);
      }
    }
  lastButtonState = buttonState;
}