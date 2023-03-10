//
//  main.cpp
//  DMProj2
//
//  Created by Harry Paas on 2/13/23.
//


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>

#include <mkl.h>

#define CLASS "'Class'"
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
    
    auto scores = Network::cross_validate<MLPNetwork>(data, hidden_layer_sizes, num_epochs, learningrate, num_folds, activation);
    
    for(auto pair : scores){
        cout<<pair.first<<" "<<pair.second<<endl;
    }
    
    return 0;
}





