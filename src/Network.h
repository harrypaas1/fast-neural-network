/*
 * Filename: Network.h
 * Created Date: 2/18/23
 * Author: Harrison Paas
 * 
 * Description: This file contains the declaration of the Network interface and the implementation for an MLP network.
 */

#ifndef Network_h
#define Network_h


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <functional>
#include <map>
#include <stdexcept>

#include <mkl.h>

#include "ARFFDataset.h"

#define TOTAL_TIME "Total time"
#define TRAIN_TIME "Train time"

#define ACCURACY "Accuracy"
#define MICRO_RECALL "Micro Recall "
#define MACRO_RECALL "Macro Recall"
#define MICRO_PRECISION "Micro Precision "
#define MACRO_PRECISION "Macro Precision"
#define MICRO_F1 "Micro F1 "
#define MACRO_F1 "Macro F1"

#define MSE "Mean Squared Error"
#define RMSE "Root Mean Squared Error"
#define MAE "Mean Absolute Error"
#define RSQUARED "R^2"
#define MAPE "Mean Absolute Percent Error"



using namespace std;

class Network{
public:
    enum ACTIVATION {LOGISTIC, TANH, RELU};
    
    virtual void randomize_weights_and_biases(int seed)=0;
    virtual void train(ARFFEntry& e) = 0;
    virtual string classify(ARFFEntry& e, vector<string> classlabels) =0;
    virtual double predict(ARFFEntry& e) =0;
    
    template <typename NETWORK_TYPE>
    static map<string,double> cross_validate(ARFFDataset& data, vector<int> hidden_layer_sizes, int num_epochs, double lr, int num_folds=10, ACTIVATION activation = LOGISTIC, int random_state=420);

};

class MLPNetwork : Network{

private:
    int num_layers;
    
    vector<int> sizes;
    
    //2D arrays of doubles for calculations
    double** weights;
    double** layers;
    double** biases;
    double** errors;
    
    //used to compare predicted class label to actual class label
    double* expected;
    
    double learningrate;
    ACTIVATION activation;
    
    function<double(double)> activation_function;
    function<double(double)> activation_function_deriv;
    
public:
    
    
    
    MLPNetwork(vector<int> layer_sizes, double learningrate, ACTIVATION activation = LOGISTIC, int random_state=420){
    
        sizes = vector<int>();
        this->activation=activation;
        this->learningrate=learningrate;
        //activation_function = activation_func;
        //activation_function_deriv = activation_func_deriv;
        
        for( int x: layer_sizes){
            sizes.push_back(x);
        }
        num_layers=(int)sizes.size();
        
        weights = new double*[num_layers-1];
        biases = new double*[num_layers];
        layers = new double*[num_layers];
        errors = new double*[num_layers];
        
        biases[0]=NULL;
        errors[0]=NULL;
        for(int i=0;i< num_layers-1;i++){
            //allocating these so that the address of actual arrays of doubles is a multiple of 64
            //I can't explain why this is necessary but it greatly improves performance
            weights[i] = (double*)MKL_malloc(sizeof(double)*sizes.at(i)*sizes.at(i+1),DATA_ALIGNMENT);
            biases[i+1] = (double*)MKL_malloc(sizeof(double)*sizes.at(i+1),DATA_ALIGNMENT);
            layers[i+1]= (double*)MKL_malloc(sizeof(double)*sizes.at(i+1),DATA_ALIGNMENT);
            errors[i+1]= (double*)MKL_malloc(sizeof(double)*sizes.at(i+1),DATA_ALIGNMENT);
        }
        
        randomize_weights_and_biases(random_state);
    }
    
    void setLearningRate(double lr){ learningrate=lr;}
    
    void randomize_weights_and_biases(int seed=420){
        
        random_device rd;
        mt19937 rng(rd());
        rng.seed(seed);
        uniform_real_distribution<double> dist(-0.5,0.5);
        
        
        switch (activation){
            case LOGISTIC:
                goto def;
            case TANH:
                for(int i=0; i<num_layers-1;i++){
                    normal_distribution<double> norm_dist(0, sqrt(1/(double)sizes.at(i)));
                    for(int j=0;j<sizes.at(i+1);j++){
                        biases[i+1][j]=0;
                        for(int k=0;k<sizes.at(k);k++){
                            weights[i][j*sizes.at(i)+k]=norm_dist(rng);
                            //cout<<weights[i][j*sizes.at(i)+k]<<endl;
                        }
                    }
                    
                }
                break;
            case RELU:
                goto def;
            default:
                def:
                for(int i=0;i<num_layers-1;i++){
                    for(int j=0; j<sizes.at(i+1);j++){
                        biases[i+1][j]=dist(rng);
                        for(int k=0;k<sizes.at(i);k++){
                            weights[i][j*sizes.at(i)+k]=dist(rng);
                        }
                    }
                }
                break;
                
        }//end switch
        
    }
    
