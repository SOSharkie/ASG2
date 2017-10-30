// $Id: cppstrtok.cpp,v 1.8 2017-10-15 - -$
// Sean Odnert sodnert@ucsc.edu
// Mark Hiserodt mhiserod@ucsc.edu

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
#include <vector>
using namespace std;

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "lyutils.h"
#include "auxlib.h"
#include "string_set.h"

const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

void scan_opts (int argc, char** argv) {
   yy_flex_debug = 0;
   yydebug = 0;
   for(;;) {
      int opt = getopt (argc, argv, "@:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': //set_debugflags (optarg);   
            break;
         case 'l': yy_flex_debug = 1;         
            break;
         case 'y': yydebug = 1;               
            break;
         default:  fprintf(stderr, "bad option (%c)\n", optopt); break;
      }
   }
}

int main (int argc, char** argv) {
   exec::execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   string inputname = argv[argc-1];

   if (inputname.substr(inputname.length()-2, 2).compare("oc") != 0){
      exit_status = EXIT_FAILURE;
      fprintf(stderr, "input file %s was not '.oc'\n", argv[argc-1]);
      return exit_status;
   }

   const char* execname = basename (argv[argc-1]);
   scan_opts(argc, argv);

   string command = CPP + " " + execname;
   yyin = popen (command.c_str(), "r");

   string filestart = "./";
   string filemiddle = execname;
   string strending = ".str";
   string tokending = ".tok";

   if (yyin == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
               execname, command.c_str(), strerror (errno));
   } else {
      string tokfilename = filestart + filemiddle.substr(0, filemiddle.length()-3) + tokending;
      lexer::tokOut = fopen(tokfilename.c_str(), "w+");
      for (int i = 0; yylex() != 0; i++){
         string_set::intern(yytext);
      }  
      int pclose_rc = pclose (yyin);
      eprint_status (command.c_str(), pclose_rc);
      if (pclose_rc != 0) exit_status = EXIT_FAILURE;
   }
   
   string strfilename = filestart + filemiddle.substr(0, filemiddle.length()-3) + strending;
   FILE* strFile = fopen(strfilename.c_str(), "w+");
   if (strFile == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
            execname, strfilename.c_str(), strerror (errno));
   } else {
      string_set::dump(strFile);
      int pclose_out = fclose (strFile);
      eprint_status (strfilename.c_str(), pclose_out);
      if (pclose_out != 0) exit_status = EXIT_FAILURE;
   }
   return exit_status;
}
