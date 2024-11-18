# Tyler Sloan, EE 475 Group 5

import os
import glob
from datetime import datetime


def splitTextFiles(folderName):
    folder_path = os.path.join(os.path.dirname("imu_ml_dtree.py"), folderName)
    txt_files = glob.glob(os.path.join(folder_path, '*.txt'))
    now = datetime.now()

    for file in txt_files:
        print(os.path.basename(file)) # debug
        splitFile(file, folder_path)
    
    print(f"\nDone! {now.time()}")


def splitFile(fileName, dir):
    with open(fileName, 'r') as file:
        # Read the header
        header = file.readline().strip()

        # Initialize variables for tracking data blocks and file count
        file_count = 1
        data_block = []

        for line in file:
            line = line.strip()
            if line == "":  # New block starts after a blank line
                # Write the current data block to a new file
                if data_block:  # Skip empty blocks
                    output_filename = os.path.join(dir, f"{header}_{file_count}.txt")
                    with open(output_filename, 'w') as output_file:
                        output_file.write("\n".join(data_block))
                    file_count += 1
                    data_block = []  # Clear data for the next block
            else:
                data_block.append(line)  # Add line to the current data block

        # Write the last data block (if any)
        if data_block:
            output_filename = os.path.join(dir, f"{header}{file_count}.txt")
            with open(output_filename, 'w') as output_file:
                output_file.write("\n".join(data_block))


# select the folder in the directory of this file that contains the data
splitTextFiles('txt_files')