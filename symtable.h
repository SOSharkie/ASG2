// $Id: astree.h,v 1.7 2017-11-16 - - $
// Sean Odnert sodnert@ucsc.edu
// Mark Hiserodt mhiserod@ucsc.edu

#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__


#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>

#include "auxlib.h"
#include "astree.h"

extern FILE* symFile;

enum { ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
       ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
       ATTR_lval, ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_field,
       ATTR_variable, ATTR_typeid, ATTR_param, ATTR_bitset_size };
using attr_bitset = bitset<ATTR_bitset_size>;

struct symbol;
struct astree;
//struct location;
using symbol_table = unordered_map<const string*,symbol*>;
using symbol_entry = pair<string*,symbol*>;

struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   string typeID;
   string fieldName;
   size_t filenr;
   size_t linenr;
   size_t offset;
   size_t blockNum;
   vector<symbol*>* parameters;
};

const char* check_attr(astree* node, symbol* sym);
void set_attributes(astree* node);
void create_struct_sym(astree* root);
void create_proto_sym(astree* root);
void create_func_sym(astree* root);
void create_block(astree* root);
void run_attr(astree* root);
void print_sym_table(astree* root);


#endif
