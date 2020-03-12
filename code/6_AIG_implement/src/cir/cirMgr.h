/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr():M(0),I(0),O(0),L(0),A(0){
   }
   ~CirMgr() {
     _piList.clear();
     _poList.clear();
     _aigList.clear();
     _allList.clear();
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
     if (gid < _allList.size()){
       return _allList[gid]; 
     } 
      return 0;
      }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   //DFS


private:
    int M;
    int I;
    int O;
    int L;
    int A;
    bool readHeader();
    bool readInput();
    bool readOuput();
    vector<CirGate*> _piList;
    vector<CirGate*> _poList;
    vector<CirGate*> _aigList;
    vector<CirGate*> _allList;
};


#endif // CIR_MGR_H
