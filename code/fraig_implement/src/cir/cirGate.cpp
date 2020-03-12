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

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::count = 0;
unsigned CirGate::_globalRef = 0;

int 
CirGate::checkfaninInv(unsigned _id) 
{
	int i = -1;
	for (unsigned x = 0; x < _fanin.size(); x++) {
		if (_fanin[x]->getID() == _id) {
			i = (_ifInvIn[x]) ? (2 * _id + 1) : (2 * _id) ;
			break;
		}
	}
	return i;
}
bool 
CirGate::checkfloat() 
{
	for (unsigned i = 0; i < _fanin.size(); i++) {
		if (_fanin[i]->getTypeStr() == "UNDEF")  return true;
	}
	return false;
}
bool 
CirGate::checkunused() 
{
	if (_fanout.empty())  return true;
	else  return false;
}

void 
CirGate::saveAIG(vector<CirGate*>& dfslist) 
{
	if (getTypeStr() != "UNDEF") {
		for (unsigned i = 0; i < _fanin.size(); i++) {
			if (!_fanin[i]->isGlobalRef()) {
				_fanin[i]->setToGlobalRef();
				_fanin[i]->saveAIG(dfslist);
			}
		}
		if (getTypeStr() == "AIG"){
			dfslist.push_back(this);
		}
	}
}
void 
CirGate::netlistprint() const
{
	if (getTypeStr() != "UNDEF") {
		for (unsigned i = 0; i < _fanin.size(); i++) {
			if (!_fanin[i]->isGlobalRef()) {
				_fanin[i]->setToGlobalRef();
				_fanin[i]->netlistprint();
			}
		}
		cout << "[" << count << "] ";
		printGate();
		cout << endl;
		count++;
	}
}

void
CirGate::reportGate() const
{
	cout << "================================================================================" << endl;
	cout << "= " << report() << endl;
	cout << "= FECs:" << endl;
	cout << "= Value:" << " 00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000" << endl;
	cout << "================================================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setGlobalRef();
	printfanin(0, level);
	cout << endl;
}
void 
CirGate::printfanin(int done, int level) const {
	if (level == 0)  cout << getTypeStr() << " " << getID();
	else {
		cout << getTypeStr() << " " << getID();
		if (!_fanin.empty() && !isGlobalRef()) {
			for (unsigned i = 0; i < _fanin.size(); i++) {
				cout << endl;
				for (int j = 0; j <= done; j++) { cout << "  "; }
				if (_ifInvIn[i]) cout << "!";
				_fanin[i]->printfanin(done + 1, level - 1);
			}
			this->setToGlobalRef();
		}
		else if (!_fanin.empty() && isGlobalRef()) {
			cout << " (*)";
		}
	}
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
	setGlobalRef();
	printfanout(0, level);
	cout << endl;
}
void 
CirGate::printfanout(int done, int level) const {
	if (level == 0)  cout << getTypeStr() << " " << getID();
	else {
		cout << getTypeStr() << " " << getID();
		if (!_fanout.empty() && !isGlobalRef()) {
			for (unsigned i = 0; i < _fanout.size(); i++) {
				cout << endl;
				for (int j = 0; j <= done; j++) { cout << "  "; }
				if (_fanout[i]->checkfaninInv(getID()) != -1) {
					if ((_fanout[i]->checkfaninInv(getID()))%2 == 1)  cout << "!";
				}
				_fanout[i]->printfanout(done + 1, level - 1);
			}
			this->setToGlobalRef();
		}
		else if(!_fanout.empty() && isGlobalRef()) {
			cout << " (*)";
		}
	}
}

//--------------------------

bool 
CirGate::fanout_empty() 
{
	if(_fanout.empty() == true) return true;
	else return false;
}

bool 
CirGate::fanin_empty() 
{
	if(_fanin.empty() == true) return true;
	else return false;
}

bool
CirGate::handle_empty() 
{
	size_t t = _fanout.size();
	// cout << t;
	if(t ==0 && (this->getTypeStr()=="UNDEF" || this->getTypeStr()=="AIG") ){
      for(size_t j = 0; j < this -> getFaninSize(); j++ ){
        for(size_t k = 0; k < this->_fanin[j]->_fanout.size() ; k++ ){
			if(this->_fanin[j] -> _fanout[k] == this){
            	this->_fanin[j] -> _fanout.erase(this->_fanin[j] ->_fanout.begin() + k);
          }
		}
      }
	  return true;
    }
	return false;
}
   
