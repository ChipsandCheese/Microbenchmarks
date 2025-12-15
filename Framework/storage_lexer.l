%{
#include <stdlib.h>
#include "storage_parser.tab.h"
%} 

%%

[\n]                 { return NEWLINE; }
[ \t\v\r\n]+               /* ignore whitespace */
","                  { return COMMA; }
[0-9]+               { yylval.ival = atoi(yytext); return INTEGER; }
[0-9]+\.[0-9]+       { yylval.fval = atof(yytext); return FLOAT; }
[A-Za-z0-9_-]+         { 
    yylval.str = strndup(yytext, yyleng + 1); 
    yylval.str[yyleng] = '\0'; 
    return STRING; 
}
.                    { return yytext[0]; }
%%

int yywrap(void) {
    return 1;
}

void set_yyin(FILE* fd) {
    yyin = fd;
}