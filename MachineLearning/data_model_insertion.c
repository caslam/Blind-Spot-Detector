// #include "testModel_dtree2.h"
#include <stdint.h>
#include <stdio.h>

float gyro1_x, gyro1_y, gyro1_z, gyro2_x, gyro2_y, gyro2_z;
int16_t gyroInputs[36];
int i = 0;

// dummy file
int32_t testModel_predict(const int16_t *features, int32_t features_length) {
    
    return 1;
}

int16_t* flattenFIFO() {
    int16_t out[36];

    for (int j = 1; j <= 12; j++) {
        out[3*j] = gyroInputs[3 * ((j + i) % 12)];
        out[3*j+1] = gyroInputs[3 * ((j + i) % 12) + 1];
        out[3*j+2] = gyroInputs[3 * ((j + i) % 12) + 2];
    }

    return &out;
}

// loop()
int main() {

    while (1) {

        // this is basically a FIFO lol
        // takes the difference and converts to integers
        int16_t gyroInput_x = (int16_t) 1000 * (gyro2_x - gyro1_x);
        gyroInputs[3*i] = gyroInput_x;

        int16_t gyroInput_y = (int16_t) 1000 * (gyro2_y - gyro1_y);
        gyroInputs[3*i+1] = gyroInput_y;

        int16_t gyroInput_z = (int16_t) 1000 * (gyro2_z - gyro1_z);
        gyroInputs[3*i+2] = gyroInput_z;

        // need to find a way to begin the array that you feed into the model at the least recent data
        // i represents the most recent value, (i+1) % 12 represents the least recent value
        int16_t* inputs_flattened = flattenFIFO();
        testModel_predict(inputs_flattened, 3);

        // other stuff
    
        i = (i + 1) % 12;
    }

    return 0;
}