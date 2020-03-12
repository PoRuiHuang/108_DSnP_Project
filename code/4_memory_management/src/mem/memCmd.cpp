/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   int num1;
   int num2;
   lexOptions(option,tokens);
   if(tokens.size() ==0){
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   else if(tokens.size() ==1){
      if(myStr2Int(tokens[0], num1) && num1 >0){
         try{
            mtest.newObjs(num1);
         }
         catch(bad_alloc){
            return CMD_EXEC_ERROR;
         }
         return CMD_EXEC_DONE;
      }
      else if(myStrNCmp("-ARRAY", tokens[0], 2)== 0){
         return CmdExec::errorOption(CMD_OPT_MISSING, tokens[0]);
      }
      else{
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
      }
   }

   else if(tokens.size() ==3){
      if( (myStrNCmp("-ARRAY", tokens[0], 2)== 0) && (myStr2Int(tokens[1], num2) && num2 >0) && (myStr2Int(tokens[2], num1) && num1 >0)  ){
          try{
            mtest.newArrs(num1, num2);                  
          }
          catch(bad_alloc){
            return CMD_EXEC_ERROR;
         }
         return CMD_EXEC_DONE;
      }
      else if( (myStrNCmp("-ARRAY", tokens[1], 2)== 0) && (myStr2Int(tokens[2], num2) && num2 >0) && (myStr2Int(tokens[0], num1) && num1 >0)  ){
         try{
            mtest.newArrs(num1, num2);
          }
         catch(bad_alloc){
            return CMD_EXEC_ERROR;
         }
         return CMD_EXEC_DONE;
      }
      else{
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
      }
   }
   else{
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
      
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   vector<string> tokens;
   int num;
   lexOptions(option,tokens);

   if(tokens.size()==2){
      int size =  mtest.getObjListSize();
       if( (myStrNCmp("-INDEX", tokens[0], 2)== 0) && (myStr2Int(tokens[1], num) && num>=0)   ){
          if( num >= size){
            cerr << "Size of object list (" << size << ") is <= " << num << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
          else
          mtest.deleteObj(num);
       }

       else if( (myStrNCmp("-RANDOM", tokens[0], 2)== 0) && (myStr2Int(tokens[1], num) && num>0)   ){
          if( size==0){
            cerr << "Size of object list is 0!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
         }
          else{
            for (int i = 0; i <num; i++)
               mtest.deleteObj(rnGen(size));
         }
          
       }
       else{
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
       }
   }

   else if(tokens.size()==3){
      int size =  mtest.getArrListSize();
       if( (myStrNCmp("-RANDOM", tokens[0], 2)== 0) && (myStr2Int(tokens[1], num) && num >0) && (myStrNCmp("-ARRAY", tokens[2], 2)== 0)  ){
          if( size==0){
            cerr << "Size of array list is 0!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[1]);
         }
          else{
            for (int i = 0; i < num; i++)
               mtest.deleteArr(rnGen(size));
         }
       }

      else if( (myStrNCmp("-RANDOM", tokens[1], 2)== 0) && (myStr2Int(tokens[2], num) && num >0) && (myStrNCmp("-ARRAY", tokens[0], 2)== 0)  ){
          if( size==0){
            cerr << "Size of array list is 0!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL,tokens[2]);
         }
          else{
            for (int i = 0; i < num; i++)
               mtest.deleteArr(rnGen(size));
         }
       }

      else if( (myStrNCmp("-INDEX", tokens[0], 2)== 0) && (myStr2Int(tokens[1], num) && num >=0) && (myStrNCmp("-ARRAY", tokens[2], 2)== 0)  ){
         if( num >= size){
            cerr << "Size of array list (" << size << ") is <= " << num << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
         }
          else{
               mtest.deleteArr(num);
         }
       }

       else if( (myStrNCmp("-INDEX", tokens[1], 2)== 0) && (myStr2Int(tokens[2], num) && num >=0) && (myStrNCmp("-ARRAY", tokens[0], 2)== 0)  ){
         if( num >= size){
            cerr << "Size of array list (" << size << ") is <= " << num << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
         }
          else{
               mtest.deleteArr(num);
         }
       }
   
      else{
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
       }
   }
   
   else if(tokens.size() ==0){
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   else{
       return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   }
   // TODO

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


