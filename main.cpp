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

// // Print the meaning of a signal.
// static void eprint_signal (const char* kind, int signal) {
//    fprintf (stderr, ", %s %d", kind, signal);
//    const char* sigstr = strsignal (signal);
//    if (sigstr != nullptr) fprintf (stderr, " %s", sigstr);
// }


// Run cpp against the lines of the file.
void cpplines (const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, yyin);
      if (fgets_rc == nullptr) break;
      chomp (buffer, '\n');
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         continue;
      }
      char* savepos = nullptr;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = nullptr;
         if (token == nullptr) break;
         string_set::intern (token);
      }
      ++linenr;
   }
}

void scan_opts (int argc, char** argv) {
   for(;;) {
      int opt = getopt (argc, argv, "@:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': //set_debugflags (optarg);   
            break;
         case 'l': //yy_flex_debug = 1;         
            break;
         case 'y': //yydebug = 1;               
            break;
         default:  fprintf(stderr, "bad option (%c)\n", optopt); break;
      }
   }
}

int main (int argc, char** argv) {
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
   if (yyin == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
               execname, command.c_str(), strerror (errno));
   } else {
      cpplines (execname);
      int pclose_rc = pclose (yyin);
      eprint_status (command.c_str(), pclose_rc);
      if (pclose_rc != 0) exit_status = EXIT_FAILURE;
   }
   
   string filestart = "./";
   string filemiddle = execname;
   string fileending = ".str";
   string outfilename = filestart + filemiddle.substr(0, filemiddle.length()-3) + fileending;
   FILE* outputFile = fopen(outfilename.c_str(), "w+");
   if (outputFile == nullptr) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
            execname, outfilename.c_str(), strerror (errno));
   } else {
      string_set::dump(outputFile);
      int pclose_out = fclose (outputFile);
      eprint_status (outfilename.c_str(), pclose_out);
      if (pclose_out != 0) exit_status = EXIT_FAILURE;
   }
   return exit_status;
}
