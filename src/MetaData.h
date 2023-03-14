/*
 * Filename: MetaData.h
 * Created Date: 2/18/23
 * Author: Harrison Paas
 * 
 * Description: This file contains the implementation of a data object to represent an ARFF file's metadata
 */

#ifndef MetaData_h
#define MetaData_h

#ifndef CLASSLABEL
#define CLASSLABEL "class"
#endif

#ifndef STR_MISSING_VAL
#define STR_MISSING_VAL "?"
#endif

#ifndef NUM_MISSING_VAL
#define NUM_MISSING_VAL "?"
#endif

#ifndef OPEN_BRACKET
#define OPEN_BRACKET '{'
#endif

#ifndef CLOSE_BRACKET
#define CLOSE_BRACKET '}'
#endif

#ifndef META_DELIM
#define META_DELIM ','
#endif

#ifndef DATA_DELIM
#define DATA_DELIM ','
#endif

#ifdef CLASS
#undef CLASSLABEL
#define CLASSLABEL CLASS
#endif

#ifdef CATEGORICAL_MISSING_VAL
#undef STR_MISSING_VAL
#define STR_MISSING_VAL CATEGORICAL_MISSING_VAL
#endif

#ifdef NUMERIC_MISSING_VAL
#undef NUM_MISSING_VAL
#define NUM_MISSING_VAL NUMERIC_MISSING_VAL
#endif

#ifdef UNIQUE_VALS_OPEN
#undef OPEN_BRACKET
#define OPEN_BRACKET UNIQUE_VALS_OPEN
#endif

#ifdef UNIQUE_VALS_CLOSE
#undef CLOSE_BRACKET
#define CLOSE_BRACKET UNIQUE_VALS_CLOSE
#endif

#ifdef UNIQUE_VALS_DELIM
#undef META_DELIM
#define META_DELIM UNIQUE_VALS_DELIM
#endif

#ifdef FILE_DELIM
#undef DATA_DELIM
#define DATA_DELIM FILE_DELIM
#endif

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <stdexcept>

#include "attribute.h"

using namespace std;

class ARFFMetaData{
    
private:
    string relation;
    vector<Attribute> attributes;
    int entry_data_length;
    int expected_data_length;
    
public:
    
    string getRelation(){ return relation;}
    
    void setRelation(string r){relation=r;}
    
    vector<Attribute>& getAttributes(){ return attributes;}
    
    void addAttribute(Attribute& a) {attributes.push_back(a);}
    
    int get_num_attributes(){return (int)attributes.size();}
    
    string getClassLabel(){return CLASSLABEL;}
    
    //calculates the length of the array produced by the "one hot encoding"
    int calcEntryVectorLength(){
        int length=0;
        for(Attribute& a : attributes){
            if(a.getLabel()==CLASSLABEL) continue;
            else if(a.getType()==NUMERIC) length++;
            else if(a.getType()==CATEGORICAL){
                for(string s: a.getValues()) length++;
            }
        }
        
        return length;
    }
    
    int get_input_layer_size(){return entry_data_length;}
    void update_input_layer_size(){entry_data_length=calcEntryVectorLength();}
    void set_input_layer_size(int size){entry_data_length=size;}
    
    //calculates the length of the class values by the "one hot encoding"
    //class values and data values are stored in different arrays
    int calcExpectedVectorLength(){
        int length=0;
        for(Attribute& a : attributes){
            if(a.getLabel()==CLASSLABEL){
                if(a.getType()==NUMERIC) length=1;
                else if(a.getType()==CATEGORICAL){
                    for(string s : a.getValues()) length++;
                }
            }
        }
        
        return length;
    }
    
    int get_output_layer_size(){return expected_data_length;}
    void update_output_layer_size(){expected_data_length = calcExpectedVectorLength();}
    void set_output_layer_size(int size){expected_data_length=size;}
    
    //calculates the index of a numeric value in the array produced by the one hot encoding
    int calcNumericDataIndex(string label){
        
        if(label==CLASSLABEL){
            cerr<<"Error. You cannot use calcNumericDataIndex on the class label. The class values are stored in the expected array\n";
            return -1;
        }
        int index=0;
        for(Attribute& a : attributes){
            if(a.getLabel()==CLASSLABEL) continue;
            else if(a.getLabel()==label){
                if(a.getType()!=NUMERIC){
                    cerr<<"Error. Cannot use calcNumericDataIndex on categorical attribute.\n";
                    return -1;
                }
                return index;
            }
            else if(a.getType()==NUMERIC) index++;
            else if(a.getType()==CATEGORICAL){
                for(string val : a.getValues()) index++;
            }
        }
        cerr<<"Error.  Label "<<label<<" not found in calcNumericDataIndex\n.";
        return -1;
        
    }
    
    //calculates the range of indices that a categorical value is mapped to with one hot encoding
    tuple<int,int> calcCategoricalIndexRange(string label){
        
        if(label==CLASSLABEL){
            cerr<<"Error. You cannot use calcCategoricalIndexRange on the class label. The class values are stored in the expected array\n";
            return make_tuple(-1,-1);
        }
        
        int index=0, start=0;
        for(Attribute& a : attributes){
            if(a.getLabel()==CLASSLABEL) continue;
            else if(a.getLabel()==label){
                if(a.getType()!=CATEGORICAL){
                    cerr<<"Error. Cannot use calcNumericDataIndex on categorical attribute.\n";
                    return make_tuple(-1,-1);
                }
                start=index;
                for(string val : a.getValues()) index++;
                return make_tuple(start,index);
            }
            else if(a.getType()==NUMERIC) index++;
            else if(a.getType()==CATEGORICAL){
                for(string val : a.getValues()) index++;
            }
        }
        cerr<<"Error.  Label "<<label<<" not found in calcNumericDataIndex\n.";
        return make_tuple(-1,-1);
        
    }
    
    //return a reference to an attribute object in the metadata with specified label
    Attribute& getAttribute(string label){
        for(Attribute&a : attributes){
            if(a.getLabel()==label) return a;
        }
        cerr<<"Error. Unable to find attribute with label "<<label<<endl;
        throw invalid_argument("label not found\n");
    }
    
    //returns the unique values of an attribute by label
    vector<string>& getValues(string label){
        for(Attribute& a : attributes){
            if(a.getLabel()==label){
                if(a.getType()==NUMERIC) {
                    cerr<<"Did you mean to try to get the values of a numeric column in ARFFMetaData:: getValues?\n";
                    
                }
                else return a.getValues();
            }
        }
        cerr<<"Error. unable to find values for label: "<<label<<" in ARFFMetaData::getValues\n";
        vector<string> v;
        throw invalid_argument("label not found\n");
    }
    
    //returns the unique values for the class label
    //can only be used when class is categorical
    vector<string>& getClassLabels(){
        return getValues(CLASSLABEL);
    }
    
    
    friend ostream& operator<<(ostream& os, ARFFMetaData& meta){
        for(Attribute& a : meta.attributes){
            os<<"@attribute "<<a.getLabel()<<" ";
            
            if(a.getType()==NUMERIC) os<<NUMERIC<<endl;
            else{
                os<<"{ ";
                for(int i=0;i<a.getValues().size()-1;i++) os<<a.getValues().at(i)<<", ";
                os<<a.getValues().back()<<"}"<<endl;
            }
        }
        return os;
    }
    
    
};

#endif /* MetaData_h */
