// $Id: astree.h,v 1.7 2017-11-16 - - $
// Sean Odnert sodnert@ucsc.edu
// Mark Hiserodt mhiserod@ucsc.edu

#include <bitset>
#include <iostream>
#include <string>
#include <unordered_map>

#include "symtable.h"
#include "astree.h"
#include "string_set.h"
#include "lyutils.h"

vector<symbol_table*> symStack;
vector<int> blockCounter;
int nextBlock = 1;
FILE* symFile;

symbol* newsymbol(astree* node){
   if(symStack.empty()){
      symStack.push_back(new symbol_table);
      symStack.push_back(nullptr);
   }
   if(blockCounter.empty() ){
      blockCounter.push_back(0);
   }
   symbol* sym = new symbol();
   sym->filenr = node->lloc.filenr;
   sym->linenr = node->lloc.linenr;
   sym->offset = node->lloc.offset;
   sym->attributes = node->attributes;
   sym->fields = nullptr;
   sym->fieldName = "";
   sym->typeID = "";
   sym->blockNum = blockCounter.back();
   sym->parameters = nullptr;
   return sym;
}


const char* check_attr(astree* node, symbol* sym){
   string out = "";
   attr_bitset attr = node->attributes;
    if (attr[ATTR_bool] == 1) {
        out += "bool ";}
    if (attr[ATTR_char] == 1) {
        out += "char ";}
    if (attr[ATTR_int] == 1) {
        out += "int ";}
    if (attr[ATTR_void] == 1) {
        out += "void ";}
    if (attr[ATTR_field]) {
        out += "field {" + sym->fieldName + "} ";}
    if (attr[ATTR_array] == 1) {
        out += "array ";}
    if (attr[ATTR_null] == 1) {
        out += "null ";}
    if (attr[ATTR_string] == 1) {
        out += "string ";}
    if (attr[ATTR_typeid] == 1) {
        out += "\"" + sym->typeID + "\" ";}
    if (attr[ATTR_function] == 1) {
        out += "function ";}
    if (attr[ATTR_variable] == 1) {
        out += "variable ";}
    if (attr[ATTR_lval] == 1) {
        out += "lval ";}
    if (attr[ATTR_param] == 1) {
        out += "param ";}
    if (attr[ATTR_const] == 1) {
        out += "const ";}
    if (attr[ATTR_vreg] == 1) {
        out += "vreg ";}
    if (attr[ATTR_vaddr] == 1) {
        out += "vaddr ";}
    if (attr[ATTR_struct] == 1) {
        out += "struct ";}
   return out.c_str();
}

void check_array(astree* node){
   switch(node->children[0]->symbol){
      case TOK_BOOL:       
        node->attributes[ATTR_bool]= 1;
        node->attributes[ATTR_const] = 1;
        node->attributes[ATTR_lval] = 1;
        break;

      case TOK_CHAR:       
        node->attributes[ATTR_char] = 1;
        node->attributes[ATTR_lval] = 1;
        break;

      case TOK_INT:        
        node->attributes[ATTR_int] = 1;  
        node->children[0]->attributes[ATTR_variable] = 1;
        node->children[0]->attributes[ATTR_int] = 1;
        node->attributes[ATTR_lval] = 1;
        break;
     } 
}