bool
CirGate::opt_merging(CirGate* const_0)
{
	
	if(this->_fanin[0] -> getTypeStr() == "CONST" || this->_fanin[1] -> getTypeStr() == "CONST" ){   //condition 1 & 2 -> return true
		if(this->_fanin[0] -> getTypeStr() == "CONST"){
			if(this-> _ifInvIn[0] ==true){ //become a -> fanin 1
				bool a_inv = _ifInvIn[1]; //record fanin 1 inv

				for(size_t k = 0; k < this->_fanin[1]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
					}
				}

				for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
					}
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 1's fanout
					this->_fanin[1] -> push_fanout(this->_fanout[l]);
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[1];

							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
				}
					
			}

			else{ //become 0-> fanin 0
				bool a_inv = _ifInvIn[0]; //record fanin 0 inv

				for(size_t k = 0; k < this->_fanin[1]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
					}
				}

				for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
					}
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					this->_fanin[0] -> push_fanout(this->_fanout[l]);
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[0];
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
				}
			}
		}

		else{
			if(this-> _ifInvIn[1] ==true){ //become a -> fanin 0
				bool a_inv = _ifInvIn[0]; //record fanin 0 inv

				for(size_t k = 0; k < this->_fanin[1]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
					}
				}

				for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
					}
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					this->_fanin[0] -> push_fanout(this->_fanout[l]);
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[0];
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
				}
					
			}

			else{ //become 0-> fanin 1
				bool a_inv = _ifInvIn[1]; //record fanin 1 inv

				for(size_t k = 0; k < this->_fanin[1]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
					}
				}

				for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
					}
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					this->_fanin[1] -> push_fanout(this->_fanout[l]);
				}

				for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[1];
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
				}
			}
		}
		// cout << "ok1";
		return true;
	}

	else if(this->_fanin[0] -> getID() == this->_fanin[1] -> getID() ){ //condition 3 & 4 -> return true
		bool _inv0 = _ifInvIn[0];
		bool _inv1 = _ifInvIn[1];

		if(_inv0 == true && _inv1 == true){ // a with  inv = true
			bool a_inv = true;
			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
						break; //delete only one
					}
			}

			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
						break; //delete only one
					}
			}

			for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					this->_fanin[0] -> push_fanout(this->_fanout[l]);
			}

			for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[0];
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
				}
		}


		else if(_inv0 != true && _inv1 != true){ // a with  inv = false
			bool a_inv = false;
			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
						break; //delete only one
					}
			}

			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
						break; //delete only one
					}
			}

			for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					this->_fanin[0] -> push_fanout(this->_fanout[l]);
			}

			for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] -> _fanin[m]=  this-> _fanin[0];
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
			}
		}

		else{	//0 with inv = false
			bool a_inv = false;
			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 0's fanout of this
					if(this->_fanin[0] -> _fanout[k] == this){
						this->_fanin[0] -> _fanout.erase(this->_fanin[0] ->_fanout.begin() + k);
					}
			}
			for(size_t k = 0; k < this->_fanin[0]->_fanout.size() ; k++ ){ //delete fanin 1's fanout of this twice
					if(this->_fanin[1] -> _fanout[k] == this){
						this->_fanin[1] -> _fanout.erase(this->_fanin[1] ->_fanout.begin() + k);
					}
			}

			for(size_t l = 0; l < this-> _fanout.size(); l++){ //push this's fanout to fanin 0's fanout
					const_0 -> push_fanout(this->_fanout[l]);
			}


			for(size_t l = 0; l < this-> _fanout.size(); l++){ //xor comparison
					for(size_t m = 0; m< this-> _fanout[l] -> _fanin.size(); m++){
						if(this-> _fanout[l] -> _fanin[m]== this){
							bool b_inv = this-> _fanout[l] -> _ifInvIn[m];
							this-> _fanout[l] ->  _fanin[m] = const_0;
							if((a_inv == true && b_inv == true) || (a_inv != true && b_inv != true)){
								this-> _fanout[l] -> _ifInvIn[m] = false;
							}
							else{
								this-> _fanout[l] -> _ifInvIn[m] = true;
							}
						}
					}
			}
		}
		// cout << "ok2";
		return true;
	}

	else{
		// cout << "fuck";
		return false;
	}
}

void 
mergeFanin(CirGate* c){

}