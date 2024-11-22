#include <Wire.h>
#include "FastIMU.h" 
#include "modelHeader_dtree_final.h"

#define IMU_ADDRESS1 0x68 // First IMU (AD0 to GND)
#define IMU_ADDRESS2 0x69 // Second IMU (AD0 to VCC)
#define LIDAR_ADDRESS 0x74

enum msg_id {GYRO_MSG, LIDAR_MSG};

typedef struct BtData {
  enum msg_id msg_id;
  int data1;
  int data2;
  bool data3;
} BtData;

// Instantiate two IMU objects
MPU6050 IMU1;
MPU6050 IMU2;
calData calib1 = { 0 };
calData calib2 = { 0 };
GyroData gyroData1;
GyroData gyroData2;

// Store offsets for calibration
float offsetX1 = 0, offsetY1 = 0, offsetZ1 = 0;
float offsetX2 = 0, offsetY2 = 0, offsetZ2 = 0;

// Sliding window of gyro inputs to feed into model
// gyroInputs is a circular buffer, while gyroToModel is a time-sorted copy of gyroInputs
int16_t gyroInputs[36], gyroToModel[36];
// Index for circular buffer
int i = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("I2C Scanner with IMUs");

  // Initialize I2C bus
  Wire.begin();
  Wire.setClock(400000); // 400kHz clock speed

  // Initialize and calibrate first IMU
  int err1 = IMU1.init(calib1, IMU_ADDRESS1);
  if (err1) {
    Serial.print("Error initializing IMU 1: ");
    Serial.println(err1);
  } else {
    Serial.println("IMU 1 initialized successfully.");
    calibrateIMU(IMU1, gyroData1, offsetX1, offsetY1, offsetZ1);
    Serial.println("IMU 1 calibrated successfully.");
  }

  // Initialize and calibrate second IMU
  int err2 = IMU2.init(calib2, IMU_ADDRESS2);
  if (err2) {
    Serial.print("Error initializing IMU 2: ");
    Serial.println(err2);
  } else {
    Serial.println("IMU 2 initialized successfully.");
    calibrateIMU(IMU2, gyroData2, offsetX2, offsetY2, offsetZ2);
    Serial.println("IMU 2 calibrated successfully.");
  }

  // Set up Bluetooth on Serial1
  Serial1.begin(115200);

  delay(50);
}

void loop() {
  // Scan for I2C devices
  // byte error, address;
  // int nDevices = 0;
  
  // static unsigned long lastReadTime = 0;
  // unsigned long currentTime = millis();

  // if (currentTime - lastReadTime >= 500) { // Slower update rate (500 ms)
  printGyroData();
  //   lastReadTime = currentTime;
  // }

  // Computes the difference in gyro values
  int16_t gyroInput_x = (int16_t) 1000 * ((gyroData2.gyroX - offsetX2)- (gyroData1.gyroX - offsetX1));
  gyroInputs[3*i] = gyroInput_x;

  int16_t gyroInput_y = (int16_t) 1000 * ((gyroData2.gyroY - offsetY2) - (gyroData1.gyroY - offsetY1));
  gyroInputs[3*i+1] = gyroInput_y;

  int16_t gyroInput_z = (int16_t) 1000 * ((gyroData2.gyroZ - offsetZ2) - (gyroData1.gyroZ - offsetZ1));
  gyroInputs[3*i+2] = gyroInput_z;

  // Copies the FIFO and sorts it from oldest to newest data
  sortFIFO();
  // Sends the copied and sorted FIFO into the model
  int32_t model_output = testModel_predict(gyroToModel, 3); // Do something with this output (0/1)!
  i = (i + 1) % 12;


  delay(250);
}

void sortFIFO() {
  for (int j = 0; j < 12; j++) {
        gyroToModel[3*j] = gyroInputs[3 * ((j + i + 1) % 12)];
        gyroToModel[3*j+1] = gyroInputs[3 * ((j + i + 1) % 12) + 1];
        gyroToModel[3*j+2] = gyroInputs[3 * ((j + i + 1) % 12) + 2];
    }
}

void printGyroData() {
  // Update and retrieve gyro data from IMU 1
  IMU1.update();
  IMU1.getGyro(&gyroData1);
  
  // Update and retrieve gyro data from IMU 2
  IMU2.update();
  IMU2.getGyro(&gyroData2);

  // Print gyro data from both IMUs in one line
  Serial.printf("%f, %f, %f, %f, %f, %f\n", 
                gyroData1.gyroX - offsetX1, gyroData1.gyroY - offsetY1, gyroData1.gyroZ - offsetZ1,
                gyroData2.gyroX - offsetX2, gyroData2.gyroY - offsetY2, gyroData2.gyroZ - offsetZ2);

  // Check if both IMUs are stationary
  // if (abs(gyroData1.gyroX - offsetX1) < 0.1 && abs(gyroData1.gyroY - offsetY1) < 0.1 && abs(gyroData1.gyroZ - offsetZ1) < 0.1 &&
  //     abs(gyroData2.gyroX - offsetX2) < 0.1 && abs(gyroData2.gyroY - offsetY2) < 0.1 && abs(gyroData2.gyroZ - offsetZ2) < 0.1) {
  //   Serial.println("Both IMUs are stationary (0, 0, 0)");
  // }
}

void calibrateIMU(MPU6050& imu, GyroData& gyroData, float& offsetX, float& offsetY, float& offsetZ) {
  int samples = 100;
  int validSamples = 0;
  float tempX = 0, tempY = 0, tempZ = 0;

  Serial.println("Calibrating IMU...");
  for (int i = 0; i < samples; i++) {
    imu.update();
    imu.getGyro(&gyroData);
    
    // Discard outliers
    if (abs(gyroData.gyroZ) < 1000) { // Check if Z reading is reasonable
      tempX += gyroData.gyroX;
      tempY += gyroData.gyroY;
      tempZ += gyroData.gyroZ;
      validSamples++;
    }
    delay(10);
  }

  if (validSamples > 0) {
    offsetX = tempX / validSamples;
    offsetY = tempY / validSamples;
    offsetZ = tempZ / validSamples;
  }

  Serial.printf("Offsets calculated - X: %f, Y: %f, Z: %f\n", offsetX, offsetY, offsetZ);
}

String formatData(BtData& data) {
  return (String)data.data1 + "," + (String)data.data2 + "," + (String)data.data3 + "\n";
}

