/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
	static unsigned count;
	static unsigned _globalRef;
	mutable unsigned _ref;

public:
   CirGate(unsigned i = 0, unsigned l = 0) :_ref(0), id(i), line(l) 
		{ _fanin.clear(); _fanout.clear(); _ifInvIn.clear(); }
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   unsigned getLineNo() const { return line; }
	unsigned getID() const { return id; }
	virtual bool isAig() const { return false; }
	size_t getFaninSize() const{return _fanin.size();}
	size_t getFanoutSize() const{return _fanout.size();}
	
	// Ref for DFS
	static void setGlobalRef() { _globalRef++; }
	bool isGlobalRef() const { return (_ref == _globalRef); }
   void setToGlobalRef() const { _ref = _globalRef; }
	static void setcount() { count = 0; }
	
	// Set&check fanin,fanout
	void push_fanin(CirGate* c, bool i) { _fanin.push_back(c); _ifInvIn.push_back(i); }
	void push_fanout(CirGate* c) { _fanout.push_back(c); }
	void push_ALL_fanout_and_Fanin(CirGate* c){
		for(size_t i = 0; i< this -> _fanin.size(); i++){
			for(size_t j = 0; j < this-> _fanin[i] -> _fanout.size(); j++){
				if(this->_fanin[i]-> _fanout[j] == c){
					// c->_fanout[i]-> _fanin[j] = NULL;
					this->_fanin[i]-> _fanout.erase(this->_fanin[i]->_fanout.begin()+j);
				}
			}
		}
		for(size_t i = 0; i< c -> _fanout.size(); i++){
			push_fanout( c->_fanout[i]);
		}
		for(size_t i = 0; i< c -> _fanout.size(); i++){
			for(size_t j = 0; j < c-> _fanout[i] -> _fanin.size(); j++){
				if(c->_fanout[i]-> _fanin[j] == c){
					// c->_fanout[i]-> _fanin[j] = NULL;
					c->_fanout[i]-> _fanin[j] = this;
				}
			}
		}
	}
	void mergeFanin(CirGate* c);
	virtual bool setname(string& n) { return false; }
	virtual string getname() const { return ""; }
	int checkfaninInv(unsigned _id);
	bool checkfloat();
	bool checkunused();
	bool fanout_empty();
	bool fanin_empty();
	bool handle_empty();
	bool opt_merging(CirGate* c) ;
	CirGate* getFanin0(){ return this->_fanin[0];}
	CirGate* getFanin1(){ return this->_fanin[1];}
	bool getInv0(){ return this->_ifInvIn[0];}
	bool getInv1(){ return this->_ifInvIn[1];}

	// Printing functions
   void netlistprint() const;
	virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
	void printfanin(int, int) const;
   void reportFanout(int level) const;
	void printfanout(int, int) const;
   virtual string report() const = 0;
	void saveAIG(vector<CirGate*>& dfslist);
	virtual string write() const { return ""; } 

protected:
	unsigned id;
	vector <CirGate*> _fanin;
	vector <bool> _ifInvIn;
	vector <CirGate*> _fanout;
private:
	unsigned line;
};

class AIG : public CirGate
{
public:
	AIG(unsigned i = 0, unsigned l = 0) :CirGate(i, l) {}
	~AIG() {}

	virtual bool isAig() const { return false; }
	virtual string getTypeStr() const { return "AIG"; }
	virtual void printGate() const {
		cout << "AIG " << id;
		for(unsigned i = 0; i < _fanin.size(); i++) {
			cout << " ";
			if(_fanin[i]->getTypeStr() == "UNDEF")  cout << "*";
			if(_ifInvIn[i])  cout << "!";
			cout << _fanin[i]->getID();
		}
	}
	virtual string report() const {
		string s = "AIG";
		s.append("(");
		s.append(to_string(id));
		s.append("), line ");
		s.append(to_string(getLineNo()));
		return s;
	}
	virtual string write() const {
		string s = "";
		s.append(to_string(2*id));
		for (int i = 0; i < 2; i++) {
			s.append(" ");
			s.append((_ifInvIn[i])?(to_string(2*_fanin[i]->getID()+1)):(to_string(2*_fanin[i]->getID())));
		}
		return s;
	}

	
};

class PI : public CirGate
{
public:
	PI(unsigned i = 0, unsigned l = 0) :CirGate(i, l), name("") {}
	~PI() {}
	virtual bool setname(string& n) { 
		if(name.size()) { return false; }
		else { name = n; return true; }
	} 
	virtual bool isAig() const { return false; }
	virtual string getTypeStr() const { return "PI"; }
	virtual void printGate() const {
		cout << "PI  " << id;
		if(!name.empty())  cout << " (" << name << ")";
	}
	virtual string getname() const { return name; }
	virtual string report() const {
		string s = "PI";
		s.append("(");
		s.append(to_string(id));
		s.append(")");
		if (!name.empty()) {
			s.append("\"");
			s.append(name);
			s.append("\"");
		}
		s.append(", line ");
		s.append(to_string(getLineNo()));
		return s;
	}
private:
	string name;
};

class PO : public CirGate
{
public:
	PO(unsigned i = 0, unsigned l = 0) :CirGate(i, l), name("") {}
	~PO() {}
	virtual bool setname(string& n) { 
		if(name.size()) { return false; }
		else { name = n; return true; }
	} 
	virtual bool isAig() const { return false; }
	virtual string getTypeStr() const { return "PO"; }
	virtual string getname() const { return name; }
	virtual void printGate() const {
		cout << "PO  " << id << " ";
		if(_fanin[0]->getTypeStr() == "UNDEF")  cout << "*";
		if(_ifInvIn[0])  cout << "!";
		cout << _fanin[0]->getID();
		if(!name.empty())  cout << " (" << name << ")";
	}
	virtual string report() const {
		string s = "PO";
		s.append("(");
		s.append(to_string(id));
		s.append(")");
		if (!name.empty()) {
			s.append("\"");
			s.append(name);
			s.append("\"");
		}
		s.append(", line ");
		s.append(to_string(getLineNo()));
		return s;
	}
	virtual string write() const {
		string s = "";
		s.append((_ifInvIn[0])?(to_string(2*_fanin[0]->getID()+1)):(to_string(2*_fanin[0]->getID())));
		return s;
	}
private:
	string name;
};

class UNDEF : public CirGate
{
public:
	UNDEF(unsigned i = 0, unsigned l = 0) :CirGate(i, l) {}
	~UNDEF() {}
	virtual bool isAig() const { return false; }
	virtual string getTypeStr() const { return "UNDEF"; }
	virtual void printGate() const { cout << "UNDEF"; }
	virtual string report() const {
		string s = "UNDEF";
		s.append("(");
		s.append(to_string(id));
		s.append("), line 0");
		return s;
	}
};

class CONST : public CirGate
{
public:
	CONST(unsigned i = 0, unsigned l = 0) :CirGate(i, l) {}
	~CONST() {}
	virtual bool isAig() const { return false; }
	virtual string getTypeStr() const { return "CONST"; }
	virtual void printGate() const { cout << "CONST0"; }
	virtual string report() const {
		string s = "CONST";
		s.append("(");
		s.append(to_string(id));
		s.append("), line 0");
		return s;
	}
};

#endif // CIR_GATE_H
