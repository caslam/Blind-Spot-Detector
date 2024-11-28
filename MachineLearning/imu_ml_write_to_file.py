# Tyler Sloan, EE 475 Group 5

import os
import re


def writeFromTerminal(file_name, folder_path):
    text_block = ""
    while (text_block == ""):
        text_block = input("Enter data block here: ").strip()

    string_of_data = re.findall(r"-?\d+\.\d{6}", text_block)
    data_array_1D = [float(num) for num in string_of_data]
    # print(f"\n{data_array_1D}\n")
    
    number_of_rows = len(data_array_1D) // 6

    output_filename = os.path.join(folder_path, f"{file_name}.txt")
    with open(output_filename, "w") as file:
        for row in range(number_of_rows):
            # Convert each row to a string of comma-separated numbers
            print(data_array_1D[6*row:6*(row+1)])
            file.write(', '.join(map(str, data_array_1D[6*row:6*(row+1)])) + '\n')  # Write each row on a new line

    print(f"File saved to {file_name}.txt")





folder_path = os.path.join(os.path.dirname("imu_ml_dtree.py"), "txt_files_dtree2")
files_created = []

print("\n")
while True:
    file_name = input("Enter file name [.txt] (X to quit): ").strip()
    if (file_name == 'X' or file_name == 'x'):
        print("Done.")
        break
    elif (len(file_name) > 0 and file_name not in files_created):
        writeFromTerminal(file_name, folder_path)
        files_created.append(file_name)
    else:
        print("Invalid file name\n")


# select the folder in the directory of this file that contains the data
# splitTextFiles(folder_path)