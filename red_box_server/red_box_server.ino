#include <ESP8266HTTPClient.h>

#include <BufferedPrint.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include "certs.h"

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

const int RELAY_PIN = 5;

const char wireless_ssid[] = "24-25-UNC-PSK";
const char wireless_pass[] = "DeepSenseOfService";
byte mac[6];

// void setup() {

//   Serial.begin(115200);
//   pinMode(RELAY_PIN, OUTPUT);
//   pinMode(LED_BUILTIN, OUTPUT);
//   // Serial.setDebugOutput(true);

//   Serial.println();
//   Serial.println();
//   Serial.println();

//   for (uint8_t t = 4; t > 0; t--) {
//     Serial.printf("[SETUP] WAIT %d...\n", t);
//     Serial.flush();
//     delay(1000);
//   }

//   WiFi.mode(WIFI_STA);
//   WiFiMulti.addAP(wireless_ssid, wireless_pass);

//   WiFi.macAddress(mac);
//   Serial.print("MAC: ");
//   Serial.print(mac[5], HEX);
//   Serial.print(":");
//   Serial.print(mac[4], HEX);
//   Serial.print(":");
//   Serial.print(mac[3], HEX);
//   Serial.print(":");
//   Serial.print(mac[2], HEX);
//   Serial.print(":");
//   Serial.print(mac[1], HEX);
//   Serial.print(":");
//   Serial.println(mac[0], HEX);
// }

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wireless_ssid, wireless_pass);

  Serial.println("Connecting to WiFi...");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);  // Give WiFi some time to connect
  }

  Serial.println("WiFi connected!");
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  for (int i = 5; i >= 0; i--) {
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}


// void loop() {
//   // wait for WiFi connection
//   if ((WiFiMulti.run() == WL_CONNECTED)) {

//     std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

//     //client->setFingerprint(fingerprint_sni_cloudflaressl_com);
//     // Or, if you're happy to ignore the SSL certificate, then use the following line instead:
//     client->setInsecure();

//     HTTPClient http;

//     Serial.print("[HTTP] begin...\n");
//     if (http.begin(*client, "http://tmo.chapelthrillescapes.com/api/room2/lights")) {  // HTTP

//       Serial.print("[HTTP] POST...\n");
//       // Set your desired payload here
//       String payload = "the_truth_is_out_there";  // Your specified password payload

//       int httpCode = http.POST(payload); // Send request with the payload

//       // Check if the HTTP request was sent successfully
//       if (httpCode > 0) {
//         // Print "request sent" when the request was successfully sent
//         Serial.println("Request sent");

//         // HTTP header has been sent and Server response header has been handled
//         Serial.printf("[HTTP] POST... code: %d\n", httpCode);

//         // File found at server
//         if (httpCode == HTTP_CODE_OK) {
//           String responsePayload = http.getString();
//           if (responsePayload == "false") {
//             Serial.printf("Locking");
//             digitalWrite(LED_BUILTIN, HIGH);
//             digitalWrite(RELAY_PIN, LOW);
//           } else {
//             digitalWrite(LED_BUILTIN, LOW);
//             digitalWrite(RELAY_PIN, HIGH);
//           }
//           Serial.printf("%s\n", responsePayload.c_str());
//         }
//       } else {
//         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
//       }

//       http.end();
//     } else {
//       Serial.printf("[HTTP] Unable to connect\n");
//     }
//   }

//   delay(1000);
// }

void loop() {
  if (WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("WiFi connected!");

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    // client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    client->setInsecure();

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(*client, "http://tmo.chapelthrillescapes.com/api/room2/lights")) {  // HTTP
      Serial.print("[HTTP] POST...\n");
      String payload = "the_truth_is_out_there";  // Your specified payload
      int httpCode = http.POST(payload);

      // Check if the HTTP request was sent successfully
      if (httpCode > 0) {
        Serial.println("Request sent");

        // HTTP header has been sent and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {
          String responsePayload = http.getString();
          if (responsePayload == "false") {
            Serial.printf("Locking");
            digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(RELAY_PIN, LOW);
          } else {
            digitalWrite(LED_BUILTIN, LOW);
            digitalWrite(RELAY_PIN, HIGH);
          }
          Serial.printf("%s\n", responsePayload.c_str());
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP] Unable to connect\n");
    }
  } else {
    Serial.println("WiFi not connected yet.");
  }

  delay(1000);
}

