// $Id: hello.cpp,v 1.9 2019-04-03 13:52:31-07 - - $

#include <iostream>
#include <string>
using namespace std;

#include "hello.h"

#define SHOW { \
        cout << this << "->" << __PRETTY_FUNCTION__ \
             << " {\"" << message << "\"}" << endl; \
        }

hello::hello() {
   SHOW;
}

hello::hello (const hello& that): message (that.message) {
   SHOW;
}

hello& hello::operator= (const hello& that) {
   SHOW;
   if (this != &that) message = that.message;
   return *this;
}

hello::hello (hello&& that): message (std::move (that.message)) {
   SHOW;
}

hello& hello::operator= (hello&& that) {
   SHOW;
   if (this != &that) message = std::move (that.message);
   return *this;
}

hello::~hello() {
   SHOW;
}

hello::hello (const string& message_): message(message_) {
   SHOW;
}

void hello::say (ostream& out) {
   SHOW;
   out << message << endl;
}

