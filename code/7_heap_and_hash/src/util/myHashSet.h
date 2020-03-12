/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(Data* hash_node = 0, size_t num_b = 0, vector<Data>* b = 0 ):_hash_node(hash_node), _num_b(num_b), _b(b){}
      ~iterator(){}
      const Data& operator * () const { return *_hash_node; }
      iterator& operator ++ () {
         size_t num = (*_hash_node)()%_num_b;
         size_t idx = 0;
         bool find = false;
         for(size_t i = 0; i< _b[num].size(); i++){
            idx = i;
            if(_b[num][i] ==*_hash_node ){
               find = true;
               break;
            }
         }
         if(find == true&& idx!=_b[num].size()-1 ) idx++;
         else{
            num = num+1;
            for(; num <_num_b;num++){
               if(_b[num].size()){
                  idx = 0;
                  find = true;
                  break;
               }
            }
            if(find ==false) num = (*_hash_node)()%_num_b;
         }

         _hash_node = &_b[num][idx];
         return (*this); 
      }


      iterator& operator ++ (int) {
         iterator temp = (*this);
         ++(*this);
         return temp;
      }

      iterator& operator -- () {
         size_t num = (*_hash_node)()%_num_b;
         size_t idx = 0;
         bool find = false;
         for(size_t i = 0; i< _b[num].size(); i++){
            idx = i;
            if(_b[num][i] ==*_hash_node ){
               find = true;
               break;
            }
         }
         if(find == true&&idx!=0) idx--;
         else{
            num = num-1;
            for(; num >=0;num--){
               if(_b[num].size()){
                  idx = _b[num].size()-1;
                  find = true;
                  break;
               }
            }
            if(find ==false) num = (*_hash_node)()%_num_b;
         }

         _hash_node = &_b[num][idx];
         return (*this); 
      }

      iterator& operator -- (int) {
         iterator temp = (*this);
         --(*this);
         return temp;
      }

      iterator& operator = (const iterator& i){
         this->_hash_node = i._hash_node;
         this->_num_b = i._num_b;
         this->_b = i._b;
         return(*this);
      }
      bool operator == (const iterator& i) const { 
         if(this->_hash_node == i._hash_node&& this->_num_b == i._num_b&&this->_b == i._b) return true;
         return false; 
      }
      bool operator != (const iterator& i) const { 
         if(this->_hash_node == i._hash_node&& this->_num_b == i._num_b&&this->_b == i._b) return false;
         return true; 
      }
   private:
      Data* _hash_node;
      size_t _num_b;  //separate from member of hashset
      vector<Data>*   _b; 
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      for(size_t i = 0; i <_numBuckets; i++){
         if(_buckets[i].size()){
            return iterator(&_buckets[i][0], _numBuckets, _buckets);
         }
      }
      return iterator(); 
   }
   // Pass the end
   iterator end() const { 
      for(size_t i = _numBuckets-1 ; i >=0; i--){
         if(_buckets[i].size()){
            return iterator(&_buckets[i].back(), _numBuckets, _buckets);
         }
      }
      return iterator();  
   }
   // return true if no valid data
   bool empty() const { 
      if(size()==0) return true; 
      return false;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(size_t i = 0; i<_numBuckets;i++){
         s = s + _buckets[_numBuckets].size();
      } 
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t d_pos = bucketNum(d);
      for(size_t i = 0; i<_buckets[d_pos].size(); i++){
         if(d == _buckets[d_pos][i]){
            return true;
         }
      } 
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
     size_t d_pos = bucketNum(d);
      for(size_t i = 0; i<_buckets[d_pos].size(); i++){
         if( _buckets[d_pos][i] == d){
            d =_buckets[d_pos][i];
            return true;
         }
      } 
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t d_pos = bucketNum(d);
      for(size_t i = 0; i<_buckets[d_pos].size(); i++){
         if( _buckets[d_pos][i] ==d){
             _buckets[d_pos][i] = d;
            return true;
         }
      }
      _buckets[d_pos].push_back(d); 
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      if(check(d)==true) return false;
      _buckets[bucketNum(d)].push_back(d); 
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t d_pos = bucketNum(d);
      for(size_t i = 0; i<_buckets[d_pos].size(); i++){
         if(d == _buckets[d_pos][i]){
            _buckets[d_pos][i] = _buckets[d_pos].back(); 
				_buckets[d_pos].pop_back();
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
