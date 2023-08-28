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
   
   // check options
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   // check "options" is not empty
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   int numObjects;
   int arraySize;
   // Check if the first option is "-Array"
   if (myStrNCmp("-Array", options[0], 2) != 0) {
      if (!myStr2Int(options[0], numObjects) || numObjects <= 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      
      if (options.size() == 1) {
         try {
            mtest.newObjs(numObjects);
         } catch (const exception& e) {
            cerr << e.what() << '\n';
         }
         return CMD_EXEC_DONE;
      }

      if (myStrNCmp("-Array", options[1], 2) != 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      
      if (options.size() == 2)
         return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
      
      if (!myStr2Int(options[2], arraySize) || arraySize <= 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      
      if (options.size() > 3)
         return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);

      try {
         mtest.newArrs(numObjects, arraySize);
      } catch (const bad_alloc& e) {
         cerr << e.what() << '\n';
      }
      return CMD_EXEC_DONE;
   }

   // If the first options is "-Array"
   if (options.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
   
   if (!myStr2Int(options[1], arraySize) || arraySize <= 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
   
   if (options.size() == 2)
      return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
   
   if (!myStr2Int(options[2], numObjects) || numObjects <= 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
   
   try {
      mtest.newArrs(numObjects, arraySize);
   } catch (const bad_alloc& e) {
      cerr << e.what() << '\n';
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
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 0))
      return CMD_EXEC_ERROR;

   // none of "-Index" or "-Random" is specified
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   // check if "-Array" is specified in first option
   int num;
   size_t arraySize;
   bool array = false;
   size_t objId;
   RandomNumGen rnGen;
   if (myStrNCmp("-Array", options[0], 2) != 0) {
      // check if the first options is "-Index" or "-Random"
      if (myStrNCmp("-Index", options[0], 2) != 0 && myStrNCmp("-Random", options[0], 2) != 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      bool index = (myStrNCmp("-Index", options[0], 2) == 0);
      
      // check if "objId" or "numRandId" is specified
      if (options.size() < 2)
         return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
      
      // check if "objId" or "numRandId" is specified
      if (!myStr2Int(options[1], num) || num < 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      
      if (options.size() == 3) {
         if (myStrNCmp("-Array", options[2], 2) != 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         array = true;
      }
      
      if (options.size() > 3)
         return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
      
      arraySize = (array)? mtest.getArrListSize():mtest.getObjListSize();
      objId = num;
      if (index) {
         if (objId >= arraySize) {
            string out = (array)? "object":"array";
            cerr << "Size of "<< out << " list (" << arraySize << ") is <= " << objId << "!!\n";
            return CMD_EXEC_DONE;
         }
         (array)? mtest.deleteArr(objId):mtest.deleteObj(objId);
      }
      else {
         if (arraySize == 0) {
            string out = (array)? "object": "array";
            cerr << "Size of " << out << " list is 0!!\n";
            return CMD_EXEC_DONE;
         }
         for (int i = 0; i < num; ++i) {
            objId = rnGen(arraySize);
            (array)? mtest.deleteArr(objId):mtest.deleteObj(objId);
         }
      }
      return CMD_EXEC_DONE;
   }

   array = true; arraySize = mtest.getArrListSize();
   if (options.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
   
   // check if the first options is "-Index" or "-Random"
   if (myStrNCmp("-Index", options[1], 2) != 0 && myStrNCmp("-Random", options[1], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
   bool index = (myStrNCmp("-Index", options[1], 2) == 0);
   
   if (options.size() < 3)
      return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
   
   // check if "objId" or "numRandId" is int
   if (!myStr2Int(options[2], num))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

   // check if "objId" or "numRandId" is legal
   if (num < 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
   
   // check if extra options is specified
   if (options.size() >= 4)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);

   objId = num;
   if (index) {
      // check if "objId" is no smaller than the size of "_objList" (if no "-Array") or "_arrList" (if with "-Array")
      if (objId >= arraySize) {
         cerr << "Size of array list (" << arraySize << ") is <= " << objId << "!!\n";
         return CMD_EXEC_DONE;
      }
      mtest.deleteArr(objId);
   } else {
      if (arraySize == 0) {
         string out = (array)? "object": "array";
         cerr << "Size of " << out << " list is 0!!\n";
         return CMD_EXEC_DONE;
      }
      for (int i = 0; i < num; ++i) {
         size_t objId = rnGen(arraySize);
         mtest.deleteArr(objId);
      }
   }

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


