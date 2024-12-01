#include <stdio.h>
#include <stdint.h>

char *file_name;
char *data_block;


/* When given a block of data values into the terminal, it saves it to a text file.*/


// floats are retrieve through global variables
void newData(char* file_name, char mode, int lines) {
    // Open the file for writing ("w" mode writes a new file, "a" appends to existing)
    FILE *file = fopen(file_name, "w");  // Change "w" to "a" to append data instead of overwriting
    
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int i = 0; i < lines; i++) {
        fileWrite()
    }


}

void fileWrite(float g1x, float g1y, float g1z, float g2x, float g2y, float g2z, char* fileName, int lines) {
    // Assume these are the variables holding your sensor data

    // Write the sensor data to the file in a comma-separated format
    fprintf(file, "%f, %f, %f, %f, %f, %f\n", g1x, g1y, g1z, g2x, g2y, g2z);

    // Close the file
    fclose(file);

    printf("Sensor data written to file.\n");
}


int main() {
    while (1) {
        printf("Enter file name: ");
        scanf("%s", &file_name);
        
    }
}