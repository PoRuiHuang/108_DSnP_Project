/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
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

CirMgr::CirMgr() :AIG_num(0), max_id(0)
{ 
	PIlist.clear(); POlist.clear(); totallist.clear(); totallist.push_back(new CONST(0,0)); 
}
CirMgr::~CirMgr() 
{ 
	for(size_t i = 0; i < totallist.size(); i++)  { if (totallist[i] != 0) delete totallist[i]; }
	PIlist.clear(); POlist.clear(); totallist.clear();
}

CirGate* 
CirMgr::getGate(unsigned gid) const 
{
	if (gid < totallist.size()) return totallist[gid]; 
	else return 0;
}

bool
CirMgr::readCircuit(const string& fileName)
{
	fstream fin;
	fin.open(fileName, ios::in);
	if(!fin) { 
		cerr << "Cannot open design \"" << fileName << "\"!!" << endl; 
		return false; 
	}

	//create a readbyline vector to store the whole file
	vector<string> readbyline;
	readbyline.clear();
	readbyline.push_back("");
	char ch;
	while (fin.get(ch)) {
		if (ch == '\n')  readbyline.push_back("");
		else readbyline.back().push_back(ch);
	}
	readbyline.pop_back();
	fin.close();

	//header parsing and error handling
	lineNo = 0;
	colNo = 0;
	vector<string> header;
	header.clear();
	string s = readbyline[0];
	if (!s.size()) { errMsg = "aag"; return parseError(MISSING_IDENTIFIER); }
	size_t p = s.find_first_not_of(" \t\v\f\r");
	if (p != 0) {
		if (s[0] == ' ')  return parseError(EXTRA_SPACE);
		else { errInt = (int)s[0]; return parseError(ILLEGAL_WSPACE); }
	}
	size_t n = s.find_first_of(" \t\v\f\r", p);
	while (n != string::npos && n != s.size()-1) {
		colNo += n;
		if (s[n] != ' ')  return parseError(MISSING_SPACE);
		header.push_back(s.substr(0, n));
		s = s.substr(n+1);
		colNo ++;
		p = s.find_first_not_of(" \t\v\f\r");
		if (p != 0) {
			if (s[0] == ' ')  return parseError(EXTRA_SPACE);
			else { errInt = (int)s[0]; return parseError(ILLEGAL_WSPACE); }
		}
		n = s.find_first_of(" \t\v\f\r", p);
	}
	header.push_back(s);
	
	colNo = 0;
	if (header[0] != "aag") {
		errMsg = header[0];
		if (header[0].size() >= 3 && header[0].substr(0,3) == "aag") { 
			if (header[0][3] == ' ') { colNo += 4; errMsg = "number of variables"; return parseError(MISSING_NUM); }   
			else if (isdigit(header[0][3])) { colNo += 3; return parseError(MISSING_SPACE); } 
			else  return parseError(ILLEGAL_IDENTIFIER);
		}
		else  return parseError(ILLEGAL_IDENTIFIER);
	}
	else if (header.size() < 6) { colNo = readbyline[0].size(); errMsg = "number of variables"; return parseError(MISSING_NUM); }
	else if (header.size() == 6 && isspace(header[5].back())) { colNo = readbyline[0].size()-1; return parseError(MISSING_NEWLINE); }
	else if (header.size() > 6) {
		colNo = (header[0].size()+header[1].size()+header[2].size()+header[3].size()+header[4].size()+header[5].size()+5);
		return parseError(MISSING_NEWLINE);
	}

	//set header variables (max_id, PIsize, POsize, AIG_num)
	int PIsize = 0, POsize = 0;
	if (!myStr2Int(header[1], max_id)) 
		{ errMsg = "number of variables("; errMsg.append(header[1]); errMsg.append(")"); return parseError(ILLEGAL_NUM); }
	if (!myStr2Int(header[2], PIsize)) 
		{ errMsg = "number of PIs("; errMsg.append(header[2]); errMsg.append(")"); return parseError(ILLEGAL_NUM); }
	if (header[3] != "0") { errMsg = "latches"; return parseError(ILLEGAL_NUM); }
	if (!myStr2Int(header[4], POsize)) 
		{ errMsg = "number of POs("; errMsg.append(header[4]); errMsg.append(")"); return parseError(ILLEGAL_NUM); }
	if (!myStr2Int(header[5], AIG_num)) 
		{ errMsg = "number of AIGs("; errMsg.append(header[5]); errMsg.append(")"); return parseError(ILLEGAL_NUM); }
	if (max_id < PIsize + AIG_num) { errMsg = "Number of variables"; errInt = max_id; return parseError(NUM_TOO_SMALL); }

	//resize totallist
	totallist.resize(max_id + POsize + 1, 0);

	//generate PIs,POs (not connected)
	CirGate* _p = 0;
	int literalID = 0;
	for (int i = 1; i <= PIsize; i++) {
		colNo = 0;
		lineNo++;
		if (lineNo >= readbyline.size()) { errMsg = "PI"; return parseError(MISSING_DEF); }
		if (readbyline[lineNo][0] == ' ')  return parseError(EXTRA_SPACE);
		else if (isspace(readbyline[lineNo][0])) { errInt = (int)readbyline[lineNo][0]; return parseError(ILLEGAL_WSPACE); }
		n = readbyline[lineNo].find_first_of(" \t\v\f\r", p);
		if (n != string::npos) { colNo = (unsigned)n; return parseError(MISSING_NEWLINE); }
		if (!myStr2Int(readbyline[lineNo], literalID)) { errMsg = "PI literal ID"; return parseError(MISSING_NUM); }
		if (literalID < 2) { errInt = literalID; return parseError(REDEF_CONST); }
		if (literalID%2) { errMsg = "PI"; errInt = literalID; return parseError(CANNOT_INVERTED); }
		if (literalID/2 > max_id) { errInt = literalID; return parseError(MAX_LIT_ID); }
		if (totallist[(unsigned)(literalID / 2)] == 0) {
			_p = new PI((unsigned)(literalID / 2), lineNo + 1);
			PIlist.push_back(_p);
			totallist[(unsigned)(literalID / 2)] = _p;
		}
		else { errGate = totallist[(unsigned)(literalID / 2)]; errInt = literalID; return parseError(REDEF_GATE); }
	}
	for (int i = 1; i <= POsize; i++) {
		colNo = 0;
		lineNo++;
		if (lineNo >= readbyline.size()) { errMsg = "PO"; return parseError(MISSING_DEF); }
		if (readbyline[lineNo][0] == ' ')  return parseError(EXTRA_SPACE);
		else if (isspace(readbyline[lineNo][0])) { errInt = (int)readbyline[lineNo][0]; return parseError(ILLEGAL_WSPACE); }
		n = readbyline[lineNo].find_first_of(" \t\v\f\r", p);
		if (n != string::npos) { colNo = (unsigned)n; return parseError(MISSING_NEWLINE); }
		if (!myStr2Int(readbyline[lineNo], literalID)) { errMsg = "PO literal ID"; return parseError(MISSING_NUM); }
		if (literalID/2 > max_id) { errInt = literalID; return parseError(MAX_LIT_ID); }
		_p = new PO((unsigned)(max_id + i), lineNo + 1);
		POlist.push_back(_p);
		totallist[(unsigned)(max_id + i)] = _p;
	}
	
	//generate AIGs (not connected) and save ids
	vector<unsigned> idsforAIG;
	vector<int> literalids;
	vector<string> component;
	idsforAIG.clear();
	literalids.clear();
	for (int i = 1; i <= AIG_num; i++) {
		colNo = 0;
		lineNo++;
		if (lineNo >= readbyline.size()) { errMsg = "AIG"; return parseError(MISSING_DEF); }
		if (readbyline[lineNo][0] == ' ')  return parseError(EXTRA_SPACE);
		else if (isspace(readbyline[lineNo][0])) { errInt = (int)readbyline[lineNo][0]; return parseError(ILLEGAL_WSPACE); }

		component.clear();
		string s1 = readbyline[lineNo];
		size_t p1 = s1.find_first_not_of(" \t\v\f\r");
		size_t n1 = s1.find_first_of(" \t\v\f\r", p1);
		while (n1 != string::npos && n1 != s1.size()-1) {
      	colNo += n1;
			if (s1[n1] != ' ')  return parseError(MISSING_SPACE);
			component.push_back(s1.substr(0, n1));
      	s1 = s1.substr(n1+1);
			colNo++;
			p1 = s1.find_first_not_of(" \t\v\f\r");
			if (p1 != 0) {
				if (s1[0] == ' ')  return parseError(EXTRA_SPACE);
				else { errInt = (int)s1[0]; return parseError(ILLEGAL_WSPACE); }
			}
			n1 = s1.find_first_of(" \t\v\f\r", p1);
   		}
		component.push_back(s1);
		colNo = 0;
		int l1 = 0, l2 = 0;
		if (component.size() == 3) {
			if (!myStr2Int(component[0], literalID)) { errMsg = "AIG literal ID"; return parseError(MISSING_NUM); }
			if (literalID < 2) { errInt = literalID; return parseError(REDEF_CONST); }
			if (literalID%2) { errMsg = "AIG"; errInt = literalID; return parseError(CANNOT_INVERTED); }
			if (literalID/2 > max_id) { errInt = literalID; return parseError(MAX_LIT_ID); }
			colNo += component[0].size()+1;
			if (!myStr2Int(component[1], l1)) { errMsg = "AIG literal ID"; return parseError(MISSING_NUM); }
			if (l1/2 > max_id) { errInt = l1; return parseError(MAX_LIT_ID); }
			colNo += component[1].size()+1;
			if (isspace(component[2].back())) { colNo = readbyline[lineNo].size()-1; return parseError(MISSING_NEWLINE); }
			if (!myStr2Int(component[2], l2)) { errMsg = "AIG literal ID"; return parseError(MISSING_NUM); }
			if (l2/2 > max_id) { errInt = l2; return parseError(MAX_LIT_ID); }
		}
		else if (component.size() < 3) { 
			colNo += readbyline[lineNo].size();
			if (!isspace(readbyline[lineNo].back())) { return parseError(MISSING_SPACE); }
			else { errMsg = "number of variables"; return parseError(MISSING_NUM); }
		}
		else if (component.size() > 3) {
			colNo += (component[0].size()+component[1].size()+component[2].size()+2);
			return parseError(MISSING_NEWLINE);
		}
		
		if (totallist[(unsigned)(literalID / 2)] == 0) {
			_p = new AIG((unsigned)(literalID / 2), lineNo + 1);
			totallist[(unsigned)(literalID / 2)] = _p;
			idsforAIG.push_back((unsigned)(literalID / 2));
		}
		else { errGate = totallist[(unsigned)(literalID / 2)]; errInt = literalID; return parseError(REDEF_GATE); }

		literalids.push_back(l1);
		literalids.push_back(l2);
	}
	
	//naming for PIs,POs
	string nameid_temp = "", name = "";
	size_t pos = 0;
	int nameid = 0;
	while (lineNo < readbyline.size()-1) {
		colNo = 0;
		lineNo++;
		if (readbyline[lineNo][0] == ' ')  return parseError(EXTRA_SPACE);
		else if (isspace(readbyline[lineNo][0])) { errInt = (int)readbyline[lineNo][0]; return parseError(ILLEGAL_WSPACE); }
		else if (readbyline[lineNo][0] == 'c') {
			colNo++;
			if (readbyline[lineNo].size() > 1) return parseError(MISSING_NEWLINE);
			break;
		}
		
		else if (readbyline[lineNo][0] == 'i') {
			colNo++;
			if (readbyline[lineNo][1] == ' ') return parseError(EXTRA_SPACE);
			else if (isspace(readbyline[lineNo][1])) { errInt = (int)readbyline[lineNo][1]; return parseError(ILLEGAL_WSPACE); }
			
			nameid_temp = readbyline[lineNo].substr(1);
			pos = nameid_temp.find_first_of(" \t\v\f\r");
			
			if (pos != string::npos) {
				if(nameid_temp[pos] != ' ') { colNo = pos + 1; return parseError(MISSING_SPACE); }
				name = nameid_temp.substr(pos + 1);
				nameid_temp = nameid_temp.substr(0, pos);
				
				if(!myStr2Int(nameid_temp, nameid)) {
					errMsg = "symbol index("; errMsg.append(nameid_temp); errMsg.append(")");
					return parseError(ILLEGAL_NUM);
				}
				colNo += nameid_temp.size() + 1;
				if (!name.size()) { errMsg = "symbolic name"; return parseError(MISSING_IDENTIFIER); }
				if(nameid < PIsize) {
					for (unsigned i = 0; i < name.size(); i++) {
						if (!isprint(name[i])) { errInt = (int)name[i]; return parseError(ILLEGAL_SYMBOL_NAME); }
						colNo++;
					}
					if(!PIlist[nameid]->setname(name)) { errInt = nameid; errMsg = "i"; return parseError(REDEF_SYMBOLIC_NAME); }
				}
				else { errInt = nameid; errMsg = "PI index"; return parseError(NUM_TOO_BIG); }
			}
			else { errMsg = "symbolic name"; return parseError(MISSING_IDENTIFIER); }
		}
		
		else if (readbyline[lineNo][0] == 'o') {
			colNo++;
			if (readbyline[lineNo][1] == ' ') return parseError(EXTRA_SPACE);
			else if (isspace(readbyline[lineNo][1])) { errInt = (int)readbyline[lineNo][1]; return parseError(ILLEGAL_WSPACE); }
			
			nameid_temp = readbyline[lineNo].substr(1);
			pos = nameid_temp.find_first_of(" \t\v\f\r");
			
			if (pos != string::npos) {
				if(nameid_temp[pos] != ' ') { colNo = pos + 1; return parseError(MISSING_SPACE); }
				name = nameid_temp.substr(pos + 1);
				nameid_temp = nameid_temp.substr(0, pos);
				
				if(!myStr2Int(nameid_temp, nameid)) {
					errMsg = "symbol index("; errMsg.append(nameid_temp); errMsg.append(")");
					return parseError(ILLEGAL_NUM);
				}
				colNo += nameid_temp.size() + 1;
				if (!name.size()) { errMsg = "symbolic name"; return parseError(MISSING_IDENTIFIER); }
				if(nameid < POsize) {
					for (unsigned i = 0; i < name.size(); i++) {
						if (!isprint(name[i])) { errInt = (int)name[i]; return parseError(ILLEGAL_SYMBOL_NAME); }
						colNo++;
					}
					if(!POlist[nameid]->setname(name)) { errInt = nameid; errMsg = "o"; return parseError(REDEF_SYMBOLIC_NAME); }
				}
				else { errInt = nameid; errMsg = "PO index"; return parseError(NUM_TOO_BIG); }
			}
			else { errMsg = "symbolic name"; return parseError(MISSING_IDENTIFIER); }
		}
		else { errMsg = readbyline[lineNo][0]; return parseError(ILLEGAL_SYMBOL_TYPE); }
	}

	//connect cirgates (FOR P0)
	lineNo = PIsize + 1;
	for (int i = 0; i < POsize; i++) {
		myStr2Int(readbyline[lineNo], literalID);
		bool isinv = literalID%2;
		if (totallist[(unsigned)(literalID/2)] == 0) {
			_p = new UNDEF((unsigned)(literalID/2), 0);
			POlist[i]->push_fanin(_p, isinv);
			totallist[(unsigned)(literalID/2)] = _p;
		}
		else {
			POlist[i]->push_fanin(totallist[(unsigned)(literalID/2)], isinv);
			totallist[(unsigned)(literalID/2)]->push_fanout(POlist[i]);
		}
		lineNo++;
	}

	//connect cirgates (FOR AIG)
	for (int i = 0; i < AIG_num; i++) {
		bool isinv1 = literalids[2*i]%2, isinv2 = literalids[2*i+1]%2;
		if (totallist[(unsigned)(literalids[2*i]/2)] == 0) {
			_p = new UNDEF((unsigned)(literalids[2*i]/2), 0);
			totallist[idsforAIG[i]]->push_fanin(_p, isinv1);
			totallist[(unsigned)(literalids[2*i]/2)] = _p;
			_p->push_fanout(totallist[idsforAIG[i]]);
		}
		else {
			totallist[idsforAIG[i]]->push_fanin(totallist[(unsigned)(literalids[2*i]/2)], isinv1);
			totallist[(unsigned)(literalids[2*i]/2)]->push_fanout(totallist[idsforAIG[i]]);
		}
		if (totallist[(unsigned)(literalids[2*i+1]/2)] == 0) {
			_p = new UNDEF((unsigned)(literalids[2*i+1]/2), 0);
			totallist[idsforAIG[i]]->push_fanin(_p, isinv2);
			totallist[(unsigned)(literalids[2*i+1]/2)] = _p;
			_p->push_fanout(totallist[idsforAIG[i]]);
		}
		else {
			totallist[idsforAIG[i]]->push_fanin(totallist[(unsigned)(literalids[2*i+1]/2)], isinv2);
			totallist[(unsigned)(literalids[2*i+1]/2)]->push_fanout(totallist[idsforAIG[i]]);
		}
		lineNo++;
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
	int count_a = 0;
	for(size_t i = 0; i <totallist.size();i++){
		if(totallist[i]!=NULL){
			if(totallist[i]->getTypeStr()=="AIG")
			count_a++;
		}
	}
	cout << endl
	     << "Circuit Statistics" << endl
	     << "==================" << endl
	     << "  PI" << setw(12) << right << PIlist.size() << endl
	     << "  PO" << setw(12) << right << POlist.size() << endl
	     << "  AIG" << setw(11) << right << count_a << endl
	     << "------------------" << endl
		  << "  Total" << setw(9) << right << PIlist.size() + POlist.size() + count_a<< endl;
}

void
CirMgr::printNetlist() const
{
	cout << endl;
	CirGate::setGlobalRef();
	CirGate::setcount();
	for (size_t i = 0; i < POlist.size(); i++) {
		POlist[i]->netlistprint();
	}
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i = 0; i < PIlist.size(); i++) {
		cout << " " << PIlist[i]->getID();
	}
	cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i = 0; i < POlist.size(); i++) {
		cout << " " << POlist[i]->getID();
	}
	cout << endl;
}

