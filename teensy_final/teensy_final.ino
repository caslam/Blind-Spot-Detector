#include <Wire.h>
#include "FastIMU.h" 

// model header
#include "modelHeader_dtree_final.h"

#define IMU_ADDRESS 0x68 // IMU (AD0 to GND)
#define LIDAR_ADDRESS 0x74

// Instantiate the IMU
MPU6050 IMU;
calData calib = { 0 };
GyroData gyroData;
GyroData phoneGyroData;

// Store offsets for calibration
float offsetX1 = 0, offsetY1 = 0, offsetZ1 = 0;

// Sliding window of gyro inputs to feed into model
// gyroInputs is a circular buffer, while gyroToModel is a time-sorted copy of gyroInputs
// gyroBuffer are three buffers to synchronize phone readings with IMU readings
int16_t gyroInputs[36], gyroToModel[36];
float gyroBuffer1[3], gyroBuffer2[3], gyroBuffer3[3];
// Index for circular buffer
int i = 0;

int previousDistance1 = 0; // Previous distance for Wire (LIDAR 1)
int previousDistance2 = 0; // Previous distance for Wire1 (LIDAR 2)
int previousDistance3 = 0; // Previous distance for Wire2 (LIDAR 3)
const int threshold = 500; // Change threshold in mm
const int buzzerPin = 20;   // Pin for the buzzer
const int DETECTION_RANGE = 2000;

// Threshold for setting turning flag based on phone gyro outputs
const int rotation_threshold = 5;
// Max value of turning flag, set when rotation_threshold is exceeded to
// deal with the delay for model output
const int turning_flag_max = 15;
int turning = 0; // Flag for phone gyro measurements over certain value

void setup() {
  Serial.begin(115200);
  Serial.println("I2C Scanner with IMUs");

  // Initialize I2C bus
  Wire.begin();
  Wire.setClock(400000); // 400kHz clock speed

  // Initialize and calibrate IMU
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
  } else {
    Serial.println("IMU initialized successfully.");
    calibrateIMU(IMU, gyroData, offsetX1, offsetY1, offsetZ1);
    Serial.println("IMU calibrated successfully.");
  }

  // Set up Bluetooth on Serial1
  Serial1.begin(115200);

  Wire1.begin();
  Wire2.begin();

  delay(20);
}


void loop() {
  // Prints bluetooth data to the serial monitor
  printBluetoothData();
  
  // Prints gyro data to the serial monitor
  printGyroData();

  // Updates the model input with new data
  newModelInput();

  // Sends the copied and sorted FIFO into the model
  int32_t model_output = testModel_predict(gyroToModel, 3);
  // if (model_output) {
  //   Serial.println("model output");
  // }

  // If the phone's gyro reports rate of rotation over threshold, set
  // flag to indicate that the bike is turning. Otherwise if the flag is
  // already set, decrement the turning flag.
  float phone_gyro_output = (abs(phoneGyroData.gyroX) + abs(phoneGyroData.gyroY) + abs(phoneGyroData.gyroZ));
  if (phone_gyro_output > rotation_threshold) {
    turning = turning_flag_max;
  } else {
    turning = (turning > 0) ? (turning - 1) : 0;
  }

  // if (turning) {
  //   Serial.println("turning");
  // }

  // Sends message to phone if model output is 1 and bike is not turning
  if (model_output && (!turning)) {
    Serial1.printf("$1,4\n");
  }
  
  i = (i + 1) % 12;

  // Serial.printf("%f, %f, %f, %f, %f, %f\n", (gyroData.gyroX - offsetX1/9.0), phoneGyroData.gyroX, (gyroData.gyroY - offsetY1/9.0),  phoneGyroData.gyroY, (gyroData.gyroZ - offsetZ1)/9.0, (phoneGyroData.gyroZ));
  // Serial.printf("gyro[%f, %f, %f], phone[%f, %f, %f]", (gyroData.gyroX - offsetX1/9.0), (gyroData.gyroY - offsetY1/9.0), (gyroData.gyroZ - offsetZ1)/9.0, phoneGyroData.gyroX, phoneGyroData.gyroY, phoneGyroData.gyroZ);
  // Serial.printf("%f, %f, %f, %f, %f, %f\n", gyroData.gyroX - offsetX1, phoneGyroData.gyroX, (gyroData.gyroY - offsetY1),  phoneGyroData.gyroY, (gyroData.gyroZ - offsetZ1), (phoneGyroData.gyroZ));
  // Serial.printf("%f, %f, %f", ((gyroData.gyroX - offsetX1)- (phoneGyroData.gyroX)), ((gyroData.gyroY - offsetY1)- (phoneGyroData.gyroY)), ((gyroData.gyroZ - offsetZ1)- (phoneGyroData.gyroZ)));
  // Serial.printf("%d, %d, %d, %d", gyroInput_x, gyroInput_y, gyroInput_z, model_output);

  // Prints LiDAR measurements
  printLidarData();
  delay(5);
}

