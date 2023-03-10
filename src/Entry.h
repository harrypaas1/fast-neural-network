//
//  Entry.h
//  DMProj2
//
//  Created by Harry Paas on 2/15/23.
//

#ifndef Entry_h
#define Entry_h


#endif /* Entry_h */

#include <string>
#include <iostream>
#include <fstream>
#include <functional>

using namespace std;

class Entry : public ARFFEntry{
  
private:
    double age;
    string workclass;
    double fnlwgt;
    string education;
    double education_num;
    string marital_status;
    string occupation;
    string relationship;
    string race;
    string sex;
    double capital_gain;
    double capital_loss;
    double hours_per_week;
    string country;
    string classlabel;
    
public:
    
    
    
    Entry(){
        
        age=0;
        workclass="";
        fnlwgt=0;
        education="";
        education_num=0;
        marital_status="";
        occupation="";
        relationship="";
        race="";
        sex="";
        capital_gain=0;
        capital_loss=0;
        hours_per_week=0;
        country="";
        classlabel="";
    }
    
    
    double getAge(){return age;}
    string getWorkclass(){return workclass;}
    double getFnlwgt(){ return fnlwgt;}
    string getEducation(){ return education;}
    double getEducationNum(){ return education_num;}
    string getMarital(){ return marital_status;}
    string getOccupation(){ return occupation;}
    string getRelationship(){ return relationship;}
    string getRace(){ return race;}
    string getSex(){ return sex;}
    double getCapGain(){ return capital_gain;}
    double getCapLoss(){ return capital_loss;}
    double getHrs(){ return hours_per_week;}
    string getCountry(){ return country;}
    string getClass(){ return classlabel;}
    
    
    void setAge(double age){ this->age=age;}
    void setWorkclass(string workclass){this->workclass=workclass;}
    void setFnlwgt(double fnlwgt){ this->fnlwgt= fnlwgt;}
    void setEducation( string education){ this->education=education;}
    void setEducationNum(double education_num){ this->education_num =education_num;}
    void setMarital(string marital_status){ this->marital_status=marital_status;}
    void setOccupation(string occupation){ this->occupation= occupation;}
    void setRelationship(string relationship){ this->relationship= relationship;}
    void setRace(string race){ this->race=race;}
    void setSex(string sex){ this->sex= sex;}
    void setCapGain(double capital_gain){ this->capital_gain= capital_gain;}
    void setCapLoss( double capital_loss){ this->capital_loss= capital_loss;}
    void setHrs(double hours_per_week){ this->hours_per_week=hours_per_week;}
    void setCountry(string country){ this->country=country;}
    void setClass(string classlabel){ this->classlabel=classlabel;}
    
    
    /*
    vector< function<double()> > get_numeric_getters() override {
        vector< function<double()> > getters = {
            bind(&Entry::getAge, this),
            bind(&Entry::getFnlwgt, this),
            bind(&Entry::getEducationNum, this),
            bind(&Entry::getCapGain, this),
            bind(&Entry::getCapLoss, this),
            bind(&Entry::getHrs, this)
        };
        return getters;
    }
    
    
    
    vector< function<string()> > get_categorical_getters() override {
        vector< function<string()> > getters = {
            bind(&Entry::getWorkclass, this),
            bind(&Entry::getEducation, this),
            bind(&Entry::getMarital, this),
            bind(&Entry::getOccupation, this),
            bind(&Entry::getRelationship, this),
            bind(&Entry::getRace, this),
            bind(&Entry::getSex, this),
            bind(&Entry::getCountry, this),
            bind(&Entry::getClass, this)
        };
        return getters;
    }
    
    
    vector< function<void(double)>> get_numeric_setters() override {
        vector< function<void(double)> > setters = {
            bind(&Entry::setAge, this, placeholders::_1),
            bind(&Entry::setFnlwgt, this, placeholders::_1),
            bind(&Entry::setEducationNum, this, placeholders::_1),
            bind(&Entry::setCapGain, this, placeholders::_1),
            bind(&Entry::setCapLoss, this, placeholders::_1),
            bind(&Entry::setHrs, this, placeholders::_1)
        };
    
        return setters;
    }
    
    vector< function<void(string)>> get_categorical_setters() override {
        vector< function<void(string)> > setters = {
            bind(&Entry::setWorkclass, this, placeholders::_1),
            bind(&Entry::setEducation, this, placeholders::_1),
            bind(&Entry::setMarital, this, placeholders::_1),
            bind(&Entry::setOccupation, this, placeholders::_1),
            bind(&Entry::setRelationship, this, placeholders::_1),
            bind(&Entry::setRace, this, placeholders::_1),
            bind(&Entry::setSex, this, placeholders::_1),
            bind(&Entry::setCountry, this, placeholders::_1),
            bind(&Entry::setClass, this, placeholders::_1)
        };
        return setters;
    }
    
    
    
    vector<string> getLabels() override {
        return {
            "age",
            "workclass",
            "fnlwgt",
            "education",
            "education-num",
            "marital-status",
            "occupation",
            "relationship",
            "race",
            "sex",
            "capital-gain",
            "capital-loss",
            "hours-per-week",
            "native-country",
            "class"
        };
    }*/
    
    
    
    
    //does not get the endline character at end of line
    friend istream& operator>>(istream& is, Entry& obj){
        string s;
        char comma;
        double x;
        is>>x;
        obj.setAge(x);
        is>>comma;
        is>>s;
        s.pop_back();
        obj.setWorkclass(s);
        is>>x;
        obj.setFnlwgt(x);
        is>>comma;
        is>>s;
        s.pop_back();
        obj.setEducation(s);
        is>>x;
        obj.setEducationNum(x);
        is>>comma;
        is>>s;
        s.pop_back();
        obj.setMarital(s);
        is>>s;
        s.pop_back();
        obj.setOccupation(s);
        is>>s;
        s.pop_back();
        obj.setRelationship(s);
        is>>s;
        s.pop_back();
        obj.setRace(s);
        is>>s;
        s.pop_back();
        obj.setSex(s);
        is>>x;
        obj.setCapGain(x);
        is>>comma;
        is>>x;
        obj.setCapLoss(x);
        is>>comma;
        is>>x;
        obj.setHrs(x);
        is>>comma;
        is>>s;
        s.pop_back();
        obj.setCountry(s);
        is>>s;
        obj.setClass(s);
        
        
        return is;
    }
    
    friend ostream& operator<<(ostream& os, Entry& obj){
        os<<obj.getAge()<<", ";
        os<<obj.getWorkclass()<<", ";
        os<<obj.getFnlwgt()<<", ";
        os<<obj.getEducation()<<", ";
        os<<obj.getEducationNum()<<", ";
        os<<obj.getMarital()<<", ";
        os<<obj.getOccupation()<<", ";
        os<<obj.getRelationship()<<", ";
        os<<obj.getRace()<<", ";
        os<<obj.getSex()<<", ";
        os<<obj.getCapGain()<<", ";
        os<<obj.getCapLoss()<<", ";
        os<<obj.getHrs()<<", ";
        os<<obj.getCountry()<<", ";
        os<<obj.getClass()<<std::endl;
        
        return os;
    }
    
    
    
    
    
    
    
    
    
    
};
