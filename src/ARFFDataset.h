/*
 * Filename: ARFFDataset.h
 * Created Date: 2/18/23
 * Author: Harrison Paas
 * 
 * Description: This file contains the implementation of various methods for loading and processing data in ARFF format.
 */

#ifndef ARFFDataset_h
#define ARFFDataset_h


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>
#include <unordered_map>
#include "ARFFEntry.h"
#include "ARFFMetaData.h"



class ARFFDataset{
    
private:
    vector<ARFFEntry> data;
    ARFFMetaData meta;
    
public:
    
    ARFFDataset(){}
    
    vector<ARFFEntry>& getData(){return data;}
    
    void addEntry(ARFFEntry& e){data.emplace_back(e);}
    
    void setMeta(ARFFMetaData& meta){ this->meta=meta;}
    
    ARFFMetaData& getMeta(){return meta;}
    
    long int getSize(){return data.size();}
    
    string getClassLabel(){return meta.getClassLabel();}
    
    //returns the mean of the attribute with specified label
    double getMean(string label){
        
        int val_index = meta.calcNumericDataIndex(label);
        
        if(val_index==-1){
            cerr<<"Error. invalid label used getting mean of dataset "<<meta.getRelation()<<endl;
            cerr<<"Invalid label: "<<label<<endl;
            return 0;
        }
        
        double mean =0, total=0;
        for(ARFFEntry& e : data){
            if(!isnan(e.data[val_index])){
                mean+=e.data[val_index];
                total++;
            }
        }
        mean/=total;
        return mean;
    }

    //returns the mean for each class of the attribute with specified label
    //output[classlabel] = mean_for_that_class
    unordered_map<string, double> getMeanByClass(string label){
        
        vector<string> classlabels = meta.getValues(CLASSLABEL);
        unordered_map<string,double> means;
        
        int index = meta.calcNumericDataIndex(label);

        for(string classlabel : classlabels){
            
            double mean=0, total=0;
            for(ARFFEntry& e : data){
                if(e.getClass()==classlabel && !isnan(e.data[index])){
                    mean+=e.data[index];
                    total++;
                }
            }
            mean/=total;
            means[classlabel]=mean;
        }
        return means;
    }

    //returns the standard deviation of the attribute with specified label
    double getStdDev(string label){
        
        int val_index = meta.calcNumericDataIndex(label);
        
        if(val_index<0){
            cerr<<"Error. invalid label used getting std dev of dataset "<<meta.getRelation()<<endl;
            cerr<<"Invalid label: "<<label<<endl;
            return 0;
        }
        
        double mean = getMean(label);
        double dev =0, total=0;
        for(ARFFEntry& e : data){
            if(!isnan(e.data[val_index])){
                dev+=pow((e.data[val_index]-mean),2);
                total++;
            }
        }
        dev/=total;
        dev=sqrt(dev);
        return dev;
    }

    //z-score normalize attribute with specified label
    void normalizeAttribute(string label){
        
        int val_index = meta.calcNumericDataIndex(label);
        
        if(val_index==-1){
            cerr<<"Error. invalid label used getting mean of dataset "<<meta.getRelation()<<endl;
            cerr<<"Invalid label: "<<label<<endl;
            return;
        }
        
        double mean = getMean(label);
        double std = getStdDev(label);
        
        if(!isnan(mean) && !isnan(std)){
            for(ARFFEntry& e : data){
                e.data[val_index]-=mean;
                e.data[val_index]/=std;
            }
        }
    }

    //z-score normalize all numeric attributes
    void normalize(){
        for(Attribute& a : meta.getAttributes()){
            if(a.getType()==NUMERIC && a.getLabel()!=CLASSLABEL) normalizeAttribute(a.getLabel());
        }
    }

    //returns the index of the mode value in ARFFMetaData::getValues() for the attribute with specified label
    int getModeIndex(string label){
        
        tuple<int,int> range = meta.calcCategoricalIndexRange(label);
        vector<string> values = meta.getValues(label);
        
        int start = get<0>(range);
        int end = get<1>(range);
        
        unordered_map<int, int> freq_map;
        for(ARFFEntry& e : data){
            for(int i=start;i<end;i++){
                if(e.data[i]==1) freq_map[i-start]++;
            }
            
        }
        
        int mode_index=0;
        int max_freq = 0;
        for (const auto& pair : freq_map) {
            if (pair.second > max_freq) {
                max_freq = pair.second;
                mode_index = pair.first;
            }
        }
        return mode_index;
        
    }

