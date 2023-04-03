#include <pop7.h>
#include <SoftwareSerial.h>
#include "HUSKYLENS.h"
SoftwareSerial mySerial(30, 29); //rx,tx
HUSKYLENS on; //declare varible instead of HUSKYLENS
int fullyDimension = 9450;
int total_package = 0, failed_package = 0, passed_package = 0;

float dim;
int index;
bool sdChk;

void printResult(HUSKYLENSResult result);
void printResultF(HUSKYLENSResult result);

void setup() {
  glcdMode(3);
  setTextSize(1);
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(100);
  Wire.begin();
  while (!on.begin(Wire)) {
    Serial.println(F("Failed to connect, Green wire >> SCL, Orange wire >> SDA"));
    delay(200);
  }
  Serial.println(F("Successfully connected"));
  //F() is macro that will store a string in a flash momory instead of store in SRAM.
  //This is useful when we need to save memory

  on.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION); //fixed algorithm in huskylens
  on.setCustomName("Side dish", 1); on.setCustomName("Rice", 2); on.setCustomName("Error", 3);
}

void loop() {
  setTextColor(GLCD_BLUE);
  glcd(0, 10, "Total= %d ", total_package);
  setTextColor(GLCD_RED);
  glcd(1, 10, "Failed= %d ", failed_package);
  setTextColor(GLCD_GREEN);
  glcd(2, 10, "Pass= %d ", passed_package);
  setTextColor(GLCD_WHITE);

  if (read_ir(A0) <= 12) {
    total_package += 1;
  }

  if (on.requestBlocks()) {
    if (on.countBlocks(1) == 1) {
      glcd(5, 0, "Rice detected!     ");
      sdChk = false;
      if (on.countBlocks(3) == 1) {
        setTextColor(GLCD_RED);
        glcd(5, 19, "Error!");
        index = 1;
        for (int i = 0; i < on.countBlocks(3); i++) {
          HUSKYLENSResult result = on.get(3, i);
          printResultF(result);
          delay(300);
        }
      } else {
        for (int i = 0; i < on.countBlocks(1); i++) {
          HUSKYLENSResult result = on.get(1, i);
          printResult(result);
          delay(300);
        }
      }
    }

    if (on.countBlocks(2) == 1) {
      glcd(5, 0, "Side dish detected!     ");
      sdChk = true;
      if (on.countBlocks(3) == 1) {
        setTextColor(GLCD_RED);
        glcd(5, 19, "Error!");
        index = 2;
        for (int i = 0; i < on.countBlocks(3); i++) {
          HUSKYLENSResult result = on.get(3, i);
          printResultF(result);
          delay(300);
        }
      } else {
        for (int i = 0; i < on.countBlocks(2); i++) {
          HUSKYLENSResult result = on.get(2, i);
          printResult(result);
          delay(300);
        }
      }
    }

    if (on.countBlocks(1) == 0 && on.countBlocks(2) == 0 && on.countBlocks(3) == 0) {
      glcd(5, 0, "Waiting for packages...");
      glcd(6, 0, "                         ");
      glcd(7, 0, "                         ");
    }
  }
}

void printResult(HUSKYLENSResult result) {
  glcd(0, 0, "X = %d ", result.xCenter);
  glcd(1, 0, "Y = %d ", result.yCenter);
  glcd(2, 0, "W = %d ", result.height);
  glcd(3, 0, "H = %d ", result.width);
  dim = result.width * result.height;
  Serial.println(dim);
  glcd(6, 0, "Dimension= %f ", dim);
  float percentage = (dim / fullyDimension) * 100;
  glcd(7, 0, "Integrity= %f% ", percentage);
  //on.setTimeOutDuration(10000); //millisecond format
  if (percentage > 80) {
    Serial.print("Pass");
    mySerial.write('P');
    if(sdChk == true){
      passed_package += 1;
      //move package into passed storage
    }
  } else {
    Serial.print("Failed");
    servo(1, 90);
    delay(600);
    mySerial.write('F');
    failed_package += 1;
    //move this package into failed parcel
  }
}

void printResultF(HUSKYLENSResult result) {
  setTextColor(GLCD_GREEN);
  glcd(0, 0, "X = %d ", result.xCenter);
  glcd(1, 0, "Y = %d ", result.yCenter);
  glcd(2, 0, "W = %d ", result.height);
  glcd(3, 0, "H = %d ", result.width);
  float falseDim = result.width * result.height;
  for (int i = 0; i < on.countBlocks(index); i++) {
    HUSKYLENSResult result = on.get(index, i);
    dim = result.height * result.width;
  }
  float newFalse = dim - falseDim;
  float falsePercentage = (newFalse / fullyDimension) * 100;
  glcd(6, 0, "Dimension= %f ", falseDim);
  glcd(7, 0, "Total area= %f% ", falsePercentage);
  if (falsePercentage > 80.00) {
    Serial.print("Pass");
    mySerial.write('P');
    if(sdChk == true){
      passed_package += 1;
      //move package into passed storage
    }
  } else {
    Serial.print("Failed");
    servo(1, 90);
    delay(600);
    mySerial.write('F');
    failed_package += 1;
  }
}

float read_ir(int pin) { //cm format
  float volts = analogRead(pin) * 0.0048828125;
  float distance = 13 * pow(volts, -1);
  if (distance <= 30.0f) {
    return distance;
  }
}
