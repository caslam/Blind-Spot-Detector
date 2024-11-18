# Machine Learning Glossary

## Folders:
- "training_data_storage": raw .txt files from collecting the data
- "text_files_dtree": individual datasets from data in "training_data_storage"
- "text_files_svm": individual datasets from first ML attempt (old)

## Files (by type):
- **.c:** C files that were not coded in the Arduino IDE
    - "data_model_insertion.c": Attempt at figuring out how to insert IMU data into ML header file
    - "writeToFile.c": old file for writing to a file from an Arduio environment
- **.csv:** Compact way of storing data and labels before training a model (decision trees only)
    - "combined_data_old.csv": Used to create dtree model with old data
    - "combined_data.csv": Attempt 1 at a dtree with new data
    - "combined_data2.csv": Attempt 2 (FINAL)
    - "combined_data3.csv": Attempt 3
- **.h:** Final output of the model, converted to C to be used in Arduino
    - "testModel_dtree_original.h": Model made using old data
    - "testModel_dtree1.h": Model attempt 1 (unoptimized)
    - "testModel_dtree2_FINAL.h": Model attempt 2 (optimized)
        - I added the "_FINAL" part just recently but the model should still be the same
    - "testModel_dtree3_multilabel.h": Model attempt 3 (tried to do a label for evey 30 degrees)
- **.pkl:** File format that stores the models
    - "model_dtree1.pkl": Model attempt 1 (unoptimized)
    - "model_dtree2.pkl": Model attempt 2 (optimized)
    - "model_dtree3.pkl": Model attempt 3 (multi-label)
    - "model_svm.pkl": SVM model (old)
- **.py:** Machine learning code 
    - "imu_ml_dtree.py": Code to create dtree model
    - "imu_ml_dtree_backup20241114.py": backup of dtree model code
        - This file specifically makes the dtree 2 model. The other has different code commented/uncommented to make the dtree 3 model.
    - "imu_ml_split_data.py": Code to read Dat's raw data and convert them into customized individual datasets.
    - "imu_ml_svm.py": Code to create svm model (old)