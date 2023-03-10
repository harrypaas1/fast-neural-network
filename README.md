# C++ Neural Network

This is a lightweight implementation of a multi-layer perceptron neural network in C++. It provides a simple and efficient way to read an ARFF file, preprocess the data, and train and cross-validate a multilayer perceptron network for classification or regression tasks.

The main goal of this library is to optimize the training time of the network. The library uses the Intel Math Kernel Library for optimized matrix computations.

## Dependencies

To use this code, you must have the Intel Math Kernel Library installed. 



## Getting Started

To get started, clone the repository and include the "Network.h" header file in your C++ project.
You must use the #define CLASS "yourclasslabel" macro before you include the Network.h file. (the default is "class")  

```cpp
#define classlabel "Class"
#include "Network.h"
```
The library is intended to optimize the training time, so it performs the "one hot encoding" for categorical values when it reads the file instead of doing the encoding for every training. This means that the file's metadata is needed to manipulate the dataset, which means that the class label must be explicitly stated when the code is compiled.  
The main function in "main.cpp" provides an example of how to load a dataset from an ARFF file, preprocess it, and train a multi-layer perceptron (MLP) network using cross-validation. You can modify this code to fit your own use case.


## ARFF Dataset

This library provides a class for loading and preprocessing datasets in ARFF format. To load an ARFF dataset, use the following static method:
```cpp
ARFFDataset data;
ARFFDataset::loadARFF(filename, data);
```

This will load the dataset from the specified file and store it in a ARFFDataset object named "data".
For the python enjoyers you can also use
```cpp
auto data = ARFFDataset::loadARFF(filename);
```

and for the old school

```cpp
ifstream inFile(filename);
ARFFDataset data;
inFile>>data;
```
## Preprocessing

This library provides several preprocessing functions for handling missing values and normalizing data. These functions can be called on a ARFFDataset object, as shown below:

```cpp
data.replaceMissingValuesByClass();
data.normalize();
data.shuffle() //note cross_validate does not automatically shuffle data
```
The "replaceMissingValuesByClass()" function replaces missing values in the dataset with the mean/mode value for the attribute within the same class. The "normalize()" function normalizes the dataset to have zero mean and unit variance.

## Training

This library currently only supports multilayer perceptron networks with stochastic gradient descent backpropogation for training. The supported activation functions are sigmoid, tanh, and relu. The Network class provides a static method cross_validate performs k-fold cross-validation and returns a map with a variety of statistics, automatically detecting whether the task is a regression or classification task.

```cpp
map<string,double> scores = Network::cross_validate<MLPNetwork>(data, hidden_layer_sizes, num_epochs, learningrate, num_folds, activation);
```
This code performs k-fold cross-validation on the specified dataset using an MLP network with the specified hyperparameters. The "num_folds" parameter specifies the number of folds to use in cross-validation.  
Output:  

```output
Accuracy 0.859237
Macro F1 0.795801
Macro Precision 0.815687
Macro Recall 0.781015
Micro F1 <=50K 0.909606
Micro F1 >50K 0.681997
Micro Precision <=50K 0.889147
Micro Precision >50K 0.742228
Micro Recall <=50K 0.931049
Micro Recall >50K 0.63098
Total time 42.3828
Train time 40.5367
```

## Compiling
To build your program on the command line, follow the two steps:  
- Run the Intel oneAPI setvars script to set the environment variables necessary to compile the library.  
  - On Linux/macOS use the command: source /opt/intel/oneapi/setvars.sh. 
  - On Windown use the command: "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"  
- Build your program with the provided Makefile. Simply type "make" to build then "./main" to run. 
If your program is in a different directory than the Network.h files, you need to copy the Makefile to your working directory and add the flag  
**-I/path/to/your/network/installation/src** (that's a capital i) to the LINKFLAGS section.
  
Unfortunately, it is a bit more involved to link this library with an IDE. You must link your code with both the Intel MKL as well as the fast-neural-network files. You need to specify paths to all of the library and header files required and include linker flags for the dynamically linked MKL libraries. See Xcode Configuration for a detailed list of my configuration options for Xcode.  
Keep in mind that the library is intended to optimize training time, and compiling with the command line will give you the best performance since you can make use of compiler optimization flags.  

## License

Fast Neural Network is open source and is available under the GNU Public license. You are free to use, modify, and distribute the code as you see fit. See the LICENSE file for more information.
