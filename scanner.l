/* $Id: scanner.l,v 1.7 2016-10-27 19:45:59-07 - - $ */

%{

#include "lyutils.h"
#include "auxlib.h"

#define YY_USER_ACTION  { lexer::advance(); }

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);

    fprintf(lexer::tokOut, "  %-2zd %zd.%-2zd %-4d %-10s (%s)\n", 
    	lexer::lloc.filenr, lexer::lloc.linenr, lexer::lloc.offset, symbol, parser::get_tname (symbol), yytext);

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
EXPONENT        ([Ee][+-]?{DIGIT}+)
TOK_INT          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
TOK_IDENT           ({LETTER}({LETTER}|{DIGIT})*)

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

{TOK_INT}        { return yylval_token (TOK_INT); }
{TOK_IDENT}         { return yylval_token (TOK_IDENT); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }

{NOTNUMBER}     { lexer::badtoken (yytext);
                  return yylval_token (TOK_INT); }
.               { lexer::badchar (*yytext); }

%%
