// #include "testModel_dtree3.h"
#include <stdint.h>
#include <stdio.h>

float gyro1_x, gyro1_y, gyro1_z, gyro2_x, gyro2_y, gyro2_z;
int16_t gyroInputs[4];

int32_t testModel_predict(const int16_t *features, int32_t features_length) {
    // dummy file
    return 1;
}



int main() {

    while (1) {
        int16_t gyroInput_x = (int16_t) 1000 * (gyro2_x - gyro1_x);
        gyroInputs[0] = gyroInput_x;

        int16_t gyroInput_y = (int16_t) 1000 * (gyro2_y - gyro1_y);
        gyroInputs[1] = gyroInput_y;

        int16_t gyroInput_z = (int16_t) 1000 * (gyro2_z - gyro1_z);
        gyroInputs[2] = gyroInput_z;
    }

    return 0;
}