%{
//#include "globals.h"
//#include "util.h"
//#include "scan.h"

#include "globals.h"
#include "lex.h"

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

extern int lines_number;

extern FILE * source;
extern FILE * listing;
extern FILE * code;

char* getTokenName(TokenType t) {

  if (t == IMPORT) return "IMPORT";

  if (t == ENDFILE) return "ENDFILE";
  if (t == ERROR) return "ERROR";

  if (t == IF) return "IF";
  if (t == ELSE) return "ELSE";
  if (t == WHILE) return "WHILE";
  if (t == RETURN) return "RETURN";

  if (t == VOID) return "VOID";
  if (t == INT) return "INT";

  if (t == ID) return "ID";
  if (t == NUM) return "NUM";

  if (t == ASSIGN) return "ASSIGN";

  if (t == EQ) return "EQ";
  if (t == DIFF) return "DIFF";
  if (t == LT) return "LOWERTHAN";
  if (t == LET) return "LOWEREQUALTHAN";
  if (t == GT) return "GREATERTHAN";
  if (t == GET) return "GREATEREQUALTHAN";
  if (t == PLUS) return "PLUS";
  if (t == MINUS) return "MINUS";
  if (t == TIMES) return "TIMES";
  if (t == OVER) return "OVER";

  if (t == LPAREN) return "LPAREN";
  if (t == RPAREN) return "RPAREN";
  if (t == LBRACKET) return "LBRACKET";
  if (t == RBRACKET) return "RBRACKET";
  if (t == LCBRACE) return "LCBRACE";
  if (t == RCBRACE) return "RCBRACE";

  if (t == SEMI) return "SEMI";
  if (t == COMMA) return "COMMA";

  return "NOTATOKEN";
}

%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z_]
id          [a-zA-Z0-9_]
identifier  {id}+
newline     \n
whitespace  [ \t]+
ignore      \r

%x COMMENT_MULTI_BLOCK
%x COMMENT_MONO_BLOCK

%%

"/*"                          {BEGIN(COMMENT_MULTI_BLOCK);}
<COMMENT_MULTI_BLOCK>"*/"     {BEGIN(INITIAL);}
<COMMENT_MULTI_BLOCK>"\n"     {lines_number++;}
<COMMENT_MULTI_BLOCK>.        {}

"//"                          {BEGIN(COMMENT_MONO_BLOCK);}
<COMMENT_MONO_BLOCK>"\n"      {lines_number++; BEGIN(INITIAL);}
<COMMENT_MONO_BLOCK>.         {}

"import"        {return IMPORT;}

"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"return"        {return RETURN;}

"void"          {return VOID;}
"int"           {return INT;}

"="             {return ASSIGN;}
"=="            {return EQ;}
"!="            {return DIFF;}
"<"             {return LT;}
"<="            {return LET;}
">"             {return GT;}
">="            {return GET;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}

"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
"{"             {return LCBRACE;}
"}"             {return RCBRACE;}
";"             {return SEMI;}
","             {return COMMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lines_number++;}
{whitespace}    {/* skip whitespace */}
{ignore}        {/* ignore */}
.               {return ERROR;}

%%

void openFile(char* file_name) {
  yyin = fopen(file_name, "r");
}

Token getToken(void)
{
  static int firstTime = TRUE;
  Token currentToken;

  if (firstTime)
  { firstTime = FALSE;
    lines_number++;
    yyin = source;
    yyout = listing;
  }

  currentToken.type = yylex();
  strncpy(currentToken.lexical_unit,yytext,MAXTOKENLEN);
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: %s",lines_number, tokenString);
    //printf(">> Getting token => %s\n", getTokenName(currentToken));
  }
  return currentToken;
}

int getTokenByType(void)
{
  static int firstTime = TRUE;
  Token currentToken;

  if (firstTime)
  { firstTime = FALSE;
    lines_number++;
    yyin = source;
    yyout = listing;
  }

  currentToken.type = yylex();
  strncpy(currentToken.lexical_unit,yytext,MAXTOKENLEN);
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: %s",lines_number, tokenString);
    //printf(">> Getting token => %s\n", getTokenName(currentToken));
  }
  return currentToken.type;
}