    void train(ARFFEntry& e){
        
        bool classification = e.get_expected_size()>1;
        layers[0]=e.data;
        
        expected = e.expected;
        
        for(int i=0;i<num_layers-1;i++){
            
            //multiply weights[i] by layers[i] and store it in layers[i+1]
            // W[i] L[i] + 0*L[i+1] -> L[i+1]
            cblas_dgemv(CblasRowMajor, CblasNoTrans, sizes.at(i+1), sizes.at(i), 1, weights[i], sizes.at(i), layers[i], 1, 0, layers[i+1], 1);
            
            //add biases[i]
            //B[i+1] + L[i+1] -> L[i+1]
            cblas_daxpy(sizes.at(i+1), 1, biases[i+1], 1, layers[i+1], 1);
            
            //take sigmoid/softmax
            if(i<num_layers-2){
                activation_func(layers[i+1], sizes.at(i+1));
            }
            else if(classification){
                softmax(layers[i+1], sizes.at(i+1));
            }
        }//end for
        
        //calc output error
        /*
        for(int i=0;i<sizes.back();i++){
            errors[num_layers-1][i] = sigmoid_deriv(layers[num_layers-1][i])*(expected[i]-layers[num_layers-1][i]);
        }*/
        for(int i=0;i<sizes.at(num_layers-1);i++) {
            errors[num_layers-1][i]=(expected[i]-layers[num_layers-1][i]);
            //cout<<expected[i]<<" "<<layers[num_layers-1][i]<<endl;
        }
        //cout<<endl;
        times_activation_func_deriv(layers[num_layers-1], errors[num_layers-1], sizes.back());
        
        for(int i = num_layers-1;i>0;i--){
            
            //update bias
            //lr * E[i] + B[i] -> B[i]
            cblas_daxpy(sizes.at(i), learningrate, errors[i], 1, biases[i], 1);
            
            //calc error
            //need to do this before updating weights
            //but dont need to do this for input layer
            if(i>1){
                //backpropogate error
                // W[i-1]^T E[i] + 0*E[i-1] -> E[i-1]
                cblas_dgemv(CblasRowMajor, CblasTrans, sizes.at(i), sizes.at(i-1), 1, weights[i-1], sizes.at(i-1), errors[i], 1, 0, errors[i-1], 1);
                
                //calc gradient in previous layer
                //multiply errors[i-1][j] by layers[i-1][j]*(1-layers[i-1][j])
                times_activation_func_deriv(layers[i-1], errors[i-1], sizes.at(i-1));
            }
            
            //update weights
            //weights increment is lr * the outer product of E[i] and L[i-1]
            // lr*E[i]L[i-1]^T + W[i-1] -> W[i-1]
            cblas_dger(CblasRowMajor, sizes.at(i), sizes.at(i-1), learningrate, errors[i], 1, layers[i-1], 1, weights[i-1], sizes.at(i-1));
        }
    }//end train method
    
    
    string classify(ARFFEntry& e, vector<string> classlabels){
        
        if(e.get_expected_size()<2 || sizes.back()<2){
            cerr<<"Error. Must have at least two distinct class values to classify\n";
            throw invalid_argument("invalid entry to classify or invalid network architecture\n");
        }
        
        
        if(e.get_expected_size()!=sizes.back()){
            cerr<<"Error. Data entries must have the same number of values for the class label as there are neurons in the output layer\n";
            throw invalid_argument("invalid data layout or network architecture\n");
        }
        
        
        if(classlabels.size()!=sizes.back()){
            cerr<<"Error. Classlabel list must be the same size as output layer\n";
            throw invalid_argument("invalid label list or network architecture\n");
        }
        
        //getInputVector(e, meta, layers[0]);
        layers[0]=e.data;
        for(int i=0;i<num_layers-1;i++){
            
            //multiply weights[i] by layers[i] and store it in layers[i+1]
            cblas_dgemv(CblasRowMajor, CblasNoTrans, sizes.at(i+1), sizes.at(i), 1, weights[i], sizes.at(i), layers[i], 1, 0, layers[i+1], 1);
            
            //add biases[i]
            cblas_daxpy(sizes.at(i+1), 1, biases[i+1], 1, layers[i+1], 1);
            
            //take sigmoid/softmax
            if(i==num_layers-2) softmax(layers[i+1], sizes.at(i+1));
            
            else activation_func(layers[i+1], sizes.at(i+1));
            
        }
        int prediction_index = 0;
        double max = -numeric_limits<double>::infinity();
        for(int i=0;i<classlabels.size();i++){
            if(layers[num_layers-1][i]>max) {
                prediction_index=i;
                max=layers[num_layers-1][i];
            }
        }
        
        return classlabels.at(prediction_index);
    }
    
