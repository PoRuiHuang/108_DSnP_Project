/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
   string get_key();
   int get_value();
   void setElem();
   void save_key(const string& k);
   int save_value(const int& v);
   bool is_key(const string& k);
   bool is_value(const int& v);
   friend ostream& operator << (ostream&, const JsonElem&);
   friend void get_key(const string&);
private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
   // TODO: define constructor & member functions on your own
   void get_key(unsigned int i, const string&);
   void push_back(const JsonElem&);
   bool read(const string&);
   void print();
   void ave();
   void sum();
   void min();
   void max();
   void add(const string&);

private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
};

#endif // P2_TABLE_H
