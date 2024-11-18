#include <stdio.h>


int fileWrite() {
    // Assume these are the variables holding your sensor data
    float imu1_x = g1.gyro.x;
    float imu1_y = g1.gyro.y;
    float imu1_z = g1.gyro.z;
    float imu2_x = g2.gyro.x;
    float imu2_y = g2.gyro.y;
    float imu2_z = g2.gyro.z;
    
    // Open the file for writing ("w" mode writes a new file, "a" appends to existing)
    FILE *file = fopen("placeholder_name.txt", "w");  // Change "w" to "a" to append data instead of overwriting
    
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;  // Exit if file couldn't be opened
    }

    // Write the sensor data to the file in a comma-separated format
    fprintf(file, "%f, %f, %f, %f, %f, %f\n", imu1_x, imu1_y, imu1_z, imu2_x, imu2_y, imu2_z);

    // Close the file
    fclose(file);

    printf("Sensor data written to file.\n");

    return 0;
}



int main() {

}