void set_attributes(astree* node){
   switch(node->symbol) {

      case TOK_BOOL:       
        node->attributes[ATTR_bool]= 1;
        node->attributes[ATTR_const] = 1;
        node->attributes[ATTR_lval] = 1;
        if (node->children.size() > 0)
          node->children[0]->attributes[ATTR_bool] = 1;
        break;

      case TOK_CHAR:       
        node->attributes[ATTR_char] = 1;
        node->attributes[ATTR_lval] = 1;
        if (node->children.size() > 0)
          node->children[0]->attributes[ATTR_char] = 1;
        break;

      case TOK_INT:        
        node->attributes[ATTR_int] = 1; 
        node->attributes[ATTR_lval] = 1; 
        if (node->children.size() > 0)
          node->children[0]->attributes[ATTR_int] = 1;
        break;

      case TOK_VOID:       
        node->attributes[ATTR_void] = 1;
        if (node->children.size() > 0)
          node->children[0]->attributes[ATTR_void] = 1;
        break;

      case TOK_NULL:       
        node->attributes[ATTR_null] = 1;
        node->attributes[ATTR_const] = 1;
        break;

      case TOK_STRING:     
        node->attributes[ATTR_string] = 1;
        node->attributes[ATTR_lval] = 1;
        if (node->children.size() > 0)
          node->children[0]->attributes[ATTR_string] = 1;
        break;

      case TOK_ARRAY:      
        node->attributes[ATTR_array] = 1;
        break;
      
      case TOK_FUNCTION:   
        node->attributes[ATTR_function] = 1;
        node->children[0]->children[0]->attributes[ATTR_function] = 1;
        node->attributes[ATTR_variable] = 0;
        node->attributes[ATTR_lval] = 0;
        node->children[0]->children[0]->attributes[ATTR_variable] = 0;
        node->children[0]->children[0]->attributes[ATTR_lval] = 0;
        break;

      case TOK_PROTOTYPE:
        node->children[0]->children[0]->attributes[ATTR_variable] = 0;
        node->children[0]->children[0]->attributes[ATTR_lval] = 0;
        break;

      case TOK_PARAMLIST:  
        for (size_t index = 0;index < node->children.size(); ++index){
          node->children[index]->children[0]
          ->attributes[ATTR_param] = 1;
        }
        break;

      case TOK_STRINGCON:  
        node->attributes[ATTR_string] = 1;
        node->attributes[ATTR_const] = 1;
        node->attributes[ATTR_lval] = 1;
        break;

      case TOK_CHARCON:    
        node->attributes[ATTR_char] = 1;
        node->attributes[ATTR_const] = 1;  
        node->attributes[ATTR_lval] = 1;
        break;

      case TOK_INTCON:     
        node->attributes[ATTR_int] = 1;
        node->attributes[ATTR_const] = 1;
        node->attributes[ATTR_lval] = 1;
        break;

      case TOK_FIELD:      
        node->attributes[ATTR_field] = 1;
        if("TOK_TYPEID" == string(parser::get_tname(node->symbol)))
          node->attributes[ATTR_struct] = 1;
        break;

      case TOK_TYPEID:     
        node->attributes[ATTR_typeid] = 1;
        for(size_t index = 0; index < node->children.size(); ++index){
          node->children[index]->attributes[ATTR_typeid] = 1;
          node->children[index]->attributes[ATTR_struct] = 1;
          break;
        }

      case TOK_VARDECL:    
        node->attributes[ATTR_variable] = 1;
        break;

      case TOK_IDENT:      
        node->attributes[ATTR_lval] = 1;
        node->attributes[ATTR_variable] = 1;
        break;

      case TOK_DECLID:     
        node->attributes[ATTR_lval] = 1;
        node->attributes[ATTR_variable] = 1;
        break;

      case TOK_STRUCT:     
        node->attributes[ATTR_struct] = 1;
        node->attributes[ATTR_typeid] = 1;
        node->children[0]->attributes[ATTR_struct] = 1;
        node->attributes[ATTR_variable] = 0;
        node->attributes[ATTR_lval] = 0;
        node->children[0]->attributes[ATTR_variable] = 0;
        node->children[0]->attributes[ATTR_lval] = 0;
        break;

      default:           
        break;
   }
}

void insert_symbol (symbol_table table, const string* key, 
                     symbol* sym, astree* node) {
   table[key] = sym;
   for (size_t size = 1; size < blockCounter.size(); ++size) {
      fprintf(symFile, "   ");
   }
   fprintf (symFile, "%s (%zu.%zu.%zu) {%zu} %s\n", key->c_str(), 
            sym->filenr, sym->linenr, sym->offset, sym->blockNum, 
            check_attr (node, sym));
}

void create_call_symbol(astree* root){
   astree* call = root->children[0];
   symbol* sym = newsymbol(call);
   sym->typeID = root->children[0]->lexinfo->c_str();
   vector<symbol*> params;
   sym->parameters = &params;
   const string* key;
   key = call->lexinfo;
   insert_symbol (*symStack[0], key, sym, call);
   
   for (size_t index = 1; index < root->children.size(); ++index){
      astree* call = root->children[index];
      sym = newsymbol(call);
      key = call->lexinfo;
      ++sym->blockNum;
      params.push_back(sym);
      fprintf(symFile, "   ");
      insert_symbol (*symStack[0], key, sym, call);
   }
   fprintf(symFile, "\n");
}

void create_struct_sym(astree* root) {
   symbol* sym = newsymbol(root->children[0]);
   sym->typeID = root->children[0]->lexinfo->c_str();
   symbol_table fields;
   sym->fields = &fields;
   const string* key;
   key = root->children[0]->lexinfo;
   insert_symbol(*symStack[0], key, sym, root->children[0]);
   if(root->children.size() > 1){
      astree* fieldlist = root->children[1];
      for (size_t i = 0;i < fieldlist->children.size(); ++i){
         astree* field = fieldlist->children[i]->children[0];
         sym = newsymbol(field);
         key = field->lexinfo;
         sym->fieldName = root->children[0]->lexinfo->c_str();
         if("TOK_TYPEID" == string(parser::get_tname(
            fieldlist->children[i]->symbol))){
              sym->typeID = fieldlist->children[i]
                ->lexinfo->c_str();
          }
         fprintf (symFile, "   ");
         insert_symbol(fields, key, sym, field);
      }
   }
}

