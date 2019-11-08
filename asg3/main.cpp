// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

#include <cassert>
#include <regex>
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

/*void scan_file (istream& infile, const string& filename){
   for(;;){
      string line;
      getline(infile, line);
      if (infile.eof()) break;
   }
}*/

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   //string progname (basename (argv[0]));
   /*for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;*/

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   //int count = 1;
   for (;;) {
      string line;
      getline (cin, line);
      if (cin.eof()) break;
      cout << endl << "input: \"" << line << "\"" << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         cout << "Comment or empty line." << endl;
         continue;
      }
      if (regex_search (line, result, key_value_regex)) {
         cout << "key  : \"" << result[1] << "\"" << endl;
         cout << "value: \"" << result[2] << "\"" << endl;
         if (result[1].length() != 0 && result[2].length() != 0){
            xpair<const string,string> pair (result[1], result[2]);
            test.insert(pair);
         } else if (result[1].length() != 0 && result[2].length() == 0){
            /*string discard (result[1]);
            for (str_str_map::iterator i = test.begin();
                 i != test.end(); ++i){
               if (i->first == discard){
                  test.erase(i);
                  break;
               }
            }*/
         } else if (result[1].length() == 0 && result[2].length() != 0){
            string search (result[2]);
            for (str_str_map::iterator i = test.begin();
                 i != test.end(); ++i){
               if (i->second == search){
                  cout << i->first << " = " << i->second << endl;
               }
            }
         } else {
            for (str_str_map::iterator i = test.begin();
                 i != test.end(); ++i){
               cout << i->first << " = " << i->second << endl;
            }
         }
      }else if (regex_search (line, result, trimmed_regex)) {
         cout << "query: \"" << result[1] << "\"" << endl;
         string key_name (result[1]);
         //cout << key_name << endl;
         str_str_map::iterator found = test.find(key_name);
         if (found == test.end()){
            cout << key_name << ": key not found" << endl;
         } else {
            cout << found->first << " = "
                 << found->second << endl;
         }
      }else {
         assert (false and "This can not happen.");
      }
   }
   return 0;
}

