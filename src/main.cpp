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
#include <variant>
#include <tuple>
#include <unordered_map>
#include <map>
#include <typeinfo>

#include <mkl.h>

#define CLASS "'Class'"
#include "Network.h"

using namespace std;
 
int main(int argc, char** argv){
    
    string filename = "/Users/harrypaas/Desktop/DMProj2/DMProj2/hypothyroid.arff";
    
    ARFFDataset data;
    
    ARFFDataset::loadARFF(filename, data);
    
    data.replaceMissingValuesByClass();
    
    data.normalize();

    data.shuffle();
    
    cout<<data<<endl;
    
    vector<int> hidden_layer_sizes = {100, 100, 100, 100, 100};
    double learningrate = 0.01;
    int num_epochs = 100;
    int num_folds = 10;
    Network::ACTIVATION activation = Network::RELU;
    
    auto scores = Network::cross_validate<MLPNetwork>(data, hidden_layer_sizes, num_epochs, learningrate, num_folds, activation);
    
    for(auto pair = scores.rbegin(); pair!=scores.rend();pair++){
        cout<<(*pair).first<<" "<<(*pair).second<<endl;
    }
     
 
    
    /*
    
    random_device rd;
    mt19937 rng(rd());
    rng.seed(420);
    shuffle(data.begin(), data.end(), rng);
    
    
    learningrate = 0.01;
    int num_epochs = 10;
    vector<int> sizes = {105,100,100,2};
    
    double lr_exp = -2.5;
    double max_lr_exp=0.5;
    for(lr_exp = -3 ;lr_exp<=-1; lr_exp+=0.05){
        
        learningrate=pow(10,lr_exp);
        
        
        tuple<double, double, double, double> scores = cross_validate(data, meta, sizes, 10, 0.25);
        
        cout<<"Accuracy: "<<get<0>(scores)<<endl;
        cout<<"Precision: "<<get<1>(scores)<<endl;
        cout<<"Recall: "<<get<2>(scores)<<endl;
        cout<<"F1: "<<get<3>(scores)<<endl;
     
    }
    */
    
    return 0;
}





