#include "common.h"

ESP8266WiFiMulti wifiMulti;
BearSSL::WiFiClientSecure client;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_LED_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  initialize_it();
  run_tasks();

  for(int b = 100; b > -1; b--) {
    strip.setBrightness(b);
    strip.show();
    delay(12);
  }
  delay(100);
  DEBUG_PRINTLN(F("(turn off)"));
  digitalWrite(HOLD_PIN, LOW);
  delay(1000);
}

void loop() {}

//------------------------------------------------------------------------------

void initialize_it() {
  DEBUG_SERIAL_INIT(115200);
  initialize_pins();
  strip.begin();
  strip.setBrightness(0);
  strip.show();
  strip.setBrightness(100);
  DEBUG_PRINTLN(F("(initialized)"));
}

void initialize_pins() {
  pinMode(HOLD_PIN, OUTPUT);
  digitalWrite(HOLD_PIN, HIGH);
}

//------------------------------------------------------------------------------

void run_tasks() {
  colorWipe(strip.Color(0,0,16), 50);
  colorWipe(strip.Color(0,0,0), 12);

  connect_wifi();

  colorWipe(strip.Color(32,0,32), 25);
  colorWipe(strip.Color(0,0,0), 12);

  setClock();

  colorWipe(strip.Color(64,64,0), 25);
  colorWipe(strip.Color(0,0,0), 6);

  send_message_with_retry();

  disable_wifi();
}

//------------------------------------------------------------------------------

void connect_wifi() {
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PW);
  while(wifiMulti.run() != WL_CONNECTED) {
    colorWipe(strip.Color(0,0,64), 12);
    colorWipe(strip.Color(0,0,0), 12);
  }

  DEBUG_PRINTLN(F("WiFi connected"));
  DEBUG_PRINTLN(F("IP address: "));
  DEBUG_PRINTLN(WiFi.localIP());

  client.setFingerprint(SLACK_SSL_FINGERPRINT);
}

void disable_wifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "ptbtime1.ptb.de", "zeit.fu-berlin.de");

  DEBUG_PRINT(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    DEBUG_PRINT(F("."));
    now = time(nullptr);
  }
  DEBUG_PRINTLN("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  DEBUG_PRINT(F("Current time: "));
  DEBUG_PRINT(asctime(&timeinfo));
}

//------------------------------------------------------------------------------

void send_message_with_retry() {
  bool success = false;

  for(int r = 0; r < 5; r++) {
    DEBUG_PRINT(F("Attempt #")); DEBUG_PRINTLN(r + 1);
    success = send_message();
    if(success) break;
    colorWipe(strip.Color(64,24,0), 50);
    colorWipe(strip.Color(0,0,0), 50);
    delay(250);
  }

  if(success) {
    DEBUG_PRINTLN(F("Message sent."));
    colorWipe(strip.Color(0,64,0), 100);
    //colorWipe(strip.Color(0,0,0), 100);
  } else {
    DEBUG_PRINTLN(F("... message sending failed."));
    colorWipe(strip.Color(64,0,0), 100);
    //colorWipe(strip.Color(0,0,0), 50);
  }
}

bool send_message() {
/*
{
    "attachments": [
        {
            "fallback": "Check the kitchen, there might be fresh coffee! :chemex: :coffee: :praisethesun:",
            "color": "#331100",
            "title": "Check the kitchen, there might be fresh coffee! :coffee:",
            "footer": "Coffee Notification Button by Christoph",
            "ts": 150123123
        }
    ]
}
*/

  DynamicJsonBuffer jsonBuffer;
  DynamicJsonBuffer attachmentBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& attachments = json.createNestedArray("attachments");

  JsonObject& attachment = attachmentBuffer.createObject();
  attachment["fallback"] = String("Check the kitchen, there might be fresh coffee! :chemex: :coffee: :praisethesun:");
  attachment["color"] = String("#552200");
  attachment["title"] = String("Check the kitchen, there might be fresh coffee! :chemex:");
  attachment["text"] = String("Location: BER-2-SW-Kitchen-RR");
  attachment["footer"] = String("Coffee Notification Button by Christoph");

  time_t now = time(nullptr);
  attachment["ts"] = now;

  attachments.add(attachment);

  DEBUG_PRINTLN(F("Connecting to host ..."));
  if (!client.connect(F(SLACK_WH_HOST), 443)) {
    DEBUG_PRINTLN(F("Connection failed"));

    client.stop();
    return false;
  }
  DEBUG_PRINTLN(F("Connected to host"));

  String message;
  json.printTo(message);

  String request = "";
    request += "POST " + String(F(SLACK_WH_PATH)) + " HTTP/1.1\r\n";
    request += "Host: " + String(F(SLACK_WH_HOST)) + "\r\n";
    request += "User-Agent: esp8266/arduino/slack-test\r\n";
    request += "Accept: application/json\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Connection: close\r\n";
    request += "Content-Length: "+ String(message.length()) + "\r\n";
    request += "\r\n";
    request += message;

  DEBUG_PRINTLN(request);
  client.print(request);

  long timeout = millis() + 5000;
  while(!client.available()) {
    dotted(strip.Color(32,16,0), 12);
    dotted(strip.Color(0,0,0), 6);
    if (millis() > timeout) {
      DEBUG_PRINTLN(F("Request timed out"));
      client.stop();
      return false;
    }
  }

  client.stop();
  return true;
}


//--- adafruit neopixel examples

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void dotted(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i % 3 != 0) continue;
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
//}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
