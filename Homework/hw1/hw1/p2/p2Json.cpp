/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   string line;
   ifstream f(jsonFile);
   bool inObj = false;
   if (f.is_open()) {
      while (getline(f, line)) {
         // check start and end of object
         if (line.find('{') != string::npos)
            inObj = true;
         else if (line.find('}') != string::npos){
            inObj = false;
            break;
         }
         else if (inObj) {
            // check there is an object in this line
            if (line.find('\"') == string::npos)
               continue;
            else {
               // get key
               string key;
               size_t start_pos, end_pos;
               start_pos = line.find('\"');
               end_pos = line.find('\"', start_pos + 1);
               key = line.substr(start_pos + 1, end_pos - start_pos - 1);
               // cout << key << endl;

               // get value
               int value;
               string tmp;
               start_pos = line.find(':');
               end_pos = line.find(',');
               if (end_pos == string::npos) {
                  tmp = line.substr(start_pos + 1);
                  value = stoi(tmp);
               }
               else {
                  tmp = line.substr(start_pos + 1, end_pos - start_pos - 1);
                  value = stoi(tmp);
               }
               _obj.push_back(JsonElem(key, value));
            }
         }
      }
      f.close();
      return true; // TODO
   }
   else
      return false;
}

void
Json::print()
{
   cout << '{' << endl;
   for (size_t i = 0; i < _obj.size(); ++i){
      if (i == _obj.size() - 1)
         cout << "  " << _obj[i] << endl;
      else
         cout << "  " << _obj[i] << ',' << endl;
   }
   cout << '}' << endl;
}

void
Json::sum()
{
   int sum = 0;
   for (size_t i = 0; i < _obj.size(); ++i) {
      sum += _obj[i].getValue();
   }
   cout << "The summation of the values is: " << sum << '.' << endl;
}

void
Json::ave()
{
   double sum = 0;
   for (size_t i = 0; i < _obj.size(); ++i) {
      sum += double(_obj[i].getValue());
   }
   cout << "The average of the values is: " << fixed << setprecision(1) << sum / _obj.size() << '.' << endl;
}

void
Json::max()
{
   size_t ind;
   int max = -1000;
   for (size_t i = 0; i < _obj.size(); ++i){
      if (max < _obj[i].getValue()) {
         ind = i;
         max = _obj[i].getValue();
      }
   }
   cout << "The maximum element is: { " << _obj[ind] << " }." << endl;
}

void
Json::min()
{
   size_t ind;
   int min = 1000;
   for (size_t i = 0; i < _obj.size(); ++i){
      if (min > _obj[i].getValue()) {
         ind = i;
         min = _obj[i].getValue();
      }
   }
   cout << "The minimum element is: { " << _obj[ind] << " }." << endl;
}

void
Json::add(const string& key, const int& value)
{
   _obj.push_back(JsonElem(key, value));
}

int
JsonElem::getValue()
{
   return this->_value;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

