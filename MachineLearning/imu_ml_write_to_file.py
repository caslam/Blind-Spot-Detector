# Tyler Sloan
# EE 475 Group 5
# Takes in a block of data submitted into the terminal, formats it, and saves it to a unique file. 
# Used to train the model. 

import os
import re

row_count = 12 # customize the number of rows of data the program accepts

def writeFromTerminal(file_name, index, folder_path):
    # Waits for block of data to be inputted
    text_block = ""
    while (text_block == ""):
        text_block = input("Enter data block here: ").strip()

    # Isolates the floats in the text block and puts them into an array
    string_of_data = re.findall(r"-?\d+\.\d{6}", text_block)
    data_array_1D = [float(num) for num in string_of_data]
    number_of_rows = len(data_array_1D) // 6

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

# global variables
folder_path = os.path.join(os.path.dirname("imu_ml_dtree.py"), "txt_files_dtree2") # Make sure these are to your liking
files_created = []
file_name = ""
file_index = 1

print("\n")
while True:
    file_name = input("Enter a file name prefix or (Q)uit: ").strip()
    if (file_name == 'Q' or file_name == 'q'): # Quitting
        print("Done!")
        break
    elif (file_name in files_created):
        print("File prefix already exists.")
    elif (len(file_name) > 0): # prevents saving to the same file
        files_created.append(file_name)
        while True:
            confirm = input(f"Creating \"{file_name}{file_index}.txt\", \n(C)onfirm, (S)kip, (R)edo previous file, or (Q)uit? ").strip().upper()
            if (confirm == 'Q'): # Exits the loop and allows you to choose a different file name
                file_index = 1
                break
            elif (confirm == 'C'): # Confirms file creation process
                if (writeFromTerminal(file_name, file_index, folder_path) == None):
                    file_index = file_index + 1
            elif (confirm == 'R' and file_index > 1): # Lets you retry the previous file. no files named '0'
                print(f"Retrying \"{file_name}{file_index-1}.txt\":")
                writeFromTerminal(file_name, file_index-1, folder_path)
            elif (confirm == 'S'): # Skips the current number. Useful if you have existing files with that name.
                file_index = file_index + 1
    else:
        print("Invalid file name.")
