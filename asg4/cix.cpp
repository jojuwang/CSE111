// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get" , cix_command::GET },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void cix_get (client_socket& server, const string& fname) {
   cix_header header;
   header.command = cix_command::GET;
   strncpy(header.filename, fname.c_str(), FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   } else {
      ofstream outfile;
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "recieved " << header.nbytes << " bytes" << endl;
      outfile.open(fname);
      outfile.write(buffer.get(), header.nbytes);
      outfile.close();
   }
}

void cix_put (client_socket& server, const string& fname) {
   cix_header header;
   header.command = cix_command::PUT;
   ifstream locfile(fname);
   if (locfile.fail()){
      outlog << fname << ": File not found in local folder"
             << endl;
      return;
   }
   locfile.seekg(0, locfile.end);
   uint32_t fsize = locfile.tellg();
   locfile.seekg(0, locfile.beg);

   auto buffer = make_unique<char[]> (fsize);
   locfile.read(buffer.get(), fsize);
   locfile.close();
   header.command = cix_command::PUT;
   header.nbytes = fsize;
   strncpy(header.filename, fname.c_str(), FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, buffer.get(), fsize);

   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command == cix_command::ACK){
      outlog << "Put acknowledged. Process successful." << endl;
   } else {
      outlog << "Put failure. Process failed." << endl;
   }
}

void cix_rm (client_socket& server, const string& fname) {
   cix_header header;
   header.command = cix_command::RM;
   strncpy(header.filename, fname.c_str(), FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command == cix_command::ACK){
      outlog << "rm acknowledged. Process successful." << endl;
   } else {
      outlog << "rm failure. Process failed." << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

vector<string> split (const string& line, const string& delimiters) {
   vector<string> words;
   size_t end = 0;

   // Loop over the string, splitting out words, and for each word
   // thus found, append it to the output wordvec.
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   return words;
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         outlog << "command " << line << endl;
         vector<string> params = split(line, " \t");
         if (params.size() == 0){
            outlog << "Command necessary.  Type 'help' for details."
                   << endl;
         } else {
            const auto& itor = command_map.find (params[0]);
            cix_command cmd = itor == command_map.end()
                            ? cix_command::ERROR : itor->second;
            switch (cmd) {
               case cix_command::EXIT:
                  throw cix_exit();
                  break;
               case cix_command::HELP:
                  cix_help();
                  break;
               case cix_command::LS:
                  cix_ls (server);
                  break;
               case cix_command::GET:
                  if (params.size() != 2){
                     outlog << "Invalid args.  "
                            << "Must specify a file." << endl;
                     break;
                  }
                  cix_get (server, params[1]);
                  break;
               case cix_command::PUT:
                  if (params.size() != 2){
                     outlog << "Invalid args.  "
                            << "Must specify a file." << endl;
                     break;
                  }
                  cix_put (server, params[1]);
                  break;
               case cix_command::RM:
                  if (params.size() != 2){
                     outlog << "Invalid args.  "
                            << "Must specify a file." << endl;
                     break;
                  }
                  cix_rm  (server, params[1]);
                  break;
               default:
                  outlog << line << ": invalid command" << endl;
                  break;
            }
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

