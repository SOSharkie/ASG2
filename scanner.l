/* $Id: scanner.l,v 1.7 2016-10-27 19:45:59-07 - - $ */
/* Sean Odnert sodnert@ucsc.edu */
/* Mark Hiserodt mhiserod@ucsc.edu */

%{

#include "lyutils.h"
#include "auxlib.h"

#define YY_USER_ACTION  { lexer::advance(); }

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);

    fprintf(lexer::tokOut, "  %-2zd %zd.%-4zd %-4d %-10s (%s)\n", 
    lexer::lloc.filenr, lexer::lloc.linenr, lexer::lloc.offset,
    symbol, parser::get_tname (symbol), yytext);

   return symbol;
}

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
INTEGER         ({DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)

%%

"#".*           { lexer::include(); }
[ \t]+          { /* Skip whitespace. */ }
\n              { lexer::newline(); }

"return"        { return yylval_token (TOK_RETURN); }
"string"        { return yylval_token (TOK_STRING); }
"struct"        { return yylval_token (TOK_STRUCT); }
"if"            { return yylval_token (TOK_IF); }
"else"          { return yylval_token (TOK_ELSE); }
"while"         { return yylval_token (TOK_WHILE); }
"null"          { return yylval_token (TOK_NULL); }
"new"           { return yylval_token (TOK_NEW); }
"void"          { return yylval_token (TOK_VOID); }
"char"          { return yylval_token (TOK_CHAR); }
"int"           { return yylval_token (TOK_INT); }
"bool"          { return yylval_token (TOK_BOOL); }

"<="            { return yylval_token (TOK_LE); }
">="            { return yylval_token (TOK_GE); }
"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }

{CHAR}          { return yylval_token (TOK_CHARCON); }
{STRING}        { return yylval_token (TOK_STRINGCON); }
{INTEGER}       { return yylval_token (TOK_INTCON); }
{IDENT}         { return yylval_token (TOK_IDENT); }

"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }
"[]"            { return yylval_token (TOK_ARRAY); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
"<"             { return yylval_token (TOK_LT); }
">"             { return yylval_token (TOK_GT); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }

{NOTNUMBER}     { lexer::badtoken (yytext);
                  return yylval_token (NUMBER); }
.               { lexer::badchar (*yytext); }

%%
