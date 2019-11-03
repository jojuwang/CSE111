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

void inode_state::set_prompt(string s) {
   prompt_ = s;
}

const vector<string> inode_state::get_path() {
   return path;
}

void inode_state::push_path(string s) {
   path.push_back(s);
}

void inode_state::pop_path(){
   path.pop_back();
}

inode_state::~inode_state() {
   cwd = nullptr;
   if (root) root->disown();
   root = nullptr;
}

inode_ptr inode_state::get_root() {
   return root;
}

inode_ptr inode_state::get_cwd() {
   return cwd;
}

void inode_state::set_cwd(inode_ptr dest) {
   cwd = dest;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           type_nr = 0;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           type_nr = 1;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr inode::get_contents(){
   return contents;
}

int inode::is_dir() {
   return type_nr;
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

/*inode_ptr plain_file::mkdir (const string& dirname){
   throw runtime_error("This is a plain file");
}*/


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   for (unsigned i = 0; i < data.size(); i++){
      size += data[i].length();
      if (i != data.size() - 1){
         size += 1;
      }
   }
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data.clear();
   for (unsigned i = 0; i < words.size(); i++){
      data.push_back(words[i]);
   }
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   for (auto iter = dirents.begin(); iter != dirents.end(); ++iter){
      size += 1;
   }
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   for (auto iter = dirents.begin(); iter != dirents.end(); ++iter){
      if (iter->first == filename){
         if (iter->second->is_dir() == 0){
            dirents.erase(filename);
         } else {
            if (dirents[filename]->get_contents()->size() != 2){
               throw file_error ("Directory" + filename
                                 + "is not empty");
            } else {
               dirents[filename]->get_contents()->disown();
               dirents.erase(filename);
            }
         }
      }
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr newDir = make_shared<inode>(file_type::DIRECTORY_TYPE);
   return newDir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   inode_ptr newFile = make_shared<inode>(file_type::PLAIN_TYPE);
   return newFile;
}

void directory::insert_into_dirents (const string& key,
                                      inode_ptr value) {
   dirents[key] = value;
}

void inode::disown() {
   contents->disown();
   contents = nullptr;
}

void base_file::disown() {}

void plain_file::disown() {}

void directory::disown() {
   for (auto iter = dirents.begin(); iter != dirents.end(); ){
      if (iter->first == "." || iter->first == ".."){
         // do nothing
      } else {
         iter->second->disown();
      }
      dirents.erase(iter++);
   }
}

