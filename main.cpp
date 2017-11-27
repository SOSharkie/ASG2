// $Id: main.cpp,v 1.8 2017-11-16 - -$
// Sean Odnert sodnert@ucsc.edu
// Mark Hiserodt mhiserod@ucsc.edu

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
#include "astree.h"
#include "symtable.h"

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
   string astending = ".ast";
   string symending = ".sym";

   if (yyin == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
               execname, command.c_str(), strerror (errno));
   } else {

      //------------------ parse and dump the tokens ------------------
      string tokfilename = filestart + 
         filemiddle.substr(0, filemiddle.length()-3) + tokending;
      lexer::tokOut = fopen(tokfilename.c_str(), "w+");
 
      int parse_code = yyparse();
      if (parse_code == 1) exit_status = EXIT_FAILURE;

      int pclose_rc = pclose (yyin);
      eprint_status (command.c_str(), pclose_rc);
      if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;

      int pclose_tok = fclose(lexer::tokOut);
      eprint_status (tokfilename.c_str(), pclose_tok);
      if (pclose_tok != 0) exit_status = EXIT_FAILURE;

      //-------------------- dump the string set --------------------
      string strfilename = filestart + 
         filemiddle.substr(0, filemiddle.length()-3) + strending;
      FILE* strFile = fopen(strfilename.c_str(), "w+");
      string_set::dump(strFile);
      int pclose_out = fclose (strFile);
      eprint_status (strfilename.c_str(), pclose_out);
      if (pclose_out != 0) exit_status = EXIT_FAILURE;

      //-------------------- dump the astree --------------------
      string astfilename = filestart + 
         filemiddle.substr(0, filemiddle.length()-3) + astending;
      FILE* astFile = fopen(astfilename.c_str(), "w+");
      astree::print(astFile, parser::root);
      int pclose_ast = fclose (astFile);
      eprint_status (astfilename.c_str(), pclose_ast);
      if (pclose_ast != 0) exit_status = EXIT_FAILURE;

      //-------------------- dump the symbol table --------------------
      string symfilename = filestart + 
         filemiddle.substr(0, filemiddle.length()-3) + symending;
      symFile = fopen(symfilename.c_str(), "w+");
      run_attr(parser::root);
      print_sym_table(parser::root);
      int pclose_sym = fclose (symFile);
      eprint_status (symfilename.c_str(), pclose_sym);
      if (pclose_sym != 0) exit_status = EXIT_FAILURE;

   }
   return exit_status;
}
