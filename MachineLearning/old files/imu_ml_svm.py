# Tyler Sloan, EE 475 Group 5

import os
import glob

from sklearn import svm
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import emlearn

import numpy as np
import matplotlib.pyplot as plt
import joblib


dataset_arrays = [] # an array of data arrays
label_arrays = [] # stores the labels for each array
database = [] # stores the names of the files corresponding to each array, only used to plot
# labels = [] # I'm not using this right now, but it may be needed in the future


# reads all the data files and compiles them into the global arrays
def accessDataFolder():
    folder_path = os.path.join(os.path.dirname("imu_ml_svm.py"), 'txt_files')
    txt_files = glob.glob(os.path.join(folder_path, '*.txt'))

    for file_path in txt_files:
        file_name = os.path.basename(file_path)
        data_array = fileRead([], file_path)
        dataset_arrays.append(data_array)
        database.append(file_name)
        
        
        if "left45" in file_name:
            label_arrays.append("left 45")
        elif "left90" in file_name:
            label_arrays.append("left 90")
        elif "right45" in file_name:
            label_arrays.append("right 45")
        elif "right90" in file_name:
            label_arrays.append("right 90")
        else:
            label_arrays.append("invalid")

def computeDiff(arr):
    if (len(arr) != 6):
        print("Invalid length\n")
        return arr

    new_arr = []
    for i in range(3):
        new_arr.append(arr[i + 3] - arr[i])
    return new_arr


# reads from a file and creates a 2D array from the file data
# file must have data in the form "n,n,n,n,n,n" (assumes length of 6, n is any float)
def fileRead(itemList, fileName):
    with open(fileName, "r") as file:
        for line in file:
            # Split each line by commas, convert to integers, and add to the 2D array
            row = list(map(float, line.strip().split(',')))
            # Find the difference between the two gyroscope data
            row_final = computeDiff(row)
            itemList.append(row_final)
    return np.array(itemList)
    
# write from an array to a text file
def fileWrite(itemList, fileName):
    with open(fileName, "w") as file:
        for row in itemList:
            # Convert each row to a string of comma-separated numbers
            file.write(','.join(map(str, row)) + '\n')  # Write each row on a new line

# plot many at once
def plotDataFolder():
    i = 0
    for data in dataset_arrays:
        plotData(dataset_arrays[i], database[i])
        i += 1

# fix the axis transpose thing because previously, I had to transpose to plot the data (not doing that anymore)
# not necessary to the project
def plotData(arr, file):
    titles = ["gyro 1 X", "gyro 1 Y", "gyro 1 Z", "gyro 2 X", "gyro 2 Y", "gyro 2 Z"]
    titles2 = ["gyro 1 X", "gyro 1 Y", "gyro 1 Z", "gyro 1", "gyro 2 X", "gyro 2 Y", "gyro 2 Z", "gyro 2"]

    # figsize: width, height
    fig = plt.figure(figsize=(16,32))
    fig.suptitle(file)

    for i in range(8):
        plt.subplot(2,4,i+1)
        match i:
            case 0 | 1 | 2:
                plt.plot(arr[i])
            case 3:
                plt.plot(arr[i-3])
                plt.plot(arr[i-2])
                plt.plot(arr[i-1])
            case 4 | 5 | 6:
                plt.plot(arr[i-1])
            case 7:
                plt.plot(arr[i-4])
                plt.plot(arr[i-3])
                plt.plot(arr[i-2])
        plt.title(titles2[i])
    
    plt.show()


def trainModel(saveToName):
    accessDataFolder()

    # Prepares the data to be fed into the training function
    max_length = max(seq.shape[0] for seq in dataset_arrays)
    dataset_arrays_padded = np.array([np.pad(seq, ((0, max_length - seq.shape[0]), (0,0)), 'constant') for seq in dataset_arrays])
    dataset_arrays_flattened = dataset_arrays_padded.reshape(dataset_arrays_padded.shape[0], -1)

    data_train, data_test, label_train, label_test = train_test_split(dataset_arrays_flattened, label_arrays, test_size=0.3, random_state=42)

    # Create an SVM classifier
    clf = svm.SVC(kernel='linear')  # You can also use 'rbf', 'poly', etc.

    # Train the model
    clf.fit(data_train, label_train)

    # Make predictions on the test set
    label_pred = clf.predict(data_test)

    # Check accuracy
    accuracy = accuracy_score(label_test, label_pred)
    print(f'Accuracy: {accuracy * 100:.2f}%')

    # Save the model to a file
    joblib.dump(clf, saveToName)

def improveModel(modelFileName):
    # Load the model from a file
    clf = joblib.load(modelFileName)

    # tuning to improve performance
    # Example of using an RBF kernel and adjusting the regularization parameter
    clf = svm.SVC(kernel='rbf', C=1.0)

    # Save the model to a file
    joblib.dump(clf, modelFileName)


def predict(modelFileName, new_data):
    clf = joblib.load(modelFileName)

    # Predict the label for the new data
    label = clf.predict(new_data) # what should label be?
    if label == 1:
        print("Trigger alert: " + label)
    else:
        print("No alert: IMU returning or stationary")

# should convert the model to C code, which enables it to exist on Arduino
def convertToC(pklName, cName):
    # Load the model
    model = joblib.load(pklName)

    # Convert the model using emlearn
    emlearn_model = emlearn.convert(model, method='inline')

    # Export to C code
    emlearn_model.save(file=cName, name='testModel')

def main():
    # accessDataFolder()
    # plotDataFolder() # have not checked if this still works

    # warning: it WILL overwrite existing models if you are not careful
    # trainModel('svm_model.pkl')
    # improveModel('svm_model.pkl')

    # .h files preferred because they are space efficient
    convertToC('svm_model.pkl', 'testModel.h')
    return
    

if (__name__ == "__main__"):
    main()