    //returns the index of the mode value for each class in ARFFMetaData::getValues() for the attribute with specified label
    //output[classlabel] = mode_index_for_that_class
    unordered_map<string, int> getModeIndexByClass(string label){
        
        vector<string> classlabels = meta.getValues(CLASSLABEL);
        unordered_map<string,int> modes;
        
        tuple<int,int> range = meta.calcCategoricalIndexRange(label);
        vector<string> values = meta.getValues(label);
        
        int start = get<0>(range);
        int end = get<1>(range);
        
        for(string classlabel : classlabels){
            
            unordered_map<string, int> freq_map;
            for(ARFFEntry& e : data){
                if(e.getClass()==classlabel){
                    for(int i=start;i<end;i++){
                        if(e.data[i]==1) freq_map[values.at(i-start)]++;
                    }
                }
            }
            
            string mode;
            int max_freq = 0;
            for (const auto& pair : freq_map) {
                if (pair.second > max_freq) {
                    max_freq = pair.second;
                    mode = pair.first;
                }
            }
            int index=0;
            for(string val : values){
                if(val==mode) modes[classlabel]=index;
                index++;
            }
        }
        
        return modes;
    }
    
    //returns the mode for each class in ARFFMetaData::getValues() for the attribute with specified label
    //output[classlabel] = mode_for_that_class
    unordered_map<string, string> getModeByClass(string label){
        
        unordered_map<string, string> modes;
        auto indices = getModeIndexByClass(label);
        for(Attribute& a : meta.getAttributes()){
            if(a.getType()==CATEGORICAL && a.getLabel()==label){
                for(string classlabel : meta.getClassLabels()){
                    modes[classlabel] = a.getValues().at(indices[classlabel]);
                }
            }
        }
        return modes;
    }

    //replace missing values with means and modes
    void replaceMissingValues(){
        
        for(Attribute& a : meta.getAttributes()){
            
            if(a.getLabel()==CLASSLABEL) continue;
            else if(a.getType()==NUMERIC){
                
                int index = meta.calcNumericDataIndex(a.getLabel());
                double mean = getMean(a.getLabel());
                for(ARFFEntry& e : data){
                    if(e.isMissing(index, index+1)) e.data[index]=(isnan(mean)) ? 0 : mean;
                }
            }
            else if(a.getType()==CATEGORICAL){
                
                tuple<int, int> range = meta.calcCategoricalIndexRange(a.getLabel());
                int start = get<0>(range);
                int end = get<1>(range);
                int mode_index = getModeIndex(a.getLabel());
                for(ARFFEntry& e : data){
                    if(e.isMissing(start,end)) e.data[start+mode_index]=1;
                }
            }
        }
    }

    //replace missing values with means and modes by class label
    void replaceMissingValuesByClass(){
        
        vector<string> classlabels = meta.getValues(CLASSLABEL);
        for(Attribute& a : meta.getAttributes()){
            
            if(a.getLabel()==CLASSLABEL) continue;
            
            else if(a.getType()==NUMERIC){
                unordered_map<string, double> means = getMeanByClass(a.getLabel());
                int index = meta.calcNumericDataIndex(a.getLabel());
                for(string classlabel : classlabels){
                    for(ARFFEntry& e : data){
                        if(e.getClass()==classlabel && e.isMissing(index, index+1)) e.data[index]= (isnan(means[classlabel])) ? 0 : means[classlabel];
                    }
                }
            }
            
            else if(a.getType()==CATEGORICAL){
                unordered_map<string, int> mode_indices = getModeIndexByClass(a.getLabel());
                tuple<int, int> range = meta.calcCategoricalIndexRange(a.getLabel());
                int start = get<0>(range);
                int end = get<1>(range);
                for(string classlabel : classlabels){
                    int mode_index = mode_indices[classlabel];
                    for(ARFFEntry& e : data){
                        if(e.getClass()==classlabel && e.isMissing(start,end)) e.data[start+mode_index]=1;
                    }
                }
            }
        }
    }

    //shuffle data
    void shuffle(unsigned seed = 420){
        std::random_device rd;
        std::mt19937 rng(rd());
        rng.seed(seed);
        std::shuffle(data.begin(), data.end(), rng);
    }
    
    //load arff file into data
    static void loadARFF(string filename, ARFFDataset& data){
        ifstream inFile;
        inFile.open(filename.c_str());
        
        if(!inFile) {
            cerr<<"unable to open file: "<<filename<<endl;
            return;
        }
        
        inFile>>data;
        inFile.close();
        
    }
    
    //for the python enjoyers
    static ARFFDataset loadARFF(string filename){
        ARFFDataset data;
        loadARFF(filename, data);
        return data;
        
    }
    
