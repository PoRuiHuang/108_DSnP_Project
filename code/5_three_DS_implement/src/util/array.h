/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) { _isSorted = 0;}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      
      iterator& operator ++ () {
         _node++;
         return (*this); 
         }
      
      iterator operator ++ (int) {
         iterator temp = *this;
         this->_node++;
         return temp; 
         }

      iterator& operator -- () {
         _node--;
         return (*this); 
         }

      iterator operator -- (int) {
         iterator temp = *this;
        this->_node--;
         return temp; 
         }

      iterator operator + (int i) const { return iterator(_node+i); }

      iterator& operator += (int i) { 
         _node = _node+i;
         return (*this); 
         }

      iterator& operator = (const iterator& i) { 
         this->_node = i._node;
         return (*this); 
         }

      bool operator != (const iterator& i) const { 
         if(_node != i._node){
            return true;
         }
         return false; 
         }

      bool operator == (const iterator& i) const {  
         if(_node != i._node){
            return false;
         }
         return true; 
         }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data+_size); }
   bool empty() const {
      if(_size ==0){
         return true;
      }
       return false;
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return *(_data+i); }
   const T& operator [] (size_t i) const {return *(_data+i);}

   void push_back(const T& x) {
      if(_capacity==0){
         _capacity = 1;
         _data = new T[1];
         _data[0] = x;
         _size++;
      }
      else if(_capacity == _size){
         _capacity = _capacity*2;
         T* temp = _data;
         _data = new T[_capacity];
         for (size_t i = 0; i< _size; i++){
            _data[i] = temp[i];
         }
         delete[] temp;
         _data[_size]  = x;
         _size++;
      }
      else{
         _data[_size] = x;
         _size++;
      }
    }
   void pop_front() {
      if(!empty()){
            _data[0] = _data[_size-1];
            _size--;
         }
    }
   void pop_back() { 
      if(!empty()){
         _size--;
      }
   }

   bool erase(iterator pos) { 
      if(empty()){
         return false;
      }
      *(pos._node) = _data[_size-1];
      _size--;
      return true; 
      }
   bool erase(const T& x) { 
      if(!empty()){
         iterator pos = begin();
         while(pos != end()){
         if(*pos ==x){
            erase(pos);
            return true;
         }
         pos++;
      }
    }
      return false; 
   }

   iterator find(const T& x) { 
   if(!empty()){
       for(size_t i = 0; i<_size; i++){
         if(_data[i]==x){
            return iterator(&_data[i]);
         }
      }
    }
      return end(); 
   }


   void clear() {
      _size = 0;
      _isSorted = false;
    }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); _isSorted = 1;}

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted //就算是const也可以改mutable的變數

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
