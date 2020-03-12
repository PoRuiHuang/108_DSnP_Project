/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include "unordered_map"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*                  Class Type                    */
/*******************************/
class Key
{
public:
    Key(CirGate* f0, CirGate* f1, bool i_0, bool i_1): fanin0(f0), fanin1(f1), inv_0(i_0), inv_1(i_1){}
    ~Key(){}

    bool operator==(const Key &other) const{
      if          (fanin0 == other.fanin0 && fanin1 == other.fanin1 && inv_0 == other.inv_0 && inv_1 == other.inv_1) {return true;}
      else if(fanin0 == other.fanin1 && fanin1 == other.fanin0 && inv_0 == other.inv_1 && inv_1 == other.inv_0) {return true;}
      return false;
    }


    CirGate* fanin0;
    CirGate* fanin1;
    bool inv_0;
    bool inv_1;
};

  struct hash_Key
  {
    size_t operator()(const Key& k) const
    {
      size_t hash_0 = (size_t)(((size_t)k.fanin0) ^k.inv_0);
      size_t hash_1 = (size_t)(((size_t)k.fanin1) ^k.inv_1);

      // if(hash_0 > hash_1){
        
      //   size_t temp = hash_0;
      //   hash_0 = hash_1;
      //   hash_1 = temp;
      // }

      return (hash_0^hash_1);
    }
  };



/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  unordered_map<Key, CirGate*, hash_Key> map;
  for(size_t i = 0; i<totallist.size(); i++){
    if(totallist[i] !=NULL ){
      if(totallist[i]-> getTypeStr() == "AIG" && totallist[i]->fanout_empty() == false){
        bool repeat = false;
        Key AIG_pair(totallist[i]-> getFanin0(), totallist[i]-> getFanin1(), totallist[i]-> getInv0(), totallist[i]->getInv1() );
        unordered_map<Key, CirGate*, hash_Key>::iterator it;
        it = map.find(AIG_pair);
      // for(size_t j = 0; j<AIG_vector.size();j++){
      //   // cout << AIG_vector.size();
          if(it !=map.end()){
            repeat = true;
            unsigned a = it -> second -> getID();
            unsigned b = totallist[i] -> getID();
          // for(size_t k = 0; k< totallist[i]-> getFanoutSize(); k++){
            it -> second -> push_ALL_fanout_and_Fanin(totallist[i]);
          // }
          
            cout << "Strashing: " << a << " merging " << b << "..." << endl;
            for(size_t k = 0; k< totallist.size(); k++){
              if(totallist[i] == totallist[k]){
                  totallist[k] = NULL;
              }
            }
          }
      // }

       if(repeat == false){
         map.insert({AIG_pair, totallist[i]});
       }
     }
    }
    
  }
  // for(size_t i = 0;i<map.size(); i++){
  //   cout << map[i] ->second -> getID();
  // }
  // cout << map.size();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
