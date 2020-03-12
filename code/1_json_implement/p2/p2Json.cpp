/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <iomanip>
#include <stdlib.h>



using namespace std;
// void print(vector<string> const &input)
// {
// 	for (unsigned int i = 0; i < input.size(); i++) {
// 		cout << input[i] << ' ';
// 	}
// }
//Implement member functions of class Row and Table here
bool Json::read(const string& jsonFile){
    ifstream file(jsonFile, ios::in);
    if(file.is_open()){
//     string str;
//     vector<string> aStr;
//     while (getline(file, str)){
// aStr.push_back(str);
file.close();
//   cout << aStr.size(); 
// print(aStr);   
   return true; // TODO
    }
    return false;
}

// bool JsonElem::is_key(const string& k){
// 	for (unsigned int i = 0; i < k.size(); i++)
// 	{
// 		int tmp = (int)k[i];
// 		if (((tmp >= 65 && tmp <= 90) || (tmp >= 97 && tmp <= 122))|| (tmp >= 48 && tmp <= 57)|| (tmp==95) )
// 		{
// 			continue;
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}
// 	return true;
// }

string JsonElem::get_key(){
	return _key;
}

int JsonElem::get_value(){
	return _value;
}
void Json::push_back(const JsonElem&k){
	this->_obj.push_back(k);
}

void Json::print(){
	cout << "{" << endl;
	for(unsigned int i = 0;i<this->_obj.size();i++){
		cout << "  "<< char(34)<< this->_obj[i].get_key() << char(34) << " : "  <<  this->_obj[i].get_value();
		if(i<this->_obj.size()-1 ){
			cout << ",";
		}
		cout << endl; 
	}
	cout <<"}" << endl;
}

void Json::sum(){
	if(this->_obj.size() == 0){
		cout << "Error: No element found!!" << endl;
	}
	else{
		int sum = 0;
		for(unsigned int i = 0;i<this->_obj.size();i++){
			sum = sum + this->_obj[i].get_value();
		}
	cout <<"The summation of the values is: " << sum <<"."<<endl; 
	}
	
}
void Json::ave(){
	if(this->_obj.size() == 0){
		cout << "Error: No element found!!" << endl;
	}
	else{
		float ave = 0;
		float sum1 = 0;
		for(unsigned int i = 0;i<this->_obj.size();i++){
			sum1 = sum1 + this->_obj[i].get_value();
		}
		ave = sum1/this->_obj.size();
	cout <<"The average of the values is: " << fixed <<setprecision(1) << ave  <<"."<<endl; 
	}
}

void Json::max(){
	if(this->_obj.size() == 0){
		cout << "Error: No element found!!" << endl;
	}
	else{
		int max = this->_obj[0].get_value();
		unsigned int num = 0;
		for(unsigned int i = 1;i<this->_obj.size();i++){
			if(this->_obj[i].get_value()>max){
				max = this->_obj[i].get_value();
				num = i;
			}
		}
	cout <<"The maximum element is: { " << char(34)  << this->_obj[num].get_key()  <<char(34)<< " : "<< max << " }." << endl; 
	}
}

void Json::min(){
	if(this->_obj.size() == 0){
		cout << "Error: No element found!!" << endl;
	}
	else{
		int min = this->_obj[0].get_value();
		unsigned int num = 0;
		for(unsigned int i = 1;i<this->_obj.size();i++){
			if(this->_obj[i].get_value()<min){
				min = this->_obj[i].get_value();
				num = i;
			}
		}
	cout <<"The minimum element is: { " << char(34)  << this->_obj[num].get_key()  <<char(34)<< " : "<< min << " }." << endl; 
	}
}

void Json::add(const string&add){
	string Key;
	string Value;
	string effect;
	size_t sz;
	// for(unsigned int i = 4; i <add.size();i++){
		unsigned int i = 4;
		while((char)add[i] == char(32)){
			i++;
		}
		while((char)add[i] != char(32)&& i < add.size()){
                    Key = Key + add[i];
                    i++;

                 }
		while((char)add[i] == char(32)){
			i++;
		}
		while((char)add[i] != char(32)&& i < add.size()) {
               Value = Value + add[i];
               i++;
			   }
		effect = Value;
		// break;
            //  }
	if(Value.empty()){
		cout <<  "Error: Missing argument after " << char(34) << Key << char(34) << "!!" << endl;
		Key.clear();
		return;
	}
	// cout << add<<endl;
	// cout << add.size()<<endl;
	// cout << Key<<endl;
	// cout << Key.size()<<endl;
	// cout <<Value<<endl;
	// cout << Value.size()<<endl;
	// cout << effect.size()<<endl;
	for(unsigned int j = 0; j<effect.size();j++){
		if(effect[0] == 45 ||(effect[j]  >= 48 && effect[j]  <= 57)){
		}
		else{
			cout<<"Error: Illegal argument " << char(34) << effect << char(34) <<"!!"<< endl;
			effect.clear();
			return;
		}
	}
	int fuck = stoi(Value, &sz);
	JsonElem  New(Key,fuck);
      this->push_back(New);
	  Key.clear();
	  Value.clear();
	  effect.clear();

}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

