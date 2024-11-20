#include <Wire.h>
#include "FastIMU.h"

// FastIMU by LiquidCGS

#define IMU_ADDRESS 0x68
#define LIDAR_ADDRESS 0x74

enum msg_id {GYRO_MSG, LIDAR_MSG};

typedef struct BtData {
  enum msg_id msg_id;
  int data1;
  int data2;
  bool data3;
} BtData;

MPU6050 IMU;
calData calib = { 0 };
GyroData gyroData;

int previousDistance1 = 0; // Previous distance for Wire (LIDAR 1)
int previousDistance2 = 0; // Previous distance for Wire1 (LIDAR 2)
int previousDistance3 = 0; // Previous distance for Wire2 (LIDAR 3)
const int threshold = 300; // Change threshold in mm
const int buzzerPin = 6;   // Pin for the buzzer
const int DETECTION_RANGE = 2000;

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin();
  Wire.setClock(400000); // 400kHz clock
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
  }

  Serial1.begin(115200);

  Wire1.begin();
  Wire2.begin();

  delay(50);
}

void loop() {
  if (Serial1.available()) {
   // Serial.print(Serial1.read());
  }
 // Serial1.write("Hi");

  printLidarData();
  delay(5000);
}

void printLidarData() {
  checkLidarData(Wire, previousDistance1,1);
  checkLidarData(Wire1, previousDistance2,2);
  checkLidarData(Wire2, previousDistance3,3);
  Serial.print("\n");
}

void checkLidarData(TwoWire& Wire, int &previousDistance, int sensor) {
  uint8_t buf[2] = { 0 };
  uint8_t dat = 0xB0;
  int distance = 0;

  writeReg(0x10, &dat, 1, Wire);
  delay(50);
  readReg(0x02, buf, 2, Wire);
  distance = buf[0] * 0x100 + buf[1] + 10;
  //Serial.printf("%4dmm\t", distance);

  if (distance < DETECTION_RANGE) {
    Serial1.printf("1,%d\n", sensor); // Object detected
  } else {
    Serial1.printf("0,%d\n", sensor); // No object detected
  }
  // Check for high change from the previous distance
  if ((previousDistance - distance) >= threshold) {
    tone(buzzerPin, 1000); // Buzz with 1000 Hz frequency
    delay(100); // Buzz duration
    noTone(buzzerPin);
    // Serial1.write("1 + sensor %d", sensor);
    
  }

  // Update previous distance
  previousDistance = distance;
}

uint8_t readReg(uint8_t reg, const void* pBuf, size_t size, TwoWire& Wire) {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;
  Wire.beginTransmission((uint8_t)LIDAR_ADDRESS);
  Wire.write(&reg, 1);
  if (Wire.endTransmission() != 0) {
    return 0;
  }
  delay(20);
  Wire.requestFrom((uint8_t)LIDAR_ADDRESS, (uint8_t)size);
  for (uint16_t i = 0; i < size; i++) {
    _pBuf[i] = Wire.read();
  }
  return size;
}

bool writeReg(uint8_t reg, const void* pBuf, size_t size, TwoWire& Wire) {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;
  Wire.beginTransmission((uint8_t)LIDAR_ADDRESS);
  Wire.write(&reg, 1);

  for (uint16_t i = 0; i < size; i++) {
    Wire.write(_pBuf[i]);
  }
  if (Wire.endTransmission() != 0) {
    return 0;
  } else {
    return 1;
  }
}

void printGyroData() {
  IMU.update();
  IMU.getGyro(&gyroData);
  Serial.printf("%f, %f, %f\n", gyroData.gyroX, gyroData.gyroY, gyroData.gyroZ);
}

String formatData(BtData& data) {
  return (String)data.data1 + "," + (String)data.data2 + "," + (String)data.data3 + "\n";
}
