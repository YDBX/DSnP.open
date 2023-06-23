/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string line;
   bool inObj = false;
   while (getline(is, line)) {
      if (line.find('{') != string::npos)
         inObj = true;
      else if (line.find('}') != string::npos)
         break;
      else if (inObj) {
         if (line.find('\"') == string::npos)
            continue;

         string key;
         size_t begin = line.find_first_of('\"');
         size_t end = line.find_first_of('\"', begin + 1);
         key = line.substr(begin + 1, end - begin - 1);
         
         int value;
         begin = line.find(':');
         begin = line.find_first_not_of(' ', begin + 1);
         for (size_t i = line.size() - 1; i >= 0; --i) {
            if (isdigit(line[i])) {
               end = i;
               break;
            }
         }
         myStr2Int(line.substr(begin, end - begin + 1), value);

         j._obj.push_back(DBJsonElem(key, value));
      }
   }
   j.hasRead = true;

   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   cout << "{" << endl;
   for (size_t i = 0; i < j._obj.size(); ++i){
      if (i == j._obj.size() - 1)
         cout << "   " << j._obj[i] << endl;
      else 
         cout << "   " << j._obj[i] << "," << endl;
   }
   cout << "}" << endl;
   cout << "Total JSON elements: " << j._obj.size() << endl;

   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   size_t s = _obj.size();
   for (size_t i = 0; i < s; ++i)
      _obj.pop_back();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for (size_t i = 0; i < _obj.size(); ++i)
      if (_obj[i].key() == elm.key())
         return false;
   _obj.push_back(elm);

   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   float avg = 0;
   for (size_t i = 0; i < _obj.size(); ++i)
      avg += (float)_obj[i].value();
   return avg / _obj.size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   for (size_t i = 0; i < _obj.size(); ++i) {
      if (maxN < _obj[i].value()) {
         maxN = _obj[i].value();
         idx = i;
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   for (size_t i = 0; i < _obj.size(); ++i) {
      if (minN > _obj[i].value()) {
         minN = _obj[i].value();
         idx = i;
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   for (size_t i = 0; i < _obj.size(); ++i)
      s += _obj[i].value();
   return s;
}
