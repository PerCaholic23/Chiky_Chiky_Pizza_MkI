//This will display on lcd built in board.
//There's another version that display via serial monoter
//@NOTE@ will be in repo soon I guess

#include <pop7.h>
#include <SoftwareSerial.h>
#include "HUSKYLENS.h"


SoftwareSerial mySerial(30, 29); //rx,tx
HUSKYLENS on; //declare varible instead of HUSKYLENS
int fullyDimension = 19881;
int total_package = 0, failed_package = 0, passed_package = 0;
int del = 550, del_m = 1500;

float dim, dim_backup;

void printResult(HUSKYLENSResult result);
void printResultF(HUSKYLENSResult result);

void setup() {
  glcdMode(3);
  setTextSize(1);
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(100);
  Wire.begin(); //I2c init
  while (!on.begin(Wire)) {
    Serial.println(F("Failed to connect, Green wire >> SCL, Orange wire >> SDA"));
    delay(200);
  }
  Serial.println(F("Successfully connected"));
  //F() is macro that will store a string in a flash momory instead of store in SRAM.
  //This is useful when we need to save memory
  on.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION); //fixed algorithm in huskylens
  on.setCustomName("Pizza", 1); on.setCustomName("Error", 2);
  servo(1, 90);
}

void loop() {
  setTextColor(GLCD_BLUE);
  glcd(0, 10, "Total= %d ", total_package);
  setTextColor(GLCD_RED);
  glcd(1, 10, "Failed= %d ", failed_package);
  setTextColor(GLCD_GREEN);
  glcd(2, 10, "Pass= %d ", passed_package);
  setTextColor(GLCD_WHITE);

  while (on.countBlocks() == 0) {
    on.requestBlocks();
    motor(4, -65);
  }

  if (on.requestBlocks()) {
    if (on.countBlocks(1) == 1) {
      glcd(5, 0, "Detected!                ");
      if (on.countBlocks(2) == 1) {
        total_package += 1;
        delay(del);
        motor(4, 0);
        delay(del);
        on.requestBlocks();
        setTextColor(GLCD_RED);
        glcd(5, 19, "Error!");
        for (int i = 0; i < on.countBlocks(2); i++) {
          HUSKYLENSResult result = on.get(2, i);
          printResultF(result);
          delay(300);
        }
      } else {
        total_package += 1;
        delay(del);
        motor(4, 0);
        delay(del);
        on.requestBlocks();
        for (int i = 0; i < on.countBlocks(1); i++) {
          HUSKYLENSResult result = on.get(1, i);
          printResult(result);
          delay(300);
        }
      }
    }

    if (on.countBlocks(1) == 0 && on.countBlocks(2) == 0) {
      mySerial.write('W');
      glcd(5, 0, "Waiting for packages...        ");
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
  dim_backup = result.width * result.height;
  glcd(6, 0, "Dimension= %f ", dim_backup);
  float percentage = (dim_backup / fullyDimension) * 100;
  glcd(7, 0, "Integrity= %f% ", percentage);
  //on.setTimeOutDuration(10000); //millisecond format
  if (percentage > 80) {
    swnd('P');
    Serial.print("Pass");
    mySerial.write('P');
    passed_package += 1;
    while (read_ir(A2) > 12) {
      motor(4, -50);
    }
    delay(10);
    motor(4, 0);
  } else {
    swnd('F');
    Serial.print("Failed");
    mySerial.write('F');
    failed_package += 1;
    while (read_ir(A1) > 12) {
      motor(4, -50);
    }
    delay(del_m);
    motor(4, 0);
//    while (read_ir(A1) < 12) {
//      motor(4, -50);
//    }
//    delay(10);
//    motor(4, 0);
    delay(500);
    servo(1, 30);
    delay(750);
    servo(1, 90);
  }
}

void printResultF(HUSKYLENSResult result) {
  setTextColor(GLCD_GREEN);
  glcd(0, 0, "X = %d ", result.xCenter);
  glcd(1, 0, "Y = %d ", result.yCenter);
  glcd(2, 0, "W = %d ", result.height);
  glcd(3, 0, "H = %d ", result.width);
  float falseDim = result.width * result.height;
  for (int i = 0; i < on.countBlocks(1); i++) {
    HUSKYLENSResult result = on.get(1, i);
    dim = result.height * result.width;
  }
  float newFalse = abs(dim - falseDim);
  float falsePercentage = (newFalse / fullyDimension) * 100;
  float true_area = (falseDim / fullyDimension) * 100;
  true_area = 100 - true_area;
  //  if(falsePercentage > truePercentage){
  //    float sum = falsePercentage - truePercentage;
  //    glcd(7, 0, "Total area= %f% ", sum);
  //  }else{
  //    float sum1 = truePercentage - falsePercentage;
  //    glcd(7, 0, "Total area= %f% ", sum1);
  //  }
  //float newArea = 100 - falsePercentage;
  glcd(6, 0, "Dimension= %f ", falseDim);
  glcd(7, 0, "Total area= %f% ", true_area);
  if (falsePercentage > 80.00) {
    swnd('P');
    Serial.print("Pass");
    mySerial.write('P');
    passed_package += 1;
    while (read_ir(A2) > 12) {
      motor(4, -50);
    }
    delay(10);
    motor(4, 0);
  } else {
    swnd('F');
    Serial.print("Failed");
    mySerial.write('F');
    failed_package += 1;
    while (read_ir(A1) > 12) {
      motor(4, -50);
    }
//    delay(10);
//    motor(4, 0);
//    while (read_ir(A1) < 12) {
//      motor(4, -50);
//    }
    delay(del_m);
    motor(4, 0);
    delay(500);
    servo(1, 30);
    delay(750);
    servo(1, 90);
  }
}

float read_ir(int pin) { //cm format
  float volts = analogRead(pin) * 0.0048828125;
  float distance = 13 * pow(volts, -1);
  if (distance <= 30.0f) {
    return distance;
  }
}

void swnd(char mode) {
  if (mode == 'P') {
    for (int i = 0; i < 2; i++) {
      beep();
      delay(50);
    }
  } else {
    for (int i = 0; i <  3; i++) {
      beep();
      delay(250);
    }
  }
}

//Start range = 18
//Accept range = 30
//Block size threshold = 150

//servo
//0 = close
//50 = open
