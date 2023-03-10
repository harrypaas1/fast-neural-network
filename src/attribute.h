/*
 * Filename: attribute.h
 * Created Date: 2/18/23
 * Author: Harrison Paas
 * 
 * Description: This file contains the implementation of a data object used to represent individual attributes of an ARFF file.
 */

#ifndef attribute_h
#define attribute_h
#define CATEGORICAL "categorical"
#define NUMERIC "numeric"
#endif /* attribute_h */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Attribute{

private:
    vector<string> values;
    string label;
    string type;
    
public:
    
    Attribute(){}
    
    Attribute(string label){this->label=label;}
    
    Attribute(string label, string type){
        this->label=label;
        this->type=type;
        values = vector<string>();
    }
    
    string getLabel(){return label;}
    
    string getType(){return type;}
    
    void setType(string type){this->type=type;}
    
    void setLabel(string label){this->label=label;}
    
    void addValue(string val){values.push_back(val);}
    
    vector<string>& getValues(){return values;}
    
    
    friend ostream& operator<<(ostream& os, const Attribute& obj){
        os<<"Label: "<<obj.label<<endl;
        os<<"Type: "<<obj.type<<endl;
        if(obj.type==CATEGORICAL){
            os<<"Values : {";
            for(int i=0; i<obj.values.size()-1;i++){
                os<<obj.values.at(i)<<", ";
                
            }
            os<<obj.values.back()<<"}"<<endl;
        }
        return os;
    }
    
};




