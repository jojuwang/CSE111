// $Id: bigint.cpp,v 1.1 2019-10-15 19:34:36-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue);
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   //ubigint result = uvalue + that.uvalue;
   //return result;
   ubigint result {};
   if (is_negative == that.is_negative){
      result = uvalue + that.uvalue;
      return {result, is_negative};
   } else {
      if ( uvalue < that.uvalue ){
         result = that.uvalue - uvalue;
         return {result, that.is_negative};
      } else {
         result = uvalue - that.uvalue;
         return {result, is_negative};
      }
   }
   //return {result, that.is_negative};
}

bigint bigint::operator- (const bigint& that) const {
  /*ubigint result = uvalue - that.uvalue;
    return result;*/
   ubigint result{};
   if (is_negative != that.is_negative){
      result = uvalue + that.uvalue;
      return {result, is_negative};
   } else {
     /*result = uvalue + that.uvalue;
       return {result, is_negative};*/
     if (uvalue >= that.uvalue){
       result = uvalue - that.uvalue;
       return {result, is_negative};
     } else {
       result = that.uvalue - uvalue;
       return {result, not is_negative};
     }
   }
}


bigint bigint::operator* (const bigint& that) const {
  /*bigint result = uvalue * that.uvalue;
    return result;*/
   bigint result;
   if (is_negative == that.is_negative){
     result = {uvalue * that.uvalue, false};
   } else {
     result = {uvalue * that.uvalue, true};
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
  /* return out << "bigint(" << (that.is_negative ? "-" : "+")
     << "," << that.uvalue << ")";*/
   if (that.is_negative == true){
      out << "-";
   }
   out << that.uvalue;
   return out;
}

