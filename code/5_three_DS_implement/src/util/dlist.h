/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _isSorted = 0;
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }

      iterator& operator ++ () { 
         _node = _node -> _next;
         return *(this); 
         }
      iterator operator ++ (int) {
          iterator temp = *this;
         this->_node = _node -> _next;
         return temp; 
         }
      iterator& operator -- () { 
         _node = _node -> _prev;
         return *(this); 
         }
      iterator operator -- (int) {
         iterator temp = *this;
         this->_node = _node -> _prev;
         return temp; 
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
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const {
      if(_head->_next == _head&& _head->_prev ==_head){
         return true;
      }
      return false;
   }
   size_t size() const { 
      size_t count = 0;
      DListNode<T> *pos = _head;
      while(pos != _head->_prev){
         pos = pos->_next;
         count++;
      }
       return count;
   }

   void push_back(const T& x) {
      if(empty()){
         DListNode<T>* t = new DListNode<T>(x);
         DListNode<T>* dummy = _head->_prev;
         _head = t;
         _head->_next = dummy; 
         _head->_prev = dummy;
         dummy->_next = t;
         dummy->_prev = t;
      }
      else{
         DListNode<T>* dummy = _head->_prev;
          DListNode<T>* t = new DListNode<T>(x,dummy->_prev,dummy);
          dummy->_prev->_next = t;
          dummy->_prev = t;
      }
    }
   void pop_front() {
      if(!empty()){
           DListNode<T>* dummy = _head->_prev;
           DListNode<T>* Next = _head->_next;
           delete _head;
           _head = Next;
            _head->_prev = dummy;
           dummy->_next = _head;
      }
   }
   void pop_back() {
      if(!empty()){
         if(_head->_prev ==_head->_next){
           DListNode<T>* dummy = _head->_prev;
           dummy ->_next = dummy;
           dummy ->_prev = dummy;
           delete _head;
           _head = dummy; 
         }
         else{
           DListNode<T>* dummy = _head->_prev;
           DListNode<T>* Prev = dummy->_prev;
           Prev->_prev->_next = dummy;
           dummy->_prev = Prev->_prev;
           delete Prev;
         }
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(empty()){
         return false;
      }
      DListNode<T>* dummy = _head->_prev;
      if(pos._node == _head)
         pop_front();
      else if(pos._node == dummy->_prev)
         pop_back();
      else{
         pos._node -> _prev -> _next = pos._node -> _next;
         pos._node -> _next -> _prev = pos._node -> _prev;
      }
      return true;
    }

   bool erase(const T& x) {
      if(empty()){
         return false;
      }
      else{
         for(iterator i = begin(); i !=end();i++){
            if(i._node->_data == x){
               erase(i);
               return true;
            }
         }
      }
      return false;
   }

   iterator find(const T& x) {  
      if(empty()){
         return end();
      }
      else{
         for(iterator i = begin(); i !=end();i++){
            if(i._node->_data == x){
               return i;
            }
         }
      }
      return end(); 
   }

   void clear() {
      if(!empty()){
            while(_head!= _head->_prev){
               pop_front();
            }
      }
      _head = end()._node; 
      _head->_prev = _head;
      _head->_next = _head;
    }  // delete all nodes except for the dummy node

   void sort() const {
     iterator i = begin();
     size_t length = size();
     for (size_t j = 0; j < length - 1; j++) {
       for (size_t k = 0; k < length - 1 - j; k++) {
         if (i._node->_data > i._node->_next->_data) {
           T temp = i._node->_data;
           i._node->_data = i._node->_next->_data;
           i._node->_next->_data = temp;
         }
         i++;
       }
       i = begin();
     }
     _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
