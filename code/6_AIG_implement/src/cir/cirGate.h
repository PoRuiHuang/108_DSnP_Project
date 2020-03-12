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
  static unsigned globalRef ;
public:
   CirGate(int ID = 0, int lineNum = 0):id(ID),line(lineNum), ref(0) {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const { return ""; }
   virtual void setName(string name){};
   virtual string getName(){return"";};
   int getLineNo() const { return line; }
   void faninList_push(CirGate* gate);
   void fanoutList_push(CirGate* gate);
   void invertList_push(bool invert);
   bool isfloat();
   bool isunused();
   

   // Printing functions
   virtual void printGate() const = 0;
   int printID()const{return this->id;}
   virtual void reportGate() const;
   virtual string gateInfo(string &info)const=0;
   void reportFanin(int level) const;
   void faninTraversal(int fin, int level)const;
   void reportFanout(int level) const;
   void fanoutTraversal(int fin, int level)const;
   int finfout(int id);
   //DFS
   bool isGlobalRef()const{return (ref==globalRef); }
   void setToGlobalRef()const{ref = globalRef;}
   static void setGlobalRef(){globalRef++;}
   void genDFSList(vector<CirGate*>&); 
   void genNetList(int &count );
   //
   virtual string writeAag() const=0;


private:

protected:
  vector<CirGate*> faninList;
  vector<CirGate*> fanoutList;
  vector<bool> invertList;
  int id;
  int line;
  mutable unsigned ref;
  
};




class CirPiGate : public CirGate{
  public:
    CirPiGate(int gateID = 0, int lineNum = 0): CirGate(gateID, lineNum){}
    ~CirPiGate() {}

  // Printing functions
    void printGate()const {
           cout<<" PI  "  <<this->id;
     if(name !=""){
       cout << " (" << name << ")";
      }
    };

    string gateInfo(string &info)const{
      info = "= PI(";
      info = info + to_string(printID())+ ")";
      if(name!=""){
        info = info+"\"" + name + "\"";
      }
      info = info +  ", line " + to_string(getLineNo());
      return info;
    };

    // Basic access methods
    string getTypeStr() const { return "PI"; }
    virtual void setName(string name){
      this->name = name;
    }
    virtual string getName(){
      return this->name;
    };
  //
    virtual string writeAag() const{
      string num = to_string(2*id);
      return (string)num;
    }

  private:
    string name;
};

class CirPoGate : public CirGate{
  public:
    CirPoGate(int gateID = 0, int lineNum = 0): CirGate(gateID, lineNum){}
    ~CirPoGate() {}

// Printing functions
   void printGate()const {
    cout<<" PO  "  << this->id << " ";
    if(faninList[0]->getTypeStr() == "UNDEF"){
     cout << "*";
    }
		if(invertList[0]){
      cout << "!";
    }  
		cout << faninList[0]->printID();
		if(name!=""){
      cout << " (" << name << ")";
    }  
   };

      // Basic access methods
    string getTypeStr() const { return "PO"; }
    void setName(string name){
      this->name = name;
    }
    virtual string getName(){
      return this->name;
    };
    string gateInfo(string &info)const{
      info = "= PO(";
      info = info + to_string(printID()) + ")";
      if(name!=""){
        info = info+"\"" + name + "\"";
      }
      info = info +  ", line " + to_string(getLineNo());
      return info;
    };
    virtual string writeAag() const{
      string s ;
      if(invertList[0]==1){
        s = to_string(2*faninList[0]->printID()+1);
      } 
      else{
        s = to_string(2*faninList[0]->printID());
      }
      return s;
    }

    private:
      string name;
};

class AigGate : public CirGate{
  public:
    AigGate(int gateID = 0, int lineNum = 0): CirGate(gateID, lineNum){}
    ~AigGate() {}

  // Printing functions
   void printGate()const {
     cout<<" AIG "  << this->id;
   for(size_t i = 0; i < faninList.size(); i++){
			cout << " ";
			if(faninList[i]->getTypeStr() == "UNDEF"){
        cout << "*";
      }  
			if(invertList[i]==1)  {
        cout << "!";
      }
			cout << faninList[i]->printID();
		}
   };

   string gateInfo(string &info)const{
       info = "= AIG(";
      info = info + to_string(printID()) + ")";
      info = info +  ", line " + to_string(getLineNo());
      return info;
    };

  // Basic access methods
   string getTypeStr() const { return "AIG"; }

  //
    virtual string writeAag() const{
      string s ;
      s = s + to_string(2*id);
      if(invertList[0]==1){
        s =s+" "+  to_string(2*faninList[0]->printID()+1);
      } 
      else{
        s =s+" "+ to_string(2*faninList[0]->printID());
      }
       if(invertList[1]==1){
        s =s+" "+  to_string(2*faninList[1]->printID()+1);
      } 
      else{
        s =s+" "+ to_string(2*faninList[1]->printID());
      }
      return s;
  }
};

class UnDefGate : public CirGate{
  public:
    UnDefGate(int gateID = 0, int lineNum = 0): CirGate(gateID, lineNum){}
    ~UnDefGate() {}

  // Printing functions
   void printGate()const {
     cout <<" UNDEF " << this->id;
     };
   string gateInfo(string &info)const{
       info = "= UNDEF(";
      info = info + to_string(printID()) + ")";
      info = info +  ", line " + to_string(getLineNo());
      return info;
    };


      // Basic access methods
    string getTypeStr() const { return "UNDEF"; }
    //
    virtual string writeAag() const{return"";}
};


class Const0Gate : public CirGate{
  public:
    Const0Gate(int gateID = 0, int lineNum = 0): CirGate(gateID, lineNum){}
    ~Const0Gate() {}

  // Printing functions
   void printGate()const {
     cout <<" CONST0 " ;
     };

    string gateInfo(string &info)const{
       info = "= CONST0(";
      info = info + to_string(printID()) + ")";
      info = info +  ", line " + to_string(getLineNo());
      return info;
    };

      // Basic access methods
   string getTypeStr() const { return "CONST0"; }
   //
   virtual string writeAag() const{return"";}
};
#endif // CIR_GATE_H
