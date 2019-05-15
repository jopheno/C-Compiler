#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "globals.h"
#include "lex.h"
#include "util.h"
#include "bison.tab.h"

int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

FILE * source;
FILE * listing;
FILE * code;

int lines_number = 0;


int runCompiler(char* source_file, bool trace, bool show_tree) {

    if (trace) {
      TraceScan = TRUE;
    }

    // If forgot to put extension .cc, auto insert.
    if (strchr (source_file, '.') == NULL)
        strcat(source_file,".cc");

    source = fopen(source_file,"r");
    if (source==NULL)
    {
        fprintf(stderr,"File %s not found\n",source_file);
        exit(1);
    }

    listing = stdout;

    Token tok = getToken();
  
    bool lexical_errors = false;

    while(tok.type != ENDFILE) {

        if (tok.type == ERROR) {
          fprintf(stderr, "> Lexical error found at line %d near '%s' classified as '%s'\n", lines_number, tok.lexical_unit, getTokenName(tok.type));
          lexical_errors = true;
        }

        tok = getToken();
    }

    //if (lexical_errors) return 1;

    lines_number = 1;
    fprintf(stdout, ">> Initializing Bison ...\n");

    openFile(source_file);

    TreeNode * syntaxTree;

    syntaxTree = parse();

    if (Error) return 1;

    if (show_tree)
      printTreeC(syntaxTree);

    printf("Building Symbol Table...\n");

    TraceAnalyze = TRUE;
    buildSymtab(syntaxTree);

    //if (Error) return 1;

    printf("Checking Types...\n");

    typeCheck(syntaxTree);

    if (Error) return 1;

    printf("Type Checking Finished\n");

    char * codefile;
    int fnlen = strcspn(source_file,".");
    codefile = (char *) calloc(fnlen+4, sizeof(char));
    strncpy(codefile,source_file,fnlen);
    strcat(codefile,".ci");
    code = fopen(codefile,"w");
    if (code == NULL)
    { printf("Unable to open %s\n",codefile);
      exit(1);
    }
    printf("Trying to generate CI at %s\n", codefile);
    codeGen(syntaxTree,codefile);
    fclose(code);

    return 1;
}