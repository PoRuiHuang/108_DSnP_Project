/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
vector<CirGate*> _dfsList;

bool
CirMgr::readCircuit(const string& fileName)
{
   fstream ifs;
   vector<string> content;
   ifs.open(fileName,ios::in);
   if (!ifs) {
      cerr <<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      ifs.close();
      return false;
   }
   else{
      do{
         ifs.getline(buf,sizeof(buf));
         content.push_back(buf);
      }
      while(!ifs.eof());
      ifs.close();
      }
   CirGate *const0 = new Const0Gate(0,0);
   _allList.push_back(const0);
   string header = content[0];
   string aag = header.substr(0,3);
   
   //M
   size_t M_d = header.find_first_of(' ',4);
   string Max = header.substr(4,M_d-4);
   if (!myStr2Int(Max, M)) { 
      return parseError(ILLEGAL_NUM); 
      }
   // I
   size_t I_d = header.find_first_of(' ',M_d+1);
   string Input = header.substr(M_d+1,I_d-M_d-1);
   if (!myStr2Int(Input, I)) { 
      return parseError(ILLEGAL_NUM); 
      }
   //L
   size_t L_d = header.find_first_of(' ',I_d+1);
   string Latch = header.substr(I_d+1,L_d-I_d-1);
   if (!myStr2Int(Latch, L)) { 
      return parseError(ILLEGAL_NUM); 
      }
   //O
   size_t O_d = header.find_first_of(' ',L_d+1);
   string Output = header.substr(L_d+1,O_d-L_d-1);
   if (!myStr2Int(Output, O)) { 
      return parseError(ILLEGAL_NUM); 
      }
   //A
   string Aig = header.substr(O_d+1,content.size()-O_d-1);
   if (!myStr2Int(Aig, A)) { 
      return parseError(ILLEGAL_NUM); 
      }
   lineNo++;
   _allList.resize(1+M+O, NULL);


   for(int i =0; i< I; i++){
      int gateID = 0;
      if(myStr2Int(content[lineNo], gateID)){
         CirGate *pi = new CirPiGate(gateID/2, lineNo+1);
         _piList.push_back(pi);
         _allList[gateID/2] = pi;
         lineNo++;
      }  
   }

   for(int i =1; i<= O; i++){
      int gateID = 0;
      if(myStr2Int(content[lineNo], gateID)){
         CirGate *po = new CirPoGate(M+i, lineNo+1);
         _poList.push_back(po);
         _allList[M+i] = po;
         lineNo++;
      }
   }


   for(int i=0; i<A; i++){
      string AIG = content[lineNo];
      vector<string> element;
      size_t space = AIG.find_first_of(' ',0);
      while(space!= string::npos ){
         string s1 = AIG.substr(0, space);
         element.push_back(s1);
         AIG = AIG.substr(space+1);
         space = AIG.find_first_of(' ',0);
      }
      element.push_back(AIG);

      if(element.size()==3){
         int AIGNum;
         int in1;
         int in2;
         if(myStr2Int(element[0], AIGNum)){
            if(myStr2Int(element[1], in1)){
               if(myStr2Int(element[2], in2)){
                  CirGate *aig = new AigGate(AIGNum/2, lineNo+1);
                  _aigList.push_back(aig);
                  _allList[AIGNum/2] = aig;
                  lineNo++;
               }
            }
         }
      }
   }

   for(size_t i = lineNo; i<content.size();i++){
      string sym_com = content[lineNo];
      int num;
      string group =sym_com.substr(0,1);
      if(group == "i"){
         size_t space = sym_com.find_first_of(" ",0);
         string numstr = sym_com.substr(1,space-1);
         myStr2Int(numstr, num);
         string gateName = sym_com.substr(space+1);
         _piList[num] -> setName(gateName);
      }
      else if(group =="o"){
         size_t space = sym_com.find_first_of(" ",0);
         string numstr = sym_com.substr(1,space-1);
         myStr2Int(numstr, num);
         string gateName = sym_com.substr(space+1);
         _poList[num] -> setName(gateName);
      }
      else if(group =="c"){
         break;
      }
      lineNo++;
   }


   for(int i=1; i<=O; i++ ){
      //PO
      int poID;
      (myStr2Int(content[I+i], poID));
      bool invert = poID%2;
      if(_allList[poID/2] != NULL){
         _poList[i-1]->faninList_push(_allList[poID/2]);
         _poList[i-1]->invertList_push(invert);
         _allList[poID/2]->fanoutList_push(_poList[i-I]);
      }
      else{
         CirGate *aig = new UnDefGate(poID/2, 0);
         _allList[poID/2] = aig;
         _poList[i-1]->faninList_push(aig);
         _poList[i-1]->invertList_push(invert);
         _allList[poID/2]->fanoutList_push(_poList[i-I]);
      }
   }

   for(int i = I+O+1; i<=I+O+A;i++){
      string AIG = content[i];
      vector<string> element;
   size_t A = AIG.find_first_of(' ',0);
   element.push_back(AIG.substr(0,A));
   size_t i1 = AIG.find_first_of(' ',A+1);
   element.push_back(AIG.substr(A+1,i1-A-1));
   element.push_back(AIG.substr(i1+1,AIG.size()-i1-1));
      int AIGNum, in1, in2;
      bool invert1, invert2;
      myStr2Int(element[0], AIGNum);
      myStr2Int(element[1], in1);
      myStr2Int(element[2], in2);
      // cout << AIGNum << endl;
      //       cout <<  in1 << endl;
      //             cout << in2 << endl;
      invert1 = in1%2;
      invert2 = in2%2;
      if(_allList[in1/2] != NULL){
         _allList[AIGNum/2]->faninList_push(_allList[in1/2]);
         _allList[AIGNum/2]->invertList_push(invert1);
         _allList[in1/2]->fanoutList_push(_allList[AIGNum/2]);
      }
      else{
         CirGate *gate = new UnDefGate(in1/2, 0);
         _allList[in1/2] = gate;
         _allList[AIGNum/2]->faninList_push(_allList[in1/2]);
         _allList[AIGNum/2]->invertList_push(invert1);
         _allList[in1/2]->fanoutList_push(_allList[AIGNum/2]);
      }
      if(_allList[in2/2] != NULL){
         _allList[AIGNum/2]->faninList_push(_allList[in2/2]);
         _allList[AIGNum/2]->invertList_push(invert2);
         _allList[in2/2]->fanoutList_push(_allList[AIGNum/2]);
      }
      else{
         CirGate *gate = new UnDefGate(in2/2, 0);
         _allList[in2/2] = gate;
         _allList[AIGNum/2]->faninList_push(_allList[in2/2]);
         _allList[AIGNum/2]->invertList_push(invert2);
         _allList[in2/2]->fanoutList_push(_allList[AIGNum/2]);
      }
   }
   
   CirGate::setGlobalRef();
   for(int i = 1; i<=O;i++){
      _poList[i-1]->genDFSList(_dfsList);
   }
  
   return true;
}



