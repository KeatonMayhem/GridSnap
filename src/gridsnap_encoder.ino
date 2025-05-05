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
###  version 2.0
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <UTMConversion.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Encoder.h>
#include "graphics.h"




// === Screen Config ===
#define SCREEN_I2C_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RST_PIN -1

#define gpsSerial Serial1

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

float waypointLat = 0.0;
float waypointLon = 0.0;
bool waypointActive = false;

const unsigned long LONG_PRESS_TIME = 1000;
bool buttonHeld = false;
unsigned long buttonPressStart = 0;
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

void saveWaypoint() {
  if (gps.location.isValid()) {
    waypointLat = gps.location.lat();
    waypointLon = gps.location.lng();
    waypointActive = true;
    Serial.println("Waypoint saved.");
  }
}

void checkEncoderButton() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(ENCODER_BTN_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Button just pressed
    buttonPressStart = millis();
    buttonHeld = false;
  } else if (lastButtonState == LOW && currentButtonState == LOW) {
    // Button is being held
    if (!buttonHeld && (millis() - buttonPressStart >= LONG_PRESS_TIME)) {
      saveWaypoint();
      buttonHeld = true;
    }
  }
  lastButtonState = currentButtonState;
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

  if (waypointActive && gps.location.isValid()) {
    double distance = TinyGPSPlus::distanceBetween(
        gps.location.lat(), gps.location.lng(), waypointLat, waypointLon);
    double course = TinyGPSPlus::courseTo(
        gps.location.lat(), gps.location.lng(), waypointLat, waypointLon);

    utm.UTM(waypointLat, waypointLon);

    display.setCursor(0, 16);
    display.printf("Dist: %.1f m", distance);

    display.setCursor(0, 28);
    display.printf("Bear: %.1f deg", course);

    display.setCursor(0, 40);
    display.printf("UTM: %d%s", utm.zone(), utm.band());

    display.setCursor(0, 52);
    display.printf("%07d E", utm.X());
    display.setCursor(64, 52);
    display.printf("%07d N", utm.Y());
  } else {
    display.setCursor(0, 20);
    display.println("No waypoint set.");
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
}

// === Main Loop ===
void loop() {
  // Read GPS data
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // Update stored coordinates if new data is available
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

  // Rotary encoder handling
  int newPosition = knob.read() / 4;
  if (newPosition != lastPosition) {
    screenIndex = (screenIndex + (newPosition > lastPosition ? 1 : -1) + NUM_SCREENS) % NUM_SCREENS;
    lastPosition = newPosition;
  }

  // 🆕 Check for long-press on encoder button
  checkEncoderButton();

  // If GPS is invalid, show animation
  if (!gps.location.isValid()) {
    showAcquiringAnimation();
    return;
  }

  // Display screen based on index using stored lat/lon
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

  delay(200);  // Reduce flicker and CPU load
}
