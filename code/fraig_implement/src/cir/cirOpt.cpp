/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;


// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{

  for(size_t i = totallist.size()-1; i>0; i--){
    if(totallist[i]!= NULL){
      if(totallist[i] -> handle_empty() == true){
        cout <<"Sweeping: "<<  totallist[i]-> getTypeStr() << "(" << totallist[i] -> getID() << ")"<< " removed..." << endl ;
        totallist[i] = NULL;
     }
    }
    
  }

}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  CirGate::setGlobalRef();
	vector <CirGate*> dfslist;
	for (size_t i = 0; i < POlist.size(); i++) {
		POlist[i]->saveAIG(dfslist);
  }
  
  CirGate* const_0 = totallist[0];

  for(size_t i =0; i < dfslist.size(); i++){
    if(dfslist[i] -> opt_merging(const_0)== true){
        cout <<"Simplifying... "<< endl ;
        for(size_t j = 0; j< totallist.size(); j++){
          if(dfslist[i] == totallist[j]){
            totallist[j] = NULL;
          }
        }
    }
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
