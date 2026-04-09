/*
###   ██████╗ ██████╗ ██╗██████╗ ███████╗███╗   ██╗ █████╗ ██████╗ 
###  ██╔════╝ ██╔══██╗██║██╔══██╗██╔════╝████╗  ██║██╔══██╗██╔══██╗
###  ██║  ███╗██████╔╝██║██║  ██║███████╗██╔██╗ ██║███████║██████╔╝
###  ██║   ██║██╔══██╗██║██║  ██║╚════██║██║╚██╗██║██╔══██║██╔═══╝ 
###  ╚██████╔╝██║  ██║██║██████╔╝███████║██║ ╚████║██║  ██║██║     
###   ╚═════╝ ╚═╝  ╚═╝╚═╝╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝     
###                                                              
###  Created by KeatonMayhem (C) 2025
###  https://github.com/KeatonMayhem
###  version 1.2
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <UTMConversion.h>
#include <TinyGPSPlus.h>
#include "graphics.h"

// === Screen Config ===
#define SCREEN_I2C_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RST_PIN -1

// === GPS UART Pins ===
const int RXPin = D7;
const int TXPin = D6;

// === Objects ===
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);
SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;
GPS_UTM utm;

#define FRAME_DELAY (42)
#define FRAME_WIDTH (32)
#define FRAME_HEIGHT (32)

int frame = 0;
unsigned long lastFrameUpdate = 0;

// === State Tracking ===
unsigned long bootTime;
bool firstFix = true;
unsigned long fixTime = 0;

// === Display Helpers ===
void showMessage(const char* msg, int textSize = 1) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

void showBootLogo() {
  display.clearDisplay();
  display.drawBitmap(0, 0, gridSnapLogo, 128, 64, WHITE);
  display.display();
  delay(2000);
}

void showAcquiringAnimation() {
  while (!gps.location.isValid()) {
    if (millis() - lastFrameUpdate > FRAME_DELAY) {
      display.clearDisplay();
      display.drawBitmap(48, 16, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, WHITE);
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Acquiring GPS...");
      display.display();

      frame = (frame + 1) % (sizeof(frames) / sizeof(frames[0]));
      lastFrameUpdate = millis();
    }
    if (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
    }
  }
}

void displayUTM(float lat, float lon) {
  utm.UTM(lat, lon);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Zone: ");
  display.print(utm.zone());
  display.print(utm.band());

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.printf("%07d", utm.X());
  display.print(" E");

  display.setCursor(0, 38);
  display.printf("%07d", utm.Y());
  display.print(" N");

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print("Fix in ");
  display.print(fixTime);
  display.print("s");

  display.display();
}

// === Arduino Setup ===
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
  showBootLogo();
  showMessage("Connecting to GPS...");
  bootTime = millis();

  showAcquiringAnimation();  // Wait until we get a valid GPS fix
}

// === Main Loop ===
void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    float lat = gps.location.lat();
    float lon = gps.location.lng();

    if (firstFix) {
      fixTime = (millis() - bootTime) / 1000;
      Serial.print("GPS Lock Acquired in ");
      Serial.print(fixTime);
      Serial.println(" seconds");
      firstFix = false;
    }

    displayUTM(lat, lon);
  }
}