    double predict(ARFFEntry& e){
        if(e.get_expected_size()!=1 || sizes.back()!=1){
            cerr<<"Error. Regression tasks can only have one output. Use Network::classify for classification tasks\n";
            throw invalid_argument("invalid data layout or network architecture\n");
        }

        layers[0]=e.data;
        for(int i=0;i<num_layers-1;i++){
            
            cblas_dgemv(CblasRowMajor, CblasNoTrans, sizes.at(i+1), sizes.at(i), 1, weights[i], sizes.at(i), layers[i], 1, 0, layers[i+1], 1);
            
            cblas_daxpy(sizes.at(i+1), 1, biases[i+1], 1, layers[i+1], 1);
            
            if(i!=num_layers-2) activation_func(layers[i+1], sizes.at(i+1));
        }
        return layers[num_layers-1][0];

    }
    
    static double sigmoid(const double x){ return 1/(1+exp(-1*x));}
    
    static double sigmoid_deriv(const double y){return y*(1-y);}
    
    
    void activation_func(double* arr, int size){
        switch (activation){
            case LOGISTIC:
                for(int i=0;i< size;i++) arr[i]=1/(1+exp(-1*arr[i]));
                break;
            case TANH:
                /*
                for(int i=0;i< size;i++) {
                    double val = arr[i];
                    double x = exp(val);
                    double y = exp(-1*val);
                    arr[i]=(x-y)/(x+y);
                }*/
                vdTanh(size, arr, arr);
                
                break;
            case RELU:
                for(int i=0;i< size;i++) arr[i]=fmax(0,arr[i]);
                break;
        }
    }
    
    static void softmax(double* arr, int size){
        double total=0;
        for(int i=0;i<size;i++) total+=exp(arr[i]);
        
        for(int i=0;i<size;i++){
            if(total>=INFINITY) arr[i]= arr[i]>=INFINITY? 1 : 0;
            else if(total==0)  arr[i]=0;
            else arr[i]=exp(arr[i])/total;
        }
            
    }
    
    
    
    void times_activation_func_deriv(double* timesarr, double* outarr, int size){
        switch(activation){
            case LOGISTIC:
                for(int i=0;i<size;i++){
                    double val = timesarr[i];
                    outarr[i]*=val*(1-val);
                }
                break;
            case TANH:
                for(int i=0;i<size;i++) outarr[i]*=1-pow(timesarr[i],2);
                break;
            case RELU:
                for(int i=0;i<size;i++) outarr[i]*= timesarr[i]>0 ? 1 : 0;
                break;
                
        }
    }

    ~MLPNetwork(){
        for(int i=1;i< num_layers;i++){
            MKL_free(weights[i-1]);
            MKL_free(biases[i]);
            MKL_free(layers[i]);
            MKL_free(errors[i]);
        }
        
        delete[] weights;
        delete[] layers;
        delete[] biases;
        delete[] errors;
    }
    
};

