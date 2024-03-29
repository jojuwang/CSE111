// $Id: ubigint.cpp,v 1.16 2019-04-02 16:28:42-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
   DEBUGF ('~', this << " -> " << *this);
   //(void) that;
   while (that != 0){
     ubig_value.push_back(that % 10);
     that /= 10;
   }
}

ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (auto i = that.rbegin(); i != that.rend(); ++i){
      if (not isdigit (*i)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      char digit = *i - '0';
      ubig_value.push_back(digit);
   }
   while (ubig_value.size() > 0 and ubig_value.back() == 0) ubig_value.pop_back();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   unsigned sum = 0;
   unsigned carry = 0;
   unsigned left_op = 0;
   unsigned right_op = 0;
   ubigint result {};
   unsigned max_dig = max(ubig_value.size(), that.ubig_value.size());
   for (unsigned i = 0; i < max_dig; i++){
      if (i >=ubig_value.size()    ){
         left_op = 0;
         right_op = that.ubig_value[i];
      } else if (i >=that.ubig_value.size()    ){
         left_op = ubig_value[i];
         right_op = 0;
      } else {
         left_op = ubig_value[i];
         right_op = that.ubig_value[i];
      }
      sum = left_op + right_op + carry;
      carry = sum / 10;
      sum = sum % 10;
      result.ubig_value.push_back(sum);
   }
   result.ubig_value.push_back(carry);
   while (result.ubig_value.size() > 0 and result.ubig_value.back() == 0) result.ubig_value.pop_back();
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
  if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
  /*if (ubig_value.size() < that.ubig_value.size()){
    throw domain_error ("ubigint::operator-(a<b>");
    }*/
   int diff = 0;
   unsigned borrow = 0;
   int left_op = 0;
   int right_op = 0;
   ubigint result {};
   for (unsigned i = 0; i < ubig_value.size(); i++){
      //this->ubig_value[i] -= borrow;
      left_op = ubig_value[i] - borrow;

      if (i >= that.ubig_value.size()){
         right_op = 0;
      } else {
         right_op = that.ubig_value[i];
      }

     if (left_op < right_op){
         left_op += 10;
         borrow = 1;
     } else {
         borrow = 0;
     }
     diff = left_op - right_op;
     result.ubig_value.push_back(diff);
   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   unsigned carry = 0;
   unsigned d = 0;
   ubigint result {};
   for (unsigned n = 0; n < (ubig_value.size() + that.ubig_value.size()); n++){
      result.ubig_value.push_back(0);
   }
   for (unsigned i = 0; i < ubig_value.size(); i++){
      carry = 0;
      for (unsigned j = 0; j < that.ubig_value.size(); j++){
         d = result.ubig_value[i+j] + ubig_value[i]*that.ubig_value[j] + carry;
         result.ubig_value[i+j] = d % 10;
         carry = d / 10;
      }
      result.ubig_value[i + that.ubig_value.size()] = carry;
   }
  //return ubigint (uvalue * that.uvalue);
   while (result.ubig_value.size() > 0 and result.ubig_value.back() == 0) result.ubig_value.pop_back();
   return result;
}

void ubigint::multiply_by_2() {
  //uvalue *= 2;
  unsigned carry = 0;
  unsigned d = 0;
  for (unsigned i = 0; i < ubig_value.size(); i++){
    //carry = 0;
    d = ubig_value[i] * 2 + carry;
    ubig_value[i] = d % 10;
    carry = d / 10;
  }
  ubig_value.push_back(carry);
  while (ubig_value.size() > 0 and ubig_value.back() == 0) ubig_value.pop_back();
}

void ubigint::divide_by_2() {
  //ubig_value /= 2;
  int c = 0;
  for (unsigned i = 0; i < ubig_value.size(); i++){
    c = ubig_value[i] / 2;
    if ( (i + 1) < ubig_value.size() && ubig_value[i+1] % 2 == 1 ){
      c += 5;
    }
    ubig_value[i] = c;
  }
  while (ubig_value.size() > 0 and ubig_value.back() == 0) ubig_value.pop_back();
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if (ubig_value.size() != that.ubig_value.size()){
      return false;
   } else {
     for (unsigned i = ubig_value.size(); i >= 1; i--){
         if (ubig_value[i-1] != that.ubig_value[i-1]){
            return false;
         }
      }
   }
  return true;
}

bool ubigint::operator< (const ubigint& that) const {
  //return uvalue < that.uvalue;
   if (ubig_value.size() > that.ubig_value.size()){
      return false;
   } else if (ubig_value.size() < that.ubig_value.size()){
      return true;
   } else {
     for (unsigned i = ubig_value.size(); i >= 1; i--){
         if (ubig_value[i-1] > that.ubig_value[i-1]){
            return false;
         }
	 if (ubig_value[i-1] < that.ubig_value[i-1]){
	    return true;
	 }
      }
   }
  return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
  /*for (unsigned i = 0; i < that.ubig_value.size(); i++){
    out << static_cast<char>(that.ubig_value[i] + '0');
    }*/
  int n = 0;
  for (unsigned i = that.ubig_value.size(); i >= 1; i--){
    out << static_cast<char>(that.ubig_value[i-1] + '0');
    n++;
    if (n >= 69){
      out << "\\\n";
      n = 0;
    }
  }
  return out;
}

