/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY :if(_readBufPtr != _readBuf){
                                                            _readBufPtr =  _readBufPtr -1;
                                                            cout <<'\b';
                                                            deleteChar();
                                                            }
                                                            else{
                                                               mybeep();
                                                            }
                                                            break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); 
                               break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break; //8->0(index9)9->0(index10); 10->1 (11)
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr+1);/* TODO */ break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr-1);/* TODO */ break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        :  { int tabCount = 0; 
                                                   char* tabPtr = _readBufPtr;
                                                   while(tabPtr !=_readBuf){
                                                        tabPtr = tabPtr-1;
                                                        tabCount++; 
                                                   }
                                                   tabCount = tabCount%8;
                                                   for(int i = 0;i<8- tabCount;i++){
                                                      insertChar(char(32));
                                                   }                
            /* TODO */ break;
         }
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   if(ptr == _readBufPtr-1){
      if(_readBufPtr!=_readBuf){
         cout << '\b';
         _readBufPtr =  _readBufPtr -1;
      }
      else{
         mybeep();
      } 
   }
   else if(ptr == _readBufPtr+1){
      int countR = 0;
      if(_readBufPtr!=_readBufEnd){
         while(_readBufPtr!=_readBufEnd){
               cout <<  *_readBufPtr;
               _readBufPtr =  _readBufPtr +1;
               countR++; 
            }
         for(int i = 0; i <countR-1;i++){
               cout <<'\b';
               _readBufPtr =  _readBufPtr -1;
            }  
      }
      else{
         mybeep();
      } 
   }
   else if(ptr == _readBuf){
     
      while(_readBufPtr!=_readBuf){
               _readBufPtr =  _readBufPtr -1;
               cout << '\b';
            }
   }
   else if(ptr == _readBufEnd){
     
      while(_readBufPtr!=_readBufEnd){
               cout <<  *_readBufPtr;
               _readBufPtr =  _readBufPtr +1;
            }
   }
   // TODO...
   return true;
}

// CmdParser::moveRight(){
//    if(Position!=size){
//     for(int i = Position; i <size;i++){
//           cout << _readBuf[i];
//        }
//     for(int i = Position; i <size-1;i++){
//           cout <<'\b';
//        }
//       Position ++;
//       _readBufPtr =  _readBufPtr +1;
//    }
//    else{
//       mybeep();
//    } 
// }



// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   if(_readBufPtr==_readBufEnd){
      mybeep();
   }
   else{
   int countD = 0;
   _readBufPtr= (_readBufPtr+1);
   while(_readBufPtr !=_readBufEnd){
      cout << *_readBufPtr ; 
      *(_readBufPtr -1) = *_readBufPtr;
      _readBufPtr = _readBufPtr +1;
      countD ++;
   }
    _readBufEnd = _readBufEnd -1;
    *_readBufEnd= '\0';
    cout << " "; 

    for(int i = 0; i<countD+1;i++){
       _readBufPtr = _readBufPtr -1;
       cout <<'\b';
    }

   }
   // TODO...
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);

       char temp;
       char temp2;
       int count = 0;
       cout << ch;
      _readBufEnd = _readBufEnd+ 1;
       if(_readBufPtr == _readBufEnd ){
               *_readBufPtr = ch; 
       }
       else{
          temp = *(_readBufPtr );
          *_readBufPtr = ch;
          _readBufPtr =  _readBufPtr +1;
          count++;
       while(_readBufPtr!=_readBufEnd){
          temp2 = *_readBufPtr ;
          *_readBufPtr  = temp;
          temp = temp2;
          temp2 = *(_readBufPtr +1);
          cout <<  *_readBufPtr;
          _readBufPtr =  _readBufPtr +1;
          count++;
       }

       for(int i =1; i <count;i++){
          cout << '\b';
          _readBufPtr =  _readBufPtr -1;
       }
    }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   while(_readBufPtr!=_readBuf){
      cout <<  '\b';
       _readBufPtr =  _readBufPtr -1;
   }
   while(_readBufPtr!=_readBufEnd){
               cout <<  *_readBufPtr;
               _readBufPtr =  _readBufPtr +1;
            }
   while(_readBufPtr!=_readBuf){
               cout <<  ' ';
                *_readBufPtr = '\0'; 
               _readBufPtr =  _readBufPtr -1;
               cout <<  '\b' <<'\b';
            }
   if(_readBufPtr==_readBuf){
               cout <<  ' ';
                cout <<  '\b';
               *_readBufPtr = '\0'; 
            } 
   _readBufEnd  = _readBuf;          
   // TODO...
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{  
   string tempS;

   // TODO...

   if(index == _historyIdx -1){   //up situation
       if(_historyIdx ==0){
         mybeep();   
      }
      else if(_historyIdx  == static_cast<int>(_history.size()) ){
          if(*_readBuf=='\0'){
             _historyIdx = _historyIdx -1;
            retrieveHistory();
          }
          else{
             
             tempS.assign(_readBuf);
            _history[_historyIdx] = tempS;
            _tempCmdStored = 1;
            _historyIdx = _historyIdx -1;
            retrieveHistory();
          }
      }
      else{
         _historyIdx = _historyIdx -1;
         retrieveHistory();
      }
   }

   if(index==_historyIdx - PG_OFFSET){ //PG_UP situation
      if(_historyIdx ==0) mybeep();
      else if(_historyIdx  == static_cast<int>(_history.size()) ){
          if(*_readBuf=='\0'){
             if(_historyIdx>0 && _historyIdx<= 10){
               _historyIdx = 0;
               retrieveHistory();
             }
             else{
               _historyIdx = _historyIdx -10;
               retrieveHistory();
             }
          }
          else{
             
             tempS.assign(_readBuf);
            _history[_historyIdx] = tempS;
            _tempCmdStored = 1;
             if(_historyIdx>0 && _historyIdx<= 10){
               _historyIdx = 0;
             }
             else{
               _historyIdx = _historyIdx -10;
             }
            retrieveHistory();
          }
      }
      else{
         if(_historyIdx>0 && _historyIdx<= 10){
               _historyIdx = 0;
               retrieveHistory();
             }
         else{
               _historyIdx = _historyIdx -10;
               retrieveHistory();
         }   
      }
   }



   if(index == _historyIdx +1){   //down situation
       if(_historyIdx ==static_cast<int>(_history.size())){
         mybeep();   
      }
      else if(_historyIdx  == static_cast<int>(_history.size())-1 ){
          if(_tempCmdStored == 0 ){
            deleteLine();
            _readBufPtr = _readBufEnd = _readBuf;
        *_readBufPtr = 0;
         _historyIdx = _historyIdx +1;
          }
          else{
             deleteLine();
            _historyIdx = _historyIdx +1;
            strcpy(_readBuf, _history[_historyIdx].c_str());
            cout << _readBuf;
           _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
            
          }
      }
      else{
         _historyIdx = _historyIdx +1;
         retrieveHistory();
      }
   }

   if(index==_historyIdx + PG_OFFSET){  //PG_DN situation
      if(_historyIdx ==static_cast<int>(_history.size())){
         mybeep();   
      }
      else if(_historyIdx  <= static_cast<int>(_history.size())-1  && _historyIdx  >= static_cast<int>(_history.size())-10){
         if(_tempCmdStored == 0 ){
            deleteLine();
            _readBufPtr = _readBufEnd = _readBuf;
        *_readBufPtr = 0;
         _historyIdx =  static_cast<int>(_history.size());
          }
          else{
             deleteLine();
            _historyIdx = static_cast<int>(_history.size());
            strcpy(_readBuf, _history[_historyIdx].c_str());
            cout << _readBuf;
           _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
            
          }
      }
       else{
          if(_historyIdx  <= static_cast<int>(_history.size())-1  && _historyIdx  >= static_cast<int>(_history.size())-10){
             _historyIdx = static_cast<int>(_history.size());
         retrieveHistory();
          }
          else{
             _historyIdx = _historyIdx +10;
         retrieveHistory();
          }
         
      }
   }
  
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   bool Goon = 0;
   char* checkPtr = _readBuf;
   while(checkPtr != _readBufEnd){
      if(*checkPtr != char(32) or '\0' ){
         Goon = 1;
         break;
      }
      checkPtr = checkPtr +1;         
   }
  
   if(Goon ==1){
      while(_readBufPtr !=  _readBuf){
         _readBufPtr =  _readBufPtr -1;
         cout <<  '\b';
      }

      
      string history;
      history.assign(_readBuf);
       cout << history;
      history.erase(history.find_last_not_of(" ") + 1);
      history.erase(0, history.find_first_not_of(" "));
      _history.push_back(history);
      _historyIdx =  static_cast<int>(_history.size());
      history.clear();
      deleteLine();
      // cout << _history[static_cast<int>(_history.size()) -1];
      _tempCmdStored = 0;
   }
   // TODO...
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
