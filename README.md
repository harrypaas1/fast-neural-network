# C++ Neural Network

This is a simple multi-layer perceptron neural network implementation in C++. It can be used to read an ARFF file and train and cross-validate a multilayer perceptron network for classification or regression tasks. The code uses the Intel Math Kernel Library for optimized matrix computations.

## Dependencies

To use this code, you must have the Intel Math Kernel Library installed. 



##Getting Started
To get started, clone the repository and include the "Network.h" header file in your C++ project.
You must use the #define CLASS "yourclasslabel" macro before you include the Network.h file. (the default is "class") 
The library is intended to optimize the training time, so it performs the "one hot encoding" for categorical values when it reads the file instead of doing the encoding for every training. This means that the file's metadata is needed to manupulate the dataset, which means that the class label must be explicitly stated when the code is compiled.
```cpp
#include "Network.h"
The main function in "main.cpp" provides an example of how to load a dataset from an ARFF file, preprocess it, and train a multi-layer perceptron (MLP) network using cross-validation. You can modify this code to fit your own use case.
```

ARFF Dataset
This library provides a class for loading and preprocessing datasets in ARFF format. To load an ARFF dataset, use the following static method:

'''cpp
ARFFDataset data;
ARFFDataset::loadARFF(filename, data);```
This will load the dataset from the specified file and store it in a ARFFDataset object named "data".
For the python enjoyers you can also use
```cpp
auto data = loadARFF(filename);```
and for the old school
##Preprocessing
This library provides several preprocessing functions for handling missing values and normalizing data. These functions can be called on a ARFFDataset object, as shown below:

```cpp
data.replaceMissingValuesByClass();
data.normalize();```
The "replaceMissingValuesByClass()" function replaces missing values in the dataset with the mean/mode value for the attribute within the same class. The "normalize()" function normalizes the dataset to have zero mean and unit variance.

##Training
This library currently only supports multilayer perceptron networks with stochastic gradient descent backpropogation for training. The supported activation functions are sigmoid, tanh, and relu. The Network class provides a static method cross_validate which returns a map with a variety of statistics.

```cpp
auto scores = Network::cross_validate<MLPNetwork>(data, hidden_layer_sizes, num_epochs, learningrate, num_folds, activation);```
This code performs cross-validation on the specified dataset using an MLP network with the specified hyperparameters. The "num_folds" parameter specifies the number of folds to use in cross-validation.

##License
Fast Neural Network is open source and is available under the MIT license. You are free to use, modify, and distribute the code as you see fit. See the LICENSE file for more information.