void
CirMgr::printFloatGates() const
{
	vector<unsigned> floating, unused;
	floating.clear();
	unused.clear();

	for (unsigned i = 0; i < totallist.size(); i++) {
		if(totallist[i] != 0) {
			if (totallist[i]->getTypeStr() == "AIG") {
				if (totallist[i]->checkfloat())  floating.push_back(i);
				if (totallist[i]->checkunused())  unused.push_back(i);
			}
			else if (totallist[i]->getTypeStr() == "PI") {
				if (totallist[i]->checkunused())  unused.push_back(i);
			}
			else if (totallist[i]->getTypeStr() == "PO") {
				if (totallist[i]->checkfloat())  floating.push_back(i);
			}
		}
	}

	sort(floating.begin(), floating.end());
	sort(unused.begin(), unused.end());

	if (!floating.empty()) {
		cout << "Gates with floating fanin(s):";
		for (size_t i = 0; i < floating.size(); i++)  cout << " " << floating[i];
		cout << endl;
	}
	if (!unused.empty()) {
		cout << "Gates defined but not used  :";
		for (size_t i = 0; i < unused.size(); i++)  cout << " " << unused[i];
		cout << endl;
	}
}

void
CirMgr::writeAag(ostream& outfile) const
{
	CirGate::setGlobalRef();
	vector <CirGate*> dfslist;
	for (size_t i = 0; i < POlist.size(); i++) {
		POlist[i]->saveAIG(dfslist);
	}

	outfile << "aag " << max_id << " " << PIlist.size() << " 0 " << POlist.size() << " " << dfslist.size() << "\n";
	for (size_t i = 0; i < PIlist.size(); i++)  
		outfile << 2 * (PIlist[i]->getID()) << "\n";
	for (size_t i = 0; i < POlist.size(); i++)
		outfile << POlist[i]->write() << "\n";
	for (size_t i = 0; i < dfslist.size(); i++)
		outfile << dfslist[i]->write() << "\n";
	for (size_t i = 0; i < PIlist.size(); i++) {
		if (PIlist[i]->getname().size()) {
			outfile << "i" << i << " " << PIlist[i]->getname() << "\n";
		}
	}
	for (size_t i = 0; i < POlist.size(); i++) {
		if (POlist[i]->getname().size()) {
			outfile << "o" << i << " " << POlist[i]->getname() << "\n";
		}
	}
	outfile << "c\nAAG output by Chung-Yang (Ric) Huang\n";
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}