template <typename NETWORK_TYPE>
map<string, double> Network::cross_validate(ARFFDataset& data, vector<int> hidden_layer_sizes, int num_epochs, double lr, int num_folds, ACTIVATION activation, int random_state){
    
    map<string, double> avgscores;
    int max_folds=num_folds;
    
    auto meta = data.getMeta();
    vector<int> sizes = {meta.get_input_layer_size()};
    for(int x : hidden_layer_sizes) sizes.push_back(x);
    sizes.push_back(meta.get_output_layer_size());
    
    NETWORK_TYPE net(sizes, lr, activation, random_state);
    
    vector<string> classlabels = meta.getValues(meta.getClassLabel());
    bool classification = meta.get_output_layer_size()>1;
    
    long double train_time=0, tot_time=0;
    auto tot_start = chrono::system_clock::now();
    for(int fold =0; fold<max_folds;fold++){
        net.randomize_weights_and_biases();
        ARFFDataset trainingset;
        ARFFDataset testset;
        auto entries = data.getData();
        long int num_entries = data.getSize();
        for(int i=0;i<num_entries;i++){
            if(i<=fold*num_entries/max_folds || i>(fold+1)*num_entries/max_folds) trainingset.addEntry(entries[i]);
            else testset.addEntry(entries[i]);
        }
        
        chrono::time_point<chrono::system_clock> start, end;
        chrono::duration<long double> elapsed;
        
        start = chrono::system_clock::now();
        /*
        for(int i=0;i<num_epochs;i++){
            for(int i=0;i<num_entries;i++){
                if(i<=fold*num_entries/max_folds || i>(fold+1)*num_entries/max_folds) net.train(entries[i]);
                else testset.addEntry(entries[i]);
            }
        }*/
        for(int i=0;i<num_epochs;i++){
            for(ARFFEntry& e : trainingset.getData()){
                net.train(e);
            }
        }
        
        end = chrono::system_clock::now();
        elapsed = end - start;
        train_time += chrono::duration_cast<chrono::nanoseconds>(elapsed).count();
        
        
        if(classification){
            
            vector<tuple<string, string>> results;
            for(ARFFEntry& e : testset.getData()){
                string predicted = net.classify(e, classlabels);
                string actual = e.getClass();
                results.push_back(make_tuple(actual,predicted));
            }
            
            map<string, double> scores;
            
            
            double accuracy=0,macro_recall=0,macro_precision=0,macro_f1=0;
            for(string classlabel : classlabels){
                int tp=0,fp=0,fn=0;
                for(auto& pair: results){
                    string actual = get<0>(pair);
                    string predicted = get<1>(pair);
                    if(actual==classlabel){
                        if(predicted==classlabel)tp++;
                        else fn++;
                    }
                    else if(predicted==classlabel)fp++;
                }
                
                double micro_recall = (double)tp/(double)(tp+fn);
                string mr = MICRO_RECALL;
                mr.append(classlabel);
                scores[mr]=micro_recall;
                macro_recall+=micro_recall;
                
                double micro_prec = (double)tp/(double)(tp+fp);
                string mp = MICRO_PRECISION;
                mp.append(classlabel);
                scores[mp]=micro_prec;
                macro_precision+=micro_prec;
                
                double micro_f1 = (2*micro_recall*micro_prec)/(micro_recall+micro_prec);
                string mf = MICRO_F1;
                mf.append(classlabel);
                scores[mf]=micro_f1;
                macro_f1+=micro_f1;
                
                accuracy+=tp;
            }
            
            accuracy/=results.size();
            macro_recall/=classlabels.size();
            macro_precision/=classlabels.size();
            macro_f1/=classlabels.size();
            
            scores[MACRO_RECALL]=macro_recall;
            scores[MACRO_PRECISION]=macro_precision;
            scores[MACRO_F1]=macro_f1;
            scores[ACCURACY]=accuracy;
            
            
            for(const auto& pair: scores){
                avgscores[pair.first]+=pair.second/max_folds;
            }
            
        }//end if task is classification
        
        else{//if task is regression
            
            double mean_val = testset.getMean(meta.getClassLabel());
            double mae=0, mse=0, rmse=0, ssr=0, ss=0, mape=0;
            for(ARFFEntry& e : testset.getData()){
                double predicted = net.predict(e);
                double actual = stof(e.getClass());
                mae+=abs(actual-predicted);
                mse+=pow(actual-predicted,2);
                ssr+=pow(actual-predicted,2);
                ss+=pow(actual-mean_val,2);
                mape+=abs(actual-predicted)/abs(actual);
                
            }
            
            map<string, double> scores;
            
            scores[MAE]=mae/testset.getSize();
            scores[MSE]=mse/testset.getSize();
            scores[RMSE]=sqrt(mse)/testset.getSize();
            scores[RSQUARED]= 1 - (ssr/ss);
            scores[MAPE] = mape*100/testset.getSize();
            
            
            for(const auto& pair: scores){
                avgscores[pair.first]+=pair.second/max_folds;
            }
            
        }//end if task is regression
        
    }//end for every fold
    auto tot_end = chrono::system_clock::now();
    tot_time = chrono::duration_cast<chrono::nanoseconds>(tot_end-tot_start).count();
    tot_time/=1e9;//convert to seconds
    avgscores[TOTAL_TIME]=tot_time;
    
    train_time/=1e9;
    avgscores[TRAIN_TIME]=train_time;
    
    return avgscores;
}



#endif /* Network_h */
