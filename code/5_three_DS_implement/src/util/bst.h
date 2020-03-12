/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
     _data(d), _parent(p), _left(l), _right(r) {}

   ~BSTreeNode(){}

   T               _data;
   BSTreeNode<T>*  _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   public:
   BSTree() {
     _dummy = new BSTreeNode<T>(T("end"));
     _root = _min = _dummy;
   }
   ~BSTree() { clear(); delete _dummy; }
   // TODO: design your own class!!
   class iterator {
      friend class BSTREE;

   public:
      iterator(BSTreeNode<T>* n = 0): _node(n) {}
     iterator(const iterator& i): _node(i._node) {}
     ~iterator(){}// Should NOT delete _node



      
      // TODO: implement these overloaded operators
     const T& operator * () const { return _node->_data; }
     T& operator * () { return _node->_data; }

     iterator& operator ++ () {
       iterator add = *this;
       if (_node->_right) {
         _node = successor(_node);
         return *(this); 
       }
       else {
         while (true) {
           if(_node->_parent==NULL) {
               _node = add._node;
               return *(this); 
           }
           else if(_node == _node->_parent->_left) {
               _node = _node->_parent;
               return *(this); 
           }
           else if(_node == _node->_parent->_right) {
             _node = _node->_parent;
           }
           else {
              break;
           }
         }
         return *(this); 
       }
     }

      iterator& operator ++ (int) {
       iterator add = *this;
       if (_node->_right) {
         _node = successor(_node);
         return *(this); 
       }
       else {
         while (true) {
           if(_node->_parent==NULL) {
               _node = add._node;
               return *(this); 
           }
           else if(_node == _node->_parent->_left) {
               _node = _node->_parent;
               return *(this); 
           }
           else if(_node == _node->_parent->_right) {
             _node = _node->_parent;
           }
           else {
              break;
           }
         }
         return *(this); 
       }
     }

      iterator& operator -- () {
       iterator add = *this;
       if (_node->_left) {
         _node = desuccessor(_node);
         return *(this); 
       }
       else {
         while (true) {
           if(_node->_parent==NULL) {
               _node = add._node;
               return *(this); 
           }
           else if(_node == _node->_parent->_left) {
             _node = _node->_parent;
           }
           else if(_node == _node->_parent->_right) {
               _node = _node->_parent;
               return *(this); 
           }
           else {
              break;
           }
         }
         return *(this); 
       }
     }

      iterator& operator -- (int) {
       iterator add = *this;
       if (_node->_left) {
         _node = desuccessor(_node);
         return *(this); 
       }
       else {
         while (true) {
           if(_node->_parent==NULL) {
               _node = add._node;
               return *(this); 
           }
           else if(_node == _node->_parent->_right) {
               _node = _node->_parent;
               return *(this); 
           }
           else if(_node == _node->_parent->_left) {
             _node = _node->_parent;
           }
           else {
              break;
           }
         }
         return *(this); 
       }
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
         BSTreeNode<T>* _node;
         BSTreeNode<T>* successor(BSTreeNode<T>* _node){
            _node = _node->_right;
            while (_node->_left) {
            _node = _node->_left;
            }
            return _node;
         }
         BSTreeNode<T>* desuccessor(BSTreeNode<T>* _node){
            _node = _node->_left;
            while (_node->_right) {
            _node = _node->_right;
            }
            return _node;
         }
   };

      // TODO: implement these functions
   iterator begin() const {
      if (empty()) {
       return end();
     }
     return iterator(min(_root));
   }

   iterator end() const {
     return iterator(_dummy);
   }



   bool empty() const { 
    if(_root == _dummy){
       return true;
      }  
      return false;
   }

   size_t size() const {
     size_t count = 0;
     if(!empty()) {
       for (iterator i = begin(); i != end(); i++) {
         count++;
       }
     }
     return count;
   }

   void insert(const T& x){
      if(empty()){
       _root = new BSTreeNode<T>(x, 0, 0, _dummy);
       _min = _root;
       _dummy->_parent = _root;
      }
      else {
       BSTreeNode<T>* search = _root;
       BSTreeNode<T>* _insert = new BSTreeNode<T>(x);
       while(search!=NULL) {

         if(x < search->_data ) {
           if(search->_left != NULL){
              break;
           }  
           else{
              search = search->_left;
           }  
         }

         else if (x>= search->_data) {
           if(search->_right != NULL)  {
              break;
           }
           else if(search->_right == _dummy){
              break;
           }
           else{
              
           }  
         }
       }

       _insert->_parent = search;
       if(x < search->_data ) {
         if(search == _min) {
            _min = _insert;
            }
         search->_left = _insert;
       }
       else {
         if(search->_right == _dummy) { 
           _insert->_right = _dummy;
           _dummy->_parent = _insert; 
         }
         search->_right = _insert;
       }  
     }
   } 

   void pop_front() {
   }
   void pop_back() {
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      return true;
    }

   bool erase(const T& x) {
      return false;
   }

   iterator find(const T& x) {  
      return end(); 
   }

   void clear() {
    }  // delete all nodes except for the dummy node

   void sort() const {}
   void print() const {}



   private:
   BSTreeNode<T>* min(BSTreeNode<T>* _node) const {
      if (_node->_left == NULL) {
         return _node;
      } 
      else {
      while(_node->_left !=NULL){
         _node = _node ->_left;
      }
    }
    return _node;
  }
   BSTreeNode<T>*  _root;
   BSTreeNode<T>*  _min;
   BSTreeNode<T>*  _dummy;

};

#endif // BST_H
