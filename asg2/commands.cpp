// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   }
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      throw command_error ("Must specify a file");
   } else {
      inode_ptr curr = state.get_cwd();
      base_file_ptr curr_con = curr->get_contents();
      map<string, inode_ptr> ents = curr_con->get_dirents();
      wordvec result {};
      string miss_file = words[1];
      int file_found = 0;
      int all_files_found = 1;
      for (unsigned j = 1; j < words.size(); j++){
         for (auto iter = ents.begin(); iter != ents.end(); ++iter){
            if (iter->first == words[j] && iter->second->is_dir() == 0){
               file_found = 1;
               break;
            }
         }
         if (file_found == 0){
            all_files_found = 0;
            miss_file = words[j];
            break;
         }
      }

      if (all_files_found == 0){
         throw command_error (miss_file + ": "
                              + "No such file or directory");
      } else {
         for (unsigned i = 1; i < words.size(); i++){
            result = ents[words[i]]->get_contents()->readfile();
            for (unsigned n = 0; n < result.size(); n++){
               cout << result[n] << " ";
            }
            cout << '\n';
         }
      }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr targ = state.get_cwd();
   int targ_found = 0;
   /*if (words.size() == 1){
      state->cwd = state->root;
   }*/
   base_file_ptr targ_con = targ->get_contents();
   map<string, inode_ptr> ents = targ_con->get_dirents();
   if (words[1] == ".."){
      state.set_cwd(ents[".."]);
      state.pop_path();
   } else {
      for (auto iter = ents.begin(); iter != ents.end(); ++iter){
         if (iter->first == words[1]){
            targ_found = 1;
         }
      }
      if (targ_found == 0){
         throw command_error (words[1] + ": No such directory");
      } else {
         state.set_cwd(ents[words[1]]);
         state.push_path(words[1]);
      }
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   vector<string> path = state.get_path();
   if (words.size() == 1){
      if (path.size() == 0){
         cout << "/";
      } else {
         for (unsigned n = 0; n < path.size(); n++){
         cout << "/" << path[n];
         }
      }
      cout << ':' << endl;
   }
   inode_ptr targ = state.get_cwd();
   // I know this is inefficent, but i'm just tired.
   if (words.size() >= 2){
      if (words[1] == "." || words[1] == "/"){
         if (path.size() == 0){
            cout << "/";
         } else {
            for (unsigned n = 0; n < path.size(); n++){
               cout << "/" << path[n];
            }
         }
         cout << ':' << endl;
      }
      if (words[1] == "/"){
         targ = state.get_root();
      } else if (words[1] == ".."){
         base_file_ptr temp_con = targ->get_contents();
         map<string,inode_ptr> tempent = temp_con->get_dirents();
         targ = tempent[".."];
         for (unsigned k = 0; k < path.size() - 1; k++){
            cout << "/" << path[k];
         }
         cout << ':' << endl;
      }
   }
   base_file_ptr targ_con = targ->get_contents();
   map<string, inode_ptr> ents = targ_con->get_dirents();
   for (auto iter = ents.begin(); iter != ents.end(); ++iter){
      int ent_num = iter->second->get_inode_nr();
      string numStr = to_string(ent_num);
      int spaces = 6 - numStr.length();
      for (int i = 1; i <= spaces; i++){
         cout << " ";
      }
      cout << ent_num << "  ";
      base_file_ptr ent_con = iter->second->get_contents();
      size_t ent_size = ent_con->size();
      string sizeStr = to_string(ent_size);
      int spaces2 = 6 - sizeStr.length();
      for (int j = 1; j <= spaces2; j++){
         cout << " ";
      }
      cout << ent_size << "  ";
      cout << iter->first;
      if ( iter->first != "." && iter->first != ".."
          && iter->second->is_dir() == 1){
         cout << "/";
      }
      cout << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string filename = words[1];
   inode_ptr curr = state.get_cwd();
   base_file_ptr curr_con = curr->get_contents();
   inode_ptr new_file = curr_con->mkfile(filename);
   curr_con->insert_into_dirents(filename, new_file);
   if (words.size() > 2){
      wordvec write{};
      base_file_ptr plain = new_file->get_contents();
      for (unsigned i = 2; i < words.size(); i++){
         write.push_back(words[i]);
      }
      plain->writefile(write);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string dirname = words[1];
   inode_ptr curr = state.get_cwd();
   base_file_ptr curr_con = curr->get_contents();
   inode_ptr new_dir = curr_con->mkdir(dirname);
   curr_con->insert_into_dirents(dirname, new_dir);
   new_dir->get_contents()->insert_into_dirents(".", new_dir);
   new_dir->get_contents()->insert_into_dirents("..", curr);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string result {};
   for (unsigned i = 1; i < words.size(); i++){
      result += words[i];
      result += " ";
   }
   state.set_prompt(result);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   vector<string> result = state.get_path();
   if (result.size() == 0){
      cout << "/";
   } else {
      for (unsigned i = 0; i < result.size(); i++){
         cout<< result[i];
      }
   }
   cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

