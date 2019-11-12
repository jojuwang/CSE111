// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

#include <cassert>
#include <regex>
#include <fstream>
#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void mapit(istream& infile, const string& inName,
           str_str_map& myMap) {
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   int count = 1;

   for (;;) {
      string line;
      getline (infile, line);
      if (infile.eof()) break;
      cout << inName << ": " << count << ": " << line << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         ++count;
         continue;
      }
      if (regex_search (line, result, key_value_regex)) {
         if (result[1].length() != 0 && result[2].length() != 0){
            xpair<const string,string> pair (result[1], result[2]);
            myMap.insert(pair);
            cout << result[1] << " = " << result[2] << endl;
         } else if (result[1].length() != 0 && result[2].length() == 0){
            string discard (result[1]);
            for (str_str_map::iterator i = myMap.begin();
                 i != myMap.end(); ++i){
               if (i->first == discard){
                  myMap.erase(i);
                  break;
               }
            }
         } else if (result[1].length() == 0 && result[2].length() != 0){
            string search (result[2]);
            for (str_str_map::iterator i = myMap.begin();
                 i != myMap.end(); ++i){
               if (i->second == search){
                  cout << i->first << " = " << i->second << endl;
               }
            }
         } else {
            for (str_str_map::iterator i = myMap.begin();
                 i != myMap.end(); ++i){
               cout << i->first << " = " << i->second << endl;
            }
         }
      }else if (regex_search (line, result, trimmed_regex)) {
         string key_name (result[1]);
         str_str_map::iterator found = myMap.find(key_name);
         if (found == myMap.end()){
            cout << key_name << ": key not found" << endl;
         } else {
            cout << found->first << " = "
                 << found->second << endl;
         }
      }else {
         assert (false and "This can not happen.");
      }
      ++count;
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   int status = 0;
   str_str_map myMap;

   if (argc == 1){
      mapit(cin, "-", myMap);
   } else {
      str_str_map test;
      string progname (basename (argv[0]));
      for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
         str_str_pair pair (*argp, to_string<int> (argp - argv));
         test.insert (pair);
      }

      for (str_str_map::iterator itor = test.begin();
           itor != test.end(); ++itor) {
         string filename = itor->first;
         if (filename == "-"){
            mapit(cin, filename, myMap);
         } else {
            ifstream infile (itor->first);
            if (infile.fail()) {
               status = 1;
               cerr << progname << ": " << filename << ": "
                    << strerror (errno) << endl;
            } else {
               mapit (infile, filename, myMap);
               infile.close();
            }
         }
      }

      str_str_map::iterator itor = test.begin();
      test.erase (itor);
   }
   return status;
}

