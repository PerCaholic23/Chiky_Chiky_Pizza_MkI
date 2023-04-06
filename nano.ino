#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int val = 0;

SoftwareSerial mySerial(12, 11);

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(100);
  pinMode(PD4, OUTPUT);
  pinMode(PD3, OUTPUT);
  if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Allocation failed");
  } else {
    Serial.println("Started");
  }
}

void loop() {
  mySerial.listen();
  while (mySerial.available() > 0) {
    char inByte = mySerial.read();
    if (inByte == 'P') {
      digitalWrite(PD4, HIGH);
      digitalWrite(PD3, LOW);
      oledd("Pass",40,25);
    }
    if (inByte == 'F') {
      digitalWrite(PD4, LOW);
      digitalWrite(PD3, HIGH);
      oledd("Failed",32,25);
    }
  }
}

void oledd(String text, int x, int y){
  OLED.clearDisplay(); //clear all things on screen
  OLED.setTextColor(WHITE, BLACK); //determines WHITE text and BLACK bg
  OLED.setCursor(x,y); //determines x,y location
  OLED.setTextSize(2); //determines size
  OLED.println(text);
  OLED.display();
}
