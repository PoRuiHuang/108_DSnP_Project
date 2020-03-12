/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d){ 
      int t = _data.size(); // next to the last  14
      _data.push_back(d); //data[14]
      while (t > 0) {
         int p = (t-1) / 2;
         if ( _data[p]<d) break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() { 
      delData(0);
   }
   void delData(size_t i) {
      size_t p = (size_t)i+1;
      size_t t = 2*p;
      while (_data.size() >= t) {
         if (_data.size() > t) {
            if (_data[t] < _data[t-1]) t++;
         }
         if ((_data[t-1]< _data.back())== false) break;
         _data[p-1] = _data[t-1];
         p = t;
         t = 2*p;
      }
      _data[p-1] = _data.back();
      _data.pop_back();
      
      Data d = _data[p-1];
      while (p > 1) {
         t = p / 2;
         if ((d < _data[t-1])==false) break;
         _data[p-1] = _data[t-1];
         p = t;
      }
      _data[p-1] = d;
    }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
