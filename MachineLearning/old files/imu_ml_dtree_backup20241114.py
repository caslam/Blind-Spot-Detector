# Tyler Sloan, EE 475 Group 5

import os
import glob

from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score
import emlearn

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import joblib

# REMEMBER TO CHANGE
dataset_csv_file = 'combined_data3.csv' 
model_name = 'svm_model_dtree3.pkl'
header_name = 'testModel_dtree3.h'

dataset_arrays = [] # an array of data arrays
database = [] # stores the names of the files corresponding to each array, only used to plot
max_length = 0


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
            row = list(map(float, line.strip().split(', ')))
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
# NOT necessary to the project
def plotData(arr, file):
    # titles = ["gyro 1 X", "gyro 1 Y", "gyro 1 Z", "gyro 2 X", "gyro 2 Y", "gyro 2 Z"]
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


# reads all the data files and compiles them into the global arrays
def accessDataFolder():
    folder_path = os.path.join(os.path.dirname("imu_ml_dtree.py"), 'txt_files')
    txt_files = glob.glob(os.path.join(folder_path, '*.txt'))
    label_array = [] # stores the labels for each array

    for file_path in txt_files:
        file_name = os.path.basename(file_path)
        database.append(file_name)

        data_array = fileRead([], file_path)
        # first method of labels: '1' if big turn, '0' if small turn
        if "Left 10-25" in file_name or "Left 25-45" in file_name or "Right 10-15" in file_name or "Right 20-30" in file_name or "Right 30-45" in file_name:
            label_array.append(0)
        else:
            label_array.append(1)

        # second method of labels: 1 per 30 deg of turning. CCW is positive, CW is negative
        # num = 0
        # if "10" in file_name or "20" in file_name:
        #     num = 1
        # elif "30" in file_name or "45" in file_name:
        #     num = 2
        # elif "60" in file_name or "75" in file_name:
        #     num = 3
        
        # if "Right" in file_name:
        #     num = -num
        # label_array.append(num)
        
        dataset_arrays.append(data_array)

    max_length = max(seq.shape[0] for seq in dataset_arrays)
    dataset_arrays_padded = np.array([np.pad(seq, ((0, max_length - seq.shape[0]), (0,0)), 'constant') for seq in dataset_arrays])
    
    # print(dataset_arrays_padded[:5])
    # print(label_arrays[:5])

    dataframe_arrays = []  
    
    for i in range(len(dataset_arrays_padded)):
        data_flattened = dataset_arrays_padded[i].reshape(dataset_arrays_padded[i].shape[0], -1)

        data_and_label_flattened = np.empty((data_flattened.shape[0], data_flattened.shape[1] + 1))
        data_and_label_flattened[:, :-1] = data_flattened  # Copy flattened data
        data_and_label_flattened[:, -1] = label_array[i]   # Add label as the last column

        dataframe_arrays.append(pd.DataFrame(data_and_label_flattened, columns=[f"feature_{j}" for j in range(data_flattened.shape[1])] + ["label"]))


    # Concatenate all DataFrames into one
    combined_data = pd.concat(dataframe_arrays, axis=0, ignore_index=True)

    # Save combined data to a .csv file
    combined_data.to_csv(dataset_csv_file, index=False)
        
        

def trainModel(saveToName):
    accessDataFolder()

    csv = pd.read_csv(dataset_csv_file)
    dataset_nums = csv.drop(columns=["label"], axis=1)
    dataset_labels = csv["label"]

    # Prepares the data to be fed into the training function
    data_train, data_test, label_train, label_test = train_test_split(dataset_nums, dataset_labels, test_size=0.3, random_state=42)

    # Create an decision tree classifier
    clf = DecisionTreeClassifier(random_state=42)
    # ccp_alpha removes branches with a cost complexity lower than the set threshold
    # clf = DecisionTreeClassifier(ccp_alpha=0.01)  # Adjust ccp_alpha as needed

    # Train the model
    clf.fit(data_train, label_train)

    # Make predictions on the test set
    label_pred = clf.predict(data_test)

    # Check accuracy
    accuracy = accuracy_score(label_test, label_pred)
    print(f'Accuracy: {accuracy * 100:.2f}%')

    # improve the model
    parameters = {
        'max_depth': [4, 8, 10, 12, None],
        'min_samples_split': [2, 4, 8],
        'min_samples_leaf': [1, 2, 5],
        'max_features': ['auto', 'sqrt', 'log2', None],
        'criterion': ['gini', 'entropy']
    }

    grid_search = GridSearchCV(clf, parameters, cv=5)
    grid_search.fit(data_train, label_train)
    estimator = grid_search.best_estimator_
    best_params = grid_search.best_params_

    print("Best score:", grid_search.best_score_)
    print("Best parameters:", best_params)

    # gridSearch = improveModel(clf, data_train, label_train)
    test_accuracy = accuracy_score(label_test, estimator.predict(data_test))
    print("Test accuracy:", test_accuracy)

    # Make predictions on the test set
    optimized_model = DecisionTreeClassifier(max_depth=best_params['max_depth'], 
                                            min_samples_split=best_params['min_samples_split'], 
                                            min_samples_leaf=best_params['min_samples_leaf'],
                                            max_features=best_params['max_features'],
                                            criterion=best_params['criterion'])
    optimized_model.fit(data_train, label_train)  

    optimized_label_pred =  optimized_model.predict(data_test)

    # Check accuracy
    accuracy = accuracy_score(label_test, optimized_label_pred)
    print(f'Accuracy: {accuracy * 100:.2f}%')

    # Save the model to a file
    joblib.dump(clf, saveToName)




def predict(model_file, new_data):
    clf = joblib.load(model_file)

    # Predict the label for the new data
    label = clf.predict(new_data) # what should label be?
    if label == 1:
        print("Trigger alert: " + label)
    else:
        print("No alert: IMU returning or stationary")

# should convert the model to C code, which enables it to exist on Arduino
def convertToC(pkl_name, c_name):
    # Load the model
    model = joblib.load(pkl_name)

    # Convert the model using emlearn
    emlearn_model = emlearn.convert(model, method='inline')

    # Export to C code
    emlearn_model.save(file=c_name, name='testModel')


print("\n")
# plotDataFolder() # have not checked if this still works

# warning: it WILL overwrite existing files if you are not careful
trainModel(model_name)

# .h files preferred because they are space efficient
convertToC(model_name, header_name)