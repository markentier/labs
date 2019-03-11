extern "C" {
#include "user_interface.h"
}
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>

#include "wifi_settings.h"
#include "slack_settings.h"

// #define CDATE __DATE__
// #define CTIME __TIME__
// #define CTS __TIMESTAMP__

#ifdef DEBUG
#define DEBUG_SERIAL_INIT(baud) Serial.begin(baud)
#define DEBUG_PRINT(data) Serial.print(data)
#define DEBUG_PRINTLN(data) Serial.println(data)
#else
#define DEBUG_SERIAL_INIT(...) (void)0
#define DEBUG_PRINT(...) (void)0
#define DEBUG_PRINTLN(...) (void)0
#endif

#define HOLD_PIN 0
#define NEOPIXEL_PIN 2
#define NEOPIXEL_LED_COUNT 24

#define DEFAULT_LOOPS 3
#define DEFAULT_DELAY 125

#define SHORT_LOOPS 9
#define SHORT_DELAY 33
#define BLIP_LOOPS 3

void initialize_it();
void initialize_pins();

void run_tasks();

void connect_wifi();
void disable_wifi();
void setClock();

void send_message_with_retry();
bool send_message();

void colorWipe(uint32_t c, uint8_t wait);
void dotted(uint32_t c, uint8_t wait);
uint32_t Wheel(byte WheelPos);
