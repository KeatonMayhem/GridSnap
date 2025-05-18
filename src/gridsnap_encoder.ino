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
###  version 2.1
### Added EEPROM saving of waypoint
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <SoftwareSerial.h>
#include <UTMConversion.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Encoder.h>
#include <FlashStorage.h>
#include "graphics.h"

#define gpsSerial Serial1


// === Screen Config ===
#define SCREEN_I2C_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RST_PIN -1

// Rotary encoder pins
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define ENCODER_BTN_PIN 4

// === GPS UART Pins ===
const int RXPin = D7;
const int TXPin = D6;

// === Objects ===
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);
//SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;
GPS_UTM utm;
Encoder knob(ENCODER_PIN_A, ENCODER_PIN_B);


#define FRAME_DELAY (42)
#define FRAME_WIDTH (32)
#define FRAME_HEIGHT (32)

int screenIndex = 0;
int lastPosition = 0;
const int NUM_SCREENS = 4;

int frame = 0;
unsigned long lastFrameUpdate = 0;

// === State Tracking ===
unsigned long bootTime;
bool firstFix = true;
unsigned long fixTime = 0;

float currentLat = 0.0;
float currentLon = 0.0;

// === Waypoint Tracking ===
struct Waypoint {
  float lat;
  float lon;
};

FlashStorage(savedWaypoint, Waypoint);
Waypoint waypoint = {-1, -1};
bool hasWaypoint = false;

float waypointLat = 0.0;
float waypointLon = 0.0;
bool waypointActive = false;

const unsigned long LONG_PRESS_TIME = 1000;
unsigned long lastButtonPress = 0;
bool encoderPressed = false;
bool waypointSet = false;

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

void displayUTMScreen(float lat, float lon) {
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

void displayMotionScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Motion Info:");
  display.setCursor(0, 16);
  display.printf("Speed: %.2f mph", gps.speed.mph());
  display.setCursor(0, 28);
  display.printf("Bearing: %.2f deg", gps.course.deg());
  display.setCursor(0, 40);
  //display.printf("Moving: %s", gps.speed.kmph() > 0.2 ? "YES" : "NO");
  display.printf("Altitude: %.2f ft", gps.altitude.feet());
  display.display();
}

void displayGPSScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("GPS Info:");
  display.setCursor(0, 16);
  display.printf("Satellites: %d", gps.satellites.value());
  display.setCursor(0, 28);
  display.printf("HDOP: %.2f", gps.hdop.hdop());
  display.setCursor(0, 40);

  if (gps.time.isValid()) {
    int localHour = gps.time.hour() - 4;
    if (localHour < 0) localHour += 24;
    display.printf("Time: %02d:%02d:%02d", localHour, gps.time.minute(), gps.time.second());
  } else {
    display.print("Time: --:--:--");
  }

  display.display();
}

void displayWaypointScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Waypoint Tracker");
  if (hasWaypoint) {
    double distance = TinyGPSPlus::distanceBetween(currentLat, currentLon, waypoint.lat, waypoint.lon);
    double course = TinyGPSPlus::courseTo(currentLat, currentLon, waypoint.lat, waypoint.lon);
    display.setCursor(0, 16);
    display.printf("Dist: %.1fm", distance);
    display.setCursor(0, 28);
    display.printf("Bear: %.1f deg", course);

    utm.UTM(waypoint.lat, waypoint.lon);
    display.setCursor(0, 40);
    display.printf("UTM: %07dE", utm.X());
    display.setCursor(0, 52);
    display.printf("     %07dN", utm.Y());
  } else {
    display.setCursor(0, 16);
    display.println("(No waypoint)");
  }
  display.display();
}

// === Arduino Setup ===
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  pinMode(ENCODER_BTN_PIN, INPUT_PULLUP);


  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
  showBootLogo();
  showMessage("Connecting to GPS...");
  bootTime = millis();

  showAcquiringAnimation();  // Wait until we get a valid GPS fix
  waypoint = savedWaypoint.read();
  if (waypoint.lat != -1 && waypoint.lon != -1) hasWaypoint = true;
}

// === Main Loop ===
void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    currentLat = gps.location.lat();
    currentLon = gps.location.lng();

    if (firstFix) {
      fixTime = (millis() - bootTime) / 1000;
      Serial.print("GPS Lock Acquired in ");
      Serial.print(fixTime);
      Serial.println(" seconds");
      firstFix = false;
    }
  }

  int newPosition = knob.read() / 4;
  if (newPosition != lastPosition) {
    screenIndex = (screenIndex + (newPosition > lastPosition ? 1 : -1) + NUM_SCREENS) % NUM_SCREENS;
    lastPosition = newPosition;
  }

  // Check for encoder press (assumes connected to digitalRead capable pin)
  int buttonState = digitalRead(4); // adjust to actual encoder button pin
  if (buttonState == LOW) {
    if (!encoderPressed) {
      encoderPressed = true;
      lastButtonPress = millis();
    }
    if (millis() - lastButtonPress > 1000 && gps.location.isValid()) {
      waypoint.lat = currentLat;
      waypoint.lon = currentLon;
      savedWaypoint.write(waypoint);
      hasWaypoint = true;
      showMessage("Waypoint Saved");
      delay(1000);
    }
  } else {
    encoderPressed = false;
  }

  if (!gps.location.isValid()) {
    showAcquiringAnimation();
    return;
  }

  switch (screenIndex) {
    case 0:
      displayUTMScreen(currentLat, currentLon);
      break;
    case 1:
      displayWaypointScreen();
      break;
    case 2:
      displayMotionScreen();
      break;
    case 3:
      displayGPSScreen();
      break;
  }

  delay(200);
}