void newModelInput() {
  // Computes the difference in gyro values
  // Phone data is 3 readings behind IMU data
  int16_t gyroInput_x = (int16_t) (gyroBuffer1[0] - (phoneGyroData.gyroX))/2.0;
  gyroInputs[3*i] = gyroInput_x;

  int16_t gyroInput_y = (int16_t) (gyroBuffer1[1] - (phoneGyroData.gyroY))/2.0;
  gyroInputs[3*i+1] = gyroInput_y;

  int16_t gyroInput_z = (int16_t) (gyroBuffer1[2] - (phoneGyroData.gyroZ))/2.0;
  gyroInputs[3*i+2] = gyroInput_z;

  // Shifts buffer values
  for (int k = 0; k < 3; k++) {
    gyroBuffer3[k] = gyroBuffer2[k];
    gyroBuffer2[k] = gyroBuffer1[k];
  }
  Serial.printf("gyro[%f, %f, %f], phone[%f, %f, %f]\n", gyroBuffer3[0], gyroBuffer3[1], gyroBuffer3[2], phoneGyroData.gyroX, phoneGyroData.gyroY, phoneGyroData.gyroZ);

  gyroBuffer1[0] = (gyroData.gyroX - offsetX1)/9.0;
  gyroBuffer1[1] = (gyroData.gyroY - offsetY1)/9.0;
  gyroBuffer1[2] = (gyroData.gyroZ - offsetZ1)/9.0;

  // Copies the FIFO and sorts it from oldest to newest data
  sortFIFO();
}


// Sorts FIFO so it is ready to insert into model
void sortFIFO() {
  for (int j = 0; j < 12; j++) {
        gyroToModel[3*j] = gyroInputs[3 * ((j + i + 1) % 12)];
        gyroToModel[3*j+1] = gyroInputs[3 * ((j + i + 1) % 12) + 1];
        gyroToModel[3*j+2] = gyroInputs[3 * ((j + i + 1) % 12) + 2];
    }
}

void printBluetoothData() {
  char buf[sizeof(float) * 4];
  float *f_buf = (float *)buf;

  while (Serial1.read() != '$') { }
  Serial1.readBytes(buf, sizeof(buf));

  float gyro_x = f_buf[0];
  float gyro_y = f_buf[1];
  float gyro_z = f_buf[2];
  float checksum = f_buf[3];

  if (gyro_x + gyro_y + gyro_z == checksum) {
    // Serial.printf(" bluetooth: %f, %f, %f\n", gyro_x, gyro_y, gyro_z);
    phoneGyroData.gyroX = gyro_x * 2;
    phoneGyroData.gyroY = gyro_y * 2;
    phoneGyroData.gyroZ = gyro_z * 2;
  } else {
    // Serial.print("checksum failed");
  }
  Serial.println();
}

void printGyroData() {
  // Update and retrieve gyro data from IMU 1
  IMU.update();
  IMU.getGyro(&gyroData);

  // Print gyro data from both IMUs in one line
  // Serial.printf("%f, %f, %f, %f, %f, %f\n", 
  //               gyroData1.gyroX - offsetX1, gyroData1.gyroY - offsetY1, gyroData1.gyroZ - offsetZ1,
  //               gyroData2.gyroX - offsetX2, gyroData2.gyroY - offsetY2, gyroData2.gyroZ - offsetZ2);
  // Serial.printf("%f, %f, %f", 
  //               gyroData.gyroX - offsetX1, gyroData.gyroY - offsetY1, gyroData.gyroZ - offsetZ1);

  // Check if both IMUs are stationary
  // if (abs(gyroData1.gyroX - offsetX1) < 0.1 && abs(gyroData1.gyroY - offsetY1) < 0.1 && abs(gyroData1.gyroZ - offsetZ1) < 0.1 &&
  //     abs(gyroData2.gyroX - offsetX2) < 0.1 && abs(gyroData2.gyroY - offsetY2) < 0.1 && abs(gyroData2.gyroZ - offsetZ2) < 0.1) {
  //   Serial.println("Both IMUs are stationary (0, 0, 0)");
  // }
}

void printLidarData() {
  checkLidarData(Wire, previousDistance1,1);
  checkLidarData(Wire1, previousDistance2,2);
  checkLidarData(Wire2, previousDistance3,3);
  // Serial.print("\n");
}

void checkLidarData(TwoWire& Wire, int &previousDistance, int sensor) {
  uint8_t buf[2] = { 0 };
  uint8_t dat = 0xB0;
  int distance = 0;

  writeReg(0x10, &dat, 1, Wire);
  delay(50);
  readReg(0x02, buf, 2, Wire);
  distance = buf[0] * 0x100 + buf[1] + 10;
  // Serial.printf("%4dmm\t", distance);

  // '10' is the output when a LIDAR sensor does not receive a reading. We manually change this to an arbitrarily high number.
  if (distance == 10) {
    distance = 5000;
  }

  if (distance < DETECTION_RANGE) {
    Serial1.printf("$1,%d\n", sensor); // Object detected
  } else {
    Serial1.printf("$0,%d\n", sensor); // No object detected
  }

  

  // Serial.printf("sensor: %d, %d, %d\n", sensor, previousDistance, distance);
  // Check for high change from the previous distance
  if ((previousDistance != 5000)  && ((previousDistance - distance) >= threshold)) {
    Serial.printf("sensor: %d, %d -> %d\n", sensor, previousDistance, distance); // No object detected
    tone(buzzerPin, 1000); // Buzz with 1000 Hz frequency
    delay(100); // Buzz duration
    noTone(buzzerPin);
    // Serial.printf("sensor %d", sensor);
  }


  previousDistance = distance;
  // Update previous distance
  
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

// Calibrates the IMU. Used in setup() only
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
