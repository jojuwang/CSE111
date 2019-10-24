// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->insert_into_dirents(".", root);
   root->contents->insert_into_dirents("..", root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() const { return prompt_; }

/*inode_state::~inode_state() {
   cwd = nullptr;
   if (root) root->disown();
   root = nullptr;
}*/

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::insert_into_dirents (const string&, inode_ptr){
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr self = dirents["."];
   inode_ptr parent = dirents[".."];
   for (auto iter = dirents.begin(); iter != dirents.end(); ++iter){
      auto myPair = *iter;
      cout << myPair.first << endl;
      inode_ptr ptr2 = myPair.second;
   }
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   //...
   /*inode_ptr owner_this = dirents["."];
   newDir->dirents[".."] = owner_this;*/
   return nullptr;
}

void directory::insert_into_dirents (const string& key,
                                      inode_ptr value) {
   dirents[key] = value;
}

/*void inode::disown() {
   dir_ptr dir = dynamic_pointer_cast<directory>(contents);
   contents->disown();
   contents = nullptr;
}

void directory::disown() {
   for (auto iter = dirents.begin(); iter != dirents.end(); ){
      if (iter->first == "." || iter->first == ".."){
         // do something
      } else {
         // do something
      }
      dirents.erase(iter++);
      iter++;
   }
}*/

