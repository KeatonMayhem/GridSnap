#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <UTMConversion.h>
#include <TinyGPSPlus.h>

#define SCREEN_I2C_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RST_PIN -1

const int RXPin = D7;
const int TXPin = D6;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);
SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;
GPS_UTM utm;

void showMessage(const char* msg, int textSize = 1) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
  showMessage("Booting...");
  delay(1000);
  showMessage("Connecting to GPS...");
}

void displayUTM(float lat, float lon) {
  utm.UTM(lat, lon);  // Convert to UTM

  display.clearDisplay();

  // Line 1: UTM Zone + Band
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Zone: ");
  display.print(utm.zone());
  display.print(utm.band());

  // Line 2: Easting
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.printf("%07d", utm.X());
  display.print(" E");

  // Line 3: Northing
  display.setCursor(0, 38);
  display.printf("%07d", utm.Y());
  display.print(" N");

  display.display();
}


void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());

    if (gps.location.isUpdated()) {
      static bool firstFix = true;

      float lat = gps.location.lat();
      float lon = gps.location.lng();

      displayUTM(lat, lon);

      if (firstFix) {
        Serial.println("GPS Lock Acquired");
        firstFix = false;
      }
    }
  }
}
