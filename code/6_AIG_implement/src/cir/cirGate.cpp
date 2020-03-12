/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::globalRef = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

  void
 CirGate::faninList_push(CirGate* gate)
  {
     this-> faninList.push_back(gate);
  }

  void 
 CirGate::fanoutList_push(CirGate* gate)
  {
     this-> fanoutList.push_back(gate);
  }

void 
CirGate::invertList_push(bool invert) 
{
   this-> invertList.push_back(invert);
}

bool
CirGate::isfloat(){
   for(unsigned int i = 0; i< faninList.size();i++){
      if (faninList[i]->getTypeStr() =="UNDEF")
         return true;
   }
   return false;
}

bool
CirGate::isunused(){
   if(fanoutList.empty()){
      return true;
   }
   return false;
}
void
CirGate::reportGate() const
{
   string gate;
   cout << "==================================================" << endl;
   cout <<  gateInfo(gate) << setw(50-gate.length()) << right << "=" << endl;
	cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   if(level>=0){
       setGlobalRef();
       faninTraversal(0,level);
       cout << endl;
   }
}

void 
CirGate::faninTraversal(int fin, int level)const{
	if (level >0)  {
		cout << this-> getTypeStr() << " " << this->printID();
		if (faninList.empty()==false ) {
         if(isGlobalRef()==true) {
			   cout << " (*)";
		   }
         else{
            for (size_t i = 0; i < faninList.size(); i++) {
				   cout << endl;
				   for (int j = 0; j <= fin; j++) { 
                     cout << "  "; 
               }
				   if (invertList[i]==true){
                  cout << "!";
               } 
				   faninList[i]->faninTraversal(fin + 1, level - 1);
			   }
			   this->setToGlobalRef();
         }
			
		}	
	}
   else{
      cout << this->getTypeStr() << " " << this->printID();
   }
}

void
CirGate::reportFanout(int level) const
{
   if(level>=0){
       setGlobalRef();
       fanoutTraversal(0,level);
       cout << endl;
   }
}
void 
CirGate::fanoutTraversal(int fin, int level) const {
	if(level>0){
		 cout << this->getTypeStr() << " " << this->printID();
		if (fanoutList.empty() ==false) {
         if(isGlobalRef()==true){
            cout << " (*)";
         }
         else{
            for (size_t i = 0; i < fanoutList.size(); i++) {
				   cout << endl;
				   for (int j = 0; j <= fin; j++){
                  cout << "  "; 
                  }
               int odd = fanoutList[i]->finfout(printID());
               if (odd  != -1 &&odd%2 == 1){
					      cout << "!";
			   	}
				   fanoutList[i]->fanoutTraversal(fin + 1, level - 1);
			   }  
			   this->setToGlobalRef();
         }
		}

	}
   else{
      cout << this->getTypeStr() << " " << this->printID();
   }
}

int 
CirGate::finfout(int id)
{
	int odd = -1;
	for (size_t i = 0; i < faninList.size(); i++) {
		if (faninList[i]->printID() == id) {
         if(invertList[i]==1){
            odd = 2*id+1;
            break;
         }
         else{
            odd = 2*id;
            break;
         }
		}
	}
	return odd;
}
	


void
CirGate::genDFSList(vector<CirGate*>& DFS){
  for(size_t i = 0;i<this->faninList.size();i++){
      if(!faninList[i]->isGlobalRef()){
         faninList[i]->setToGlobalRef();
         faninList[i]->genDFSList(DFS);
   } 
 }
  DFS.push_back(this); 
}

void
CirGate::genNetList(int &count){
   if(this->getTypeStr()!="UNDEF"){
      for(size_t i = 0;i<this->faninList.size();i++){
         if(faninList[i]->isGlobalRef()==false){
            faninList[i]->setToGlobalRef();
            faninList[i]->genNetList(count);
          } 
      }
      cout <<"[" << count <<"]" ;
      count++;
      printGate();
       cout << endl;
   }
}



