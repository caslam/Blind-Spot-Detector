# Machine Learning Glossary

## Folders:
- "old_files": old data, old model files, and other files that are not needed anymore
- "text_files_dtree2": folder of data used to train the current model

## Files (by type):
- **.csv:** Compact way of storing data and labels before training a model (decision trees only)
    - "combined_data_final2.csv": Data from "text_files_dtree2" in .csv format
- **.h:** Final output of the model, converted to C to be used in Arduino
    - "modelHeader_dtree_final2.h": Current optimized model in .h format
- **.pkl:** File format that stores the model
    - "model_dtree_final2.pkl": Current optimized model using helmet data
- **.py:** Machine learning and data processing code 
    - "imu_ml_dtree.py": Code to create dtree model
    - "imu_ml_dtree_backup20241114.py": backup of dtree model code
    - "imu_ml_split_data.py": Code to read Dat's raw data and convert them into customized individual datasets.
    - "imu_ml_write_to_file.py": Code to take a block of text copied from Arudino's serial monitor and put it in a custom named .txt file