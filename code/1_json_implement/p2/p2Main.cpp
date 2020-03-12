/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <stdlib.h>
using namespace std;
void print(vector<string> const &input)
{
	for (unsigned int i = 0; i < input.size(); i++) {
		cout << input[i] << ' ';
	}
}
int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }
   size_t sz;
   ifstream file(jsonFile, ios::in);
   string str;
   vector<string> aStr;

   while (getline(file, str)){
      if(str.empty()){
         continue;
      }
      else{
         aStr.push_back(str);
      }
      
   }
   // print(aStr);
   // TODO read and execute commands
   //  cout << aStr.size();
   //  cout << aStr[0] << endl;
   //  cout << aStr[1] << endl; //"
   //  cout << aStr[2] << endl;
   //  cout << aStr[3] << endl;
   //  cout << aStr[4] << endl;
   //  cout << aStr[5] << endl;
   for(unsigned int i = 1; i<aStr.size()-1; i++){
     
      string Key;
      string Value;
      int countQuo = 2;
      for(unsigned int j = 0; j<aStr[i].size(); j++){
         // int countQuo = 2;
          if((int)aStr[i][j] ==34){
             countQuo--;
             if(countQuo !=0){               
                 while(((aStr[i][j+1] >= 65 && aStr[i][j+1]  <= 90) || (aStr[i][j+1]  >= 97 && aStr[i][j+1]  <= 122))|| (aStr[i][j+1]  >= 48 && aStr[i][j+1]  <= 57)|| (aStr[i][j+1] ==95)){
                    Key = Key + aStr[i][j+1];
                    j++;
                 }
             }
          }
          if((int)aStr[i][j] ==58){
             j++;
             while(aStr[i][j] ==32||aStr[i][j] ==44){
                j++;
             }
            while(aStr[i][j] == 45|| (aStr[i][j]  >= 48 && aStr[i][j]  <= 57)) {
               Value = Value + aStr[i][j];
               j++;
             }
             
          }
        else
        {
           continue;
        }
         
      }
      // cout << aStr[0];
      // cout << Key << endl;
      int fuck = stoi(Value, &sz);
      // cout << fuck << endl;

     
     JsonElem  New(Key,fuck);
      json.push_back(New);
   }

// json._obj[]._key
// json._obj[]._value

   cout << "Enter command: ";

   while (true) {
       string command;
       command.clear();
         getline(cin,command);
      if(command.empty() == 0){
      if(command == "PRINT"){
         json.print();
      }
      else if(command == "SUM"){
         json.sum();
      }
      else if(command == "AVE"){
         json.ave();
      }
      else if(command == "MAX"){
         json.max();
      }
      else if(command == "MIN"){
         json.min();   
      }
      else if((char)command[0] == char(65)&& (char)command[1] == char(68) &&  (char)command[2] == char(68) && (char)command[3] == char(32)){
         json.add(command); 
      }
      else if(command == "EXIT"){
         exit(-1);
      }
      else{
         cout << "Error: unknown command: " << char(34) << command << char(34)<< endl;
      }
                cout << "Enter command: ";
      }
      }
   exit(-1);
}