    friend istream& operator>>(istream& inFile, ARFFDataset& data){
        
        ARFFMetaData meta;
        
        string relation;
        while(relation[0]!='@') {
            inFile>>relation;
            if(isspace(relation[0])) relation.erase(0,1);
        }
        inFile>>relation;
        meta.setRelation(relation);
        
        
        string line, att, label, s;
        while(line[0]!='@') getline(inFile,line);
        char type;
        while(!line.empty() && tolower(line[1])!='d'){
            stringstream l(line);
            l>>att;
            l>>label;
            l>>type;
            
            Attribute a(label);
            if(type==OPEN_BRACKET){
                a.setType(CATEGORICAL);
                while(getline(l,s,META_DELIM)){
                    if(isspace(s[0])) s.erase(0,1); //removes first character if its a space
                    if(isspace(s[s.length()-1])) s.pop_back();
                    if(s[s.length()-1]==CLOSE_BRACKET)s.pop_back();
                    if(isspace(s[s.length()-1])) s.pop_back();
                    a.addValue(s);
                }
            }
            else{
                a.setType(NUMERIC);
            }
            meta.addAttribute(a);
            getline(inFile,line);
            if(!line.empty() && isspace(line[line.length()-1])) line.pop_back();
        }
        meta.update_input_layer_size();
        meta.update_output_layer_size();
        data.setMeta(meta);
        
        if(isspace(line[line.length()-1])) line.pop_back();
        while(line[0]!='@') getline(inFile,line);
        
        int entry_data_length = meta.calcEntryVectorLength();
        int entry_class_length = meta.calcExpectedVectorLength();
        while(getline(inFile,line)){
            if(isspace(line[line.length()-1])) line.pop_back();
            if(line.empty()) continue;
            
            stringstream l(line);
            
            ARFFEntry e(entry_data_length, entry_class_length);
            
            int data_index=0;
            for(Attribute& a : meta.getAttributes()){
                
                string s;
                getline(l,s,DATA_DELIM);
                if(s[0]==' ') s.erase(0,1);
                //cout<<s<<endl;
                if(a.getType()==NUMERIC){
                    if(a.getLabel()==CLASSLABEL){
                        e.expected[0]=stof(s);
                        e.setClass(s);
                    }
                    else{
                        if(s==NUM_MISSING_VAL){
                            e.data[data_index] = nanf("");
                            data_index++;
                        }
                        else{
                            e.data[data_index]=stof(s);
                            data_index++;
                        }
                    }
                }
                else if(a.getType()==CATEGORICAL){
                    
                    if(a.getLabel()==CLASSLABEL){
                        int i=0;
                        for(string val : a.getValues()){
                            if(s==val){
                                e.expected[i]=1;
                                e.setClass(val);
                            }
                            else e.expected[i]=0;
                            i++;
                        }
                    }
                    else{
                        for(string val : a.getValues()){
                            e.data[data_index] = (s==val) ? 1 : 0;
                            data_index++;
                        }
                    }
                }
            }//end of for attribute a : attributes
            
            data.addEntry(e);
        }//end of while
        
        return inFile;
    }
    
    friend ostream& operator<<(ostream& os, ARFFDataset& data){
        
        auto meta = data.getMeta();
        os<<meta<<endl;
        
        os<<"@data\n"<<endl;
        for(ARFFEntry& e : data.getData()){
            int data_index=0;
            bool end_of_line=false;
            for(Attribute& a : meta.getAttributes()){
                if(a.getLabel()==meta.getAttributes().back().getLabel()) end_of_line=true;
                
                if(a.getType()==NUMERIC){
                    if(a.getLabel()==CLASSLABEL) os<<e.expected[0]<<((end_of_line) ? "" : ", ");
                    else{
                        os<<e.data[data_index]<<((end_of_line) ? "" : ", ");
                        data_index++;
                    }
                }
                else{
                    if(a.getLabel()==CLASSLABEL){
                        int i=0;
                        for(string val : a.getValues()){
                            if(e.expected[i]==1) os<<val<<((end_of_line) ? "" : ", ");
                            i++;
                        }
                    }
                    else{
                        bool missing=true;
                        for(string val : a.getValues()){
                            if(e.data[data_index]==1){
                                os<<val<<((end_of_line) ? "" : ", ");
                                missing=false;
                            }
                            data_index++;
                        }
                        if(missing) os<<STR_MISSING_VAL<<((end_of_line) ? "" : ", ");
                    }
                }
            }//end for attribute a
            os<<endl;
        }//end for entry e
        return os;
        
    }
    
};

#endif /* ARFFDataset_h */