void create_block (astree* root) {
      blockCounter.push_back(nextBlock);
      nextBlock++;
      symStack[blockCounter.back()] = new symbol_table;
      symStack.push_back(nullptr);
      print_sym_table(root);
      blockCounter.pop_back();
}

void create_proto_sym(astree* root){
   astree* proto;
   if("TOK_ARRAY" == 
      string(parser::get_tname(root->children[0]->symbol))){
        proto = root->children[0]->children[1];
        proto->attributes = root->children[0]->children[0]->attributes;
        proto->attributes[ATTR_array] = 1;
    } else {
      proto = root->children[0]->children[0];
    }
   symbol* sym = newsymbol(proto);
   vector<symbol*> params;
   sym->parameters = &params;
   const string* key;
   key = proto->lexinfo;
   insert_symbol (*symStack[0], key, sym, proto);
   
   astree* paramlist = root->children[1];
   for (size_t index = 0; index < paramlist->children.size(); ++index){
      astree* proto = paramlist->children[index]->children[0];
      sym = newsymbol(proto);
      key = proto->lexinfo;
      if("TOK_TYPEID" == 
         string(parser::get_tname(paramlist->children[index]->symbol))){
           sym->typeID = paramlist->children[index]->lexinfo->c_str();
      }
      ++sym->blockNum;
      params.push_back(sym);
      fprintf(symFile, "   ");
      insert_symbol (*symStack[0], key, sym, proto);
   }
   fprintf(symFile, "\n"); 
}

void create_vardecl(astree* root){
   astree* vardecl = root->children[0]->children[0];
   symbol* sym = newsymbol(vardecl);
   const string* key;
   key = vardecl->lexinfo;
  if("TOK_TYPEID" == 
     string(parser::get_tname(root->children[0]->symbol))){
       sym->typeID = root->children[0]->lexinfo->c_str();
   }
   insert_symbol (*symStack[blockCounter.back()], key, sym, vardecl);
}

void create_func_sym(astree* root){
   astree* function = root->children[0]->children[0];
   symbol* sym = newsymbol(function);
   sym->typeID = root->children[0]->lexinfo->c_str();
   vector<symbol*> params;
   sym->parameters = &params;
   const string* key;
   key = function->lexinfo;
   insert_symbol (*symStack[0], key, sym, function);
   
   astree* paramlist = root->children[1];
   for (size_t index = 0; index < paramlist->children.size(); ++index){
      astree* param = paramlist->children[index]->children[0];
      sym = newsymbol(param);
      key = param->lexinfo;
      if("TOK_TYPEID" == 
         string(parser::get_tname(paramlist->children[index]->symbol))){
           sym->typeID = paramlist->children[index]->lexinfo->c_str();
      }
      ++sym->blockNum;
      params.push_back(sym);
      fprintf(symFile, "   ");
      insert_symbol (*symStack[0], key, sym, param);
   }
   fprintf(symFile, "\n");
   create_block(root->children[2]); 
}

void run_attr(astree* root){
   for( size_t index = 0; index < root->children.size(); ++index){
      run_attr(root->children[index]);
   }
   set_attributes(root);
}

void print_sym_table(astree* root){
   for (size_t index = 0; index < root->children.size(); ++index) {
      
      int nodesymbol = root->children[index]->symbol;
      switch(nodesymbol){
         case TOK_STRUCT:  
            create_struct_sym(root->children[index]);
            fprintf(symFile, "\n");
            break;
         case TOK_FUNCTION: 
            create_func_sym(root->children[index]); 
            fprintf(symFile, "\n");
            break;
         case TOK_PROTOTYPE:
            create_proto_sym(root->children[index]); 
            fprintf(symFile, "\n");
            break;
         case TOK_CALL:
            create_call_symbol(root->children[index]); 
            fprintf(symFile, "\n");
            break;
         case TOK_VARDECL:
            create_vardecl(root->children[index]);
            break;
         case TOK_IF:
            create_block(root->children[index]->children[1]);
            break;
         case TOK_IFELSE:
            create_block(root->children[index]->children[1]);
            create_block(root->children[index]->children[2]);
            break;
         default:
            break;
      }
   }  
}
