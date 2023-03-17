/*
 * Filename: main.cpp
 * Created Date: 2/13/23
 * Author: Harrison Paas
 * 
 * Description: This file contains a simple example of how to use the Network class's cross_validate method to test an MLP classifier.
 */


#include <iostream>
#include <string>
#include <fstream>

#define CLASS "class"
#include "Network.h"

using namespace std;
 
int main(int argc, char** argv){
    
    string filename = "adult-big.arff";
    
    ARFFDataset data;
    
    ARFFDataset::loadARFF(filename, data);
    
    data.replaceMissingValuesByClass(); //replace missing values with means/modes for entries with the same class label
    
    data.normalize(); //z score normalize all numeric columns

    data.shuffle();
    
    cout<<data<<endl;
    
    vector<int> hidden_layer_sizes = {100, 100};
    double learningrate = 0.1;
    int num_epochs = 10;
    int num_folds = 10;
    Network::ACTIVATION activation = Network::LOGISTIC;
   
    MLPNetwork net(hidden_layer_sizes, data.getMeta(), learningrate, activation);
    
    auto scores = Network::cross_validate(data, net, num_epochs, learningrate, num_folds);
    
    for(auto pair : scores){
        cout<<pair.first<<" "<<pair.second<<endl;
    }
    
    return 0;
}
