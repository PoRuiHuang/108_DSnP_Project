/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string JsonLine;
   string Key;
   string Value;
   int  ParaCnt = 2;
   size_t sz;
   while(ParaCnt!=0){
      getline(is,JsonLine);
      if(JsonLine.find("{")!=string::npos ||JsonLine.find("}")!=string::npos ){
         ParaCnt--;
      }
      else{
         int countQuo = 2;
         if(JsonLine.size()>0){
         for(unsigned int j = 0; j<JsonLine.size(); j++){
          if((int)JsonLine[j] ==34){
             countQuo--;
             if(countQuo !=0){
                 while(((JsonLine[j+1] >= 65 && JsonLine[j+1]  <= 90) || (JsonLine[j+1]  >= 97 && JsonLine[j+1]  <= 122))|| (JsonLine[j+1]  >= 48 && JsonLine[j+1]  <= 57)|| (JsonLine[j+1] ==95)){
                    Key = Key + JsonLine[j+1];
                    j++;
                 }
             }
          }
          if((int)JsonLine[j] ==58){
             j++;
             while(JsonLine[j] ==32||JsonLine[j] ==44){
                j++;
             }
            while(JsonLine[j] == 45|| (JsonLine[j]  >= 48 && JsonLine[j]  <= 57)) {
               Value = Value + JsonLine[j];
               j++;
             }
             
          }
        else
           continue;         
      }

   int IntValue = stoi(Value,&sz);     
   DBJsonElem  New(Key,IntValue);
   j._obj.push_back(New);
   Value.clear();
   Key.clear();
   JsonLine.clear();
    }
   }
}
   j.read = true;
   return is;
}


ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   int count = 0;
   cout << "{" << endl;
	   for(unsigned int i = 0;i<j._obj.size();i++){
		   cout << "  "<< char(34)<< j._obj[i].key() << char(34) << " : "  <<  j._obj[i].value();
		   if(i<j._obj.size()-1 ){
			   cout << ",";
		   }
         count++;
		   cout << endl; 
	   }
	cout <<"}" << endl;
  cout << "Total JSON elements: " <<count << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/

/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/




void
DBJson::reset()
{
      this->_obj.clear();
      this->read = false;
   // TODO
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   for(unsigned int i = 0;i<this->_obj.size();i++){
       if(elm.key() == this->_obj[i].key()){
          cerr << "Error: Element with key \"" << this->_obj[i].key() <<"\" already exists!!" <<endl;
          return false;
       }
    }
    _obj.push_back(elm);
   // TODO
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
  	float ave = 0;
	float sum1 = 0; 
   if(this->_obj.size() == 0){
		return sqrt(-1.0);
	}
	else{
		for(unsigned int i = 0;i<this->_obj.size();i++){
			sum1 = sum1 + this->_obj[i].value();
		}
		ave = sum1/this->_obj.size();
	
	}
   return ave;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
    int maxN = INT_MIN;
		for(unsigned int i = 0;i<this->_obj.size();i++){
			if(this->_obj[i].value()>maxN){
				maxN = this->_obj[i].value();
            idx = i;
			}
		}
	
   // TODO
  
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO

   int minN = INT_MAX;
   for(unsigned int i = 0;i<this->_obj.size();i++){
			if(this->_obj[i].value()<minN){
				minN = this->_obj[i].value();
            idx = i;
			}
		}
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int sum = 0;
		for(size_t i = 0;i<this->_obj.size();i++){
			sum = sum + this->_obj[i].value();
		}
   return sum;
}
