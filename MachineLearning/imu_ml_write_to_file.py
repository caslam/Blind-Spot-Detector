# Tyler Sloan, EE 475 Group 5

import os
import re

row_count = 12 # customize the number of rows of data the program accepts

def writeFromTerminal(file_name, index, folder_path):
    # waits for block of data to be inputted
    text_block = ""
    while (text_block == ""):
        text_block = input("Enter data block here: ").strip()

    # isolates the floats in the text block and puts them into an array
    string_of_data = re.findall(r"-?\d+\.\d{6}", text_block)
    data_array_1D = [float(num) for num in string_of_data]
    number_of_rows = len(data_array_1D) // 6
    # print(f"\n{data_array_1D}\n")

    if (number_of_rows != row_count):
        print(f"Found {number_of_rows} rows of data, not {row_count}.")
        return False
    
    # creates a new text file in the chosen folder path
    output_filename = os.path.join(folder_path, f"{file_name}{index}.txt")
    with open(output_filename, "w") as file:
        for row in range(number_of_rows):
            # converts each row to a string of comma-separated numbers
            print(data_array_1D[6*row:6*(row+1)])
            file.write(', '.join(map(str, data_array_1D[6*row:6*(row+1)])) + '\n')  # writes each row on a new line

    print(f"File saved to \"{file_name}{index}.txt\".")



# main code
folder_path = os.path.join(os.path.dirname("imu_ml_dtree.py"), "txt_files_dtree2")
files_created = []
file_name = ""
file_index = 1

print("\n")
while True:
    file_name = input("Enter a file name prefix or (Q)uit: ").strip()
    if (file_name == 'Q' or file_name == 'q'):
        print("Done!")
        break
    elif (file_name in files_created):
        print("File prefix already exists.")
    elif (len(file_name) > 0): # prevents saving to the same file
        files_created.append(file_name)
        while True:
            confirm = input(f"Creating \"{file_name}{file_index}.txt\", \n(C)onfirm, (S)kip, (R)edo previous file, or (Q)uit? ").strip().upper()
            if (confirm == 'Q'):
                file_index = 1
                break
            elif (confirm == 'C'):
                if (writeFromTerminal(file_name, file_index, folder_path) == None):
                    file_index = file_index + 1
            elif (confirm == 'R' and file_index > 1): # no files named '0'
                print(f"Retrying \"{file_name}{file_index-1}.txt\":")
                writeFromTerminal(file_name, file_index-1, folder_path)
            elif (confirm == 'S'):
                file_index = file_index + 1
    else:
        print("Invalid file name.")

# old loop
# while True:
#     file_name = input("Enter \"file name\".txt (X to quit): ").strip()
#     if (file_name == 'X' or file_name == 'x'):
#         print("Done!")
#         break
#     elif (file_name in files_created):
#         print("File already exists.")
#     elif (len(file_name) > 0 and file_name not in files_created): # prevents saving to the same file
#         writeFromTerminal(file_name, folder_path)
#         files_created.append(file_name)
#     else:
#         print("Invalid file name.")