/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI"<<setw(12)<<right<<I<<endl;
   cout<<"  PO"<<setw(12)<<right<<O<<endl;
   cout<<"  AIG"<<setw(11)<<right<<A<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<setw(9)<<right<<I+O+A<<endl;
}

void
CirMgr::printNetlist() const
{
   CirGate::setGlobalRef();
   int count = 0;
   cout << endl;
    for(int i = 1; i<=O;i++){
      _poList[i-1]->genNetList(count);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   
   for( int i = 0; i<I; i++){
      cout << " " << _piList[i]->printID();
   }
   cout << endl;
}


void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";

   for( int i = 0; i<O; i++){
      cout << " " << _poList[i]->printID();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   vector<CirGate*> floating;
   vector<CirGate*> unused;
   for(unsigned int i =0; i<_allList.size();i++){
      if(_allList[i]!=0){
         if(_allList[i]->getTypeStr() =="PI"){
            if (_allList[i]->isunused()){
               unused.push_back(_allList[i]);
               }
            }
         else if(_allList[i]->getTypeStr() =="PO"){
             if (_allList[i]->isfloat()){
              floating.push_back(_allList[i]);
            }
         }      
         else if(_allList[i]->getTypeStr() =="AIG"){
           if (_allList[i]->isunused()){
               unused.push_back(_allList[i]);
            }
            if (_allList[i]->isfloat()){
              floating.push_back(_allList[i]);
            }
          }     
      }
    
   }

   if(floating.empty()==false){
      cout << "Gates with floating fanin(s):" ;
      for(size_t  j= 0; j<floating.size();j++){
         cout << " " << floating[j]->printID();
      }
      cout << endl;
   }
    if(unused.empty()==false){
      cout << "Gates defined but not used  :";
      for(size_t  k= 0; k<unused.size();k++){
         cout << " " <<unused[k]->printID();
      }
      cout << endl;
   }
   
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << M << " " << I << " "<< L << " "<< O << " " << A << endl;
	for (int i = 0; i < I; i++)  
		outfile << _piList[i]->writeAag() << endl;


	for (int i = 0; i <O; i++)
		outfile << _poList[i]->writeAag()  << endl;


	for (int i = 0; i < A; i++)
		outfile << _aigList[i]->writeAag() << endl;

      
	for (int i = 0; i < I; i++) {
		if (_piList[i]->getName()!="") {
			outfile << "i" << i << " " << _piList[i]->getName() << endl;
		}
	}


	for (int i = 0; i < O; i++) {
		if (_poList[i]->getName()!="") {
			outfile << "o" << i << " " << _poList[i]->getName() << endl;
		}
	}
	outfile << "c" << endl;
   outfile << "AAG output by Chung-Yang (Ric) Huang" << endl;
}
