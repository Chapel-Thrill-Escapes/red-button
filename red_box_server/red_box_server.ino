//working basic version
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;

const int buttonPin = 5;  // GPIO5 (D1)
const int RELAY_PIN = 4;  // Adjust if necessary
bool buttonReleased = true;  // Track when button was last HIGH

const char wireless_ssid[] = "24-25-UNC-PSK";
const char wireless_pass[] = "DeepSenseOfService";
byte mac[6];

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("\nConnecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wireless_ssid, wireless_pass);

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nWiFi connected!");

  // Print the initial button state
  int initialButtonState = digitalRead(buttonPin);
  Serial.print("Initial button state: ");
  Serial.println(initialButtonState == HIGH ? "HIGH" : "LOW");
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && buttonReleased) {  
    delay(50);  // Debounce delay
    if (digitalRead(buttonPin) == LOW) {  // Confirm still LOW
      Serial.println("Button Pressed - Sending Request");
      buttonReleased = false;  

      if (WiFiMulti.run() == WL_CONNECTED) {
        sendHttpRequest();
      } else {
        Serial.println("WiFi not connected.");
      }
    }
  }

  // Wait until button is released before detecting another press
  if (buttonState == HIGH && !buttonReleased) {
    Serial.println("Button Released, waiting for next press...");
    buttonReleased = true;
  }
}

void sendHttpRequest() {
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient http;
  Serial.println("[HTTP] Sending request...");

  if (http.begin(*client, "https://tmo.chapelthrillescapes.com/api/player/room2/lights")) {
    http.addHeader("Content-Type", "text/plain");
    String payload = "the_truth_is_out_there";
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
      Serial.println("Request sent");
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String responsePayload = http.getString();
        Serial.printf("Response: %s\n", responsePayload.c_str());

        if (responsePayload == "false") {
          Serial.println("Locking");
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(RELAY_PIN, LOW);
        } else {
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(RELAY_PIN, HIGH);
        }
      }
    } else {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("[HTTP] Unable to connect");
  }
}