//
//  ARFFEntry.h
//  DMProj2
//
//  Created by Harry Paas on 2/27/23.
//

#ifndef ARFFEntry_h
#define ARFFEntry_h

#ifndef DATA_ALIGNMENT
#define DATA_ALIGNMENT 64
#endif

#include <string>
#include <functional>

#include <mkl.h>

using namespace std;

class ARFFEntry{
    
private:
    int data_size;
    int expected_size;
    string classlabel;
    
public:
    double* data;
    double* expected;
    
    ARFFEntry(){
        data_size=0;
        expected_size=0;
        classlabel="";
        data=NULL;
        expected=NULL;
    }
    
    ARFFEntry(int input_vector_size, int expected_vector_size){
        data_size=input_vector_size;
        expected_size=expected_vector_size;
        data = (double*)MKL_malloc(sizeof(double)*data_size, DATA_ALIGNMENT);
        expected = (double*)MKL_malloc(sizeof(double)*expected_size, DATA_ALIGNMENT);
    }
    
    ARFFEntry(const ARFFEntry& other){
        data_size=other.get_data_size();
        expected_size=other.get_expected_size();
        data = (double*)MKL_malloc(sizeof(double)*data_size, DATA_ALIGNMENT);
        expected = (double*)MKL_malloc(sizeof(double)*expected_size, DATA_ALIGNMENT);
        
        for(int i=0;i<data_size;i++) data[i]=other.data[i];
        for(int i=0;i<expected_size;i++) expected[i]=other.expected[i];
        classlabel = other.getClass();
        
    }
    
    ARFFEntry(ARFFEntry&& other) //noexcept
      : data(nullptr), expected(nullptr), data_size(0), expected_size(0), classlabel("")
    {
        swap(data, other.data);
        swap(expected, other.expected);
        swap(data_size, other.data_size);
        swap(expected_size, other.expected_size);
        swap(classlabel, other.classlabel);
    }
    
    friend void swap(ARFFEntry& first, ARFFEntry& second) noexcept {
        using std::swap;
        swap(first.data_size, second.data_size);
        swap(first.expected_size, second.expected_size);
        swap(first.classlabel, second.classlabel);
        swap(first.data, second.data);
        swap(first.expected, second.expected);
    }
    
    void setClass(string classlabel){ this->classlabel=classlabel;}
    
    string getClass() const{ return classlabel;}
    
    int get_data_size() const {return data_size;}
    int get_expected_size() const {return expected_size;}
    
    void set_data_size(int size) {data_size=size;}
    void set_expected_size(int size) {expected_size=size;}
    
    void inc_data_size() {data_size++;}
    void inc_expected_size() {expected_size++;}

    bool isMissing(int data_index_start, int data_index_end){
        
        if(data_index_end-1==data_index_start) return isnan(data[data_index_start]);
        for(int i=data_index_start;i<data_index_end;i++){
            if(data[i]!=0 && !isnan(data[i])) return false;
        }
        return true;
    }
    
    ~ARFFEntry(){
        MKL_free(data);
        MKL_free(expected);
    }
    
};

#endif /* ARFFEntry_h */
