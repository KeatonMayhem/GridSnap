#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

// === Display Setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Encoder Setup ===
Encoder myEnc(2, 3);
const int buttonPin = 4;

// === Screen Navigation ===
enum ScreenID {
  SCREEN_COORDINATES,
  SCREEN_GPS_LOCK,
  SCREEN_SPEED_BEARING,
  SCREEN_WAYPOINT,
  SCREEN_COUNT
};
ScreenID currentScreen = SCREEN_COORDINATES;
long lastEncoderStep = -999;

// === Button States ===
bool buttonPressed = false;
unsigned long buttonDownTime = 0;
bool longPressHandled = false;
const unsigned long LONG_PRESS_MS = 1000;

// === Simulated GPS Data ===
int satCount = 9;
float gpsAge = 1.2;
float speedKph = 2.3;
int bearing = 275;
String utmCoord = "16S 579321 4356789";
unsigned long waypointSetTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);

  // Initialize OLED properly
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);  // Halt
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Booting...");
  display.display();
  delay(1000);

  myEnc.write(0);
  drawCurrentScreen();
}

void loop() {
  handleEncoder();
  handleButton();
  drawCurrentScreen();
  delay(50);  // debounce / rate limit
}

void handleEncoder() {
  long rawPos = myEnc.read();
  long step = rawPos / 4;

  if (step != lastEncoderStep) {
    lastEncoderStep = step;
    currentScreen = (ScreenID)(step % SCREEN_COUNT);
    if (currentScreen < 0) currentScreen = (ScreenID)(SCREEN_COUNT + currentScreen);
  }
}

void handleButton() {
  if (digitalRead(buttonPin) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      buttonDownTime = millis();
      longPressHandled = false;
    } else if (!longPressHandled && millis() - buttonDownTime > LONG_PRESS_MS) {
      longPressHandled = true;
      if (currentScreen == SCREEN_WAYPOINT) {
        waypointSetTime = millis();  // Simulate waypoint set
      }
    }
  } else {
    buttonPressed = false;
  }
}

void drawCurrentScreen() {
  display.clearDisplay();

  switch (currentScreen) {
    case SCREEN_COORDINATES:
      drawCoordinates();
      break;
    case SCREEN_GPS_LOCK:
      drawGPSLock();
      break;
    case SCREEN_SPEED_BEARING:
      drawSpeedBearing();
      break;
    case SCREEN_WAYPOINT:
      drawWaypoint();
      break;
  }

  display.display();
}

// === Screen Content ===

void drawCoordinates() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Coordinates (UTM):");
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("16S");
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("E: 579321");
  display.setCursor(0, 50);
  display.println("N: 4356789");
}

void drawGPSLock() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("GPS Lock Info:");
  display.setCursor(0, 20);
  display.print("Satellites: ");
  display.println(satCount);
  display.setCursor(0, 35);
  display.print("Data Age: ");
  display.print(gpsAge, 1);
  display.println("s");
  display.setCursor(0, 50);
  display.println("Fix Quality: 3D");
}

void drawSpeedBearing() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Motion Info:");
  display.setCursor(0, 20);
  display.print("Speed: ");
  display.print(speedKph, 1);
  display.println(" km/h");
  display.setCursor(0, 35);
  display.print("Bearing: ");
  display.print(bearing);
  display.println(" deg");
  display.setCursor(0, 50);
  display.println("Moving: Yes");
}

void drawWaypoint() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Waypoint Tracker:");
  display.setCursor(0, 20);
  display.println("Distance: 45.3m");
  display.setCursor(0, 35);
  display.println("Bearing: 183 deg");

  display.setCursor(0, 50);
  if (millis() - waypointSetTime < 1500) {
    display.println("Waypoint Set!");
  } else {
    display.println("Hold btn: Set WP");
  }
}
